//-------------------------------------------------------------------------
// File : s3d_sphere.cpp
// Desc : Sphere Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------
#include <s3d_sphere.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////
// Sphere structure
////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------
Sphere::Sphere( const Vector3& cent, const f64 rad, IMaterial* mat )
: center( cent )
, radius( rad )
, pMaterial( mat )
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------
Sphere::Sphere( const Sphere& value )
: center( value.center )
, radius( value.radius )
, pMaterial( value.pMaterial )
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      衝突判定を行います.
//-----------------------------------------------------------------------------
bool Sphere::IsHit( const Ray& ray, const f64 mini, const f64 maxi, ShadeRec& shadeRec )
{
    Vector3      d = center - ray.GetPos();
    register f64 b = Vector3::Dot( d, ray.GetDir() );
    register f64 det = ( b * b ) - Vector3::Dot( d, d ) + ( radius * radius );

    if ( det < 0.0 )
    { return false; }
    
    det = sqrt( det );
    
#if 0
    register f64 dist = b - det;
    if ( dist < mini || dist > maxi )
    {
        dist = b + det;
        if ( dist < mini || dist > maxi )
        { return false; }
    }
#else
    register f64 t1 = b - det;
    register f64 t2 = b + det;

    if ( t1 < mini && t2 < mini )
    { return false; }

    f64 dist = 0.0;
    //f64 dist = -1.0;
    if ( t1 > mini )
    { dist = t1; }
    else
    { dist = t2; }

    //if ( dist < mini || dist > maxi )
    //{ return false; }
#endif

    Vector3 pos = Ray::Step( ray, dist );
    Vector3 nrm = Vector3::UnitVector( pos - center );

#if 0
    /* 球は，屈折により物体内部を進む場合があるので背面カリングしちゃだめ! */
    //if ( Vector3::Dot( ray.GetDir(), nrm ) > 0.0 )
    //{ return false; }
#endif
    
    shadeRec.normal     = nrm;
    shadeRec.dist       = dist;
    shadeRec.pMaterial  = pMaterial;

    return true;
}

//-----------------------------------------------------------------------------
//      軸平行境界箱を取得します.
//-----------------------------------------------------------------------------
BoundingBox Sphere::GetAABB() const
{
    Vector3 min( center.x + radius, center.y + radius, center.z + radius );
    Vector3 max( center.y - radius, center.y - radius, center.z - radius );

    return BoundingBox( min, max );
}

} // namespace s3d
