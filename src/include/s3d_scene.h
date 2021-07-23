﻿//-------------------------------------------------------------------------------------------------
// File : s3d_scene.h
// Desc : Scene Data Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_shape.h>
#include <s3d_texture.h>
#include <s3d_camera.h>
#include <vector>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Scene class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Scene
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

public:
    //=============================================================================================
    // public variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // public methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    Scene()
    : m_pBVH   ( nullptr )
    , m_pCamera( nullptr )
    { /* DO_NOTHING */ }

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    virtual ~Scene()
    { /* DO_NOTHING */ }

    //---------------------------------------------------------------------------------------------
    //! @brief      カメラからレイを取得します.
    //---------------------------------------------------------------------------------------------
    S3D_INLINE
    Ray GetRay( const f32 x, const f32 y )
    { return m_pCamera->GetRay( x, y ); }

    //---------------------------------------------------------------------------------------------
    //! @brief      交差判定を行います.
    //---------------------------------------------------------------------------------------------
    S3D_INLINE
    bool Intersect( const RaySet& raySet, HitRecord& record )
    {
#if 0
        return m_pBVH->IsHit( raySet, record );
#else
        auto flag = false;
        for (size_t i = 0; i < m_Shapes.size(); ++i)
        { flag |= m_Shapes[i]->IsHit(raySet, record); }

        return flag;
#endif
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      IBLテクスチャをフェッチします.
    //---------------------------------------------------------------------------------------------
    S3D_INLINE
    Color3 SampleIBL( const Vector3& dir )
    { return m_IBL.SampleColor( dir ) * Color3( 10.0f, 10.0f, 10.0f ); }

    S3D_INLINE
    IShape* GetLight(PCG& rand)
    {
        auto idx = rand.GetAsU32();
        idx = idx % m_pLightList.size();
        return m_pLightList[idx];
    }

    virtual void Update(float time) {}

protected:
    //=============================================================================================
    // protected variables.
    //=============================================================================================
    IShape*                 m_pBVH;
    ICamera*                m_pCamera;
    Texture                 m_IBL;
    std::vector<IShape*>    m_pLightList;
    std::vector<IShape*>    m_Shapes;
    std::vector<IMaterial*> m_Material;

    //=============================================================================================
    // protected methods.
    //=============================================================================================
    /* NOTHING */
};

} // namespace s3d

