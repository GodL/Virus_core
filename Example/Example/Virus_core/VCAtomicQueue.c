//
//  VCAtomicQueue.c
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCAtomicQueue.h"
#include "VCRuntime.h"
#include <libkern/OSAtomic.h>

typedef struct __VCAtomicQueueNode {
    const void *value;
    struct __VCAtomicQueueNode *next;
} VCAtomicQueueNode;

typedef VCAtomicQueueNode * VCAtomicQueueNodeRef;

VC_INLINE VCAtomicQueueNodeRef __VCAtomicQueueNodeCreate(const void *value) {
    VCAtomicQueueNodeRef ref = malloc(sizeof(VCAtomicQueueNode));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    ref->value = value;
    ref->next = NULL;
    return ref;
}

typedef struct __VCAtomicQueue {
    VCRuntimeBase *base;
    volatile VCIndex count;
    volatile VCAtomicQueueNodeRef head;
    volatile VCAtomicQueueNodeRef tail;
} VCAtomicQueue;

static bool __VCAtomicQueueEqual(VCTypeRef ref1,VCTypeRef ref2) {
    if (ref1 == ref2) return true;
    VCAtomicQueueRef queue1 = (VCAtomicQueueRef)ref1;
    VCAtomicQueueRef queue2 = (VCAtomicQueueRef)ref2;
    return queue1->head == queue2->head && queue1->tail == queue2->tail;
}

static void __VCAtomicQueueDealloc(VCTypeRef ref1) {
    assert(ref1);
    VCAtomicQueueRef queue = (VCAtomicQueueRef)ref1;
    VCAtomicQueueClear(queue);
    if (queue->head) {
        free(queue->head);
        queue->head = NULL;
    }
    free(queue);
    queue = NULL;
}

VC_INLINE VCIndex __VCAtomicQueueGetCount(VCAtomicQueueRef ref) {
    assert(ref);
    return ref->count;
}

VC_INLINE const VCAtomicQueueCallback *__VCAtomicQeueuGetCallback(VCAtomicQueueRef ref) {
    return (const VCAtomicQueueCallback *)ref->base->callback;
}

VC_INLINE void __VCAtomicQueueReleaseNode(VCAtomicQueueRef ref,VCAtomicQueueNodeRef node) {
    const VCAtomicQueueCallback *cb = __VCAtomicQeueuGetCallback(ref);
    if (cb && cb->release) cb->release(node->value);
    free(node);
    node = NULL;
}

VC_INLINE bool __VCAtomicQueueStringEqual(const void *c1,const void *c2) {
    return c1 == c2 || strcmp(c1, c2) == 0;
}

const VCAtomicQueueCallback kVCTypeAtomicQueueCallback = {
    VCRetain,
    VCRelease,
    VCEqual
};

const VCAtomicQueueCallback kVCCopyStringAtomicQueueCallback = {
    (VCAtomicQueueRetainCallback)strdup,
    (VCAtomicQueueReleaseCallback)free,
    (VCAtomicQueueEqualCallback)__VCAtomicQueueStringEqual
};

const VCRuntimeClass __VCAtomicQueueClass = {
    "VCAtomicQueue",
    NULL,
    NULL,
    __VCAtomicQueueEqual,
    NULL,
    __VCAtomicQueueDealloc
};

static volatile VCTypeID __VCAtomicQueueTypeID = 0;

VCTypeID VCAtomicQueueGetTypeID(void) {
    return VCRuntimeRegisterClass(__VCAtomicQueueTypeID, __VCAtomicQueueClass);
}

VCAtomicQueueRef VCAtomicQueueCreate(const VCAtomicQueueCallback *callback) {
    VCAtomicQueueRef ref = (VCAtomicQueueRef)VCRuntimeCreateInstance(VCAtomicQueueGetTypeID(), sizeof(VCAtomicQueue) - sizeof(VCRuntimeBase));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    ref->count = 0;
    ref->base->callback = (uintptr_t)callback;
    VCAtomicQueueNodeRef node = __VCAtomicQueueNodeCreate(NULL);
    ref->head = ref->tail = node;
    return ref;
}

void VCAtomicQueueEnqueue(VCAtomicQueueRef ref,const void *value) {
    assert(ref);
    assert(value);
    if (VC_UNLIKELY(ref == NULL || value == NULL)) return;
    const VCAtomicQueueCallback *cb = __VCAtomicQeueuGetCallback(ref);
    if (cb && cb->retain) value = cb->retain(value);
    VCAtomicQueueNodeRef node = __VCAtomicQueueNodeCreate(value);
    if (VC_UNLIKELY(node == NULL)) return;
    VCAtomicQueueNodeRef tail;
    do {
        tail = ref->tail;
        if (tail == NULL) tail = ref->head;
    } while (VC_CAS(&tail->next, NULL, node));
    VC_CAS(&ref->tail, tail, node);
    VC_ATOMIC_ADD(&ref->count, 1);
}

const void *VCAtomicQueueDequeue(VCAtomicQueueRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    VCAtomicQueueNodeRef result;
    do {
        result = ref->head->next;
        if (result == NULL) return NULL;
    } while (VC_CAS(&ref->head->next, result, result->next));
    const void *val = result->value;
    __VCAtomicQueueReleaseNode(ref, result);
    VC_ATOMIC_SUB(&ref->count, 1);
    return val;
}

void VCAtomicQueueClear(VCAtomicQueueRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return;
    
    while (VCAtomicQueueDequeue(ref));
}
