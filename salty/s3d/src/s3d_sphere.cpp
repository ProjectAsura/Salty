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

Sphere::Sphere( const Vector3& cent, const f64 rad, const Color3& col )
: center( cent )
, radius( rad )
, color ( col )
{ /* DO_NOTHING */ }

Sphere::Sphere( const Sphere& value )
: center( value.center )
, radius( value.radius )
, color ( value.color )
{ /* DO_NOTHING */ }

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
    shadeRec.normal = ( ray.Step(dist) - center ).UnitVector();
    shadeRec.color  = color;

    return true;
}

} // namespace s3d
