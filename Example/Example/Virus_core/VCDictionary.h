//
//  VCDictionary.h
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCDictionary_h
#define VCDictionary_h

#include "VCBase.h"

VC_EXTERN_C_BEGIN

typedef const void *(*VCDictionaryRetainCallback)(const void *value);
typedef void (*VCDictionaryReleaseCallback)(const void *value);
typedef VCHashCode (*VCDictionaryHashCallback)(const void *value);
typedef bool (*VCDictionaryEqualCallback)(const void *value1,const void *value2);

typedef struct __VCDictionaryKeyCallback {
    VCDictionaryRetainCallback retain;
    VCDictionaryReleaseCallback release;
    VCDictionaryHashCallback hash;
    VCDictionaryEqualCallback equal;
} VCDictionaryKeyCallback;

VC_OPEN
const VCDictionaryKeyCallback kVCTypeDictionaryKeyCallback;

VC_OPEN
const VCDictionaryKeyCallback kVCCopyStringDictionaryKeyCallback;

typedef struct __VCDictionaryValueCallback {
    VCDictionaryRetainCallback retain;
    VCDictionaryReleaseCallback release;
    VCDictionaryEqualCallback equal;
} VCDictionaryValueCallback;

VC_OPEN
const VCDictionaryValueCallback kVCTypeDictionaryValueCallback;

VC_OPEN
const VCDictionaryValueCallback kVCCopyStringDictionaryValueCallback;

typedef struct __VCDictionary * VCDictionaryRef;

typedef struct __VCDictionary * VCMutableDictionaryRef;

VC_OPEN
VCTypeID VCDictionaryGetTypeID(void);

VC_EXTERN_C_END

#endif /* VCDictionary_h */
