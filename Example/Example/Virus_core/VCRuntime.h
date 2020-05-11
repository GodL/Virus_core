//
//  VCRuntime.h
//  Example
//
//  Created by lihao10 on 2020/5/9.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCRuntime_h
#define VCRuntime_h

#include "VCBase.h"

VC_EXTERN_C_BEGIN

typedef struct __VCRuntimeClass {
    const char *className;
    void (*init)(VCTypeRef vc);
    VCTypeRef (*copy)(VCTypeRef vc);
    VCBool (*equal)(VCTypeRef vc1,VCTypeRef vc2);
    VCHashCode (*hash)(VCTypeRef vc);
    void (*dealloc)(VCTypeRef vc);
} VCRuntimeClass;

typedef struct __VCRuntimeBase {
    volatile unsigned int retainCount;
    uint8_t info[2];
    uintptr_t callback;
} VCRuntimeBase;

VC_OPEN VCTypeRef VCRuntimeCreateInstance(VCTypeID typeID,VCIndex extraSize);

VC_OPEN VCTypeID VCRuntimeRegisterClass(volatile VCTypeID *typeID,VCRuntimeClass runtimeClass);

VC_OPEN VCRuntimeClass VCRuntimeGetClass(VCTypeID typeID);

VC_EXTERN_C_END


#endif /* VCRuntime_h */
