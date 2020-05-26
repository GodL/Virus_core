//
//  VCAtomicQueue.c
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCAtomicQueue.h"
#include "VCRuntime.h"

typedef struct __VCAtomicQueueNode {
    const void *value;
    struct __VCAtomicQueueNode *next;
} VCAtomicQueueNode;

typedef VCAtomicQueueNode * VCAtomicQueueNodeRef;

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
    free(queue);
    queue = NULL;
}

const VCRuntimeClass __VCAtomicQueueClass = {
    "VCAtomicQueue",
    NULL,
    NULL,
    __VCAtomicQueueEqual,
    NULL,
    __VCAtomicQueueDealloc
};

static volatile VCTypeID __VCAtomQueueTypeID = 0;

VCTypeID VCAtomQueueGetTypeID(void) {
    return VCRuntimeRegisterClass(__VCAtomQueueTypeID, __VCAtomicQueueClass);
}
