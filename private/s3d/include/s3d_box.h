//----------------------------------------------------------------------------------
// File : s3d_box.h
// Desc : Bounding Box Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

#ifndef __S3D_BOX_H__
#define __S3D_BOX_H__

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <s3d_ray.h>


namespace s3d {

/////////////////////////////////////////////////////////////////////////////////////
// BoundingBox structure
/////////////////////////////////////////////////////////////////////////////////////
struct BoundingBox
{
public:
    Vector3 min;
    Vector3 max;

    BoundingBox();
    BoundingBox( const Vector3&, const Vector3& );
    BoundingBox( const BoundingBox& );

    bool IsHit( const Ray&, const f64, const f64 ) const;

    static BoundingBox Merge( const BoundingBox&, const BoundingBox& );
};

} // namespace s3d

//-----------------------------------------------------------------------------------
// Inlines
//-----------------------------------------------------------------------------------
#include <s3d_box.inl>

#endif//__S3D_BOX_H__