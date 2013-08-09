//---------------------------------------------------------------------------------
// File : s3d_sphere.h
// Desc : Sphere Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

#ifndef __S3D_SPHERE_H__
#define __S3D_SPHERE_H__

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <s3d_shape.h>

namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// Sphere class
////////////////////////////////////////////////////////////////////////////////////
class Sphere : public IShape
{
public:
    Vector3 center;
    f64     radius;
    Color3  color;

    Sphere( const Vector3&, const f64, const Color3& );
    Sphere( const Sphere& );

    bool IsHit( const Ray&, const f64, const f64, ShadeRec& );
};

} // namespace s3d


#endif//__S3D_SPHERE_H__