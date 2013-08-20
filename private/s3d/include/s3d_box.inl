﻿//-----------------------------------------------------------------------------
// File : s3d_box.inl
// Desc : Bounding Box Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#ifndef __S3D_BOX_INL__
#define __S3D_BOX_INL__

namespace s3d {

S3D_INLINE
BoundingBox::BoundingBox()
: min( 1.7976931348623158e+308, 1.7976931348623158e+308, 1.7976931348623158e+308 )
, max( 2.2250738585072014e-308, 2.2250738585072014e-308, 2.2250738585072014e-308 )
{ /* DO_NOTHING */ }

S3D_INLINE
BoundingBox::BoundingBox( const Vector3& mini, const Vector3& maxi )
: min( mini )
, max( maxi )
{ /* DO_NOTHING */ }

S3D_INLINE
BoundingBox::BoundingBox( const BoundingBox& value )
: min( value.min )
, max( value.max )
{ /* DO_NOTHING */ }

S3D_INLINE
bool BoundingBox::IsHit( const Ray& ray, const f64 tmin, const f64 tmax ) const
{
    register f64 interval_min = tmin;
    register f64 interval_max = tmax;

    Vector3 pp[2]     = { min, max };
    Vector3 rayPos    = ray.GetPos();
    Vector3 rayInvDir = ray.GetInvDir();

    register s32 idx0 = ray.GetSignX();
    register s32 idx1 = 1 - idx0;

    register f64 t0 = ( pp[ idx0 ].x - rayPos.x ) * rayInvDir.x;
    register f64 t1 = ( pp[ idx1 ].x - rayPos.x ) * rayInvDir.x;
    
    if ( t0 > interval_min )
    { interval_min = t0; }
    
    if ( t1 < interval_max )
    { interval_max = t1; }
    
    if ( interval_min > interval_max )
    { return false; }

    idx0 = ray.GetSignY();
    idx1 = 1 - idx0;

    t0 = ( pp[ idx0 ].y - rayPos.y ) * rayInvDir.y;
    t1 = ( pp[ idx1 ].y - rayPos.y ) * rayInvDir.y;
    
    if ( t0 > interval_min )
    { interval_min = t0; }
    
    if ( t1 < interval_max )
    { interval_max = t1; }
    
    if ( interval_min > interval_max )
    { return false; }

    idx0 = ray.GetSignZ();
    idx1 = 1 - idx0;

    t0 = ( pp[ idx0 ].z - rayPos.z ) * rayInvDir.z;
    t1 = ( pp[ idx1 ].z - rayPos.z ) * rayInvDir.z;
    
    if ( t0 > interval_min )
    { interval_min = t0; }
    
    if ( t1 < interval_max )
    { interval_max = t1; }

    return ( interval_min <= interval_max );
}

S3D_INLINE
BoundingBox BoundingBox::Merge( const BoundingBox& a, const BoundingBox& b )
{
    Vector3 mini = Vector3::Min( a.min, b.min );
    Vector3 maxi = Vector3::Max( a.max, b.max );

    return BoundingBox( mini, maxi );
}

} // namespace s3d  


#endif//__S3D_BOX_INL__