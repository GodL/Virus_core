//
//  VCSqlite3.c
//  Example
//
//  Created by lihao10 on 2020/5/18.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCSqlite3.h"
#include <sqlite3.h>
#include "VCDictionary.h"
#include "VCArray.h"

typedef struct __VCSqlite3 {
    sqlite3 *db;
} VCSqlite3;
