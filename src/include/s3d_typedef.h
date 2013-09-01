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


#ifndef S3D_UNUSED_VAR
#define S3D_UNUSED_VAR(x)       ((void*)&x)
#endif//S3D_UNUSED_VAR

#if defined(DEBUG) || defined(_DEBUG)
#define S3D_DEBUG  1
#else
#define S3D_DEBUG  0
#endif

#ifndef S3D_ALIGN
    #if _MSC_VER
        #define S3D_ALIGN( alignment )    __declspec( align(alignment) )
    #else
        #define S3D_ALIGN( alignment )    __attribute__( aligned(alignment) )
    #endif
#endif//S3D_ALIGN


#if defined(_M_IX86) || defined(_M_AMD64)
    #define S3D_IS_SSE2   (1)     // SSE2有効.
#else
    #define S3D_IS_SSE2   (0)     // SSE2無効.
#endif

#if defined(S3D_USE_SIMD)
    #define S3D_IS_SIMD   (1)     // SIMD演算有効.
#else
    #define S3D_IS_SIMD   (0)     // SIMD演算無効.
#endif// defined(S3D_USE_SIMD)


#endif//__S3D_TYPEDEF_H__
