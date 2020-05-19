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

VC_EXTERN_C_BEGIN

typedef struct __VCSqlite3 * VCSqlite3Ref;

VC_OPEN VCTypeID VCSqlite3GetTypeID(void);

VC_OPEN VCSqlite3Create(const char *path);

VC_OPEN bool VCSqlite3Close(VCSqlite3Ref ref);

VC_EXTERN_C_END

#endif /* VCSqlite3_h */
