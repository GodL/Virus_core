//
//  VCSqlite3Handler.c
//  Example
//
//  Created by lihao10 on 2020/5/29.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCSqlite3Handler.h"
#include "VCRuntime.h"
#include "VCBlockingQueue.h"

typedef struct __VCSqlite3Handler{
    VCRuntimeBase base;
    const char *path;
    VCIndex concurrentRead;
    VCBlockingQueueRef wQueue;
    VCBlockingQueueRef rQueue;
} VCSqlite3Handler;

static void __VCSqlite3HandlerDealloc(VCTypeRef ref) {
    if (VC_UNLIKELY(ref == NULL)) return;
    VCSqlite3HandlerRef handler = (VCSqlite3HandlerRef)ref;
    free((void *)handler->path);
    if (handler->rQueue) {
        VCRelease(handler->rQueue);
        handler->rQueue = NULL;
    }
    if (handler->wQueue) {
        VCRelease(handler->wQueue);
        handler->wQueue = NULL;
    }
    free(handler);
    handler = NULL;
}

static void __VCSqlite3HandlerSetupHandle(VCSqlite3HandlerRef ref) {
    VCSqlite3Ref sqlite3 = VCSqlite3Create(ref->path);
    VCBlockingQueueEnqueue(ref->wQueue, sqlite3);
    VCRelease(sqlite3);
    for (VCIndex i=0; i<ref->concurrentRead; i++) {
        sqlite3 = VCSqlite3Create(ref->path);
        VCBlockingQueueEnqueue(ref->rQueue, sqlite3);
        VCRelease(sqlite3);
    }
}

const VCRuntimeClass __VCSqlite3HandlerClass = {
    "VCSqlite3Handler",
    NULL,
    NULL,
    NULL,
    NULL,
    __VCSqlite3HandlerDealloc
};

static volatile VCTypeID __VCSqlite3HandlerTypeID = 0;

VCTypeID VCSqlite3HandlerGetTypeID(void) {
    return VCRuntimeRegisterClass(__VCSqlite3HandlerTypeID, __VCSqlite3HandlerClass);
}

VCSqlite3HandlerRef VCSqlite3HandlerCreate(const char *path,VCIndex concurrentRead) {
    assert(path);
    if (VC_UNLIKELY(strlen(path) == 0)) return NULL;
    VCSqlite3HandlerRef ref = (VCSqlite3HandlerRef)VCRuntimeCreateInstance(VCSqlite3HandlerGetTypeID(), sizeof(VCSqlite3Handler) - sizeof(VCRuntimeBase));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    ref->path = strdup(path);
    if (concurrentRead == 0) concurrentRead = 6;
    ref->concurrentRead = concurrentRead;
    VCBlockingQueueRef wQueue = VCBlockingQueueCreate(&kVCTypeBlockingQueueCallBack);
    ref->wQueue = wQueue;
    VCBlockingQueueRef rQueue = VCBlockingQueueCreate(&kVCTypeBlockingQueueCallBack);
    ref->rQueue = rQueue;
    __VCSqlite3HandlerSetupHandle(ref);
    return ref;
}

VCSqlite3Ref VCSqlite3HandlerGetWriteHandle(VCSqlite3HandlerRef ref) {
    assert(ref);
    VCSqlite3Ref sqlite3 = (VCSqlite3Ref)VCBlockingQueueDequeue(ref->wQueue);
    if (!VCSqlite3IsOpen(sqlite3)) {
        VCSqlite3OpenWal(sqlite3);
    }
    return sqlite3;
}

void VCSqlite3HandlerReturnWriteHandle(VCSqlite3HandlerRef ref,VCSqlite3Ref sqlite3) {
    assert(ref);
    VCBlockingQueueEnqueue(ref->wQueue, sqlite3);
    VCRelease(sqlite3);
}

VCSqlite3Ref VCSqlite3HandlerGetReadHandle(VCSqlite3HandlerRef ref) {
    assert(ref);
    VCSqlite3Ref sqlite3 = (VCSqlite3Ref)VCBlockingQueueDequeue(ref->rQueue);
    if (!VCSqlite3IsOpen(sqlite3)) {
        VCSqlite3OpenWal(sqlite3);
    }
    return sqlite3;
}
