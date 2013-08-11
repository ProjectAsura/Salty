//---------------------------------------------------------------------------
// File : s3d_bvh.h
// Desc : Bounding Volume Hierarchy Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------

#ifndef __S3D_BVH_H__
#define __S3D_BVH_H__

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include <s3d_box.h>
#include <s3d_shape.h>
#include <s3d_ray.h>


namespace s3d {

//////////////////////////////////////////////////////////////////////////////
// BVH structure
//////////////////////////////////////////////////////////////////////////////
struct BVH : public IShape
{
public:
    BoundingBox aabb;
    IShape*     pLeft;
    IShape*     pRight;

    BVH();
    BVH( IShape*, IShape* );
    BVH( IShape*, IShape*, const BoundingBox& );

    void Init   ( IShape**, const s32 );
    bool IsHit  ( const Ray&, const f64, const f64, ShadeRec& );

    BoundingBox GetAABB    () const;
    IShape*     BuildBranch( IShape**, const s32, const s32 axis = 0 );
};

} // namespace s3d

#endif//__S3D_BVH_H__