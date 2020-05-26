//
//  VCSqliteStmt.c
//  Example
//
//  Created by lihao10 on 2020/5/18.
//  Copyright © 2020 GodL. All rights reserved.
//

#include "VCSqliteStmt.h"
#include "VCRuntime.h"
#include <sqlite3.h>

typedef struct __VCSqliteStmt {
    sqlite3_stmt *stmt;
} VCSqliteStmt;

VC_INLINE const void *__VCSqliteStmtGetStmt(VCSqliteStmtRef ref) {
    return ref->stmt;
}

VC_INLINE const VCSqliteStmtCallback *__VCSqliteStmtGetCallback(VCSqliteStmtRef ref) {
    VCRuntimeBase *base = (VCRuntimeBase *)ref;
    return (const VCSqliteStmtCallback *)base->callback;
}

static void __VCSqliteStmtInit(VCSqliteStmtRef ref) {
    sqlite3_stmt *stmt = (sqlite3_stmt *)__VCSqliteStmtGetStmt(ref);
    if (stmt) {
        sqlite3_reset(stmt);
    }
}

static bool __VCSqliteStmtEqual(VCTypeRef ref1,VCTypeRef ref2) {
    return ref1 == ref2 || __VCSqliteStmtGetStmt((VCSqliteStmtRef)ref1) == __VCSqliteStmtGetStmt((VCSqliteStmtRef)ref2);
}

static VCHashCode __VCSqliteStmtHash(VCTypeRef ref) {
    return (VCHashCode)ref;
}

static void __VCSqliteStmtDealloc(VCTypeRef ref) {
    VCSqliteStmtRef sqliteStmt = (VCSqliteStmtRef)ref;
    const VCSqliteStmtCallback *callback = __VCSqliteStmtGetCallback(sqliteStmt);
    if (callback && callback->release) callback->release(sqliteStmt);
    free(sqliteStmt);
    sqliteStmt = NULL;
}

static const void * __VCSqliteStmtDefaultRetain(const void *stmt) {
    if (stmt) {
        sqlite3_reset((sqlite3_stmt *)stmt);
    }
    return stmt;
}

static void __VCSqliteStmtDefaultRelease(const void *stmt) {
    if (stmt) {
        sqlite3_finalize((sqlite3_stmt *)stmt);
        stmt = NULL;
    }
}

static bool __VCSqliteStmtDefaultEqual(const void *stmt1,const void *stmt2) {
    return stmt1 == stmt2;
}

static const VCRuntimeClass __VCSqliteStmtClass = {
    "VCSqliteStmt",
    NULL,
    NULL,
    __VCSqliteStmtEqual,
    __VCSqliteStmtHash,
    __VCSqliteStmtDealloc
};

const VCSqliteStmtCallback kVCDefaultSqliteStmtCallback = {
    __VCSqliteStmtDefaultRetain,
    __VCSqliteStmtDefaultRelease,
    __VCSqliteStmtDefaultEqual
};

static volatile VCTypeID __kVCSqliteStmtType = 0;

VCTypeID VCSqliteStmtGetTypeID(void) {
    return VCRuntimeRegisterClass(__kVCSqliteStmtType, __VCSqliteStmtClass);
}

VCSqliteStmtRef VCSqliteStmtCreate(const void *stmt,const VCSqliteStmtCallback *callback) {
    VCSqliteStmtRef ref = (VCSqliteStmtRef)VCRuntimeCreateInstance(VCSqliteStmtGetTypeID(), sizeof(VCSqliteStmt) - sizeof(VCRuntimeBase));
    if (VC_UNLIKELY(ref == NULL)) return NULL;
    VCRuntimeBase *base = (VCRuntimeBase *)ref;
    base->callback = (uintptr_t)callback;
    base->info[0] = false;
    ref->stmt = (sqlite3_stmt *)stmt;
    __VCSqliteStmtInit(ref);
    return ref;
}

const void *VCSqliteStmtGetStmt(VCSqliteStmtRef ref) {
    assert(ref);
    if (ref == NULL) return NULL;
    return __VCSqliteStmtGetStmt(ref);
}

bool VCSqliteStmtIsUnused(VCSqliteStmtRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return false;
    if (VC_UNLIKELY(ref->stmt == NULL)) return false;
    VCRuntimeBase *base = (VCRuntimeBase *)ref;
    return base->info[0] == 0;
}

void VCSqliteStmtReplaceStmt(VCSqliteStmtRef ref,const void *stmt) {
    assert(ref);
    assert(stmt);
    if (VC_UNLIKELY(ref == NULL || stmt == NULL)) return;
    const VCSqliteStmtCallback *cb = __VCSqliteStmtGetCallback(ref);
    if (cb && cb->release) cb->release(__VCSqliteStmtGetStmt(ref));
    if (cb && cb->retain) stmt = cb->retain(stmt);
    ref->stmt = (sqlite3_stmt *)stmt;
    VCRuntimeBase *base = (VCRuntimeBase *)ref;
    base->info[0] = 0;
}

void VCSqliteStmtRemoveStmt(VCSqliteStmtRef ref) {
    assert(ref);
    if (VC_UNLIKELY(ref == NULL)) return;
    const VCSqliteStmtCallback *cb = __VCSqliteStmtGetCallback(ref);
    if (cb && cb->release) cb->release(__VCSqliteStmtGetStmt(ref));
    ref->stmt = NULL;
}
