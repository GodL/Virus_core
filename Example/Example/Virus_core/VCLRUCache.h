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
typedef void (*VCLRUCacheEqualCallback)(const void *value1,const void *value2);

typedef struct __VCLRUCacheCallback {
    VCLRUCacheRetainCallback retain;
    VCLRUCacheReleaseCallback release;
    VCLRUCacheEqualCallback equal;
} VCLRUCacheCallback;

VC_OPEN const VCLRUCacheCallback kVCTypeLRUCacheCallback;

typedef struct __VCLRUCache * VCLRUCacheRef;

VC_EXTERN_C_END

#endif /* VCLRUCache_h */
