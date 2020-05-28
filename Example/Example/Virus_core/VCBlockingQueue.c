//
//  VCBlockingQueue.c
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCBlockingQueue.h"
#include "VCRuntime.h"
#include "VCAtomicQueue.h"
#include <pthread/pthread.h>

typedef struct __VCBlockingQueue {
    VCRuntimeBase *base;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    VCAtomicQueueRef queue;
    VCAtomicQueueRef waitQueue;
} VCBlockingQueue;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t thread;
} VCWaitNode;

typedef VCWaitNode * VCWaitNodeRef;

VC_INLINE VCWaitNodeRef __VCWaitNodeCreate(pthread_mutex_t lock,pthread_cond_t cond) {
    VCWaitNodeRef ref = calloc(1, sizeof(VCWaitNode));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    ref->lock = lock;
    ref->cond = cond;
    return ref;
}

static void __VCBlockingQueueDealloc(VCTypeRef ref) {
    assert(ref);
    VCBlockingQueueRef queue = (VCBlockingQueueRef)ref;
    VCBlockingQueueClear(queue);
    if (queue->queue) {
        VCRelease(queue->queue);
    }
    if (queue->waitQueue) {
        VCRelease(queue->waitQueue);
    }
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    free(queue);
    queue = NULL;
}

VC_INLINE const void *__VCBlockingQueueWaitQueueRetainCallBack(const void *value) {
    VCWaitNodeRef ref = (VCWaitNodeRef)value;
    ref->thread = pthread_self();
    return value;
}

VC_INLINE void __VCBlockingQueueWaitQueueReleaseCallBack(const void *value) {
    VCWaitNodeRef ref = (VCWaitNodeRef)value;
    pthread_mutex_lock(&ref->lock);
    pthread_cond_signal_thread_np(&ref->cond, ref->thread);
    pthread_mutex_unlock(&ref->lock);
    free(ref);
    ref = NULL;
}

const VCRuntimeClass __VCBlockingQueueClass = {
    "VCBlockingQueue",
    NULL,
    NULL,
    NULL,
    NULL,
    __VCBlockingQueueDealloc
};

const VCBlockingQueueCallBack kVCTypeBlockingQueueCallBack = {
    VCRetain,
    VCRelease
};

const VCBlockingQueueCallBack kVCCopyStringBlockingQueueCallBack = {
    (VCBlockingQueueRetainCallBack)strdup,
    (VCBlockingQueueReleaseCallBack)free
};

static volatile VCTypeID __VCBlockingQueueTypeID = 0;

VCTypeID VCBlockingQueueGetTypeID(void) {
    return VCRuntimeRegisterClass(__VCBlockingQueueTypeID, __VCBlockingQueueClass);
}

VCBlockingQueueRef VCBlockingQueueCreate(const VCBlockingQueueCallBack *callback) {
    VCBlockingQueueRef ref = (VCBlockingQueueRef)VCRuntimeCreateInstance(VCBlockingQueueGetTypeID(), sizeof(VCBlockingQueue) - sizeof(VCRuntimeBase));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    VCAtomicQueueCallback queueCallBack = {
        callback ? callback->retain : NULL,
        callback ? callback->release : NULL
    };
    VCAtomicQueueRef queue = VCAtomicQueueCreate(&queueCallBack);
    if (VC_UNLIKELY(queue == NULL)) {
        __VCBlockingQueueDealloc(ref);
        return NULL;
    }
    ref->queue = queue;
    VCAtomicQueueCallback waitQueueCallBack = {
        __VCBlockingQueueWaitQueueRetainCallBack,
        __VCBlockingQueueWaitQueueReleaseCallBack
    };
    VCAtomicQueueRef waitQueue = VCAtomicQueueCreate(&waitQueueCallBack);
    if (VC_UNLIKELY(waitQueue == NULL)) {
        __VCBlockingQueueDealloc(ref);
        return NULL;
    }
    ref->waitQueue = waitQueue;
    pthread_mutex_init(&ref->lock, NULL);
    pthread_cond_init(&ref->cond, NULL);
    return ref;
}

VCIndex VCBlockingQueueGetCount(VCBlockingQueueRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return 0;
    return VCAtomicQueueGetCount(ref->queue);
}

void VCBlockingQueueEnqueue(VCBlockingQueueRef ref,const void *value) {
    assert(ref);
    assert(value);
    if (VC_UNLIKELY(ref == NULL || value == NULL)) return;
    VCAtomicQueueEnqueue(ref->queue, value);
    VCAtomicQueueDequeue(ref->waitQueue);
}

const void *VCBlockingQueueDequeue(VCBlockingQueueRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    const void *value = VCAtomicQueueDequeue(ref->queue);
    if (value == NULL) {
        VCAtomicQueueEnqueue(ref->waitQueue, __VCWaitNodeCreate(ref->lock, ref->cond));
        pthread_mutex_lock(&ref->lock);
        pthread_cond_wait(&ref->cond, &ref->lock);
        value = VCAtomicQueueDequeue(ref->queue);
        pthread_mutex_unlock(&ref->lock);
    }
    return value;
}

void VCBlockingQueueClear(VCBlockingQueueRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return;
    VCAtomicQueueClear(ref->waitQueue);
    VCAtomicQueueClear(ref->queue);
}
