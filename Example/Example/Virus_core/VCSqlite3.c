//
//  VCSqlite3.c
//  Example
//
//  Created by lihao10 on 2020/5/18.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCSqlite3.h"
#include "VCRuntime.h"
#include <sqlite3.h>
#include "VCLRUCache.h"
#include "VCArray.h"

typedef struct __VCSqlite3 {
    VCRuntimeBase *base;
    sqlite3 *db;
    const char *dbPath;
    VCLRUCacheRef stmtCache;
} VCSqlite3;

static void __VCSqlite3Init(VCSqlite3Ref ref) {
    assert(ref);
    assert(ref->dbPath);
    if (VC_UNLIKELY(!ref->dbPath)) return;
    if (sqlite3_open(ref->dbPath, &ref->db) != SQLITE_OK) {
        printf("sqlite open failed on %s errcode %d  errmsg %s\n",ref->dbPath,sqlite3_errcode(ref->db),sqlite3_errmsg(ref->db));
        sqlite3_close(ref->db);
        return;
    }
    ref->base->info[0] = true;
}

static VCHashCode __VCSqlite3Hash(VCTypeRef ref) {
    VCSqlite3Ref sqlite3 = (VCSqlite3Ref)ref;
    return (VCHashCode)sqlite3->db;
}

static void __VCSqlite3Dealloc(VCTypeRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return;
    VCSqlite3Ref sqlite3 = (VCSqlite3Ref)ref;
    VCSqlite3Close(sqlite3);
    VCRelease(sqlite3->stmtCache);
    free((void *)sqlite3->dbPath);
    free(sqlite3);
    sqlite3 = NULL;
}

static const VCRuntimeClass __VCSqlite3Class = {
    "VCSqlite3",
    NULL,
    NULL,
    NULL,
    __VCSqlite3Hash,
    __VCSqlite3Dealloc
};

static VCTypeID *__VCSqlite3TypeID = NULL;

VCTypeID VCSqlite3GetTypeID(void) {
    return VCRuntimeRegisterClass(__VCSqlite3TypeID, __VCSqlite3Class);
}

bool VCSqlite3Close(VCSqlite3Ref ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return false;
    
    return false;
}
