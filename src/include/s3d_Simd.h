//-------------------------------------------------------------------------------------------------
// File : s3d_Simd.h
// Desc : SIMD Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_typedef.h>
#include <nmmintrin.h>  // SSE 4.2
#include <immintrin.h>  // AVX


namespace s3d {

struct S3D_ALIGN(16) Float4  { float v[4];  };
struct S3D_ALIGN(32) Float8  { float v[8];  };
struct S3D_ALIGN(64) Float16 { float v[16]; };

///////////////////////////////////////////////////////////////////////////////////////////////////
// Simd4 class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Simd4
{
public:
    static const b128 Zero;
    static const b128 One2;
    static const b128 One3;
    static const b128 One4;
    static const b128 UnitX;
    static const b128 UnitY;
    static const b128 UnitZ;
    static const b128 UnitW;
    static const b128 SignMask;
    static const b128 Maxi;
    static const b128 Lowest;
    static const b128 Epsilon;

    static b128 S3D_VECAPI Make (f32 x);
    static b128 S3D_VECAPI Make (f32 x, f32 y, f32 z, f32 w);
    static b128 S3D_VECAPI Load (const Float4& value);
    static void S3D_VECAPI Store(Float4& value, b128 v);
    static void S3D_VECAPI SetX (b128& v, f32 value);
    static void S3D_VECAPI SetY (b128& v, f32 value);
    static void S3D_VECAPI SetZ (b128& v, f32 value);
    static void S3D_VECAPI SetW (b128& v, f32 value);
    static f32  S3D_VECAPI GetX (b128 v);
    static f32  S3D_VECAPI GetY (b128 v);
    static f32  S3D_VECAPI GetZ (b128 v);
    static f32  S3D_VECAPI GetW (b128 v);
    static f32  S3D_VECAPI Mag2 (b128 v);
    static f32  S3D_VECAPI Mag3 (b128 v);
    static f32  S3D_VECAPI Mag4 (b128 v);
    static s32  S3D_VECAPI Mask (b128 v);
    static b128 S3D_VECAPI Neg  (b128 v);
    static b128 S3D_VECAPI Rcp  (b128 v);
    static b128 S3D_VECAPI Sqrt (b128 v);
    static b128 S3D_VECAPI Unit2(b128 v);
    static b128 S3D_VECAPI Unit3(b128 v);
    static b128 S3D_VECAPI Unit4(b128 v);
    static b128 S3D_VECAPI Sign (b128 v);
    static b128 S3D_VECAPI Abs  (b128 v);
    static b128 S3D_VECAPI Add  (b128 lhs, b128 rhs);
    static b128 S3D_VECAPI Sub  (b128 lhs, b128 rhs);
    static b128 S3D_VECAPI Mul  (b128 lhs, b128 rhs);
    static b128 S3D_VECAPI Div  (b128 lhs, b128 rhs);
    static b128 S3D_VECAPI Max  (b128 lhs, b128 rhs);
    static b128 S3D_VECAPI Min  (b128 lhs, b128 rhs);
    static b128 S3D_VECAPI Crs  (b128 lhs, b128 rhs);
    static f32  S3D_VECAPI Dp2  (b128 lhs, b128 rhs);
    static f32  S3D_VECAPI Dp3  (b128 lhs, b128 rhs);
    static f32  S3D_VECAPI Dp4  (b128 lhs, b128 rhs);
    static b128 S3D_VECAPI CmpLt(b128 lhs, b128 rhs);
    static b128 S3D_VECAPI CmpLe(b128 lhs, b128 rhs);
    static b128 S3D_VECAPI CmpEq(b128 lhs, b128 rhs);
    static b128 S3D_VECAPI CmpNq(b128 lhs, b128 rhs);
    static b128 S3D_VECAPI CmpGe(b128 lhs, b128 rhs);
    static b128 S3D_VECAPI CmpGt(b128 lhs, b128 rhs);
    static b128 S3D_VECAPI Trs2 (b128 lhs, const b128 rhs[4]);
    static b128 S3D_VECAPI Trs3 (b128 lhs, const b128 rhs[4]);
    static b128 S3D_VECAPI Trs4 (b128 lhs, const b128 rhs[4]);
    static b128 S3D_VECAPI TrsN (b128 lhs, const b128 rhs[4]);
    static b128 S3D_VECAPI TrsC (b128 lhs, const b128 rhs[4]);
};

#if S3D_IS_AVX

///////////////////////////////////////////////////////////////////////////////////////////////////
// Simd8 class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Simd8
{
public:
    static const b256 Zero;
    static const b256 One;
    static const b256 SignMask;
    static const b256 Maxi;
    static const b256 Lowest;
    static const b256 Epsilon;

    static b256 S3D_VECAPI Make (f32 value);
    static b256 S3D_VECAPI Make (f32 a, f32 b, f32 c, f32 d, f32 e, f32 f, f32 g, f32 h);
    static b256 S3D_VECAPI Load (const Float8& value);
    static void S3D_VECAPI Store(Float8& value, b256 v);
    static b256 S3D_VECAPI Mask (b256 v);
    static b256 S3D_VECAPI Neg  (b256 v);
    static b256 S3D_VECAPI Rcp  (b256 v);
    static b256 S3D_VECAPI Sign (b256 v);
    static b256 S3D_VECAPI Abs  (b256 v);
    static b256 S3D_VECAPI Add  (b256 lhs, b256 rhs);
    static b256 S3D_VECAPI Sub  (b256 lhs, b256 rhs);
    static b256 S3D_VECAPI Mul  (b256 lhs, b256 rhs);
    static b256 S3D_VECAPI Min  (b256 lhs, b256 rhs);
    static b256 S3D_VECAPI CmpLt(b256 lhs, b256 rhs);
    static b256 S3D_VECAPI CmpLe(b256 lhs, b256 rhs);
    static b256 S3D_VECAPI CmpEq(b256 lhs, b256 rhs);
    static b256 S3D_VECAPI CmpNq(b256 lhs, b256 rhs);
    static b256 S3D_VECAPI CmpGe(b256 lhs, b256 rhs);
    static b256 S3D_VECAPI CmpGt(b256 lhs, b256 rhs);
};

#endif//S3D_IS_AVX

} // namespace s3d

//-------------------------------------------------------------------------------------------------
// Inline Files.
//-------------------------------------------------------------------------------------------------
#include <s3d_Simd.inl>
