﻿//-------------------------------------------------------------------------------------------------
// File : s3d_mirror.h
// Desc : Mirror Material.
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
// Mirror class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Mirror : IMaterial
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
    static IMaterial* Create(const Color4& specular);

    //---------------------------------------------------------------------------------------------
    //! @brief      生成処理を行います.
    //---------------------------------------------------------------------------------------------
    static IMaterial* Create(const Color4& specular, const Color4& emssive);

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

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    std::atomic<u32>    m_Count;        //!< 参照カウントです.
    Color4              m_Specular;     //!< スペキュラーカラーです.
    Color4              m_Emissive;     //!< エミッシブカラーです.

    //=============================================================================================
    // private methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    Mirror(const Color4& specular, const Color4& emissive);

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    ~Mirror();
};

} // namespace s3d