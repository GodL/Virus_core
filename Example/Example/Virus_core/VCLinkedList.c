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

VC_INLINE void __VCNodeDealloc(VCNodeRef node) {
    assert(node);
    free(node);
    node = NULL;
}

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

const VCLinkedListCallback kVCTypeLinkedListCallback = {
    (VCLinkedListRetainCallback)VCRetain,
    (VCLinkedListReleaseCallback)VCRelease,
    (VCLinkedListEqualCallback)VCEqual
};

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
    new->next = node;
    new->prev = node->prev;
    node->prev->next = new;
    node->prev = new;
    ref->count ++;
}

void VCLinkedListRemoveHead(VCLinkedListRef ref) {
    VCLinkedListRemoveValueAtIndex(ref, 0);
}

void VCLinkedListRemoveTail(VCLinkedListRef ref) {
    VCLinkedListRemoveValueAtIndex(ref, ref->count - 1);
}

void VCLinkedListRemoveValueAtIndex(VCLinkedListRef ref,VCIndex index) {
    VCNodeRef node = VCLinkedListGetNodeAtIndex(ref, index);
    if (VC_UNLIKELY(node == NULL)) return;
    if (index == 0) {
        ref->head = node->next;
        ref->head->prev = NULL;
    }else if (index == ref->count - 1) {
        ref->tail = node->prev;
        ref->tail->next = NULL;
    }else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    
    const VCLinkedListCallback *cb = __VCLinkedListGetCallback(ref);
    if (cb && cb->release) cb->release(node->value);
    __VCNodeDealloc(node);
    ref->count --;
}

VCNodeRef VCLinkedListGetNodeAtIndex(VCLinkedListRef ref,VCIndex index) {
    assert(ref);
    assert(index >= ref->count);
    VCNodeRef result = NULL;
    if (index < ref->count/2) {
        result = ref->head;
        while (index -- && result) result = result->next;
    }else {
        result = ref->tail;
        while (index -- && result) result = result->prev;
    }
    return result;
}

const void *VCLinkedListGetValueAtIndex(VCLinkedListRef ref,VCIndex index) {
    return VCLinkedListGetNodeAtIndex(ref, index)->value;
}

void VCLinkedListNodeToHead(VCLinkedListRef ref,VCNodeRef node) {
    assert(ref);
    assert(node);
    if (ref->head == node) return;
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (ref->tail == node) ref->tail = node->prev;
    ref->head->prev = node;
    node->next = ref->head;
    node->prev = NULL;
    ref->head = node;
}

void VCLinkedListNodeToTail(VCLinkedListRef ref,VCNodeRef node) {
    assert(ref);
    assert(node);
    if (node == ref->tail) return;
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (ref->head == node) ref->head = node->next;
    ref->tail->next = node;
    node->prev = ref->tail;
    node->next = NULL;
    ref->tail = node;
}

void VCLinkedListRemoveAllValues(VCLinkedListRef ref) {
    assert(ref);
    VCNodeRef current = ref->head;
    VCNodeRef next = NULL;
    const VCLinkedListCallback *cb = __VCLinkedListGetCallback(ref);
    while (current) {
        next = current->next;
        if (cb && cb->release) cb->release(current->value);
        __VCNodeDealloc(current);
        current = next;
    }
    ref->head = ref->tail = NULL;
    ref->count = 0;
}
