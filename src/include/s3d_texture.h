﻿//----------------------------------------------------------------------------------
// File : s3d_texture.h
// Desc : Texture Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------
#pragma once

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <s3d_math.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// Texture2D class
////////////////////////////////////////////////////////////////////////////////////
class Texture2D
{
    //==============================================================================
    // list of friend classes and methods.
    //==============================================================================
    /* NOTHING */

public:
    //==============================================================================
    // public variables.
    //==============================================================================
    /* NOTHING */

    //==============================================================================
    // public methods.
    //==============================================================================

    //------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //------------------------------------------------------------------------------
    Texture2D();

    //------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //------------------------------------------------------------------------------
    Texture2D( const char* );

    //------------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //------------------------------------------------------------------------------
    Texture2D( const Texture2D& );

    //------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //------------------------------------------------------------------------------
    virtual ~Texture2D();

    //------------------------------------------------------------------------------
    //! @brief      ファイルからロードします.
    //------------------------------------------------------------------------------
    bool LoadFromFile( const char* );

    //------------------------------------------------------------------------------
    //! @brief      メモリ解放処理を行います.
    //------------------------------------------------------------------------------
    void Release();

    //------------------------------------------------------------------------------
    //! @brief      テクスチャフェッチします.
    //------------------------------------------------------------------------------
    Color4 Sample(const Vector2& ) const;

    //------------------------------------------------------------------------------
    //! @brief      アルファテストを行います.
    //------------------------------------------------------------------------------
    bool AlphaTest(const Vector2&, const f32 value ) const;

protected:
    //==============================================================================
    // protected variables.
    //==============================================================================
    /* NOTHING */

    //==============================================================================
    // protected methods.
    //==============================================================================
    /* NOTHING */

private:
    //==============================================================================
    // private variables.
    //==============================================================================
    s32     m_Width;            //!< 画像の横幅です.
    s32     m_Height;           //!< 画像の縦幅です.
    u32     m_Size;             //!< データサイズです.
    u32     m_ComponentCount;   //!< チャンネル数です(RGB=3, RGBA=4)
    f32*    m_pPixels;          //!< ピクセルデータです.

    //==============================================================================
    // private methods.
    //==============================================================================

    //------------------------------------------------------------------------------
    //! @brief      指定されたピクセルを取得します.
    //------------------------------------------------------------------------------
    inline Color4 GetPixel( s32 x, s32 y ) const
    {
        auto idx = 4 * ((m_Width * y) + x);
        assert( idx < m_Size );

        return Color4(
            m_pPixels[idx + 0],
            m_pPixels[idx + 1],
            m_pPixels[idx + 2],
            m_pPixels[idx + 3] );
    }
};


} // namespace s3d
