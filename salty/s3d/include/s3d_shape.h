//----------------------------------------------------------------------------------
// File : s3d_shape.h
// Desc : Shape Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

#ifndef __S3D_SHAPE_H__
#define __S3D_SHAPE_H__

//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------
#include <s3d_color3.h>
#include <s3d_ray.h>
#include <s3d_box.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// ShadeRec structure
////////////////////////////////////////////////////////////////////////////////////
struct ShadeRec
{
    f64     dist;
    Vector3 normal;
    Color3  color;
};


////////////////////////////////////////////////////////////////////////////////////
// IShape interface
////////////////////////////////////////////////////////////////////////////////////
struct IShape
{
public:
    virtual bool IsHit( const Ray&, const f64, const f64, ShadeRec& ) const = 0;
    virtual BoundingBox GetAABB() const = 0;
};


} // namespace s3d


#endif//__S3D_SHAPE_H__