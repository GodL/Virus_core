//
//  VCDictionary.c
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCDictionary.h"
#include "VCRuntime.h"

typedef struct {
    VCIndex hash;
    const void *key;
    const void *value;
} VCDictionaryNode;

typedef struct __VCDictionary {
    VCRuntimeBase *base;
    VCIndex count;
    VCIndex bucketCount;
    const VCDictionaryValueCallback *valueCallback;
    const void *bucket[];
} VCDictionary;

VC_INLINE VCHashCode __VCDictionaryStringHash(const void *value) {
    VCIndex p = 16777619;
    VCHashCode hash = (VCHashCode)value;
    char str[128];
    sprintf(str, "%ld",hash);
    for(VCIndex i=0;i<strlen(str);i++) {
        hash = (hash ^ str[i]) * p;
    }
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash;
}

static VCDictionaryRef __VCDictionary

static const VCRuntimeClass __VCDictionaryClass = {
    "VCDictionary",
    NULL,
}

static VCTypeID *__VCDictionaryTypeID = NULL;

VCTypeID VCDictionaryGetTypeID(void) {
    return VCRuntimeRegisterClass(__VCDictionaryTypeID, <#VCRuntimeClass runtimeClass#>)
}

