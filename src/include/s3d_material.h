﻿//-------------------------------------------------------------------------------------------------
// File : s3d_material.h
// Desc : Material Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_math.h>
#include <s3d_texture.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// ShadingArg structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ShadingArg
{
    Vector3     input;          //!< 入射方向.
    Vector3     output;         //!< 出射方向.
    Vector3     normal;         //!< 法線ベクトル.
    Vector2     texcoord;       //!< テクスチャ座標.
    Random      random;         //!< 乱数.
    f32         prob;           //!< 確率.
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// IMaterial interface
///////////////////////////////////////////////////////////////////////////////////////////////////
struct IMaterial
{
    //---------------------------------------------------------------------------------------------
    //! @brief      自己発光カラーを取得します.
    //---------------------------------------------------------------------------------------------
    virtual Color GetEmissive() const = 0;

    //---------------------------------------------------------------------------------------------
    //! @brief      ロシアンルーレットの閾値を取得します.
    //---------------------------------------------------------------------------------------------
    virtual f32 GetThreshold() const = 0;

    //---------------------------------------------------------------------------------------------
    //! @brief      シェーディングします.
    //---------------------------------------------------------------------------------------------
    virtual Color ComputeColor( ShadingArg& ) const = 0;

    //---------------------------------------------------------------------------------------------
    //! @brief      アルファテストを行います.
    //---------------------------------------------------------------------------------------------
    virtual bool AlphaTest( const Vector2&, const f32 ) const = 0;

#if 1
    virtual Color GetDebugColor() const
    { return Color( 1.0f, 0.0f, 0.0f ); }
#endif
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// MaterialBase structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MaterialBase : public IMaterial
{
    Color                   emissive;       //!< 自己発光カラーです.
    Color                   color;          //!< マテリアルカラーです.
    const Texture2D*        pTexture;       //!< 2次元テクスチャです.
    const TextureSampler*   pSampler;       //!< テクスチャサンプラーです.
    f32                     threshold;      //!< 閾値です.

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    MaterialBase();

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    MaterialBase( const Color&, const Color& );

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    MaterialBase
    (
        const Color&,
        const Color&,
        const Texture2D*,
        const TextureSampler*
    );

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    virtual ~MaterialBase();

    //---------------------------------------------------------------------------------------------
    //! @brief      自己発行カラーを取得します.
    //---------------------------------------------------------------------------------------------
    virtual Color GetEmissive() const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      ロシアンルーレットの閾値を取得します.
    //---------------------------------------------------------------------------------------------
    virtual f32 GetThreshold() const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      色を求めます.
    //---------------------------------------------------------------------------------------------
    virtual Color ComputeColor( ShadingArg& arg ) const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      アルファテストを行います.
    //---------------------------------------------------------------------------------------------
    virtual bool AlphaTest( const Vector2& texcoord, const f32 value ) const override;

#if 1
    virtual Color GetDebugColor() const
    { return color; }
#endif
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Matte structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Matte : public MaterialBase
{
    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    Matte();

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    Matte( const Color&, const Color& );

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    Matte
    ( 
        const Color&,
        const Color&,
        const Texture2D*,
        const TextureSampler*
    );

    //---------------------------------------------------------------------------------------------
    //! @brief      色を求めます.
    //---------------------------------------------------------------------------------------------
    Color ComputeColor( ShadingArg& arg ) const override;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Mirror structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Mirror : public MaterialBase
{
    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    Mirror();

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    Mirror( const Color&, const Color& = Color( 0.0f, 0.0f, 0.0f ) );

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    Mirror
    (
        const Color&,
        const Color&,
        const Texture2D*,
        const TextureSampler*
    );

    //---------------------------------------------------------------------------------------------
    //! @brief      色を求めます.
    //---------------------------------------------------------------------------------------------
    Color ComputeColor( ShadingArg& arg ) const override;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Glass structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Glass : public MaterialBase
{
    f32     ior;       //!< 屈折率です(Index Of Refraction).

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    Glass();

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    Glass
    (
        const f32,
        const Color&,
        const Color& = Color( 0.0f, 0.0f, 0.0f )
    );

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    Glass
    (
        const f32,
        const Color&,
        const Color&,
        const Texture2D*,
        const TextureSampler*
    );

    //---------------------------------------------------------------------------------------------
    //! @brief      色を求めます.
    //---------------------------------------------------------------------------------------------
    Color ComputeColor( ShadingArg& arg ) const override;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Glossy structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Glossy : public IMaterial
{
    Color                   emissive;       //!< 自己発光カラーです.
    Color                   specular;       //!< 鏡面反射色です.
    f32                     power;          //!< 鏡面反射強度です.
    f32                     threshold;      //!< 閾値です.
    const Texture2D*        pTexture;       //!< 2次元テクスチャです.
    const TextureSampler*   pSampler;       //!< テクスチャサンプラーです.

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    Glossy();

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    Glossy
    ( 
        const Color&,
        const f32,
        const Color& = Color( 0.0f, 0.0f, 0.0f )
    );

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------------------
    Glossy
    (
        const Color&,
        const f32,
        const Color&,
        const Texture2D*,
        const TextureSampler*
    );

    //---------------------------------------------------------------------------------------------
    //! @brief      自己発行カラーを取得します.
    //---------------------------------------------------------------------------------------------
    Color GetEmissive() const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      ロシアンルーレットの閾値を取得します.
    //---------------------------------------------------------------------------------------------
    f32 GetThreshold() const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      色を求めます.
    //---------------------------------------------------------------------------------------------
    Color ComputeColor( ShadingArg& ) const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      アルファテストを行います.
    //---------------------------------------------------------------------------------------------
    bool AlphaTest( const Vector2& texcoord, const f32 value ) const override;

#if 1
    Color GetDebugColor() const override
    { return Color( 0.0f, 1.0f, 0.0f ); }
#endif
};


} // namespace s3d
