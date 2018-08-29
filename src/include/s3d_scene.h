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
#include <s3d_camera.h>
#include <s3d_ibl.h>
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
    , m_Filter ( TEXTURE_FILTER_BILINEAR )
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
    { return m_pBVH->IsHit( raySet, record ); }

    //---------------------------------------------------------------------------------------------
    //! @brief      IBLテクスチャをフェッチします.
    //---------------------------------------------------------------------------------------------
    S3D_INLINE
    Color4 SampleIBL( const Vector3& dir )
    { return m_IBL.Sample( dir, m_Filter ) * Color4( 10.0f, 10.0f, 10.0f, 1.0f ); }

    S3D_INLINE
    IShape* GetLight(Random& rand)
    {
        auto idx = rand.GetAsU32();
        idx = idx % m_pLightList.size();
        return m_pLightList[idx];
    }

protected:
    //=============================================================================================
    // protected variables.
    //=============================================================================================
    IShape*                 m_pBVH;
    ICamera*                m_pCamera;
    IBL                     m_IBL;
    TEXTURE_FILTER_MODE     m_Filter;
    std::vector<IShape*>    m_pLightList;

    //=============================================================================================
    // protected methods.
    //=============================================================================================
    /* NOTHING */
};

} // namespace s3d

