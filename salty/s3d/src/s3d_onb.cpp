//----------------------------------------------------------------------------------
// File : s3d_onb.cpp
// Desc : Orthonormal Basis Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <s3d_onb.h>


namespace /* anonymous */ {

static const f64 ONB_EPSILON = 0.01;

} // namespace /* anonymous */

namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// OrthonormalBasis structure
////////////////////////////////////////////////////////////////////////////////////

OrthonormalBasis::OrthonormalBasis()
{ /* DO_NOTHING */ }

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

void OrthonormalBasis::InitFromU( const Vector3& value )
{
    Vector3 n( 1.0, 0.0, 0.0 );
    Vector3 m( 0.0, 1.0, 0.0 );
    
    u = value.UnitVector();
    v = Vector3::Cross( u, n );
    if ( v.Length() < ONB_EPSILON )
    { v = Vector3::Cross( u, m ); }
    w = Vector3::Cross( u, v );
}

void OrthonormalBasis::InitFromV( const Vector3& value )
{
    Vector3 n ( 1.0, 0.0, 0.0 );
    Vector3 m ( 0.0, 1.0, 0.0 );

    v =value.UnitVector();
    u = Vector3::Cross( v, n );
    if ( u.LengthSq() < ONB_EPSILON )
    { u = Vector3::Cross( v, m ); }
    w = Vector3::Cross( u, v );
}

void OrthonormalBasis::InitFromW( const Vector3& value )
{
    Vector3 n ( 1.0, 0.0, 0.0 );
    Vector3 m ( 0.0, 1.0, 0.0 );

    w = value.UnitVector();
    u = Vector3::Cross( w, n );
    if ( u.Length() < ONB_EPSILON )
    { u = Vector3::Cross( w, m ); }
    v = Vector3::Cross( w, u );
}

} // namespace s3d

