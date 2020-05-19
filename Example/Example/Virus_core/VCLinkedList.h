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

typedef const void *(*VCLinkedListNodeRetainCallback)(const void *value);
typedef void (*VCLinkedListNodeReleaseCallback)(const void *value);
typedef bool (*VCLinkedListNodeEqualCallback)(const void *value1,const void *value2);

typedef struct __VCLinkedListNodeCallback {
    VCLinkedListNodeRetainCallback retain;
    VCLinkedListNodeReleaseCallback release;
    VCLinkedListNodeEqualCallback equal;
} VCLinkedListNodeCallback;

VC_OPEN const VCLinkedListNodeCallback kVCTypeLinkedListNodeCallback;

typedef struct __VCNode {
    struct __VCNode *next;
    struct __VCNode *prev;
    const void *value;
} VCNode;

typedef VCNode * VCNodeRef;

typedef struct __VCLinkedList * VCLinkedListRef;

VC_EXTERN_C_END

#endif /* VCLinkedList_h */
