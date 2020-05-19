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

typedef void (*VCArrayApplierFunction)(const void *value, void *context);

typedef const void *(*VCArrayRetainCallback)(const void *value);
typedef void (*VCArrayReleaseCallback)(const void *value);
typedef bool (*VCArrayEqualCallback)(const void *value1,const void *value2);

typedef struct __VCArrayCallback {
    VCArrayRetainCallback retain;
    VCArrayReleaseCallback release;
    VCArrayEqualCallback equal;
} VCArrayCallback;

VC_OPEN
const VCArrayCallback kVCTypeArrayCallback;

VC_OPEN
const VCArrayCallback kVCCopyStringArrayCallback;

VC_OPEN VCTypeID VCArrayGetTypeID(void);

VC_OPEN VCArrayRef VCArrayCreate(const void **values,VCIndex numValues,const VCArrayCallback *callback);

VC_OPEN VCMutableArrayRef VCArrayCreateMutable(VCIndex capacity,const VCArrayCallback *callback);

VC_OPEN VCIndex VCArrayGetCount(VCArrayRef array);

VC_OPEN bool VCArrayContainsValue(VCArrayRef array,const void *value);

VC_OPEN const void *VCArrayGetValueAtIndex(VCArrayRef array,VCIndex index);

VC_OPEN VCIndex VCArrayGetFirstIndexOfValue(VCArrayRef array,const void *value);

VC_OPEN VCIndex VCArrayGetLastIndexOfValue(VCArrayRef array,const void *value);

VC_OPEN VCArrayRef VCArrayCopyWithArray(VCArrayRef array);

VC_OPEN VCMutableArrayRef VCArrayCopyMutableWithArray(VCArrayRef array);

VC_OPEN void VCArrayApplyFunction(VCArrayRef array,VCArrayApplierFunction applier,void *context);

VC_OPEN void VCArrayAppendValue(VCMutableArrayRef array,const void *value);

VC_OPEN void VCArrayPrependValue(VCMutableArrayRef array,const void *value);

VC_OPEN void VCArrayInsertValueAtIndex(VCMutableArrayRef array,const void *value,VCIndex index);

VC_OPEN void VCArrayReplaceValueAtIndex(VCMutableArrayRef array,const void *value,VCIndex index);

VC_OPEN void VCArrayReplaceValuesAtRange(VCMutableArrayRef array,const void **values,VCRange range,VCIndex newCount);

VC_OPEN void VCArrayRemoveValueAtIndex(VCMutableArrayRef array,VCIndex index);

VC_OPEN void VCArrayRemoveAllValues(VCMutableArrayRef array);

VC_OPEN void VCArrayExchangeValues(VCMutableArrayRef array,VCIndex index1,VCIndex index2);

VC_OPEN void VCArrayAppendArray(VCMutableArrayRef array,VCArrayRef other);

#endif /* VCArray_h */
