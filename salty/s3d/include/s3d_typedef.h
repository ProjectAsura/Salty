//------------------------------------------------------------------------------
// File : s3d_ypedef.h
// Desc : Type Definitions.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __S3D_TYPEDEF_H__
#define __S3D_TYPEDEF_H__


//==============================================================================
// Type definitions
//==============================================================================
typedef char                s8;
typedef short               s16;
typedef int                 s32;
typedef __int64             s64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned __int64    u64;

typedef float               f32;
typedef double              f64;


#ifndef S3D_INLINE
    #ifdef  _MSC_VER
        #if (_MSC_VER >= 1200)
            #define S3D_INLINE __forceinline
        #else
            #define S3D_INLINE __inline
        #endif//(_MSC_VER >= 1200)
    #else
        #ifdef __cplusplus
            #define S3D_INLINE inline
        #else
            #define S3D_INLINE 
        #endif//__cpulusplus
    #endif//_MSC_VER
#endif//S3D_INLINE


#endif//__S3D_TYPEDEF_H__
