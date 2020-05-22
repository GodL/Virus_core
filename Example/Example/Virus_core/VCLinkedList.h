//
//  VCLinkedList.h
//  Example
//
//  Created by lihao10 on 2020/5/11.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCLinkedList_h
#define VCLinkedList_h

#include "VCBase.h"

VC_EXTERN_C_BEGIN

typedef const void *(*VCLinkedListRetainCallback)(const void *value);
typedef void (*VCLinkedListReleaseCallback)(const void *value);
typedef bool (*VCLinkedListEqualCallback)(const void *value1,const void *value2);

typedef struct __VCLinkedListCallback {
    VCLinkedListRetainCallback retain;
    VCLinkedListReleaseCallback release;
    VCLinkedListEqualCallback equal;
} VCLinkedListCallback;

VC_OPEN const VCLinkedListCallback kVCTypeLinkedListCallback;

typedef struct __VCNode {
    struct __VCNode *next;
    struct __VCNode *prev;
    const void *value;
} VCNode;

typedef VCNode * VCNodeRef;

typedef struct __VCLinkedList * VCLinkedListRef;

VC_OPEN
VCTypeID VCLinkedListGetTypeID(void);

VC_OPEN
VCLinkedListRef VCLinkedListCreate(const VCLinkedListCallback *callback);

VC_OPEN
void VCLinkedListAddHead(VCLinkedListRef ref,const void *value);

VC_OPEN
void VCLinkedListAddTail(VCLinkedListRef ref,const void *value);

VC_OPEN
void VCLinkedListInsertValueAtIndex(VCLinkedListRef ref,const void *value,VCIndex index);

VC_OPEN
void VCLinkedListRemoveHead(VCLinkedListRef ref);

VC_OPEN
void VCLinkedListRemoveTail(VCLinkedListRef ref);

VC_OPEN
void VCLinkedListRemoveValueAtIndex(VCLinkedListRef ref,VCIndex index);

VC_OPEN
const VCNodeRef VCLinkedListGetNodeAtIndex(VCLinkedListRef ref,VCIndex index);

VC_OPEN
const void *VCLinkedListGetValueAtIndex(VCLinkedListRef ref,VCIndex index);

VC_OPEN
void VCLinkedListNodeToHead(VCLinkedListRef ref,VCNodeRef node);

VC_OPEN
void VCLinkedListNodeToTail(VCLinkedListRef ref,VCNodeRef node);

VC_OPEN
void VCLinkedListRemoveAllValues(VCLinkedListRef ref);

VC_EXTERN_C_END

#endif /* VCLinkedList_h */
