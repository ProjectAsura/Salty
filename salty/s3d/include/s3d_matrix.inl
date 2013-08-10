//-----------------------------------------------------------------------------------
// File : s3d_matrix.inl
// Desc : Matrix Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

#ifndef __S3D_MATRIX_INL__
#define __S3D_MATRIX_INL__


namespace s3d {

S3D_INLINE
Matrix::Matrix()
{ /* DO_NOTHING */ }

S3D_INLINE
Matrix::Matrix
(
    const f64 m11, const f64 m12, const f64 m13, const f64 m14,
    const f64 m21, const f64 m22, const f64 m23, const f64 m24,
    const f64 m31, const f64 m32, const f64 m33, const f64 m34,
    const f64 m41, const f64 m42, const f64 m43, const f64 m44
)
: _11( m11 ), _12( m12 ), _13( m13 ), _14( m14 )
, _21( m21 ), _22( m22 ), _23( m23 ), _24( m24 )
, _31( m31 ), _32( m32 ), _33( m33 ), _34( m34 )
, _41( m41 ), _42( m42 ), _43( m43 ), _44( m44 )
{ /* DO_NOTHING */ }

S3D_INLINE
Matrix::Matrix( const Matrix& value )
: _11( value._11 ), _12( value._12 ), _13( value._13 ), _14( value._14 )
, _21( value._21 ), _22( value._22 ), _23( value._23 ), _24( value._24 )
, _31( value._31 ), _32( value._32 ), _33( value._33 ), _34( value._34 )
, _41( value._41 ), _42( value._42 ), _43( value._43 ), _44( value._44 )
{ /* DO_NOTHING */ }

S3D_INLINE
Matrix& Matrix::operator = ( const Matrix& value )
{
    _11 = value._11; _12 = value._12; _13 = value._13; _14 = value._14;
    _21 = value._21; _22 = value._22; _23 = value._23; _24 = value._24;
    _31 = value._31; _32 = value._32; _33 = value._33; _34 = value._34;
    _41 = value._41; _42 = value._42; _43 = value._43; _44 = value._44;
    return (*this);
}

S3D_INLINE
Matrix& Matrix::operator += ( const Matrix& value )
{
    _11 += value._11; _12 += value._12; _13 += value._13; _14 += value._14;
    _21 += value._21; _22 += value._22; _23 += value._23; _24 += value._24;
    _31 += value._31; _32 += value._32; _33 += value._33; _34 += value._34;
    _41 += value._41; _42 += value._42; _43 += value._43; _44 += value._44;
    return (*this);
}

S3D_INLINE
Matrix& Matrix::operator -= ( const Matrix& value )
{
    _11 -= value._11; _12 -= value._12; _13 -= value._13; _14 -= value._14;
    _21 -= value._21; _22 -= value._22; _23 -= value._23; _24 -= value._24;
    _31 -= value._31; _32 -= value._32; _33 -= value._33; _34 -= value._34;
    _41 -= value._41; _42 -= value._42; _43 -= value._43; _44 -= value._44;
    return (*this);
}

S3D_INLINE
Matrix& Matrix::operator *= ( const Matrix& value )
{
    {
        register f64 m11 = ( _11 * value._11 ) + ( _12 * value._21 ) + ( _13 * value._31 ) + ( _14 * value._41 );
        register f64 m12 = ( _11 * value._12 ) + ( _12 * value._22 ) + ( _13 * value._32 ) + ( _14 * value._42 );
        register f64 m13 = ( _11 * value._13 ) + ( _12 * value._23 ) + ( _13 * value._33 ) + ( _14 * value._43 );
        register f64 m14 = ( _11 * value._14 ) + ( _12 * value._24 ) + ( _13 * value._34 ) + ( _14 * value._44 );

        _11 = m11;
        _12 = m12;
        _13 = m13;
        _14 = m14;
    }

    {
        register f64 m21 = ( _21 * value._11 ) + ( _22 * value._21 ) + ( _23 * value._31 ) + ( _24 * value._41 );
        register f64 m22 = ( _21 * value._12 ) + ( _22 * value._22 ) + ( _23 * value._32 ) + ( _24 * value._42 );
        register f64 m23 = ( _21 * value._13 ) + ( _22 * value._23 ) + ( _23 * value._33 ) + ( _24 * value._43 );
        register f64 m24 = ( _21 * value._14 ) + ( _22 * value._24 ) + ( _23 * value._34 ) + ( _24 * value._44 );

        _21 = m21;
        _22 = m22;
        _23 = m23;
        _24 = m24;
    }

    {
        register f64 m31 = ( _31 * value._11 ) + ( _32 * value._21 ) + ( _33 * value._31 ) + ( _34 * value._41 );
        register f64 m32 = ( _31 * value._12 ) + ( _32 * value._22 ) + ( _33 * value._32 ) + ( _34 * value._42 );
        register f64 m33 = ( _31 * value._13 ) + ( _32 * value._23 ) + ( _33 * value._33 ) + ( _34 * value._43 );
        register f64 m34 = ( _31 * value._14 ) + ( _32 * value._24 ) + ( _33 * value._34 ) + ( _34 * value._44 );

        _31 = m31;
        _32 = m32;
        _33 = m33;
        _34 = m34;
    }

    {
        register f64 m41 = ( _41 * value._11 ) + ( _42 * value._21 ) + ( _43 * value._31 ) + ( _44 * value._41 );
        register f64 m42 = ( _41 * value._12 ) + ( _42 * value._22 ) + ( _43 * value._32 ) + ( _44 * value._42 );
        register f64 m43 = ( _41 * value._13 ) + ( _42 * value._23 ) + ( _43 * value._33 ) + ( _44 * value._43 );
        register f64 m44 = ( _41 * value._14 ) + ( _42 * value._24 ) + ( _43 * value._34 ) + ( _44 * value._44 );

        _41 = m41;
        _42 = m42;
        _43 = m43;
        _44 = m44;
    }

    return (*this);
}

S3D_INLINE
Matrix& Matrix::operator *= ( const f64 value )
{
    _11 *= value; _12 *= value; _13 *= value; _14 *= value;
    _21 *= value; _22 *= value; _23 *= value; _24 *= value;
    _31 *= value; _32 *= value; _33 *= value; _34 *= value;
    _41 *= value; _42 *= value; _43 *= value; _44 *= value;
    return (*this); 
}

S3D_INLINE
Matrix Matrix::operator + () const
{ return (*this); }

S3D_INLINE
Matrix Matrix::operator - () const
{
    return Matrix(
        -_11, -_12, -_13, -_14,
        -_21, -_22, -_23, -_24,
        -_31, -_32, -_33, -_34,
        -_41, -_42, -_43, -_44 );
}

S3D_INLINE
Matrix Matrix::operator + ( const Matrix& value ) const
{
    return Matrix(
        _11 + value._11, _12 + value._12, _13 + value._13, _14 + value._14,
        _21 + value._21, _22 + value._22, _23 + value._23, _24 + value._24,
        _31 + value._31, _32 + value._32, _33 + value._33, _34 + value._34,
        _41 + value._41, _42 + value._42, _43 + value._43, _44 + value._44 );
}

S3D_INLINE
Matrix Matrix::operator - ( const Matrix& value ) const
{
    return Matrix(
        _11 - value._11, _12 - value._12, _13 - value._13, _14 - value._14,
        _21 - value._21, _22 - value._22, _23 - value._23, _24 - value._24,
        _31 - value._31, _32 - value._32, _33 - value._33, _34 - value._34,
        _41 - value._41, _42 - value._42, _43 - value._43, _44 - value._44 );
}

S3D_INLINE
Matrix Matrix::operator * ( const Matrix& value ) const
{
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
}

S3D_INLINE
Matrix Matrix::operator * ( const f64 value ) const
{
    return Matrix(
        _11 * value, _12 * value, _13 * value, _14 * value,
        _21 * value, _22 * value, _23 * value, _24 * value,
        _31 * value, _32 * value, _33 * value, _34 * value,
        _41 * value, _42 * value, _43 * value, _44 * value );
}

S3D_INLINE
f64 Matrix::Det() const
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

S3D_INLINE
Matrix Matrix::Invert( const Matrix& value )
{
    Matrix result;
    register f64 det = value.Det();

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

S3D_INLINE
Matrix Matrix::Transpose( const Matrix& value )
{
    return Matrix(
        value._11, value._21, value._31, value._41,
        value._12, value._22, value._32, value._42,
        value._13, value._23, value._33, value._43,
        value._14, value._24, value._34, value._44 );
}

S3D_INLINE
Matrix Matrix::Identity()
{
    return Matrix(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0 );
}

S3D_INLINE
Matrix Matrix::Translate( const f64 x, const f64 y, const f64 z )
{
    return Matrix(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        x,   y,   z,   1.0 ); 
}

S3D_INLINE
Matrix Matrix::Scale( const f64 x, const f64 y, const f64 z )
{
    return Matrix(
        x,   0.0, 0.0, 0.0,
        0.0, y,   0.0, 0.0,
        0.0, 0.0, z,   0.0,
        0.0, 0.0, 0.0, 1.0f );
}

S3D_INLINE
Matrix Matrix::RotateX( const f64 rad )
{
    register f64 cosRad = cos( rad );
    register f64 sinRad = sin( rad );
    return Matrix(
        1.0,     0.0,    0.0, 0.0,
        0.0,  cosRad, sinRad, 0.0,
        0.0, -sinRad, cosRad, 0.0,
        0.0,     0.0,    0.0, 1.0 );
}

S3D_INLINE
Matrix Matrix::RotateY( const f64 rad )
{
    register f64 cosRad = cos( rad );
    register f64 sinRad = sin( rad );
    return Matrix(
        cosRad,  0.0, -sinRad, 0.0,
        0.0,     1.0,     0.0, 0.0,
        sinRad,  0.0,  cosRad, 0.0,
        0.0,     0.0,     0.0, 1.0 );
}

S3D_INLINE
Matrix Matrix::RotateZ( const f64 rad )
{
    register f64 cosRad = cos( rad );
    register f64 sinRad = sin( rad );
    return Matrix(
        cosRad, sinRad, 0.0, 0.0,
        sinRad, cosRad, 0.0, 0.0,
        0.0,       0.0, 1.0, 0.0,
        0.0,       0.0, 0.0, 1.0 );
}

S3D_INLINE
Matrix Matrix::Rotate( const f64 x, const f64 y, const f64 z, const f64 rad )
{
    Matrix result;

    register f64 sinRad = sin(rad);
    register f64 cosRad = cos(rad);
    register f64 a = 1.0 -cosRad;
    
    register f64 ab = x * y * a;
    register f64 bc = y * z * a;
    register f64 ca = z * x * a;
    register f64 tx = x * x;
    register f64 ty = y * y;
    register f64 tz = z * z;

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

S3D_INLINE
Matrix Matrix::LookAt( const Vector3& position, const Vector3& target, const Vector3& upward )
{
    Vector3 zaxis = Vector3::UnitVector( target - position );
    Vector3 xaxis = Vector3::UnitVector( Vector3::Cross( upward, zaxis ) );
    Vector3 yaxis = Vector3::UnitVector( Vector3::Cross( zaxis, xaxis ) );

    return Matrix(
        xaxis.x, yaxis.x, zaxis.x, 0.0,
        xaxis.y, yaxis.y, zaxis.y, 0.0,
        xaxis.z, yaxis.z, zaxis.z, 0.0,

        -Vector3::Dot( xaxis, position ),
        -Vector3::Dot( yaxis, position ),
        -Vector3::Dot( zaxis, position ),
        1.0 );
}

S3D_INLINE
Matrix Matrix::PersFov( const f64 fieldOfView, const f64 aspectRatio, const f64 nearClip, const f64 farClip )
{
    register f64 diff = nearClip - farClip;
    register f64 yScale = 1.0 / tan( fieldOfView / 2.0 );
    register f64 xScale = yScale / aspectRatio;

    Matrix result;
    result._11 = xScale;
    result._12 = 0.0;
    result._13 = 0.0;
    result._14 = 0.0;

    result._21 = 0.0;
    result._22 = yScale;
    result._23 = 0.0;
    result._24 = 0.0;

    result._31 = 0.0;
    result._32 = 0.0;
    result._33 = farClip / diff;
    result._34 = -1.0;

    result._41 = 0.0;
    result._42 = 0.0;
    result._43 = (nearClip * farClip) / diff;
    result._44 = 0.0;

    return result;
}


} // namespace s3d

#endif//__S3D_MATRIX_INL__