//
//  VCBlockingQueue.h
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCBlockingQueue_h
#define VCBlockingQueue_h

#include "VCBase.h"

VC_EXTERN_C_BEGIN

typedef const void *(*VCBlockingQueueRetainCallBack)(const void *value);
typedef void (*VCBlockingQueueReleaseCallBack)(const void *value);

typedef struct {
    VCBlockingQueueRetainCallBack retain;
    VCBlockingQueueReleaseCallBack release;
} VCBlockingQueueCallBack;

VC_OPEN
const VCBlockingQueueCallBack kVCTypeBlockingQueueCallBack;

VC_OPEN
const VCBlockingQueueCallBack kVCCopyStringBlockingQueueCallBack;

typedef struct __VCBlockingQueue * VCBlockingQueueRef;

VC_OPEN
VCTypeID VCBlockingQueueGetTypeID(void);

VC_OPEN
VCBlockingQueueRef VCBlockingQueueCreate(const VCBlockingQueueCallBack *callback);

VC_OPEN
VCIndex VCBlockingQueueGetCount(VCBlockingQueueRef ref);

VC_OPEN
void VCBlockingQueueEnqueue(VCBlockingQueueRef ref,const void *value);

VC_OPEN
const void *VCBlockingQueueDequeue(VCBlockingQueueRef ref);

VC_OPEN
void VCBlockingQueueClear(VCBlockingQueueRef ref);

VC_EXTERN_C_END

#endif /* VCBlockingQueue_h */
