//------------------------------------------------------------------------------
// File : s3d_ypedef.h
// Desc : Type Definitions.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------
#pragma once

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

#ifndef S3D_TEMPLATE
#define S3D_TEMPLATE(T)             template< typename T >
#endif//S3D_TEMPLATE

#ifndef S3D_TEMPLATE2
#define S3D_TEMPLATE2(T, U)         template< typename T, typename U >
#endif//S3D_TEMPLATE2


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


//-------------------------------------------------------------------------
//! @def        S8_MIN
//! @brief      符号付き8bit整数型の最小値です.
//-------------------------------------------------------------------------
#ifndef S8_MIN
#define S8_MIN          (-127i8 - 1)
#endif//S8_MIN

//-------------------------------------------------------------------------
//! @def        S16_MIN
//! @brief      符号付き16bit整数型の最小値です.
//-------------------------------------------------------------------------
#ifndef S16_MIN
#define S16_MIN         (-32767i16 - 1)
#endif//S16_MIN

//-------------------------------------------------------------------------
//! @def        S32_MIN
//! @brief      符号付き32bit整数型の最小値です.
//-------------------------------------------------------------------------
#ifndef S32_MIN
#define S32_MIN         (-2147483647i32 - 1)
#endif//S32_MIN

//-------------------------------------------------------------------------
//! @def        S64_MIN
//! @brief      符号付き64bit整数型の最小値です.
//-------------------------------------------------------------------------
#ifndef S64_MIN
#define S64_MIN         (-9223372036854775807i64 - 1)
#endif//S64_MIN

//-------------------------------------------------------------------------
//! @def        S8_MAX
//! @brief      符号付8bit整数型の最大値です.
//-------------------------------------------------------------------------
#ifndef S8_MAX
#define S8_MAX          127i8
#endif//S8_MAX

//-------------------------------------------------------------------------
//! @def        S16_MAX
//! @brief      符号付き16bit整数型の最大値です.
//-------------------------------------------------------------------------
#ifndef S16_MAX
#define S16_MAX         32767i16
#endif//S16_MAX

//-------------------------------------------------------------------------
//! @def        S32_MAX
//! @brief      符号付き32bit整数型の最大値です.
//-------------------------------------------------------------------------
#ifndef S32_MAX
#define S32_MAX         2147483647i32
#endif//S32_MAX

//-------------------------------------------------------------------------
//! @def        S64_MAX
//! @brief      符号付き64bit整数型の最大値です.
//-------------------------------------------------------------------------
#ifndef S64_MAX
#define S64_MAX         9223372036854775807i64
#endif//S64_MAX

//-------------------------------------------------------------------------
//! @def        U8_MAX
//! @brief      符号無し8bit整数型の最大値です.
//-------------------------------------------------------------------------
#ifndef U8_MAX
#define U8_MAX          0xffui8
#endif//U8_MAX

//-------------------------------------------------------------------------
//! @def        U16_MAX
//! @brief      符号無し16bit整数型の最大値です.
//-------------------------------------------------------------------------
#ifndef U16_MAX
#define U16_MAX         0xffffui16
#endif//U16_MAX

//-------------------------------------------------------------------------
//! @def        U32_MAX
//! @brief      符号無し32bit整数型の最大値です.
//-------------------------------------------------------------------------
#ifndef U32_MAX
#define U32_MAX         0xffffffffui32
#endif//U32_MAX

//-------------------------------------------------------------------------
//! @def        U64_MAX
//! @brief      符号無し64bit整数型の最大値です.
//-------------------------------------------------------------------------
#ifndef U64_MAX
#define U64_MAX         0xffffffffffffffffui64
#endif//U64_MAX

//-------------------------------------------------------------------------
//! @def        S64_C
//! @brief      値xをs64型の符号付き整数定数式へ展開します.
//-------------------------------------------------------------------------
#ifndef S64_C
#define S64_C(x)        ( (x) + ( S64_MAX - S64_MAX ) )
#endif//S64_C

//-------------------------------------------------------------------------
//! @def        U64_C
//! @brief      値xをu64型の符号無し整数定数式へ展開します.
//-------------------------------------------------------------------------
#ifndef U64_C
#define U64_C(x)        ( (x) + ( U64_MAX - U64_MAX ) )
#endif//U64_C

//-------------------------------------------------------------------------
//! @def        F16_MIN
//! @brief      半精度(16bit)浮動小数型の最小値です.
//-------------------------------------------------------------------------
#ifndef F16_MIN
#define F16_MIN             5.96046448e-08F
#endif//F16_MIN

//-------------------------------------------------------------------------
//! @def        F32_MIN
//! @brief      単精度(32bit)浮動小数型の最小値です.
//-------------------------------------------------------------------------
#ifndef F32_MIN
#define F32_MIN             1.175494351e-38F
#endif//F32_MIN

//-------------------------------------------------------------------------
//! @def        F64_MIN
//! @brief      倍精度(64bit)浮動小数型の最小値です.
//-------------------------------------------------------------------------
#ifndef F64_MIN
#define F64_MIN             2.2250738585072014e-308
#endif//F64_MIN

//-------------------------------------------------------------------------
//! @def        F16_MAX
//! @brief      半精度(16bit)浮動小数型の最大値です.
//-------------------------------------------------------------------------
#ifndef F16_MAX
#define F16_MAX             65504.0f
#endif//F16_MAX

//-------------------------------------------------------------------------
//! @def        F32_MAX
//! @brief      単精度(32bit)浮動小数型の最大値です.
//-------------------------------------------------------------------------
#ifndef F32_MAX
#define F32_MAX             3.402823466e+38F
#endif//F32_MAX

//-------------------------------------------------------------------------
//! @def        F64_MAX
//! @brief      倍精度(64bit)浮動小数型の最大値です.
//-------------------------------------------------------------------------
#ifndef F64_MAX
#define F64_MAX             1.7976931348623158e+308
#endif//F64_MAX

//-------------------------------------------------------------------------
//! @def        F16_EPSILON
//! @brief      半精度(16bit)浮動小数型で表現可能な1より大きい最小値と1との差.
//-------------------------------------------------------------------------
#ifndef F16_EPSILON
#define F16_EPSILON         0.00097656f
#endif//F16_EPSILON

//-------------------------------------------------------------------------
//! @def        F32_EPSILON
//! @brief      単精度(32bit)浮動小数型で表現可能な1より大きい最小値と1との差.
//-------------------------------------------------------------------------
#ifndef F32_EPSILON
#define F32_EPSILON         1.192092896e-07F
#endif//F32_EPSILON

//-------------------------------------------------------------------------
//! @def        F64_EPSILON
//! @brief      倍精度(64bit)浮動小数型で表現可能な1より大きい最小値と1との差.
//-------------------------------------------------------------------------
#ifndef F64_EPSILON
#define F64_EPSILON         2.2204460492503131e-016
#endif//F64_EPSILON



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

S3D_TEMPLATE(T)
void SafeRelease( T*& ptr )
{
    if ( ptr != nullptr )
    {
        ptr->Release();
        ptr = nullptr;
    }
}

S3D_TEMPLATE(T)
void SafeDelete( T*& ptr )
{
    if ( ptr != nullptr )
    {
        delete ptr;
        ptr = nullptr;
    }
}

S3D_TEMPLATE(T)
void SafeDeleteArray( T*& ptr )
{
    if ( ptr != nullptr )
    {
        delete [] ptr;
        ptr = nullptr;
    }
}
