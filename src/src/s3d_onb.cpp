//----------------------------------------------------------------------------------
// File : s3d_onb.cpp
// Desc : Orthonormal Basis Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <s3d_math.h>


namespace /* anonymous */ {

static const f64 ONB_EPSILON = 0.01;

} // namespace /* anonymous */

namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// OrthonormalBasis structure
////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------
OrthonormalBasis::OrthonormalBasis()
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------
OrthonormalBasis::OrthonormalBasis
(
    const Vector3& nu,
    const Vector3& nv,
    const Vector3& nw 
)
: u( nu )
, v( nv )
, w( nw )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      U方向から基底を構築します.
//----------------------------------------------------------------------------------
void OrthonormalBasis::InitFromU( const Vector3& value )
{
    Vector3 n( 1.0, 0.0, 0.0 );
    Vector3 m( 0.0, 1.0, 0.0 );
    
    u = Vector3::UnitVector( value );
    v = Vector3::Cross( u, n );
    if ( v.Length() < ONB_EPSILON )
    { v = Vector3::Cross( u, m ); }
    w = Vector3::Cross( u, v );
}

//----------------------------------------------------------------------------------
//      V方向から基底を構築します.
//----------------------------------------------------------------------------------
void OrthonormalBasis::InitFromV( const Vector3& value )
{
    Vector3 n ( 1.0, 0.0, 0.0 );
    Vector3 m ( 0.0, 1.0, 0.0 );

    v = Vector3::UnitVector( value );
    u = Vector3::Cross( v, n );
    if ( u.LengthSq() < ONB_EPSILON )
    { u = Vector3::Cross( v, m ); }
    w = Vector3::Cross( u, v );
}

//----------------------------------------------------------------------------------
//      W方向から基底を構築します.
//----------------------------------------------------------------------------------
void OrthonormalBasis::InitFromW( const Vector3& value )
{
    Vector3 n ( 1.0, 0.0, 0.0 );
    Vector3 m ( 0.0, 1.0, 0.0 );

    w = value;
    u = Vector3::Cross( w, n );
    if ( u.Length() < ONB_EPSILON )
    { u = Vector3::Cross( w, m ); }
    u.SafeNormalize();

    v = Vector3::Cross( w, u );
    v.SafeNormalize();
}

//----------------------------------------------------------------------------------
//      U方向とV方向から基底を構築します.
//----------------------------------------------------------------------------------
void OrthonormalBasis::InitFromUV( const Vector3& _u, const Vector3& _v )
{
    u = Vector3::UnitVector( _u );
    w = Vector3::UnitVector( Vector3::Cross( _u, _v ) );
    v = Vector3::Cross( w, v );
}

//----------------------------------------------------------------------------------
//      V方向とU方向から基底を構築します.
//----------------------------------------------------------------------------------
void OrthonormalBasis::InitFromVU( const Vector3& _v, const Vector3& _u )
{
    v = Vector3::UnitVector( _v );
    w = Vector3::UnitVector( Vector3::Cross( _u, _v ) );
    u = Vector3::Cross( v, w );
}

//----------------------------------------------------------------------------------
//      U方向とW方向から基底を構築します.
//----------------------------------------------------------------------------------
void OrthonormalBasis::InitFromUW( const Vector3& _u, const Vector3& _w )
{
    u = Vector3::UnitVector( _u );
    v = Vector3::UnitVector( Vector3::Cross( _w, _u ) );
    w = Vector3::Cross( u, v );
}

//----------------------------------------------------------------------------------
//      W方向とU方向から基底を構築します.
//----------------------------------------------------------------------------------
void OrthonormalBasis::InitFromWU( const Vector3& _w, const Vector3& _u )
{
    w = Vector3::UnitVector( _w );
    v = Vector3::UnitVector( Vector3::Cross( _w, _u ) );
    u = Vector3::Cross( v, w );
}

//----------------------------------------------------------------------------------
//      V方向とW方向から基底を構築します.
//----------------------------------------------------------------------------------
void OrthonormalBasis::InitFromVW( const Vector3& _v, const Vector3& _w )
{
    v = Vector3::UnitVector( _v );
    u = Vector3::UnitVector( Vector3::Cross( _v, _w ) );
    w = Vector3::Cross( u, v );
}

//----------------------------------------------------------------------------------
//      W方向とV方向から基底を構築します.
//----------------------------------------------------------------------------------
void OrthonormalBasis::InitFromWV( const Vector3& _w, const Vector3& _v )
{
    w = Vector3::UnitVector( _w );
    u = Vector3::UnitVector( Vector3::Cross( _v, _w ) );
    v = Vector3::Cross( w, u );
}

//----------------------------------------------------------------------------------
//      等価演算子です.
//----------------------------------------------------------------------------------
bool OrthonormalBasis::operator == ( const OrthonormalBasis& value ) const
{
    return ( u == value.u )
        && ( v == value.v )
        && ( w == value.w );
}

//----------------------------------------------------------------------------------
//      非等価演算子です.
//----------------------------------------------------------------------------------
bool OrthonormalBasis::operator != ( const OrthonormalBasis& value ) const
{
    return ( u != value.u )
        || ( v != value.v )
        || ( w != value.w );
}

} // namespace s3d

