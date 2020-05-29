//
//  VCSqlite3Handler.h
//  Example
//
//  Created by lihao10 on 2020/5/29.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCSqlite3Handler_h
#define VCSqlite3Handler_h

#include "VCBase.h"
#include "VCSqlite3.h"

typedef struct __VCSqlite3Handler * VCSqlite3HandlerRef;

VC_OPEN
VCTypeID VCSqlite3HandlerGetTypeID(void);

VC_OPEN
VCSqlite3HandlerRef VCSqlite3HandlerCreate(const char *path,VCIndex concurrentRead);

VC_OPEN
VCSqlite3Ref VCSqlite3HandlerGetWriteHandle(VCSqlite3HandlerRef ref);//maybe blocking

VC_OPEN
void VCSqlite3HandlerReturnWriteHandle(VCSqlite3HandlerRef ref,VCSqlite3Ref sqlite3);

VC_OPEN
VCSqlite3Ref VCSqlite3HandlerGetReadHandle(VCSqlite3HandlerRef ref);//maybe blocking

VC_OPEN
void VCSqlite3HandlerReturnReadHandle(VCSqlite3HandlerRef ref,VCSqlite3Ref sqlite3);

#endif /* VCSqlite3Handler_h */
