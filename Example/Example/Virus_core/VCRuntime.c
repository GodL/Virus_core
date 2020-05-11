//
//  VCRuntime.c
//  Example
//
//  Created by lihao10 on 2020/5/9.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCRuntime.h"
#include <libkern/OSAtomic.h>

#define kVCRuntimeClassTableSize 32

static VCRuntimeClass VCRuntimeClassTable[kVCRuntimeClassTableSize] = {0};
static volatile VCTypeID VCRuntimeClassTableCount = 0;

VCTypeRef VCRuntimeCreateInstance(VCTypeID typeID,VCIndex extraSize) {
    VCRuntimeBase *ref = malloc(sizeof(struct __VCRuntimeBase) + extraSize);
    if (ref == NULL) return NULL;
    ref->retainCount = 1;
    VCRuntimeClass vcClass = VCRuntimeGetClass(typeID);
    if (vcClass.init) vcClass.init(ref);
    return ref;
}

VCTypeID VCRuntimeRegisterClass(volatile VCTypeID *typeID,VCRuntimeClass runtimeClass) {
    if (*typeID != 0) return *typeID;
    VCTypeID new = __sync_fetch_and_add_4(&VCRuntimeClassTableCount,1);
    if (new >= kVCRuntimeClassTableSize) return kVCNotSupport;
    if (__sync_bool_compare_and_swap(&typeID,&new,NULL)) {
        VCRuntimeClassTable[new] = runtimeClass;
    }else {
        new = __sync_fetch_and_sub_4(&VCRuntimeClassTableCount, 1);
    }
    return new;
}

VCRuntimeClass VCRuntimeGetClass(VCTypeID typeID) {
    return VCRuntimeClassTable[typeID];
}

