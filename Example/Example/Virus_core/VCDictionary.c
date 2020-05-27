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
    const void *bucket;
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

static VCTypeRef __VCDictionaryCopy(VCTypeRef ref) {
    return NULL;
}

static bool __VCDictionaryEqual(VCTypeRef ref1,VCTypeRef ref2) {
    return false;
}

static void __VCDictionaryDealloc(VCTypeRef ref) {
    
}

static const VCRuntimeClass __VCDictionaryClass = {
    "VCDictionary",
    NULL,
    __VCDictionaryCopy,
    __VCDictionaryEqual,
    VCHash,
    __VCDictionaryDealloc
};

static const VCIndex __VCDictionaryBucketSizes[64] = {
    0, 3, 7, 13, 23, 41, 71, 127, 191, 251, 383, 631, 1087, 1723,
    2803, 4523, 7351, 11959, 19447, 31231, 50683, 81919, 132607,
    214519, 346607, 561109, 907759, 1468927, 2376191, 3845119,
    6221311, 10066421, 16287743, 26354171, 42641881, 68996069,
    111638519, 180634607, 292272623, 472907251,
    765180413UL, 1238087663UL, 2003267557UL, 3241355263UL, 5244622819UL,
    8485977589UL, 13730600407UL, 22216578047UL, 35947178479UL,
    58163756537UL, 94110934997UL, 152274691561UL, 246385626107UL,
    398660317687UL, 645045943807UL, 1043706260983UL, 1688752204787UL,
    2732458465769UL, 4421210670577UL, 7153669136377UL,
    11574879807461UL, 18728548943849UL, 30303428750843UL
};

VC_INLINE VCIndex __VCDictionaryGetNextBucketSize(VCIndex size) {
    VCIndex i = 0;
    while (size < __VCDictionaryBucketSizes[i ++]);
    return __VCDictionaryBucketSizes[i - 1];
}

static VCTypeID *__VCDictionaryTypeID = NULL;

VCTypeID VCDictionaryGetTypeID(void) {
    return VCRuntimeRegisterClass(__VCDictionaryTypeID, __VCDictionaryClass);
}

