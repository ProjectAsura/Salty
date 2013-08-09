//------------------------------------------------------------------------------
// File : s3d_triangle.h
// Desc : Triangle Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __S3D_TRIANGLE_H__
#define __S3D_TRIANGLE_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <s3d_shape.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////
// Triangle class
////////////////////////////////////////////////////////////////////////////////
class Triangle : public IShape
{
public:
    Vector3 p0;
    Vector3 p1;
    Vector3 p2;
    Vector3 normal;
    Color3  color;

    Triangle( const Vector3&, const Vector3&, const Vector3&, const Color3& );
    Triangle( const Triangle& );

    bool IsHit      ( const Ray&, const f64, const f64, ShadeRec& );

private:
    void ComputeNormal();

};


} // namespace s3d


#endif//__S3D_TRIANGLE_H__