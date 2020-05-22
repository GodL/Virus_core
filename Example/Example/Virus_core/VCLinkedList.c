//
//  VCLinkedList.c
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCLinkedList.h"
#include "VCRuntime.h"

typedef struct __VCLinkedList {
    VCRuntimeBase *base;
    VCNode *head;
    VCNode *tail;
    VCIndex count;
} VCLinedList;

static void __VCLinkedListInit(VCTypeRef ref) {
    VCLinkedListRef linkedList = (VCLinkedListRef)ref;
    linkedList->head = linkedList->tail = NULL;
    linkedList->count = 0;
}

static bool __VCLinedListEqual(VCTypeRef ref1,VCTypeRef ref2) {
    if (ref1 == ref2) return true;
    VCLinkedListRef list1 = (VCLinkedListRef)ref1;
    VCLinkedListRef list2 = (VCLinkedListRef)ref2;
    if (list1->count != list2->count) return false;
    return list1->head == list2->head && list1->tail == list2->tail;
}

static VCHashCode __VCLinedListHash(VCTypeRef ref) {
    return (VCHashCode)ref;
}

static void __VCLinedListDealloc(VCTypeRef ref) {
    VCLinkedListRef list = (VCLinkedListRef)ref;
    VCLinkedListRemoveAllValues(list);
    free(list);
    list = NULL;
}

VC_INLINE const VCLinkedListCallback * __VCLinkedListGetCallback(VCLinkedListRef ref) {
    assert(ref);
    VCRuntimeBase *base = ref->base;
    return (const VCLinkedListCallback *)base->callback;
}

VC_INLINE VCNodeRef __VCLinkedListGetNodeAtIndex(VCLinkedListRef ref,VCIndex index) {
    return NULL;
}

static const VCRuntimeClass __VCLinkedListClass = {
    "VCLinkedList",
    __VCLinkedListInit,
    NULL,
    __VCLinedListEqual,
    __VCLinedListHash,
    __VCLinedListDealloc
};

static VCTypeID *__VCLinkedLisTypeID = NULL;

VC_INLINE VCNodeRef VCNodeCreate(const void *value) {
    VCNodeRef ref = calloc(1, sizeof(VCNode));
    if (ref == NULL) return NULL;
    ref->value = value;
    ref->next = ref->prev = NULL;
    return ref;
}

VCTypeID VCLinkedListGetTypeID(void) {
    return VCRuntimeRegisterClass(__VCLinkedLisTypeID, __VCLinkedListClass);
}

VCLinkedListRef VCLinkedListCreate(const VCLinkedListCallback *callback) {
    VCLinkedListRef ref = (VCLinkedListRef)VCRuntimeCreateInstance(VCLinkedListGetTypeID(), sizeof(VCLinedList) - sizeof(VCRuntimeBase));
    if (ref == NULL) return NULL;
    VCRuntimeBase *base = ref->base;
    base->callback = (uintptr_t)callback;
    return ref;
}

void VCLinkedListAddHead(VCLinkedListRef ref,const void *value) {
    assert(ref);
    assert(value);
    if (VC_UNLIKELY(ref == NULL || value == NULL)) return;
    VCNodeRef new;
    if ((new = VCNodeCreate(NULL)) == NULL) return;
    const VCLinkedListCallback *cb = __VCLinkedListGetCallback(ref);
    if (cb && cb->retain) value = cb->retain(value);
    new->value = value;
    if (ref->count == 0) {
        ref->head = ref->tail = new;
    }else {
        new->next = ref->head;
        ref->head->prev = new;
        ref->head = new;
    }
    ref->count ++;
}

void VCLinkedListAddTail(VCLinkedListRef ref,const void *value) {
    assert(ref);
    assert(value);
    if (VC_UNLIKELY(ref == NULL || value == NULL)) return;
    VCNodeRef new;
    if ((new = VCNodeCreate(NULL)) == NULL) return;
    const VCLinkedListCallback *cb = __VCLinkedListGetCallback(ref);
    if (cb && cb->retain) value = cb->retain(value);
    new->value = value;
    if (ref->count == 0) {
        ref->head = ref->tail = new;
    }else {
        new->prev = ref->tail;
        ref->tail->next = new;
        ref->tail = new;
    }
    ref->count ++;
}

void VCLinkedListInsertValueAtIndex(VCLinkedListRef ref,const void *value,VCIndex index) {
    assert(ref);
    assert(value);
    assert(index >= ref->count);
    if (VC_UNLIKELY(ref == NULL || value == NULL)) return;
    if (ref->count == 0) {
        VCLinkedListAddHead(ref, value);
        return;
    }
    VCNodeRef new;
    if ((new = VCNodeCreate(NULL)) == NULL) return;
    const VCLinkedListCallback *cb = __VCLinkedListGetCallback(ref);
    if (cb && cb->retain) value = cb->retain(value);
    new->value = value;
    VCNodeRef node = __VCLinkedListGetNodeAtIndex(ref,index);
    
}
