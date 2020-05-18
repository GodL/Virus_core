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
    ref->info[1] = typeID;
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

VCTypeID VCGetTypeID(VCTypeRef vc) {
    assert(vc);
    VCRuntimeBase *base = (VCRuntimeBase *)vc;
    return base->info[1];
}

void VCRetain(VCTypeRef vc) {
    assert(vc);
    VCRuntimeBase *base = (VCRuntimeBase *)vc;
    __sync_fetch_and_add_4(&base->retainCount, 1);
}

void VCRelease(VCTypeRef vc) {
    assert(vc);
    VCRuntimeBase *base = (VCRuntimeBase *)vc;
    int rs = __sync_fetch_and_sub_4(&base->retainCount, 1);
    assert(rs >= 0);
    if (rs == 0) {
        VCRuntimeClass class = VCRuntimeGetClass(VCGetTypeID(vc));
        if (class.dealloc) class.dealloc(vc);
    }
}

VCIndex VCGetRetainCount(VCTypeRef vc) {
    assert(vc);
    VCRuntimeBase *base = (VCRuntimeBase *)vc;
    return base->retainCount;
}

bool VCEqual(VCTypeRef vc1,VCTypeRef vc2) {
    if (vc1 == vc2) return true;
    if (VCGetTypeID(vc1) != VCGetTypeID(vc2)) return false;
    VCRuntimeClass class = VCRuntimeGetClass(VCGetTypeID(vc1));
    if (class.equal) return class.equal(vc1,vc2);
    return false;
}

VCHashCode VCHash(VCTypeRef vc) {
    assert(vc);
    VCRuntimeClass class = VCRuntimeGetClass(VCGetTypeID(vc));
    if (class.hash) return class.hash(vc);
    return (VCIndex)vc;
}
