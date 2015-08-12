//-------------------------------------------------------------------------------------------------
// File : s3d_scene.h
// Desc : Scene Data Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#ifndef __S3D_SCENE_H__
#define __S3D_SCENE_H__

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_shape.h>
#include <s3d_camera.h>
#include <s3d_ibl.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Scene class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Scene
{
public:
    Scene()
    : m_pBVH   ( nullptr )
    , m_pCamera( nullptr )
    {}

    Ray GetRay( const f32 x, const f32 y )
    { return m_pCamera->GetRay( x, y ); }

    bool Intersect( const Ray& ray, HitRecord& record )
    { return m_pBVH->IsHit( ray, record ); }

    Color SampleIBL( const Vector3& dir )
    { return m_IBL.Sample( dir ); }

protected:
    IShape*     m_pBVH;
    ICamera*    m_pCamera;
    IBL         m_IBL;
};

} // namespace s3d


#endif//__S3D_SCENE_H__
