//
//  VCLRUCache.c
//  Example
//
//  Created by lihao10 on 2020/5/19.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCLRUCache.h"
#include "VCRuntime.h"
#include "VCDictionary.h"
#include "VCLinkedList.h"

typedef struct {
    const void *key;
    const void *value;
    VCIndex cost;
} VCCacheItem;

typedef VCCacheItem * VCCacheItemRef;

typedef struct __VCLRUCache {
    VCRuntimeBase *base;
    VCIndex totalCount;
    VCFloat totalCost;
    VCIndex countLimit;
    VCFloat costLimit;
    VCLinkedListRef list;
    VCMutableDictionaryRef hash;
    const VCLRUCacheValueCallback *valueCallback;
} VCLRUCache;

static void __VCLRUCacheDealloc(VCTypeRef ref) {
    assert(ref);
    VCLRUCacheRef cache = (VCLRUCacheRef)ref;
    VCLRUCacheClear(cache);
    if (cache->hash) {
        VCRelease(cache->hash);
    }
    if (cache->list) {
        VCRelease(cache->list);
    }
    free(cache);
    cache = NULL;
}

VC_INLINE void __VCLRUCacheListRelease(const void *value) {
    VCCacheItemRef cache = (VCCacheItemRef)value;
    free(cache);
    cache = NULL;
}

VC_INLINE const VCLRUCacheKeyCallback *__VCLRUCacheGetKeyCallback (VCLRUCacheRef ref) {
    assert(ref);
    return (const VCLRUCacheKeyCallback *)ref->base->callback;
}

VC_INLINE const void *__VCLRUCacheGetKey(VCLRUCacheRef ref,const void *key) {
    const VCLRUCacheKeyCallback *cb = __VCLRUCacheGetKeyCallback(ref);
    if (cb && cb->retain) key = cb->retain(key);
    return key;
}

VC_INLINE const void *__VCLRUCacheGetValue(VCLRUCacheRef ref,const void *value) {
    const VCLRUCacheValueCallback *cb = ref->valueCallback;
    if (cb && cb->retain) value = cb->retain(value);
    return value;
}

static void __VCLRUCacheTrimValue(VCLRUCacheRef ref,VCNodeRef node) {
    VCCacheItemRef item = (VCCacheItemRef)node->value;
    const VCLRUCacheValueCallback *vcb = ref->valueCallback;
    ref->totalCount --;
    ref->totalCost -= item->cost;
    if (vcb && vcb->release) vcb->release(item->value);
    VCDictionaryRemoveValue(ref->hash, item->key);
    VCLinkedListRemoveNode(ref->list, node);
}

static void __VCLRUCacheTrimTail(VCLRUCacheRef ref) {
    assert(ref);
    if (ref->totalCount == 0) return;
    VCNodeRef node = VCLinkedListGetNodeAtIndex(ref->list, VCLinkedListGetCount(ref->list)-1);
    __VCLRUCacheTrimValue(ref, node);
}

const VCRuntimeClass __VCLRUCacheRuntime = {
    "VCLRUCache",
    NULL,
    NULL,
    NULL,
    NULL,
    __VCLRUCacheDealloc
};

static VCTypeID *__VCLRUCacheTypeID = NULL;

VCTypeID VCLRUCacheGetTypeID(void) {
    return VCRuntimeRegisterClass(__VCLRUCacheTypeID, __VCLRUCacheRuntime);
}

VCLRUCacheRef VCLRUCacheCreate(VCIndex countLimit,VCIndex costLimit, const VCLRUCacheKeyCallback *keyCallback,const VCLRUCacheValueCallback *valueCallback) {
    VCLRUCacheRef ref = (VCLRUCacheRef)VCRuntimeCreateInstance(VCLRUCacheGetTypeID(), sizeof(VCLRUCache) - sizeof(VCRuntimeClass));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    ref->base->callback = (uintptr_t)keyCallback;
    ref->valueCallback = valueCallback;
    VCLinkedListCallback listCallback = {
        NULL,
        __VCLRUCacheListRelease,
        NULL
    };
    VCLinkedListRef list = VCLinkedListCreate(&listCallback);
    if (list == NULL) {
        VCRelease(ref);
        return NULL;
    }
    ref->list = list;
    VCDictionaryKeyCallback hashKeyCallback = {
        NULL,
        NULL,
        keyCallback ? keyCallback->hash : NULL,
        keyCallback ? keyCallback->equal : NULL
    };
    VCMutableDictionaryRef hash = VCDictionaryCreateMutable(16,&hashKeyCallback, NULL);
    if (hash == NULL) {
        VCRelease(ref);
        return NULL;
    }
    ref->hash = hash;
    ref->costLimit = costLimit;
    ref->totalCost = 0;
    ref->countLimit = countLimit;
    ref->totalCount = 0;
    return ref;
}

bool VCLRUCacheContainsValue(VCLRUCacheRef ref,const void *key) {
    assert(ref);
    assert(key);
    return VCDictionaryContainsKey(ref->hash, key);
}

void VCLRUCacheSetValue(VCLRUCacheRef ref,const void *key,const void *value,VCIndex cost) {
    assert(ref);
    assert(key);
    assert(value);
    VCNodeRef node = (VCNodeRef)VCDictionaryGetValue(ref->hash, key);
    const VCLRUCacheValueCallback *vcb = ref->valueCallback;
    if (node) {
        VCCacheItemRef item = (VCCacheItemRef)node->value;
        if (vcb && vcb->release) vcb->release(item->value);
        item->value = __VCLRUCacheGetValue(ref, value);
        ref->totalCost -= item->cost;
        item->cost = cost;
        ref->totalCost += cost;
        VCLinkedListNodeToHead(ref->list, node);
    }else {
        VCCacheItemRef item = calloc(1, sizeof(VCCacheItem));
        item->key = __VCLRUCacheGetKey(ref, key);
        item->value = __VCLRUCacheGetValue(ref, value);
        item->cost = cost;
        VCLinkedListAddHead(ref->list, item);
        VCDictionarySetValue(ref->hash, item->key, VCLinkedListGetNodeAtIndex(ref->list, 0));
        ref->totalCount ++;
        ref->totalCost += cost;
    }
    if (ref->totalCount > ref->countLimit) {
        __VCLRUCacheTrimTail(ref);
    }
    while (ref->totalCost + cost > ref->costLimit && VCDictionaryGetCount(ref->hash) > 0) {
        __VCLRUCacheTrimTail(ref);
    }
}

const void *VCLRUCacheGetValue(VCLRUCacheRef ref,const void *key) {
    assert(ref);
    assert(key);
    VCNodeRef node = (VCNodeRef)VCDictionaryGetValue(ref->hash, key);
    if (node == NULL) return NULL;
    VCLinkedListNodeToHead(ref->list, node);
    VCCacheItemRef cache = (VCCacheItemRef)node->value;
    return cache->value;
}

void VCLRUCacheRmoveValue(VCLRUCacheRef ref,const void *key) {
    assert(ref);
    assert(key);
    VCNodeRef node = (VCNodeRef)VCDictionaryGetValue(ref->hash, key);
    if (node) __VCLRUCacheTrimValue(ref, node);
}

void VCLRUCacheClear(VCLRUCacheRef ref) {
    assert(ref);
    if (ref->totalCount == 0) return;
    while (ref->totalCount) {
        __VCLRUCacheTrimTail(ref);
    }
}

