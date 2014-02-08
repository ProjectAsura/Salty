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
#define S3D_NDEBUG 0
#else
#define S3D_DEBUG  0
#define S3D_NDEBUG 1
#endif

#ifndef S3D_ALIGN
    #if _MSC_VER
        #define S3D_ALIGN( alignment )    __declspec( align(alignment) )
    #else
        #define S3D_ALIGN( alignment )    __attribute__( aligned(alignment) )
    #endif
#endif//S3D_ALIGN

#ifndef S3D_RELEASE
#define S3D_RELEASE(x)     { if (x) { (x)->Release(); (x) = nullptr; } }
#endif//S3D_RELEASE

#ifndef S3D_DELETE
#define S3D_DELETE(x)       { if (x) { delete (x); (x) = nullptr; } }
#endif//S3D_DELETE

#ifndef S3D_DELETE_ARRAY
#define S3D_DELETE_ARRAY(x) { if (x) { delete[] (x); (x) = nullptr; } }
#endif//S3D_DELETE_ARRAY


#if defined(_M_IX86) || defined(_M_AMD64)
    #define S3D_IS_SSE2   (1)     // SSE2有効.
#else
    #define S3D_IS_SSE2   (0)     // SSE2無効.
#endif

#if defined(__AVX__)
    #define S3D_IS_AVX    (1)     // AVX有効.
#else
    #define S3D_IS_AVX    (0)     // AVX無効.
#endif

#if defined(S3D_USE_SIMD)
    #define S3D_IS_SIMD   (1)     // SIMD演算有効.
#else
    #define S3D_IS_SIMD   (0)     // SIMD演算無効.
#endif// defined(S3D_USE_SIMD)


#if S3D_IS_SIMD

#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <dvec.h>

typedef __m64       b64;
typedef __m128      b128;
typedef __m128d     b128d;
typedef __m128i     b128i;
typedef __m256      b256;
typedef __m256d     b256d;
typedef __m256i     b256i;

#else

typedef union S3D_ALIGN(8) _b64
{
    unsigned __int64    m64_u64;
    float               m64_f32[2];
    __int8              m64_i8[8];
    __int16             m64_i16[4];
    __int32             m64_i32[2];
    __int64             m64_i64;
    unsigned __int8     m64_u8[8];
    unsigned __int16    m64_u16[4];
    unsigned __int32    m64_u32[2];
} b64;


typedef union S3D_ALIGN(16) _b128
{
    float               m128_f32[4];
    unsigned __int64    m128_u64[2];
    __int8              m128_i8[16];
    __int16             m128_i16[8];
    __int32             m128_i32[4];
    __int64             m128_i64[2];
    unsigned __int8     m128_u8[16];
    unsigned __int16    m128_u16[8];
    unsigned __int32    m128_u32[4];
} b128;


typedef union S3D_ALIGN(16) _b128d
{
    double              m128_f64[2];
} b128d;


typedef union S3D_ALIGN(16) _b128i
{
    unsigned __int64    m128_u64[2];
    __int8              m128_i8[16];
    __int16             m128_i16[8];
    __int32             m128_i32[4];
    __int64             m128_i64[2];
    unsigned __int8     m128_u8[16];
    unsigned __int16    m128_u16[8];
    unsigned __int32    m128_u32[4];
} b128i;


typedef union S3D_ALIGN(32) _b256
{
    float               m256_f32[8];
} b256;


typedef union S3D_ALIGN(32) _b256d
{
    double              m256_f64[4];
} b256d;


typedef union S3D_ALIGN(32) _b256i
{
    __int8              m256i_i8[32];
    __int16             m256i_i16[16];
    __int32             m256i_i32[8];
    __int64             m256i_i64[4];
    unsigned __int8     m256i_u8[32];
    unsigned __int16    m256i_u16[16];
    unsigned __int32    m256i_u32[8];
    unsigned __int64    m256i_u64[4];
} b256i;

#endif//S3D_IS_SIMD


#endif//__S3D_TYPEDEF_H__
