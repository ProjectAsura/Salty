﻿//-------------------------------------------------------------------------------------------------
// File : s3d_lambert.h
// Desc : Lambert Material.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_material.h>
#include <atomic>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Lambert class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Lambert : IMaterial
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
    //! @brief      生成処理を行います.
    //---------------------------------------------------------------------------------------------
    static IMaterial* Create(const Color4& diffuse);

    //---------------------------------------------------------------------------------------------
    //! @brief      生成処理を行います.
    //---------------------------------------------------------------------------------------------
    static IMaterial* Create(const Color4& diffuse, const Color4& emissive);

    //---------------------------------------------------------------------------------------------
    //! @brief      参照カウントを増やします.
    //---------------------------------------------------------------------------------------------
    void AddRef() override;

    //---------------------------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //---------------------------------------------------------------------------------------------
    void Release() override;

    //---------------------------------------------------------------------------------------------
    //! @brief      参照カウントを取得します.
    //---------------------------------------------------------------------------------------------
    u32 GetCount() const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      シェーディングします.
    //---------------------------------------------------------------------------------------------
    Color4 Shade( ShadingArg& arg ) const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      エミッシブカラーを取得します.
    //---------------------------------------------------------------------------------------------
    Color4 GetEmissive() const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      デルタ関数をもつかどうか?
    //---------------------------------------------------------------------------------------------
    bool HasDelta() const override;

    Color4 GetBaseColor(const Vector2&) const override
    { return m_Diffuse; }

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    std::atomic<u32>    m_Count;        //!< 参照カウントです.
    Color4              m_Diffuse;      //!< ディフューズカラーです.
    Color4              m_Emissive;     //!< エミッシブカラーです.
    f32                 m_Threshold;    //!< ロシアンルーレットの閾値.

    //=============================================================================================
    // private methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    Lambert(const Color4& diffuse, const Color4& emissive);

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    ~Lambert();
};

} // namespace s3d
