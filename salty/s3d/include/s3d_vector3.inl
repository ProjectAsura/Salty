﻿//-----------------------------------------------------------------------------------------------
// File : s3d_vector3.inl
// Desc : Vector3 Module.
// Copyright(c) Project Asura. All right reserved
//-----------------------------------------------------------------------------------------------

#ifndef __S3D_VECTOR3_INL__
#define __S3D_VECTOR3_INL__

namespace s3d {

////////////////////////////////////////////////////////////////////////////////
// Vector3 structure
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//      コンストラクタです.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3::Vector3()
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3::Vector3( const f64 nx, const f64 ny, const f64 nz )
: x( nx )
, y( ny )
, z( nz )
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------
//      コピーコンストラクタです.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3::Vector3( const Vector3& value )
: x( value.x )
, y( value.y )
, z( value.z )
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------
//      代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3& Vector3::operator = ( const Vector3& value )
{
    x = value.x;
    y = value.y;
    z = value.z;
    return (*this);
}

//------------------------------------------------------------------------------
//      加算代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3& Vector3::operator += ( const Vector3& value )
{
    x += value.x;
    y += value.y;
    z += value.z;
    return (*this);
}

//------------------------------------------------------------------------------
//      減算代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3& Vector3::operator -= ( const Vector3& value )
{
    x -= value.x;
    y -= value.y;
    z -= value.z;
    return (*this);
}

//------------------------------------------------------------------------------
//      乗算代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3& Vector3::operator *= ( const f64 value )
{
    x *= value;
    y *= value;
    z *= value;
    return (*this);
}

//------------------------------------------------------------------------------
//      除算代入演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3& Vector3::operator /= ( const f64 value )
{
    x /= value;
    y /= value;
    z /= value;
    return (*this);
}

//------------------------------------------------------------------------------
//      正符号演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::operator + () const
{
    return (*this);
}

//------------------------------------------------------------------------------
//      負符号演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::operator - () const
{ return Vector3( -x, -y, -z ); }

//------------------------------------------------------------------------------
//      加算演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::operator + ( const Vector3& value ) const
{ return Vector3( x + value.x, y + value.y, z + value.z ); }

//------------------------------------------------------------------------------
//      減算演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::operator - ( const Vector3& value ) const
{ return Vector3( x - value.x, y - value.y, z - value.z ); }

//------------------------------------------------------------------------------
//      乗算演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::operator * ( const f64 value ) const
{ return Vector3( x * value, y * value, z * value ); }

//------------------------------------------------------------------------------
//      除算演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::operator / ( const f64 value ) const
{ return Vector3( x / value, y / value, z / value ); }

//------------------------------------------------------------------------------
//      等価演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
bool Vector3::operator == ( const Vector3& value ) const
{
    return ( x == value.x )
        && ( y == value.y )
        && ( z == value.z );
}

//------------------------------------------------------------------------------
//      非等価演算子です.
//------------------------------------------------------------------------------
S3D_INLINE
bool Vector3::operator != ( const Vector3& value ) const
{
    return ( x != value.x )
        || ( y != value.y )
        || ( z != value.z );
}

//------------------------------------------------------------------------------
//      正規化します.
//------------------------------------------------------------------------------
S3D_INLINE
void Vector3::Normalize()
{
    register f64 mag = sqrt( x * x + y * y + z * z );
    x /= mag;
    y /= mag;
    z /= mag;
}

//------------------------------------------------------------------------------
//      長さを求めます.
//------------------------------------------------------------------------------
S3D_INLINE
f64 Vector3::Length() const
{ return sqrt( x * x + y * y + z * z); }

//------------------------------------------------------------------------------
//      長さの2乗を求めます.
//------------------------------------------------------------------------------
S3D_INLINE
f64 Vector3::LengthSq() const
{ return ( x * x + y * y + z * z ); }

//------------------------------------------------------------------------------
//      内積を求めます.
//------------------------------------------------------------------------------
S3D_INLINE
f64 Vector3::Dot( const Vector3& a, const Vector3& b )
{ return ( a.x * b.x + a.y * b.y + a.z * b.z ); }

//------------------------------------------------------------------------------
//      外積を求めます.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::Cross( const Vector3& a, const Vector3& b )
{
    return Vector3(
        ( a.y * b.z ) - ( a.z * b.y ),
        ( a.z * b.x ) - ( a.x * b.z ),
        ( a.x * b.y ) - ( a.y * b.x ) );
}

//------------------------------------------------------------------------------
//      各成分の最小値を求めます.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::Min( const Vector3& a, const Vector3& b )
{
    return Vector3(
        ( a.x < b.x ) ? a.x : b.x,
        ( a.y < b.y ) ? a.y : b.y,
        ( a.z < b.z ) ? a.z : b.z );
}

//------------------------------------------------------------------------------
//      各成分の最大値を求める.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::Max( const Vector3& a, const Vector3& b )
{
    return Vector3(
        ( a.x > b.x ) ? a.x : b.x,
        ( a.y > b.y ) ? a.y : b.y,
        ( a.z > b.z ) ? a.z : b.z );
}

//------------------------------------------------------------------------------
//      単位ベクトルを取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Vector3 Vector3::UnitVector( const Vector3& value )
{
    register f64 mag = sqrt( value.x * value.x + value.y * value.y + value.z * value.z );
    return Vector3( value.x / mag, value.y / mag, value.z / mag );
}


} // namespace s3d


#endif//__S3D_VECTOR3_INL__