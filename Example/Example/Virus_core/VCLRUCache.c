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


typedef struct __VCLRUCache {
    VCIndex totalCount;
    VCFloat totalCost;
    VCIndex countLimit;
    VCFloat costLimit;
    VCLinkedListRef list;
    VCDictionaryRef hash;
} VCLRUCache;
