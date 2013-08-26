﻿//----------------------------------------------------------------------------------
// File : s3d_texture.h
// Desc : Texture Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

#ifndef __S3D_TEXTURE_H__
#define __S3D_TEXTURE_H__

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <s3d_math.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// TEXTURE_ADDRESS_MODE enum
////////////////////////////////////////////////////////////////////////////////////
enum TEXTURE_ADDRESS_MODE
{
    TEXTURE_ADDRESS_WRAP = 0,       //!< 0.0 ～ 1.0 の範囲外の値の場合に，テクスチャ画像を繰り返し適用します.
    TEXTURE_ADDRESS_CLAMP,          //!< テクスチャ座標を 0.0 ～ 1.0 の範囲に制限します。範囲外の部分は淵部分が引き伸ばされます.
    TEXTURE_ADDRESS_BORADER,        //!< テクスチャ座標を 0.0 ～ 1.0 の範囲に制限します。範囲外の部分は境界色になります.
};


////////////////////////////////////////////////////////////////////////////////////
// TextureSampler
////////////////////////////////////////////////////////////////////////////////////
struct TextureSampler
{
    TEXTURE_ADDRESS_MODE addressU;          //!< テクスチャU座標のアドレッシングモードです.
    TEXTURE_ADDRESS_MODE addressV;          //!< テクスチャV座標のアドレッシングモードです.
    Color                boarderColor;      //!< 境界色です.

    //------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //------------------------------------------------------------------------------
    TextureSampler()
    : addressU      ( TEXTURE_ADDRESS_WRAP )
    , addressV      ( TEXTURE_ADDRESS_WRAP )
    , boarderColor  ( 0.0f, 0.0f, 0.0f )
    { /* DO_NOTHING */ }

    //------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //------------------------------------------------------------------------------
    TextureSampler
    ( 
        const TEXTURE_ADDRESS_MODE _addressU,
        const TEXTURE_ADDRESS_MODE _addressV,
        const Color&               _boarderColor
    )
    : addressU      ( _addressU )
    , addressV      ( _addressV )
    , boarderColor  ( _boarderColor )
    { /* DO_NOTHING */ }
};


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
    Color Sample( const TextureSampler&, const Vector2& ) const;

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
    u32     m_Width;            //!< 画像の横幅です.
    u32     m_Height;           //!< 画像の縦幅です.
    u32     m_Size;             //!< データサイズです.
    f32*    m_pPixels;          //!< ピクセルデータです.

    //==============================================================================
    // private methods.
    //==============================================================================
    /* NOTHING */
};


} // namespace s3d


#endif//__S3D_TEXTURE_H__