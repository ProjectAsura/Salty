//-----------------------------------------------------------------------------------
// File : s3d_matrix.inl
// Desc : Matrix Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

#ifndef __S3D_MATRIX_INL__
#define __S3D_MATRIX_INL__


namespace s3d {

////////////////////////////////////////////////////////////////////////////////
// Matrix structure
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//      コンストラクタです.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix::Matrix()
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix::Matrix
(
    const f32 m11, const f32 m12, const f32 m13, const f32 m14,
    const f32 m21, const f32 m22, const f32 m23, const f32 m24,
    const f32 m31, const f32 m32, const f32 m33, const f32 m34,
    const f32 m41, const f32 m42, const f32 m43, const f32 m44
)
#if S3D_IS_SIMD
: v0( _mm_set_ps( m14, m13, m12, m11 ) )
, v1( _mm_set_ps( m24, m23, m22, m21 ) )
, v2( _mm_set_ps( m34, m33, m32, m31 ) )
, v3( _mm_set_ps( m44, m43, m42, m41 ) )
#else
: _11( m11 ), _12( m12 ), _13( m13 ), _14( m14 )
, _21( m21 ), _22( m22 ), _23( m23 ), _24( m24 )
, _31( m31 ), _32( m32 ), _33( m33 ), _34( m34 )
, _41( m41 ), _42( m42 ), _43( m43 ), _44( m44 )
#endif
{ /* DO_NOTHING */ }

#if S3D_IS_SIMD
//------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix::Matrix( const b128 c0, const b128 c1, const b128 c2, const b128 c3 )
: v0 ( c0 )
, v1 ( c1 )
, v2 ( c2 )
, v3 ( c3 )
{ /* DO_NOTHING */ }
#endif//S3D_IS_SIMD

//------------------------------------------------------------------------------
//      コピーコンストラクタです.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix::Matrix( const Matrix& value )
#if S3D_IS_SIMD
: v0 ( value.v0 )
, v1 ( value.v1 )
, v2 ( value.v2 )
, v3 ( value.v3 )
#else
: _11( value._11 ), _12( value._12 ), _13( value._13 ), _14( value._14 )
, _21( value._21 ), _22( value._22 ), _23( value._23 ), _24( value._24 )
, _31( value._31 ), _32( value._32 ), _33( value._33 ), _34( value._34 )
, _41( value._41 ), _42( value._42 ), _43( value._43 ), _44( value._44 )
#endif//S3D_IS_SIMD
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------
//      代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix& Matrix::operator = ( const Matrix& value )
{
#if S3D_IS_SIMD
    v0 = value.v0;
    v1 = value.v1;
    v2 = value.v2;
    v3 = value.v3;
#else
    _11 = value._11; _12 = value._12; _13 = value._13; _14 = value._14;
    _21 = value._21; _22 = value._22; _23 = value._23; _24 = value._24;
    _31 = value._31; _32 = value._32; _33 = value._33; _34 = value._34;
    _41 = value._41; _42 = value._42; _43 = value._43; _44 = value._44;
#endif//S3D_IS_SIMD
    return (*this);
}

//------------------------------------------------------------------------------
//      加算代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix& Matrix::operator += ( const Matrix& value )
{
#if S3D_IS_SIMD
    v0 = _mm_add_ps( v0, value.v0 );
    v1 = _mm_add_ps( v1, value.v1 );
    v2 = _mm_add_ps( v2, value.v2 );
    v3 = _mm_add_ps( v3, value.v3 );
#else
    _11 += value._11; _12 += value._12; _13 += value._13; _14 += value._14;
    _21 += value._21; _22 += value._22; _23 += value._23; _24 += value._24;
    _31 += value._31; _32 += value._32; _33 += value._33; _34 += value._34;
    _41 += value._41; _42 += value._42; _43 += value._43; _44 += value._44;
#endif//S3D_IS_SIMD
    return (*this);
}

//------------------------------------------------------------------------------
//      減算代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix& Matrix::operator -= ( const Matrix& value )
{
#if S3D_IS_SIMD
    v0 = _mm_sub_ps( v0, value.v0 );
    v1 = _mm_sub_ps( v1, value.v1 );
    v2 = _mm_sub_ps( v2, value.v2 );
    v3 = _mm_sub_ps( v3, value.v3 );
#else
    _11 -= value._11; _12 -= value._12; _13 -= value._13; _14 -= value._14;
    _21 -= value._21; _22 -= value._22; _23 -= value._23; _24 -= value._24;
    _31 -= value._31; _32 -= value._32; _33 -= value._33; _34 -= value._34;
    _41 -= value._41; _42 -= value._42; _43 -= value._43; _44 -= value._44;
#endif//S3D_IS_SIMD
    return (*this);
}

//------------------------------------------------------------------------------
//      乗算代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix& Matrix::operator *= ( const Matrix& value )
{
#if S3D_IS_SIMD
    b128 r0 = _mm_set_ps( value._41, value._31, value._21, value._11 );
    b128 r1 = _mm_set_ps( value._42, value._32, value._22, value._12 );
    b128 r2 = _mm_set_ps( value._43, value._33, value._23, value._13 );
    b128 r3 = _mm_set_ps( value._44, value._34, value._24, value._14 );

    Vector4 m0 = _mm_mul_ps( v0, r0 );
    Vector4 m1 = _mm_mul_ps( v0, r1 );
    Vector4 m2 = _mm_mul_ps( v0, r2 );
    Vector4 m3 = _mm_mul_ps( v0, r3 );

    v0 = _mm_set_ps(
        m3.x + m3.y + m3.z + m3.w,
        m2.x + m2.y + m2.z + m2.w,
        m1.x + m1.y + m1.z + m1.w,
        m0.x + m0.y + m0.z + m0.w );

    m0 = _mm_mul_ps( v1, r0 );
    m1 = _mm_mul_ps( v1, r1 );
    m2 = _mm_mul_ps( v1, r2 );
    m3 = _mm_mul_ps( v1, r3 );

    v1 = _mm_set_ps(
        m3.x + m3.y + m3.z + m3.w,
        m2.x + m2.y + m2.z + m2.w,
        m1.x + m1.y + m1.z + m1.w,
        m0.x + m0.y + m0.z + m0.w );

    m0 = _mm_mul_ps( v2, r0 );
    m1 = _mm_mul_ps( v2, r1 );
    m2 = _mm_mul_ps( v2, r2 );
    m3 = _mm_mul_ps( v2, r3 );

    v2 = _mm_set_ps(
        m3.x + m3.y + m3.z + m3.w,
        m2.x + m2.y + m2.z + m2.w,
        m1.x + m1.y + m1.z + m1.w,
        m0.x + m0.y + m0.z + m0.w );

    m0 = _mm_mul_ps( v3, r0 );
    m1 = _mm_mul_ps( v3, r1 );
    m2 = _mm_mul_ps( v3, r2 );
    m3 = _mm_mul_ps( v3, r3 );

    v3 = _mm_set_ps(
        m3.x + m3.y + m3.z + m3.w,
        m2.x + m2.y + m2.z + m2.w,
        m1.x + m1.y + m1.z + m1.w,
        m0.x + m0.y + m0.z + m0.w );

#else
    {
        register f32 m11 = ( _11 * value._11 ) + ( _12 * value._21 ) + ( _13 * value._31 ) + ( _14 * value._41 );
        register f32 m12 = ( _11 * value._12 ) + ( _12 * value._22 ) + ( _13 * value._32 ) + ( _14 * value._42 );
        register f32 m13 = ( _11 * value._13 ) + ( _12 * value._23 ) + ( _13 * value._33 ) + ( _14 * value._43 );
        register f32 m14 = ( _11 * value._14 ) + ( _12 * value._24 ) + ( _13 * value._34 ) + ( _14 * value._44 );

        _11 = m11;
        _12 = m12;
        _13 = m13;
        _14 = m14;
    }

    {
        register f32 m21 = ( _21 * value._11 ) + ( _22 * value._21 ) + ( _23 * value._31 ) + ( _24 * value._41 );
        register f32 m22 = ( _21 * value._12 ) + ( _22 * value._22 ) + ( _23 * value._32 ) + ( _24 * value._42 );
        register f32 m23 = ( _21 * value._13 ) + ( _22 * value._23 ) + ( _23 * value._33 ) + ( _24 * value._43 );
        register f32 m24 = ( _21 * value._14 ) + ( _22 * value._24 ) + ( _23 * value._34 ) + ( _24 * value._44 );

        _21 = m21;
        _22 = m22;
        _23 = m23;
        _24 = m24;
    }

    {
        register f32 m31 = ( _31 * value._11 ) + ( _32 * value._21 ) + ( _33 * value._31 ) + ( _34 * value._41 );
        register f32 m32 = ( _31 * value._12 ) + ( _32 * value._22 ) + ( _33 * value._32 ) + ( _34 * value._42 );
        register f32 m33 = ( _31 * value._13 ) + ( _32 * value._23 ) + ( _33 * value._33 ) + ( _34 * value._43 );
        register f32 m34 = ( _31 * value._14 ) + ( _32 * value._24 ) + ( _33 * value._34 ) + ( _34 * value._44 );

        _31 = m31;
        _32 = m32;
        _33 = m33;
        _34 = m34;
    }

    {
        register f32 m41 = ( _41 * value._11 ) + ( _42 * value._21 ) + ( _43 * value._31 ) + ( _44 * value._41 );
        register f32 m42 = ( _41 * value._12 ) + ( _42 * value._22 ) + ( _43 * value._32 ) + ( _44 * value._42 );
        register f32 m43 = ( _41 * value._13 ) + ( _42 * value._23 ) + ( _43 * value._33 ) + ( _44 * value._43 );
        register f32 m44 = ( _41 * value._14 ) + ( _42 * value._24 ) + ( _43 * value._34 ) + ( _44 * value._44 );

        _41 = m41;
        _42 = m42;
        _43 = m43;
        _44 = m44;
    }
#endif//S3D_IS_SIMD

    return (*this);
}

//------------------------------------------------------------------------------
//      乗算代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix& Matrix::operator *= ( const f32 value )
{
#if S3D_IS_SIMD
    b128 c = _mm_set_ps( value, value, value, value );
    v0 = _mm_mul_ps( v0, c );
    v1 = _mm_mul_ps( v1, c );
    v2 = _mm_mul_ps( v2, c );
    v3 = _mm_mul_ps( v3, c );
#else
    _11 *= value; _12 *= value; _13 *= value; _14 *= value;
    _21 *= value; _22 *= value; _23 *= value; _24 *= value;
    _31 *= value; _32 *= value; _33 *= value; _34 *= value;
    _41 *= value; _42 *= value; _43 *= value; _44 *= value;
#endif//S3D_IS_SIMD
    return (*this); 
}

//------------------------------------------------------------------------------
//      正符号演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::operator + () const
{ return (*this); }

//------------------------------------------------------------------------------
//      負符号演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::operator - () const
{
#if S3D_IS_SIMD
    b128 z = _mm_setzero_ps();
    return Matrix(
        _mm_sub_ps( z, v0 ),
        _mm_sub_ps( z, v1 ),
        _mm_sub_ps( z, v2 ),
        _mm_sub_ps( z, v3 ) );
#else
    return Matrix(
        -_11, -_12, -_13, -_14,
        -_21, -_22, -_23, -_24,
        -_31, -_32, -_33, -_34,
        -_41, -_42, -_43, -_44 );
#endif//S3D_IS_SIMD
}

//------------------------------------------------------------------------------
//      加算演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::operator + ( const Matrix& value ) const
{
#if S3D_IS_SIMD
    return Matrix(
        _mm_add_ps( v0, value.v0 ),
        _mm_add_ps( v1, value.v1 ),
        _mm_add_ps( v2, value.v2 ),
        _mm_add_ps( v3, value.v3 ) );
#else
    return Matrix(
        _11 + value._11, _12 + value._12, _13 + value._13, _14 + value._14,
        _21 + value._21, _22 + value._22, _23 + value._23, _24 + value._24,
        _31 + value._31, _32 + value._32, _33 + value._33, _34 + value._34,
        _41 + value._41, _42 + value._42, _43 + value._43, _44 + value._44 );
#endif//S3D_IS_SIMD
}

//------------------------------------------------------------------------------
//      減算演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::operator - ( const Matrix& value ) const
{
#if S3D_IS_SIMD
    return Matrix(
        _mm_sub_ps( v0, value.v0 ),
        _mm_sub_ps( v1, value.v1 ),
        _mm_sub_ps( v2, value.v2 ),
        _mm_sub_ps( v3, value.v3 ) );
#else
    return Matrix(
        _11 - value._11, _12 - value._12, _13 - value._13, _14 - value._14,
        _21 - value._21, _22 - value._22, _23 - value._23, _24 - value._24,
        _31 - value._31, _32 - value._32, _33 - value._33, _34 - value._34,
        _41 - value._41, _42 - value._42, _43 - value._43, _44 - value._44 );
#endif//S3D_IS_SIMD
}

//------------------------------------------------------------------------------
//      乗算演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::operator * ( const Matrix& value ) const
{
#if S3D_IS_SIMD
    b128 r0 = _mm_set_ps( value._41, value._31, value._21, value._11 );
    b128 r1 = _mm_set_ps( value._42, value._32, value._22, value._12 );
    b128 r2 = _mm_set_ps( value._43, value._33, value._23, value._13 );
    b128 r3 = _mm_set_ps( value._44, value._34, value._24, value._14 );

    Vector4 m0 = _mm_mul_ps( v0, r0 );
    Vector4 m1 = _mm_mul_ps( v0, r1 );
    Vector4 m2 = _mm_mul_ps( v0, r2 );
    Vector4 m3 = _mm_mul_ps( v0, r3 );

    b128 e0 = _mm_set_ps(
        m3.x + m3.y + m3.z + m3.w,
        m2.x + m2.y + m2.z + m2.w,
        m1.x + m1.y + m1.z + m1.w,
        m0.x + m0.y + m0.z + m0.w );

    m0 = _mm_mul_ps( v1, r0 );
    m1 = _mm_mul_ps( v1, r1 );
    m2 = _mm_mul_ps( v1, r2 );
    m3 = _mm_mul_ps( v1, r3 );

    b128 e1 = _mm_set_ps(
        m3.x + m3.y + m3.z + m3.w,
        m2.x + m2.y + m2.z + m2.w,
        m1.x + m1.y + m1.z + m1.w,
        m0.x + m0.y + m0.z + m0.w );

    m0 = _mm_mul_ps( v2, r0 );
    m1 = _mm_mul_ps( v2, r1 );
    m2 = _mm_mul_ps( v2, r2 );
    m3 = _mm_mul_ps( v2, r3 );

    b128 e2 = _mm_set_ps(
        m3.x + m3.y + m3.z + m3.w,
        m2.x + m2.y + m2.z + m2.w,
        m1.x + m1.y + m1.z + m1.w,
        m0.x + m0.y + m0.z + m0.w );

    m0 = _mm_mul_ps( v3, r0 );
    m1 = _mm_mul_ps( v3, r1 );
    m2 = _mm_mul_ps( v3, r2 );
    m3 = _mm_mul_ps( v3, r3 );

    b128 e3 = _mm_set_ps(
        m3.x + m3.y + m3.z + m3.w,
        m2.x + m2.y + m2.z + m2.w,
        m1.x + m1.y + m1.z + m1.w,
        m0.x + m0.y + m0.z + m0.w );

    return Matrix( e0, e1, e2, e3 );
#else
    return Matrix(
            ( _11 * value._11 ) + ( _12 * value._21 ) + ( _13 * value._31 ) + ( _14 * value._41 ),
            ( _11 * value._12 ) + ( _12 * value._22 ) + ( _13 * value._32 ) + ( _14 * value._42 ),
            ( _11 * value._13 ) + ( _12 * value._23 ) + ( _13 * value._33 ) + ( _14 * value._43 ),
            ( _11 * value._14 ) + ( _12 * value._24 ) + ( _13 * value._34 ) + ( _14 * value._44 ),

            ( _21 * value._11 ) + ( _22 * value._21 ) + ( _23 * value._31 ) + ( _24 * value._41 ),
            ( _21 * value._12 ) + ( _22 * value._22 ) + ( _23 * value._32 ) + ( _24 * value._42 ),
            ( _21 * value._13 ) + ( _22 * value._23 ) + ( _23 * value._33 ) + ( _24 * value._43 ),
            ( _21 * value._14 ) + ( _22 * value._24 ) + ( _23 * value._34 ) + ( _24 * value._44 ),

            ( _31 * value._11 ) + ( _32 * value._21 ) + ( _33 * value._31 ) + ( _34 * value._41 ),
            ( _31 * value._12 ) + ( _32 * value._22 ) + ( _33 * value._32 ) + ( _34 * value._42 ),
            ( _31 * value._13 ) + ( _32 * value._23 ) + ( _33 * value._33 ) + ( _34 * value._43 ),
            ( _31 * value._14 ) + ( _32 * value._24 ) + ( _33 * value._34 ) + ( _34 * value._44 ),

            ( _41 * value._11 ) + ( _42 * value._21 ) + ( _43 * value._31 ) + ( _44 * value._41 ),
            ( _41 * value._12 ) + ( _42 * value._22 ) + ( _43 * value._32 ) + ( _44 * value._42 ),
            ( _41 * value._13 ) + ( _42 * value._23 ) + ( _43 * value._33 ) + ( _44 * value._43 ),
            ( _41 * value._14 ) + ( _42 * value._24 ) + ( _43 * value._34 ) + ( _44 * value._44 ) );
#endif//S3D_IS_SIMD
}

//------------------------------------------------------------------------------
//      乗算演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::operator * ( const f32 value ) const
{
#if S3D_IS_SIMD
    b128 c = _mm_set_ps( value, value, value, value );
    return Matrix(
        _mm_mul_ps( v0, c ),
        _mm_mul_ps( v1, c ),
        _mm_mul_ps( v2, c ),
        _mm_mul_ps( v3, c ) );
#else
    return Matrix(
        _11 * value, _12 * value, _13 * value, _14 * value,
        _21 * value, _22 * value, _23 * value, _24 * value,
        _31 * value, _32 * value, _33 * value, _34 * value,
        _41 * value, _42 * value, _43 * value, _44 * value );
#endif//S3D_IS_SIMD
}

//------------------------------------------------------------------------------
//      行列式を求めます.
//------------------------------------------------------------------------------
S3D_INLINE
f32 Matrix::Det() const
{
    return (
        ( _11 * _22 * _33 * _44 ) + ( _11 * _23 * _34 * _42 ) +
        ( _11 * _24 * _32 * _43 ) + ( _12 * _21 * _34 * _43 ) +
        ( _12 * _23 * _31 * _44 ) + ( _12 * _24 * _33 * _41 ) +
        ( _13 * _21 * _32 * _44 ) + ( _13 * _22 * _34 * _41 ) +
        ( _13 * _24 * _31 * _42 ) + ( _14 * _21 * _33 * _42 ) +
        ( _14 * _22 * _31 * _43 ) + ( _14 * _23 * _32 * _41 ) -
        ( _11 * _22 * _34 * _43 ) - ( _11 * _23 * _32 * _44 ) -
        ( _11 * _24 * _33 * _42 ) - ( _12 * _21 * _33 * _44 ) -
        ( _12 * _23 * _34 * _41 ) - ( _12 * _24 * _31 * _43 ) -
        ( _13 * _21 * _34 * _42 ) - ( _13 * _22 * _31 * _44 ) -
        ( _13 * _24 * _32 * _41 ) - ( _14 * _21 * _32 * _43 ) -
        ( _14 * _22 * _33 * _41 ) - ( _14 * _23 * _31 * _42 )
    );
}

//------------------------------------------------------------------------------
//      逆行列を求めます.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::Invert( const Matrix& value )
{
    Matrix result;
    register f32 det = value.Det();

    result._11 = ( value._22 * value._33 * value._44 ) + ( value._23 * value._34 * value._42 ) + ( value._24 * value._32 * value._43 )
               - ( value._22 * value._34 * value._43 ) - ( value._23 * value._32 * value._44 ) - ( value._24 * value._33 * value._42 ) / det;
    result._12 = ( value._12 * value._34 * value._43 ) + ( value._13 * value._32 * value._44 ) + ( value._14 * value._33 * value._42 )
               - ( value._12 * value._33 * value._44 ) - ( value._13 * value._34 * value._42 ) - ( value._14 * value._32 * value._43 ) / det;
    result._13 = ( value._12 * value._23 * value._44 ) + ( value._13 * value._24 * value._42 ) + ( value._14 * value._22 * value._43 )
               - ( value._12 * value._24 * value._43 ) - ( value._13 * value._22 * value._44 ) - ( value._14 * value._23 * value._42 ) / det;
    result._14 = ( value._12 * value._24 * value._33 ) + ( value._13 * value._22 * value._34 ) + ( value._14 * value._23 * value._32 )
               - ( value._12 * value._23 * value._34 ) - ( value._13 * value._24 * value._32 ) - ( value._14 * value._22 * value._33 ) / det;

    result._21 = ( value._21 * value._34 * value._43 ) + ( value._23 * value._31 * value._44 ) + ( value._24 * value._33 * value._41 )
               - ( value._21 * value._33 * value._44 ) - ( value._23 * value._34 * value._41 ) - ( value._24 * value._31 * value._43 ) / det;
    result._22 = ( value._11 * value._33 * value._44 ) + ( value._13 * value._34 * value._41 ) + ( value._14 * value._31 * value._43 )
               - ( value._11 * value._34 * value._43 ) - ( value._13 * value._31 * value._44 ) - ( value._14 * value._33 * value._41 ) / det;
    result._23 = ( value._11 * value._24 * value._43 ) + ( value._13 * value._21 * value._44 ) + ( value._14 * value._23 * value._41 )
               - ( value._11 * value._23 * value._44 ) - ( value._13 * value._24 * value._41 ) - ( value._14 * value._21 * value._43 ) / det;
    result._24 = ( value._11 * value._23 * value._34 ) + ( value._13 * value._24 * value._31 ) + ( value._14 * value._21 * value._33 )
               - ( value._11 * value._24 * value._33 ) - ( value._13 * value._21 * value._34 ) - ( value._14 * value._23 * value._31 ) / det;

    result._31 = ( value._21 * value._32 * value._44 ) + ( value._22 * value._34 * value._41 ) + ( value._24 * value._31 * value._42 )
               - ( value._21 * value._34 * value._42 ) - ( value._22 * value._31 * value._44 ) - ( value._24 * value._32 * value._41 ) / det;
    result._32 = ( value._11 * value._34 * value._42 ) + ( value._12 * value._31 * value._44 ) + ( value._14 * value._32 * value._41 )
               - ( value._11 * value._32 * value._44 ) - ( value._12 * value._34 * value._41 ) - ( value._14 * value._31 * value._42 ) / det;
    result._33 = ( value._11 * value._22 * value._44 ) + ( value._12 * value._24 * value._41 ) + ( value._14 * value._21 * value._42 )
               - ( value._11 * value._24 * value._42 ) - ( value._12 * value._21 * value._44 ) - ( value._14 * value._22 * value._41 ) / det;
    result._34 = ( value._11 * value._24 * value._32 ) + ( value._12 * value._21 * value._34 ) + ( value._14 * value._22 * value._31 )
               - ( value._11 * value._22 * value._34 ) - ( value._12 * value._24 * value._31 ) - ( value._14 * value._21 * value._32 ) / det;

    result._41 = ( value._21 * value._33 * value._42 ) + ( value._22 * value._31 * value._43 ) + ( value._23 * value._32 * value._41 )
               - ( value._21 * value._32 * value._43 ) - ( value._22 * value._33 * value._41 ) - ( value._23 * value._31 * value._42 ) / det;
    result._42 = ( value._11 * value._32 * value._43 ) + ( value._12 * value._33 * value._41 ) + ( value._13 * value._31 * value._42 )
               - ( value._11 * value._33 * value._42 ) - ( value._12 * value._31 * value._43 ) - ( value._13 * value._32 * value._41 ) / det;
    result._43 = ( value._11 * value._23 * value._42 ) + ( value._12 * value._21 * value._43 ) + ( value._13 * value._22 * value._41 )
               - ( value._11 * value._22 * value._43 ) - ( value._12 * value._23 * value._41 ) - ( value._13 * value._21 * value._42 ) / det;
    result._44 = ( value._11 * value._22 * value._33 ) + ( value._12 * value._23 * value._31 ) + ( value._13 * value._21 * value._32 )
               - ( value._11 * value._23 * value._32 ) - ( value._12 * value._21 * value._33 ) - ( value._13 * value._22 * value._31 ) / det;

    return result;
}

//------------------------------------------------------------------------------
//      転置行列を求めます.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::Transpose( const Matrix& value )
{
    return Matrix(
        value._11, value._21, value._31, value._41,
        value._12, value._22, value._32, value._42,
        value._13, value._23, value._33, value._43,
        value._14, value._24, value._34, value._44 );
}

//------------------------------------------------------------------------------
//      単位行列を取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::Identity()
{
    return Matrix(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f );
}

//------------------------------------------------------------------------------
//      平行移動行列を取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::Translate( const f32 x, const f32 y, const f32 z )
{
    return Matrix(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x,    y,    z,    1.0f );
}

//------------------------------------------------------------------------------
//      拡大縮小行列を取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::Scale( const f32 x, const f32 y, const f32 z )
{
    return Matrix(
        x,    0.0f, 0.0f, 0.0f,
        0.0f, y,    0.0f, 0.0f,
        0.0f, 0.0f, z,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f );
}

//------------------------------------------------------------------------------
//      X軸周りの回転行列を取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::RotateX( const f32 rad )
{
    register f32 cosRad = cosf( rad );
    register f32 sinRad = sinf( rad );
    return Matrix(
        1.0f,    0.0f,   0.0f, 0.0f,
        0.0f,  cosRad, sinRad, 0.0f,
        0.0f, -sinRad, cosRad, 0.0f,
        0.0f,    0.0f,   0.0f, 1.0f );
}

//------------------------------------------------------------------------------
//      Y軸周りの回転行列を取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::RotateY( const f32 rad )
{
    register f32 cosRad = cosf( rad );
    register f32 sinRad = sinf( rad );
    return Matrix(
        cosRad,  0.0f, -sinRad, 0.0f,
        0.0f,    1.0f,    0.0f, 0.0f,
        sinRad,  0.0f,  cosRad, 0.0f,
        0.0f,    0.0f,    0.0f, 1.0f );
}

//------------------------------------------------------------------------------
//      Z軸周りの回転行列を取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::RotateZ( const f32 rad )
{
    register f32 cosRad = cosf( rad );
    register f32 sinRad = sinf( rad );
    return Matrix(
        cosRad, sinRad, 0.0f, 0.0f,
        sinRad, cosRad, 0.0f, 0.0f,
        0.0f,     0.0f, 1.0f, 0.0f,
        0.0f,     0.0f, 0.0f, 1.0f );
}

//------------------------------------------------------------------------------
//      回転行列を取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::Rotate( const f32 x, const f32 y, const f32 z, const f32 rad )
{
    Matrix result;

    register f32 sinRad = sinf(rad);
    register f32 cosRad = cosf(rad);
    register f32 a = 1.0 -cosRad;
    
    register f32 ab = x * y * a;
    register f32 bc = y * z * a;
    register f32 ca = z * x * a;
    register f32 tx = x * x;
    register f32 ty = y * y;
    register f32 tz = z * z;

    result._11 = tx + cosRad * (1.0 - tx);
    result._12 = ab + z * sinRad;
    result._13 = ca - y * sinRad;
    result._14 = 0.0;

    result._21 = ab - z * sinRad;
    result._22 = ty + cosRad * (1.0 - ty);
    result._23 = bc + x * sinRad;
    result._24 = 0.0;

    result._31 = ca + y * sinRad;
    result._32 = bc - x * sinRad;
    result._33 = tz + cosRad * (1.0 - tz);
    result._34 = 0.0;

    result._41 = 0.0;
    result._42 = 0.0;
    result._43 = 0.0;
    result._44 = 1.0;

    return result;
}

//------------------------------------------------------------------------------
//      ビュー行列を取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::LookAt( const Vector3& position, const Vector3& target, const Vector3& upward )
{
    Vector3 zaxis = Vector3::UnitVector( target - position );
    Vector3 xaxis = Vector3::UnitVector( Vector3::Cross( upward, zaxis ) );
    Vector3 yaxis = Vector3::UnitVector( Vector3::Cross( zaxis, xaxis ) );

    return Matrix(
        xaxis.x, yaxis.x, zaxis.x, 0.0f,
        xaxis.y, yaxis.y, zaxis.y, 0.0f,
        xaxis.z, yaxis.z, zaxis.z, 0.0f,

        -Vector3::Dot( xaxis, position ),
        -Vector3::Dot( yaxis, position ),
        -Vector3::Dot( zaxis, position ),
        1.0 );
}

//------------------------------------------------------------------------------
//      射影行列を取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Matrix Matrix::PersFov( const f32 fieldOfView, const f32 aspectRatio, const f32 nearClip, const f32 farClip )
{
    register f32 diff = farClip - nearClip;
    register f32 yScale = 1.0f / tanf( fieldOfView * 0.5f );
    register f32 xScale = yScale / aspectRatio;

    Matrix result;
    result._11 = xScale;
    result._12 = 0.0f;
    result._13 = 0.0f;
    result._14 = 0.0f;

    result._21 = 0.0f;
    result._22 = yScale;
    result._23 = 0.0f;
    result._24 = 0.0f;

    result._31 = 0.0f;
    result._32 = 0.0f;
    result._33 = farClip / diff;
    result._34 = -1.0f;

    result._41 = 0.0f;
    result._42 = 0.0f;
    result._43 = - (nearClip * farClip) / diff;
    result._44 = 0.0f;

    return result;
}

} // namespace s3d

#endif//__S3D_MATRIX_INL__
