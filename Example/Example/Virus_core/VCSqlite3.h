//
//  VCSqlite3.h
//  Example
//
//  Created by lihao10 on 2020/5/18.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCSqlite3_h
#define VCSqlite3_h
//
#include "VCBase.h"
#include <sqlite3.h>

VC_EXTERN_C_BEGIN

typedef struct {
    bool used;
    bool fromCache;
    sqlite3_stmt *stmt;
} VCSqlite3Stmt;

typedef VCSqlite3Stmt * VCSqlite3StmtRef;

typedef struct __VCSqlite3 * VCSqlite3Ref;

VC_OPEN
VCTypeID VCSqlite3GetTypeID(void);

VC_OPEN
VCSqlite3Ref VCSqlite3Create(const char *path);

VC_OPEN
sqlite3 *VCSqlite3GetHandle(VCSqlite3Ref ref);

VC_OPEN
bool VCSqlite3IsOpen(VCSqlite3Ref ref);

VC_OPEN
bool VCSqlite3Open(VCSqlite3Ref ref);

VC_OPEN
bool VCSqlite3Close(VCSqlite3Ref ref);

VC_OPEN
VCSqlite3StmtRef VCSqlite3PrepareStmt(VCSqlite3Ref ref,const char *sql);

VC_OPEN
void VCSqlite3ExecSql(VCSqlite3Ref ref,const char *sql);

VC_OPEN
void VCSqlite3BeginTransaction(VCSqlite3Ref ref);

VC_OPEN
void VCSqlite3Commit(VCSqlite3Ref ref);

VC_OPEN
void VCSqlite3RollBack(VCSqlite3Ref ref);

VC_OPEN
void VCSqlite3OpenWal(VCSqlite3Ref ref);

VC_OPEN
void VCSqlite3CleanStmtCache(VCSqlite3Ref ref);

VC_EXTERN_C_END

#endif /* VCSqlite3_h */
