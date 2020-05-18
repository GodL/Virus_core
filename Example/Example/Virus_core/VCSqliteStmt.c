//
//  VCSqliteStmt.c
//  Example
//
//  Created by lihao10 on 2020/5/18.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCSqliteStmt.h"
#include <sqlite3.h>

typedef struct __VCSqliteStmt {
    sqlite3_stmt *stmt;
    
} VCSqliteStmt;
