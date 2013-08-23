//----------------------------------------------------------------------------------
// File : s3d_box.inl
// Desc : Bounding Box Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

#ifndef __S3D_BOX_INL__
#define __S3D_BOX_INL__

#include <cstdio>

namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// BoundingBox structure
////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox::BoundingBox()
: min( D_MAX, D_MAX, D_MAX )
, max( D_MIN, D_MIN, D_MIN )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox::BoundingBox( const Vector3& value )
: min( value )
, max( value )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox::BoundingBox( const Vector3& mini, const Vector3& maxi )
: min( mini )
, max( maxi )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      コピーコンストラクタです.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox::BoundingBox( const BoundingBox& value )
: min( value.min )
, max( value.max )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      交差判定を行います.
//----------------------------------------------------------------------------------
S3D_INLINE
bool BoundingBox::IsHit( const Ray& ray ) const
{
    register f64 tx_min, ty_min, tz_min;
    register f64 tx_max, ty_max, tz_max;

    if ( ray.sign[0] )
    {
        tx_min = ( min.x - ray.pos.x ) * ray.invDir.x;
        tx_max = ( max.x - ray.pos.x ) * ray.invDir.x;
    }
    else
    {
        tx_min = ( max.x - ray.pos.x ) * ray.invDir.x;
        tx_max = ( min.x - ray.pos.x ) * ray.invDir.x;
    }

    if ( ray.sign[1] )
    {
        ty_min = ( min.y - ray.pos.y ) * ray.invDir.y;
        ty_max = ( max.y - ray.pos.y ) * ray.invDir.y;
    }
    else
    {
        ty_min = ( max.y - ray.pos.y ) * ray.invDir.y;
        ty_max = ( min.y - ray.pos.y ) * ray.invDir.y;
    }

    if ( ray.sign[2] )
    {
        tz_min = ( min.z - ray.pos.z ) * ray.invDir.z;
        tz_max = ( max.z - ray.pos.z ) * ray.invDir.z;
    }
    else
    {
        tz_min = ( max.z - ray.pos.z ) * ray.invDir.z;
        tz_max = ( min.z - ray.pos.z ) * ray.invDir.z;
    }

    register f64 t0 = ( tx_min > ty_min ) ? tx_min : ty_min;
                 t0 = ( tz_min > t0 )     ? tz_min : t0;

    register f64 t1 = ( tx_max > ty_max ) ? tx_max : ty_max;
                 t1 = ( tz_max > t1 )     ? tz_max : t1;

    return ( t0 < t1 && t1 > D_EPS );
}

//----------------------------------------------------------------------------------
//      2つのバウンディングボックスをマージします.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox BoundingBox::Merge( const BoundingBox& a, const BoundingBox& b )
{
    Vector3 mini = Vector3::Min( a.min, b.min );
    Vector3 maxi = Vector3::Max( a.max, b.max );

    return BoundingBox( mini, maxi );
}

//-----------------------------------------------------------------------------------
//      バウンディングボックスと点をマージします.
//-----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox BoundingBox::Merge( const BoundingBox& box, const Vector3& p )
{
    Vector3 mini = Vector3::Min( box.min, p );
    Vector3 maxi = Vector3::Max( box.max, p );

    return BoundingBox( mini, maxi );
}

} // namespace s3d


#endif//__S3D_BOX_INL__
