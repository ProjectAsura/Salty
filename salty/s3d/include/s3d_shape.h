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
#include <s3d_ray.h>
#include <s3d_box.h>
#include <s3d_color3.h>


namespace s3d {

//----------------------------------------------------------------------------------
// Forward Declarations.
//----------------------------------------------------------------------------------
struct IMaterial;


////////////////////////////////////////////////////////////////////////////////////
// ShadeRec structure
////////////////////////////////////////////////////////////////////////////////////
struct ShadeRec
{
    f64         dist;
    Vector3     normal;
    IMaterial*  pMaterial;

    ShadeRec()
    : dist      ( 1.7976931348623158e+308 )     // DBL_MAX.
    , normal    ( 0.0, 0.0, 0.0 )
    , pMaterial ( nullptr )
    { /* DO_NOTHING */ }

    ~ShadeRec()
    { pMaterial = nullptr; }
};


////////////////////////////////////////////////////////////////////////////////////
// IShape interface
////////////////////////////////////////////////////////////////////////////////////
struct IShape
{
public:
    virtual bool        IsHit   ( const Ray&, const f64, const f64, ShadeRec& ) = 0;
    virtual BoundingBox GetAABB () const = 0;
};


} // namespace s3d


#endif//__S3D_SHAPE_H__