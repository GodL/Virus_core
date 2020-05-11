//
//  VCArray.c
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCArray.h"
#include "VCRuntime.h"

struct __VCArrayImmutable {
    const void **values;
};

struct __VCArrayDeque {
    VCIndex left;
    VCIndex right;
    const void **values;
};

typedef struct __VCArray {
    VCRuntimeBase _base;
    VCIndex count;
    void *store;
} VCArray;


enum {
    __VCARRAY_DEQUE_MAX_COUNT = VC_INT_MAX
};

const struct __VCArrayImmutable array0 = {
    NULL
};

VC_INLINE VCIndex __VCArrayDequeRoundUpCapacity(VCIndex capacity) {
    if (capacity < 4) return 4;
    return VCMIN(1 << capacity, __VCARRAY_DEQUE_MAX_COUNT);
}

VCArrayRef __VCArrayCopyWithArray(VCArrayRef arr) {
    if (arr == NULL) return NULL;
    VCRuntimeBase *base = (VCRuntimeBase *)arr;
    if (base->info[0] == 0) {
        return arr;
    }
    return NULL;
}

static VCTypeID *__kVCArrayType = NULL;

static const VCRuntimeClass __VCArrayClass = {
    "VCArray",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

VCTypeID VCArrayGetTypeID(void) {
    return VCRuntimeRegisterClass(__kVCArrayType, __VCArrayClass);
}


