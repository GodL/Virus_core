//
//  VCArray.h
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCArray_h
#define VCArray_h

#include "VCBase.h"

typedef struct __VCArray * VCArrayRef;

typedef struct __VCArray * VCMutableArrayRef;

typedef const void *(*VCArrayRetainCallback)(const void *value);
typedef void (*VCArrayReleaseCallback)(const void *value);
typedef VCBool (*VCArrayEqualCallback)(const void *value1,const void *value2);
typedef void (*VCArrayDeallocCllback)(const void *value);

typedef struct __VCArrayCallback {
    VCArrayRetainCallback retain;
    VCArrayReleaseCallback release;
    VCArrayEqualCallback equal;
    VCArrayDeallocCllback dealloc;
} VCArrayCallback;

VC_OPEN VCTypeID VCArrayGetTypeID(void);

VC_OPEN VCArrayRef VCArrayCreate(const void **values,VCIndex numValues,const VCArrayCallback *callback);

VC_OPEN VCIndex VCArrayGetCount(VCArrayRef array);

VC_OPEN const void *VCArrayGetValueAtIndex(VCArrayRef array,VCIndex index);

VC_OPEN VCArrayRef VCArrayCopyWithArray(VCArrayRef array);

VC_OPEN VCArrayRef VCArrayCopyMutableWithArray(VCArrayRef array);

#endif /* VCArray_h */
