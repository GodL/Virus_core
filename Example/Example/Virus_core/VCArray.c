//
//  VCArray.c
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCArray.h"
#include "VCRuntime.h"

typedef struct __VCArrayImmutable {
    bool zero;
    const void *values[];
} VCArrayImmutable;

typedef struct __VCArrayDeque {
    VCIndex left;
    VCIndex capacity;
    const void **values;
} VCArrayDeque;

typedef struct __VCArray {
    VCRuntimeBase _base;
    VCIndex count;
    void *store;
} VCArray;


enum {
    __VCARRAY_DEQUE_MAX_COUNT = VC_INT_MAX
};

VC_INLINE VCIndex __VCArrayDequeRoundUpCapacity(VCIndex capacity) {
    if (VC_UNLIKELY(capacity < 4)) return 4;
    return VCMIN(capacity << 1, __VCARRAY_DEQUE_MAX_COUNT);
}

VC_INLINE bool __VCArrayIsZero(VCArrayRef arr) {
    return ((VCArrayImmutable *)(arr->store))->zero == 1;
}

VC_INLINE bool __VCArrayGetMutation(VCArrayRef arr) {
    return ((VCRuntimeBase *)arr)->info[0];
}

VC_INLINE const VCArrayCallback *__VCArrayGetCallback(VCArrayRef arr) {
    if (VC_UNLIKELY(arr == NULL)) return NULL;
    return (const VCArrayCallback *)((VCRuntimeBase *)arr)->callback;
}

VC_INLINE VCIndex __VCArrayGetCount(VCArrayRef arr) {
    return arr->count;
}

VC_INLINE const void **__VCArrayGetBucketPtr(VCArrayRef arr) {
    bool mutable = __VCArrayGetMutation(arr);
    return !mutable ? ((VCArrayImmutable *)(arr->store))->values : ((struct __VCArrayDeque *)arr->store)->values;
}

VC_INLINE VCIndex __VCArrayGetRealIndex(VCArrayRef arr,VCIndex index) {
    bool mutable = __VCArrayGetMutation(arr);
    return mutable == 0 ? index : ((struct __VCArrayDeque *)arr->store)->left + __VCArrayGetCount(arr) % __VCArrayGetCount(arr);
}

VC_INLINE const void *__VCArrayGetBucketAtIndex(VCArrayRef arr,VCIndex index) {
    const void **ptr = __VCArrayGetBucketPtr(arr);
    return ptr[__VCArrayGetRealIndex(arr,index)];
}

VC_INLINE bool __VCArrayDequeExpandIfNeed(VCArrayRef arr,VCIndex newCount) {
    assert(__VCArrayGetMutation(arr));
    VCArrayDeque *deque = arr->store;
    if (VC_LIKELY(deque->capacity > newCount)) return false;
    VCIndex newSize = __VCArrayDequeRoundUpCapacity(deque->capacity);
    deque->values = realloc(deque->values, newSize * sizeof(void *));
    VCIndex  moveLength = deque->capacity - deque->left;
    memmove((void *)deque->values[deque->left],(const void *)deque->values[newSize - moveLength],moveLength * sizeof(void *));
    deque->capacity = newSize;
    deque->left = deque->capacity - moveLength;
    return true;
}

VC_INLINE void __VCArrayReleaseRange(VCArrayRef arr,VCRange range) {
    const VCArrayCallback *cb = __VCArrayGetCallback(arr);
    if (!cb || !cb->release) return;
    for (VCIndex i=range.location; i<range.location + range.length; i++) {
        cb->release(__VCArrayGetBucketAtIndex(arr, i));
    }
}

static void __VCArrayDequeMove(VCArrayRef arr,VCRange range,VCIndex newCount) {
    
}

static VCArrayRef __VCArrayCopyWithArray(VCArrayRef arr) {
    VCRuntimeBase *base = (VCRuntimeBase *)arr;
    if (VC_UNLIKELY(base->info[0] == 0)) return arr;
    const void *values[arr->count];
    struct __VCArrayDeque *deque = arr->store;
    for (VCIndex i=0; i<arr->count; i++) {
        values[i] = deque->values + (deque->left + i % arr->count);
    }
    return VCArrayCreate(values, arr->count, (const VCArrayCallback *)arr->_base.callback);
}

static VCMutableArrayRef __VCArrayCopyMutableWithArray(VCArrayRef arr) {
    VCMutableArrayRef result = VCArrayCreateMutable(__VCArrayDequeRoundUpCapacity(__VCArrayGetCount(arr)), __VCArrayGetCallback(arr));
    if (VC_UNLIKELY(result == NULL)) return NULL;
    VCArrayAppendArray(result, arr);
    return result;
}

static bool __VCArrayEqual(VCArrayRef arr,VCArrayRef other) {
    if (VC_UNLIKELY(arr == other)) return true;
    VCIndex cn1 = VCArrayGetCount(arr);
    if (cn1 != VCArrayGetCount(other)) return false;
    const VCArrayCallback *cb1 ,*cb2;
    cb1 = __VCArrayGetCallback(arr);
    cb2 = __VCArrayGetCallback(other);
    if (cb1->equal != cb2->equal) return false;
    if (cn1 == 0) return true;
    for (VCIndex i=0; i<cn1; i++) {
        const void *v1 = __VCArrayGetBucketAtIndex(arr, i);
        const void *v2 = __VCArrayGetBucketAtIndex(other, i);
        if (v1 != v2) {
            if (cb1->equal == NULL) return false;
            if (!cb1->equal(v1,v2)) return false;
        }
    }
    return true;
}

static VCHashCode __VCArrayHashCode(VCTypeRef arr) {
    return __VCArrayGetCount((VCArrayRef)arr);
}

static void __VCArrayDealloc(VCTypeRef vc) {
    VCArrayRef arr = (VCArrayRef)vc;
    if (VC_UNLIKELY(__VCArrayIsZero(arr))) return;
    const VCArrayCallback *cb = __VCArrayGetCallback(arr);
    if (cb && cb->release) {
        for (VCIndex i=0; i<__VCArrayGetCount(arr); i++) {
            const void *value = __VCArrayGetBucketAtIndex(arr, i);
            cb->release(value);
        }
    }
    if (arr->store) free(arr->store);
    free(arr);
    arr = NULL;
}

static const void * __VCArrayStringRetain(const void *value) {
    assert(value);
    return strdup(value);
}

static void __VCArrayStringRelease(const void *value) {
    assert(value);
    free((void *)value);
}

static bool __VCArrayStringEqual(const void *value1,const void *value2) {
    return strcmp(value1, value2) == 0;
}

static const VCRuntimeClass __VCArrayClass = {
    "VCArray",
    NULL,
    (VCTypeRef (*)(VCTypeRef))__VCArrayCopyWithArray,
    (bool (*)(VCTypeRef,VCTypeRef))__VCArrayEqual,
    __VCArrayHashCode,
    __VCArrayDealloc
};

const VCArrayImmutable immutable0 = {
    true,
    NULL
};

const VCArray array0 = {
    {true,{0,0},(uintptr_t)&kVCTypeArrayCallback},
    0,
    (void *)&immutable0
};

const VCArrayCallback kVCTypeArrayCallback = {
    VCRetain,
    VCRelease,
    VCEqual
};

const VCArrayCallback kVCCopyStringArrayCallback = {
    __VCArrayStringRetain,
    __VCArrayStringRelease,
    __VCArrayStringEqual
};

static volatile VCTypeID __kVCArrayType = 0;

VCTypeID VCArrayGetTypeID(void) {
    return VCRuntimeRegisterClass(__kVCArrayType, __VCArrayClass);
}

VCArrayRef VCArrayCreate(const void **values,VCIndex numValues,const VCArrayCallback *callback) {
    if (VC_UNLIKELY(numValues == 0 || values == NULL)) return (VCArrayRef)&array0;
    VCArrayRef ref = (VCArrayRef)VCRuntimeCreateInstance(VCArrayGetTypeID(), sizeof(VCArray) - sizeof(VCRuntimeBase));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    VCRuntimeBase *base = (VCRuntimeBase *)ref;
    base->info[0] = 0;
    VCArrayImmutable *immutable = malloc(sizeof(VCArrayImmutable) + numValues * sizeof(void *));
    if (VC_UNLIKELY(immutable == NULL)) {
        __VCArrayDealloc(ref);
        return NULL;
    }
    base->callback = (uintptr_t)callback;
    immutable->zero = false;
    for (VCIndex i=0; i<numValues; i++) {
        const void *value = *values ++;
        if (callback && callback->retain) value = callback->retain(value);
        immutable->values[i] = value;
    }
    ref->store = immutable;
    ref->count = numValues;
    return ref;
}

VCMutableArrayRef VCArrayCreateMutable(VCIndex capacity,const VCArrayCallback *callback) {
    VCMutableArrayRef ref = (VCArrayRef)VCRuntimeCreateInstance(VCArrayGetTypeID(), sizeof(VCArray) - sizeof(VCRuntimeBase));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
     VCRuntimeBase *base = (VCRuntimeBase *)ref;
    base->info[0] = 1;
    base->info[1] = VCArrayGetTypeID();
    VCArrayDeque *deque = malloc(sizeof(VCArrayDeque));
    if (VC_UNLIKELY(deque == NULL)) {
        __VCArrayDealloc(ref);
    }
    base->callback = (uintptr_t)callback;
    deque->capacity = VCMAX(capacity, 4);
    deque->left = 0;
    deque->values = calloc(deque->capacity, sizeof(void *));
    if (VC_UNLIKELY(deque->values == NULL)) {
        free(deque);
        deque = NULL;
        __VCArrayDealloc(ref);
    }
    ref->store = deque;
    ref->count = 0;
    return ref;
}

VCIndex VCArrayGetCount(VCArrayRef array) {
    if (VC_UNLIKELY(array == NULL)) return 0;
    return __VCArrayGetCount(array);
}

bool VCArrayContainsValue(VCArrayRef array,const void *value) {
    assert(value != NULL);
    if (VC_UNLIKELY(array == NULL || value == NULL)) return false;
    if (VC_UNLIKELY(__VCArrayGetCount(array) == 0)) return false;
    const VCArrayCallback *callback = __VCArrayGetCallback(array);
    for (VCIndex i=0; i<__VCArrayGetCount(array); i++) {
        if (value == __VCArrayGetBucketAtIndex(array, i)) return true;
        if (callback && callback->equal && callback->equal(value,__VCArrayGetBucketAtIndex(array, i))) return true;
    }
    return false;
}

const void *VCArrayGetValueAtIndex(VCArrayRef array,VCIndex index) {
    if (VC_UNLIKELY(array == NULL)) return NULL;
    const void *value = __VCArrayGetBucketAtIndex(array, index);
    return value;
}

VCIndex VCArrayGetFirstIndexOfValue(VCArrayRef array,const void *value) {
    assert(value != NULL);
    if (VC_UNLIKELY(array == NULL || value == NULL)) return kVCNotFound;
    const VCArrayCallback *callback = __VCArrayGetCallback(array);
    for (VCIndex i=0; i<__VCArrayGetCount(array); i++) {
        const void *bucket = __VCArrayGetBucketAtIndex(array, i);
        if (value == bucket) return i;
        if (callback && callback->equal && callback->equal(bucket,value)) return i;
    }
    return kVCNotFound;
}

VCIndex VCArrayGetLastIndexOfValue(VCArrayRef array,const void *value) {
    assert(value != NULL);
    if (VC_UNLIKELY(array == NULL || value == NULL)) return kVCNotFound;
    const VCArrayCallback *callback = __VCArrayGetCallback(array);
    for (VCIndex i=__VCArrayGetCount(array); i>=0; i--) {
        const void *bucket = __VCArrayGetBucketAtIndex(array, i);
        if (value == bucket) return i;
        if (callback && callback->equal && callback->equal(bucket,value)) return i;
    }
    return kVCNotFound;
}

VCArrayRef VCArrayCopyWithArray(VCArrayRef array) {
    if (VC_UNLIKELY(array == NULL)) return NULL;
    return __VCArrayCopyWithArray(array);
}

VCMutableArrayRef VCArrayCopyMutableWithArray(VCArrayRef array) {
    if (VC_UNLIKELY(array == NULL)) return NULL;
    return __VCArrayCopyMutableWithArray(array);
}

void VCArrayApplyFunction(VCArrayRef array,VCArrayApplierFunction applier,void *context) {
    assert(applier);
    if (VC_UNLIKELY(array == NULL || applier == NULL)) return;
    for (VCIndex i=0; i<__VCArrayGetCount(array); i++) {
        const void *value = __VCArrayGetBucketAtIndex(array, i);
        applier(value,context);
    }
}

void VCArrayAppendValue(VCMutableArrayRef array,const void *value) {
    VCArrayInsertValueAtIndex(array, value, __VCArrayGetCount(array));
}

void VCArrayPrependValue(VCMutableArrayRef array,const void *value) {
    VCArrayInsertValueAtIndex(array, value, 0);
}

void VCArrayInsertValueAtIndex(VCMutableArrayRef array,const void *value,VCIndex index) {
    VCArrayReplaceValuesAtRange(array, &value, VCRangeMake(index, 0), 1);
}

VC_OPEN void VCArrayReplaceValueAtIndex(VCMutableArrayRef array,const void *value,VCIndex index) {
    VCArrayReplaceValuesAtRange(array, &value, VCRangeMake(index, 1), 0);
}

void VCArrayReplaceValuesAtRange(VCMutableArrayRef array,const void **values,VCRange range,VCIndex newCount) {
    assert(array);
    assert(__VCArrayGetMutation(array));
    if (VC_UNLIKELY(array == NULL || !__VCArrayGetMutation(array))) return;
    
    VCIndex newLength = array->count - range.length + newCount;
    if (newCount) while (__VCArrayDequeExpandIfNeed(array,newLength))
    
    if (range.length > 0) {
        __VCArrayReleaseRange(array, range);
    }
    VCArrayDeque *deque = array->store;
    const VCArrayCallback *cb = __VCArrayGetCallback(array);
    if (values) {
        for (VCIndex i=range.location; i<range.location+range.length; i++) {
            const void *value = __VCArrayGetBucketAtIndex(array, i);
            if (cb && cb->retain) value = cb->retain(value);
            deque->values[__VCArrayGetRealIndex(array, i)] = value;
        }
    }
    __VCArrayDequeMove(array, range, newCount);
    if (newCount) {
        for (VCIndex i=range.location; i<newCount; i++) {
            const void *value = __VCArrayGetBucketAtIndex(array, i);
            if (cb && cb->retain) value = cb->retain(value);
            deque->values[__VCArrayGetRealIndex(array, i)] = value;
        }
    }
    
    array->count = newLength;
}


void VCArrayRemoveValueAtIndex(VCMutableArrayRef array,VCIndex index) {
    VCArrayReplaceValuesAtRange(array, NULL, VCRangeMake(index, 1), 0);
}

void VCArrayRemoveAllValues(VCMutableArrayRef array) {
    assert(array);
    assert(__VCArrayGetMutation(array));
    if (VC_UNLIKELY(array == NULL || !__VCArrayGetMutation(array))) return;
    const VCArrayCallback *cb = __VCArrayGetCallback(array);
    if (cb && cb->release) __VCArrayReleaseRange(array, VCRangeMake(0, __VCArrayGetCount(array)));
    free(array->store);
    VCArrayDeque *deque = malloc(sizeof(VCArrayDeque));
    if (VC_UNLIKELY(deque == NULL)) {
        __VCArrayDealloc(array);
    }
    deque->capacity = 4;
    deque->left = 0;
    deque->values = calloc(deque->capacity, sizeof(void *));
    if (VC_UNLIKELY(deque->values == NULL)) {
        free(deque);
        deque = NULL;
        __VCArrayDealloc(array);
    }
    array->store = deque;
    array->count = 0;
}

void VCArrayExchangeValues(VCMutableArrayRef array,VCIndex index1,VCIndex index2) {
    assert(array);
    assert(__VCArrayGetMutation(array));
    if (VC_UNLIKELY(array == NULL || !__VCArrayGetMutation(array))) return;
    assert(index1<__VCArrayGetCount(array));
    assert(index2<__VCArrayGetCount(array));
    const void *value1 = __VCArrayGetBucketAtIndex(array, index1);
    const void *value2 = __VCArrayGetBucketAtIndex(array, index2);
    VCArrayDeque *deque = array->store;
    deque->values[__VCArrayGetRealIndex(array, index1)] = value2;
    deque->values[__VCArrayGetRealIndex(array, index2)] = value1;
}

void VCArrayAppendArray(VCMutableArrayRef array,VCArrayRef other) {
    assert(array);
    assert(__VCArrayGetMutation(array));
    if (VC_UNLIKELY(array == NULL || !__VCArrayGetMutation(array))) return;
    for (VCIndex i=0; i<__VCArrayGetCount(other); i++) {
        VCArrayAppendValue(array, __VCArrayGetBucketAtIndex(other, i));
    }
}
