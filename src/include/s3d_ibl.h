﻿//-------------------------------------------------------------------------------------------
// File : s3d_ibl.h
// Desc : Image Based Lighting Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <s3d_typedef.h>
#include <s3d_math.h>


namespace s3d {

/////////////////////////////////////////////////////////////////////////////////////////////
// IBL class
/////////////////////////////////////////////////////////////////////////////////////////////
class IBL
{
    //=======================================================================================
    // protected variables.
    //=======================================================================================
    /* NOTHING */

public:
    //=======================================================================================
    // public variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // public methods.
    //=======================================================================================

    //---------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------
    IBL();

    //---------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------
    virtual ~IBL();

    //---------------------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //---------------------------------------------------------------------------------------
    bool Init( const char* filename );

    //---------------------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //---------------------------------------------------------------------------------------
    void Term();

    //---------------------------------------------------------------------------------------
    //! @brief      フェッチします.
    //---------------------------------------------------------------------------------------
    Color Sample( const Vector3& dir );

protected:
    //=======================================================================================
    // protected variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // protected methods.
    //=======================================================================================
    /* NOTHING */

private:
    //=======================================================================================
    // private variables.
    //=======================================================================================
    s32         m_Width;        //!< 画像の横幅.
    s32         m_Height;       //!< 画像の縦幅.
    f32         m_Gamma;        //!< ガンマ値です.
    f32         m_Exposure;     //!< 露光値です.
    f32*        m_pPixels;      //!< ピクセルデータ.

    //=======================================================================================
    // privvate methods.
    //=======================================================================================
    /* NOTHING */
};

} // namespace s3d
