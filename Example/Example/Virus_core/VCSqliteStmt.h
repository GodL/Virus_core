//
//  VCSqliteStmt.h
//  Example
//
//  Created by lihao10 on 2020/5/18.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCSqliteStmt_h
#define VCSqliteStmt_h

#include "VCBase.h"

VC_EXTERN_C_BEGIN

typedef const void *(*VCSqliteStmtRetainCallback)(const void *value);
typedef void (*VCSqliteStmtReleaseCallback)(const void *value);
typedef bool (*VCSqliteStmtEqualCallback)(const void *value1,const void *value2);

typedef struct __VCSqliteStmtCallback {
    VCSqliteStmtRetainCallback retain;
    VCSqliteStmtReleaseCallback release;
    VCSqliteStmtEqualCallback equal;
} VCSqliteStmtCallback;

typedef struct __VCSqliteStmt * VCSqliteStmtRef;

VC_OPEN const VCSqliteStmtCallback kVCDefaultSqliteStmtCallback;//default is reset stmt and finalize stmt

VC_OPEN VCTypeID VCSqliteStmtGetTypeID(void);

VC_OPEN VCSqliteStmtRef VCSqliteStmtCreate(const void *stmt,const VCSqliteStmtCallback *callback);

VC_OPEN const void *VCSqliteStmtGetStmt(VCSqliteStmtRef ref);

VC_OPEN bool VCSqliteStmtIsUnused(VCSqliteStmtRef ref);

VC_OPEN void VCSqliteStmtReplaceStmt(VCSqliteStmtRef ref,const void *stmt);

VC_OPEN void VCSqliteStmtRemoveStmt(VCSqliteStmtRef ref);

VC_EXTERN_C_END

#endif /* VCSqliteStmt_h */
