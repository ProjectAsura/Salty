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
Sphere::Sphere( const Vector3& cent, const f64 rad, const Color3& col )
: center( cent )
, radius( rad )
, color ( col )
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------
Sphere::Sphere( const Sphere& value )
: center( value.center )
, radius( value.radius )
, color ( value.color )
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      衝突判定を行います.
//-----------------------------------------------------------------------------
bool Sphere::IsHit( const Ray& ray, const f64 mini, const f64 maxi, ShadeRec& shadeRec )
{
    Vector3      d = center - ray.GetPos();
    register f64 b = Vector3::Dot( d, ray.GetDir() );
    register f64 det = ( b * b ) - Vector3::Dot( d, d ) + ( radius * radius );

    if ( det <= 0.0 )
    { return false; }
    
    det = sqrt( det );
    
    register f64 dist = b - det;
    if ( dist < mini || dist > maxi)
    {
        dist = b + det;
        if ( dist < mini || dist > maxi )
        { return false; }
    }
    
    shadeRec.dist   = dist;
    shadeRec.normal = Vector3::UnitVector( ray.Step(dist) - center );
    shadeRec.color  = color;

    return true;
}

} // namespace s3d
