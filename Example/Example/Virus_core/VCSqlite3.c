//
//  VCSqlite3.c
//  Example
//
//  Created by lihao10 on 2020/5/18.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCSqlite3.h"
#include "VCRuntime.h"
#include "VCLRUCache.h"
#include "VCLinkedList.h"

typedef struct __VCSqlite3 {
    VCRuntimeBase base;
    sqlite3 *db;
    const char *dbPath;
    VCLRUCacheRef stmtCache;
} VCSqlite3;

VC_INLINE VCSqlite3StmtRef VCSqlite3StmtCreate(sqlite3_stmt *stmt) {
    VCSqlite3StmtRef ref = calloc(1, sizeof(VCSqlite3Stmt));
    ref->used = false;
    ref->fromCache = false;
    ref->stmt = stmt;
    return ref;
}

static void __VCSqlite3Dealloc(VCTypeRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return;
    VCSqlite3Ref sqlite3 = (VCSqlite3Ref)ref;
    VCRelease(sqlite3->stmtCache);
    VCSqlite3Close(sqlite3);
    free((void *)sqlite3->dbPath);
    free(sqlite3);
    sqlite3 = NULL;
}

VC_INLINE const void *__VCSqlite3StmtRetainCallBack(const void *stmt) {
    if (stmt) {
        VCSqlite3StmtRef ref = (VCSqlite3StmtRef)stmt;
        sqlite3_reset(ref->stmt);
    }
    return stmt;
}

VC_INLINE void __VCSqlite3StmtReleaseCallBack(const void *stmt) {
    if (stmt) {
        VCSqlite3StmtRef ref = (VCSqlite3StmtRef)stmt;
        sqlite3_finalize(ref->stmt);
        free(ref);
        ref = NULL;
    }
}

VC_INLINE VCSqlite3StmtRef __VCSqlite3GetStmtFromCacheItem(VCLinkedListRef list) {
    assert(list);
    VCNodeRef node = (VCNodeRef)VCLinkedListGetNodeAtIndex(list, VCLinkedListGetCount(list) - 1);
    if (node == NULL) return NULL;
    VCSqlite3StmtRef stmt = (VCSqlite3StmtRef)node->value;
    if (stmt == NULL || stmt->used) return NULL;
    stmt->used = true;
    VCLinkedListNodeToHead(list, node);
    return stmt;
}

VC_INLINE void __VCSqlite3PrintfError(VCSqlite3Ref ref,char *from) {
    printf("%s on %s errcode %d errmsg %s",from,ref->dbPath,sqlite3_errcode(ref->db) ,sqlite3_errmsg(ref->db));
}

VC_INLINE bool __VCSqlite3Open(VCSqlite3Ref ref) {
    if (sqlite3_open(ref->dbPath, &ref->db) != SQLITE_OK) {
        __VCSqlite3PrintfError(ref,"sqlite open failed");
        sqlite3_close(ref->db);
        return false;
    }
    ref->base.info[0] = true;
    return true;
}

VC_INLINE void __VCSqlite3OpenIfNeed(VCSqlite3Ref ref) {
    if (!VCSqlite3IsOpen(ref)) {
        VCSqlite3Open(ref);
    }
}

VC_INLINE sqlite3_stmt *__VCSqlite3PrepartSql(VCSqlite3Ref ref,const char *sql) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(ref->db, sql, 0, &stmt, NULL) != SQLITE_OK) {
        __VCSqlite3PrintfError(ref,"stmt prepare failed");
        sqlite3_finalize(stmt);
        return NULL;
    }
    return stmt;
}

VC_INLINE VCSqlite3StmtRef __VCSqlite3CreateStmtToCache(VCSqlite3Ref ref,const char *sql) {
    sqlite3_stmt *stmt = __VCSqlite3PrepartSql(ref, sql);
    if (VC_UNLIKELY(stmt == NULL)) return NULL;
    VCSqlite3StmtRef sqlite3Stmt = VCSqlite3StmtCreate(stmt);
    sqlite3Stmt->fromCache = true;
    VCLinkedListRef cacheItem = (VCLinkedListRef)VCLRUCacheGetValue(ref->stmtCache, sql);
    if (VC_UNLIKELY(cacheItem == NULL)) {
        printf("funcation called error");
        return NULL;
    }
    VCLinkedListAddHead(cacheItem, sqlite3Stmt);
    sqlite3Stmt->used = true;
    return sqlite3Stmt;
}

VC_INLINE VCSqlite3StmtRef __VCSqlite3GetStmtFromCache(VCSqlite3Ref ref,const char *sql) {
    VCLinkedListRef cacheItem = (VCLinkedListRef)VCLRUCacheGetValue(ref->stmtCache, sql);
    if (cacheItem == NULL) {
        VCLinkedListCallback callback = {
            __VCSqlite3StmtRetainCallBack,
            __VCSqlite3StmtReleaseCallBack,
            NULL
        };
        cacheItem = VCLinkedListCreate(&callback);
        if (cacheItem) {
            VCLRUCacheSetValue(ref->stmtCache, sql, cacheItem, 0);
            VCRelease(cacheItem);
        }
    }
    return __VCSqlite3GetStmtFromCacheItem(cacheItem);
}

VC_INLINE VCSqlite3StmtRef __VCSqlitePrepareStmt(VCSqlite3Ref ref,const char *sql) {
    VCSqlite3StmtRef stmt = __VCSqlite3GetStmtFromCache(ref, sql);
    if (stmt == NULL) {
        VCLinkedListRef cacheItem = (VCLinkedListRef)VCLRUCacheGetValue(ref->stmtCache, sql);
        if (VCLinkedListGetCount(cacheItem) < 20) {
            stmt = __VCSqlite3CreateStmtToCache(ref, sql);
        }else {
            sqlite3_stmt *sqlite3_stmt = __VCSqlite3PrepartSql(ref, sql);
            if (VC_UNLIKELY(sqlite3_stmt == NULL)) return NULL;
            stmt = VCSqlite3StmtCreate(sqlite3_stmt);
        }
    }
    return stmt;
}

static const VCRuntimeClass __VCSqlite3Class = {
    "VCSqlite3",
    NULL,
    NULL,
    NULL,
    NULL,
    __VCSqlite3Dealloc
};

static volatile VCTypeID __VCSqlite3TypeID = 0;

VCTypeID VCSqlite3GetTypeID(void) {
    return VCRuntimeRegisterClass(__VCSqlite3TypeID, __VCSqlite3Class);
}

VCSqlite3Ref VCSqlite3Create(const char *path) {
    assert(path);
    if (VC_UNLIKELY(path == NULL || strlen(path) == 0)) return NULL;
    VCSqlite3Ref ref = (VCSqlite3Ref)VCRuntimeCreateInstance(VCSqlite3GetTypeID(), sizeof(VCSqlite3) - sizeof(VCRuntimeBase));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    ref->dbPath = strdup(path);
    VCLRUCacheRef cache = VCLRUCacheCreate(12, 0, &kVCCopyStringLRUCacheKeyCallback, &kVCTypeLRUCacheValueCallback);
    if (cache == NULL) {
        __VCSqlite3Dealloc(ref);
        return NULL;
    }
    ref->stmtCache = cache;
    return ref;
}

sqlite3 *VCSqlite3GetHandle(VCSqlite3Ref ref) {
    assert(ref);
    return ref->db;
}

bool VCSqlite3IsOpen(VCSqlite3Ref ref) {
    assert(ref);
    return ref->base.info[0];
}

bool VCSqlite3Open(VCSqlite3Ref ref) {
    assert(ref);
    return __VCSqlite3Open(ref);
}

bool VCSqlite3Close(VCSqlite3Ref ref) {
    assert(ref);
    if (!VCSqlite3IsOpen(ref)) return true;
    if (VC_UNLIKELY(ref == NULL)) return false;
    if (VC_UNLIKELY(VCSqlite3IsOpen(ref) == false)) return false;
    VCSqlite3CleanStmtCache(ref);
    int rc = sqlite3_close(ref->db);
    if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
        sqlite3_stmt *stmt;
        while ((stmt = sqlite3_next_stmt(ref->db, NULL)) != NULL) {
            printf("Closing leaked statement on %s\n",ref->dbPath);
            sqlite3_finalize(stmt);
        }
        rc = sqlite3_close(ref->db);//retry
    }
    
    if (VC_LIKELY(rc == SQLITE_OK)) {
        ref->base.info[0] = false;
    }else {
        __VCSqlite3PrintfError(ref, "close db error");
    }
    return false;
}

VCSqlite3StmtRef VCSqlite3PrepareStmt(VCSqlite3Ref ref,const char *sql) {
    assert(ref);
    assert(sql);
    assert(strlen(sql) > 0);
    return __VCSqlitePrepareStmt(ref, sql);
}

void VCSqlite3ExecSql(VCSqlite3Ref ref,const char *sql) {
    assert(ref);
    assert(sql);
    __VCSqlite3OpenIfNeed(ref);
    sqlite3_exec(ref->db, sql, NULL, NULL, NULL);
}

void VCSqlite3BeginTransaction(VCSqlite3Ref ref) {
    VCSqlite3ExecSql(ref, "begin exclusive transaction");
}

void VCSqlite3Commit(VCSqlite3Ref ref) {
    VCSqlite3ExecSql(ref, "commit transaction");
}

void VCSqlite3RollBack(VCSqlite3Ref ref) {
    VCSqlite3ExecSql(ref, "rollback transaction");
}

void VCSqlite3OpenWal(VCSqlite3Ref ref) {
    VCSqlite3ExecSql(ref, "PRAGMA journal_mode=WAL");
}

void VCSqlite3StmtReleaseIfNeed(VCSqlite3StmtRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return;
    if (ref->fromCache) return;
    __VCSqlite3StmtReleaseCallBack(ref);
}

void VCSqlite3CleanStmtCache(VCSqlite3Ref ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return;
    VCLRUCacheClear(ref->stmtCache);
}
