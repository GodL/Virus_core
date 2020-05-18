//
//  VCBase.h
//  Example
//
//  Created by lihao10 on 2020/5/8.
//  Copyright © 2020 GodL. All rights reserved.
//

#ifndef VCBase_h
#define VCBase_h

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#ifdef __cplusplus
#define VC_EXTERN_C_BEGIN extern "C" {
#define VC_EXTERN_C_END   }
#else
#define VC_EXTERN_C_BEGIN
#define VC_EXTERN_C_END
#endif

VC_EXTERN_C_BEGIN

#ifdef __cplusplus
#define VC_OPEN        extern "C" __attribute__((visibility ("default")))
#define VC_PRIVATE      extern "C" __attribute__((visibility ("hidden")))
#else
#define VC_OPEN            extern __attribute__((visibility ("default")))
#define VC_PRIVATE          extern __attribute__((visibility ("hidden")))
#endif

#if defined(__GNUC__) && (__GNUC__ == 4) && !defined(DEBUG)
#define VC_INLINE static __inline__ __attribute__((always_inline))
#elif defined(__GNUC__)
#define VC_INLINE static __inline__
#elif defined(__cplusplus)
#define VC_INLINE static inline
#elif defined(_MSC_VER)
#define VC_INLINE static __inline
#endif

#if defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 303)
#   define VC_LIKELY(x)   __builtin_expect((x), 1)
#   define VC_UNLIKELY(x) __builtin_expect((x), 0)
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#   define VC_LIKELY(x)   (x)
#   define VC_UNLIKELY(x) (x)
#else /* not gcc >= 3.3 and not Sun Studio >= 8 */
#   define VC_LIKELY(x)   (x)
#   define VC_UNLIKELY(x) (x)
#endif

enum {
    kVCNotSupport = -1,
    kVCNotFound = -2
};

#if __LLP64__
typedef unsigned long long VCTypeID;
typedef signed long long VCIndex;
typedef unsigned long long VCHashCode;

#define VC_INT_MAX LONG_MAX
#else
typedef unsigned long VCTypeID;
typedef signed long VCIndex;
typedef unsigned long VCHashCode;

#define VC_INT_MAX INT_MAX
#endif

#define VCMIN(x,y) (x) <= (y) ? (x) : (y)

#define VCMAX(x,y) (x) >= (y) ? (x) : (y)

typedef struct {
    VCIndex location;
    VCIndex length;
} VCRange;

VC_INLINE VCRange VCRangeMake(VCIndex location,VCIndex length) {
    VCRange range;
    range.location = location;
    range.length = length;
    return range;
}

VC_OPEN const VCRange VCRangeZero;

typedef const void * VCTypeRef;

VC_OPEN VCTypeID VCGetTypeID(VCTypeRef vc);

VC_OPEN void VCRetain(VCTypeRef vc);

VC_OPEN void VCRelease(VCTypeRef vc);

VC_OPEN VCIndex VCGetRetainCount(VCTypeRef vc);

VC_OPEN bool VCEqual(VCTypeRef vc1,VCTypeRef vc2);

VC_OPEN VCHashCode VCHash(VCTypeRef vc);

VC_EXTERN_C_END

#endif /* VCBase_h */
