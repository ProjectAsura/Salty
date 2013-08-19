//----------------------------------------------------------------------------------
// File : s3d_onb.inl
// Desc : Orthonormal Basis Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

#ifndef __S3D_ONB_INL__
#define __S3D_ONB_INL__

namespace /* anonymous */ {

static const f64 ONB_EPSILON = 0.01;

} // namespace /* anonymous */

namespace s3d {

S3D_INLINE
OrthonormalBasis::OrthonormalBasis()
{ /* DO_NOTHING */ }

S3D_INLINE
OrthonormalBasis::OrthonormalBasis( const Vector3& nu, const Vector3& nv, const Vector3& nw )
: u( nu )
, v( nv )
, w( nw )
{ /* DO_NOTHING */ }

S3D_INLINE
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

S3D_INLINE
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

S3D_INLINE
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

#endif//__S3D_ONB_INL__
