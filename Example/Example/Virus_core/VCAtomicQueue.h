//
//  VCAtomicQueue.h
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCAtomicQueue_h
#define VCAtomicQueue_h

#include "VCBase.h"

VC_EXTERN_C_BEGIN

typedef const void *(*VCAtomicQueueRetainCallback)(const void *value);

typedef void (*VCAtomicQueueReleaseCallback)(const void *value);

typedef bool (*VCAtomicQueueEqualCallback)(const void *value1,const void *value2);

typedef struct {
    VCAtomicQueueRetainCallback retain;
    VCAtomicQueueReleaseCallback release;
    VCAtomicQueueEqualCallback equal;
} VCAtomicQueueCallback;

VC_OPEN
const VCAtomicQueueCallback kVCTypeAtomicQueueCallback;

VC_OPEN
const VCAtomicQueueCallback kVCCopyStringAtomicQueueCallback;

typedef struct __VCAtomicQueue * VCAtomicQueueRef;

VC_OPEN
VCTypeID VCAtomQueueGetTypeID(void);

VC_OPEN
VCAtomicQueueRef VCAtomicQueueCreate(const VCAtomicQueueCallback *callback);

VC_OPEN
void VCAtomicQueueEnqueue(VCAtomicQueueRef ref,const void *value);

VC_OPEN
const void *VCAtomicQueueDequeue(VCAtomicQueueRef ref);

VC_OPEN
void VCAtomicQueueClear(VCAtomicQueueRef ref);

VC_EXTERN_C_END


#endif /* VCAtomicQueue_h */
