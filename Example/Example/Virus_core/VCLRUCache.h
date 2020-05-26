//
//  VCLRUCache.h
//  Example
//
//  Created by lihao10 on 2020/5/19.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCLRUCache_h
#define VCLRUCache_h

#include "VCBase.h"

VC_EXTERN_C_BEGIN

typedef const void *(*VCLRUCacheRetainCallback)(const void *value);
typedef void (*VCLRUCacheReleaseCallback)(const void *value);
typedef bool (*VCLRUCacheEqualCallback)(const void *key1,const void *key2);
typedef VCHashCode (*VCLRUCacheHashCallback)(const void *key1);

typedef struct {
    VCLRUCacheRetainCallback retain;
    VCLRUCacheReleaseCallback release;
    VCLRUCacheEqualCallback equal;
    VCLRUCacheHashCallback hash;
} VCLRUCacheKeyCallback;

typedef struct {
    VCLRUCacheRetainCallback retain;
    VCLRUCacheReleaseCallback release;
} VCLRUCacheValueCallback;

VC_OPEN
const VCLRUCacheKeyCallback kVCTypeLRUCacheKeyCallback;

VC_OPEN
const VCLRUCacheKeyCallback kVCCopyStringLRUCacheKeyCallback;

VC_OPEN
const VCLRUCacheValueCallback kVCTypeLRUCacheValueCallback;

typedef struct __VCLRUCache * VCLRUCacheRef;

VC_OPEN
VCTypeID VCLRUCacheGetTypeID(void);

VC_OPEN
VCLRUCacheRef VCLRUCacheCreate(VCIndex countLimit,VCIndex costLimit, const VCLRUCacheKeyCallback *keyCallback,const VCLRUCacheValueCallback *valueCallback);

VC_OPEN
bool VCLRUCacheContainsValue(VCLRUCacheRef ref,const void *key);

VC_OPEN
void VCLRUCacheSetValue(VCLRUCacheRef ref,const void *key,const void *value,VCIndex cost);

VC_OPEN
const void *VCLRUCacheGetValue(VCLRUCacheRef ref,const void *key);

VC_OPEN
void VCLRUCacheRmoveValue(VCLRUCacheRef ref,const void *key);

VC_OPEN
void VCLRUCacheClear(VCLRUCacheRef ref);

VC_EXTERN_C_END

#endif /* VCLRUCache_h */
