//-----------------------------------------------------------------------------------
// File : s3d_material.h
// Desc : Material Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

#ifndef __S3D_MATERIAL_H__
#define __S3D_MATERIAL_H__

//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------
#include <s3d_math.h>
#include <s3d_texture.h>


namespace s3d {

//-----------------------------------------------------------------------------------
// Constant Values
//-----------------------------------------------------------------------------------
const f64 REFRACTIVITY_WATER   = 1.33;  // 水の屈折率 (1.3)
const f64 REFRACTIVITY_CRYSTAL = 1.54;  // 水晶の屈折率 (1.54).
const f64 REFRACTIVITY_DIAMOND = 2.42;  // ダイアモンドの屈折率 (2.42)


/////////////////////////////////////////////////////////////////////////////////////
// MATERIAL_TYPE enum
/////////////////////////////////////////////////////////////////////////////////////
enum MATERIAL_TYPE
{
    MATERIAL_TYPE_MATTE,    // つやけし.
    MATERIAL_TYPE_CLAY,     // 粘土.
    MATERIAL_TYPE_MIRROR,   // 完全鏡面反射
    MATERIAL_TYPE_REFRACT,  // 屈折するやつ.
    //MATERIAL_TYPE_PLASTIC,  // プラスチック.
};


/////////////////////////////////////////////////////////////////////////////////////
// IMaterial interface
/////////////////////////////////////////////////////////////////////////////////////
struct IMaterial
{
    //-------------------------------------------------------------------------------
    //! @brief      マテリアルタイプを取得します.
    //-------------------------------------------------------------------------------
    virtual MATERIAL_TYPE GetType() const = 0;

    //-------------------------------------------------------------------------------
    //! @brief      自己発光カラーを取得します.
    //-------------------------------------------------------------------------------
    virtual Color GetEmissive() const = 0;

    //-------------------------------------------------------------------------------
    //! @brief      マテリアルカラーを取得します.
    //-------------------------------------------------------------------------------
    virtual Color GetColor() const = 0;

    //-------------------------------------------------------------------------------
    //! @brief      テクスチャカラーを取得します.
    //-------------------------------------------------------------------------------
    virtual Color GetTextureColor( const Vector2& ) const = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
// IRefractionMaterial interface
/////////////////////////////////////////////////////////////////////////////////////
struct IRefractionMaterial
{
    //-------------------------------------------------------------------------------
    //! @brief      屈折率を取得します.
    //-------------------------------------------------------------------------------
    virtual f64 GetRefractivity() const = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
// MaterialBase structure
/////////////////////////////////////////////////////////////////////////////////////
struct MaterialBase : public IMaterial
{
    Color           emissive;       //!< 自己発光カラーです.
    Color           color;          //!< マテリアルカラーです.
    MATERIAL_TYPE   type;           //!< マテリアルタイプです.
    Texture2D       texture;        //!< 2次元テクスチャです.
    TextureSampler  sampler;        //!< テクスチャサンプラーです.

    //---------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------
    MaterialBase()
    : emissive  ( 0.0, 0.0, 0.0 )
    , color     ( 0.0, 0.0, 0.0 )
    , type      ( MATERIAL_TYPE_MATTE )
    , texture   ()
    , sampler   ()
    { /* DO_NOTHING */ }

    //---------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------
    MaterialBase
    (
        const MATERIAL_TYPE _type,
        const Color&        _color    = Color( 0.0, 0.0, 0.0 ),
        const Color&        _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : emissive  ( _emissive )
    , color     ( _color )
    , type      ( _type )
    , texture   ()
    , sampler   ()
    { /* DO_NOTHING */ }

    //---------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //---------------------------------------------------------------------------------
    MaterialBase
    (
        const MATERIAL_TYPE _type, 
        const Color&        _color,
        const Color&        _emissive,
        const char*         _filename,
        const TextureSampler& _sampler = TextureSampler()
    )
    : emissive  ( _emissive )
    , color     ( _color )
    , type      ( _type )
    , texture   ( _filename )
    , sampler   ( _sampler )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //--------------------------------------------------------------------------------
    virtual ~MaterialBase()
    {
        texture.Release();
    }

    //--------------------------------------------------------------------------------
    //! @brief      マテリアルタイプを取得します.
    //--------------------------------------------------------------------------------
    virtual MATERIAL_TYPE GetType() const
    { return type; }

    //--------------------------------------------------------------------------------
    //! @brief      自己発行カラーを取得します.
    //--------------------------------------------------------------------------------
    virtual Color GetEmissive() const
    { return emissive; }

    //--------------------------------------------------------------------------------
    //! @brief      マテリアルカラーを取得します.
    //--------------------------------------------------------------------------------
    virtual Color GetColor() const
    { return color; }

    //--------------------------------------------------------------------------------
    //! @brief      テクスチャカラーを取得します.
    //--------------------------------------------------------------------------------
    virtual Color GetTextureColor( const Vector2& texcoord ) const
    { return texture.Sample( sampler, texcoord );  }
};


//////////////////////////////////////////////////////////////////////////////////////
// Matte structure
//////////////////////////////////////////////////////////////////////////////////////
struct Matte: public MaterialBase
{
    //--------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------
    Matte()
    : MaterialBase( MATERIAL_TYPE_MATTE )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Matte
    (
        const Color& _color,
        const Color& _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : MaterialBase( MATERIAL_TYPE_MATTE, _color, _emissive )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Matte
    (
        const Color&         _color,
        const char*          _filename,
        const TextureSampler _sampler = TextureSampler(),
        const Color&         _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : MaterialBase( MATERIAL_TYPE_MATTE, _color, _emissive, _filename, _sampler )
    { /* DO_NOTHING */ }
};



//////////////////////////////////////////////////////////////////////////////////////
// Clay structure
//////////////////////////////////////////////////////////////////////////////////////
struct Clay : public MaterialBase
{
    f64 roughness;      //!< 面の粗さです.

    //--------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------
    Clay()
    : MaterialBase( MATERIAL_TYPE_CLAY )
    , roughness( 0.0 )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Clay
    (
        const Color& _color,
        const f64    _roughness,
        const Color& _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : MaterialBase( MATERIAL_TYPE_CLAY, _color, _emissive )
    , roughness( _roughness )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Clay
    (
        const Color&        _color,
        const f64           _roughness,
        const char*         _filename,
        const Color&        _emissive  = Color( 0.0, 0.0, 0.0 ),
        const TextureSampler& _sampler = TextureSampler()
    )
    : MaterialBase( MATERIAL_TYPE_CLAY, _color, _emissive, _filename, _sampler )
    , roughness( _roughness )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief
    //--------------------------------------------------------------------------------
    virtual f64 GetRoughness() const
    { return roughness; }
};



//////////////////////////////////////////////////////////////////////////////////////
// Mirror structure
//////////////////////////////////////////////////////////////////////////////////////
struct Mirror : public MaterialBase
{
    //--------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------
    Mirror()
    : MaterialBase( MATERIAL_TYPE_MIRROR )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Mirror
    (
        const Color& _color,
        const Color& _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : MaterialBase( MATERIAL_TYPE_MIRROR, _color, _emissive )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Mirror
    (
        const Color&         _color,
        const char*          _filename,
        const TextureSampler _sampler = TextureSampler(),
        const Color&         _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : MaterialBase( MATERIAL_TYPE_MATTE, _color, _emissive, _filename, _sampler )
    { /* DO_NOTHING */ }
};


//////////////////////////////////////////////////////////////////////////////////////
// RefractionMaterial structure
//////////////////////////////////////////////////////////////////////////////////////
struct RefractionMaterial : public MaterialBase, public IRefractionMaterial
{
    f64     refractivity;       //!< 屈折率です.

    //--------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------
    RefractionMaterial
    ( 
        const f64           _refractivity
    )
    : MaterialBase( MATERIAL_TYPE_REFRACT )
    , refractivity( _refractivity )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    RefractionMaterial
    (
        const Color&        _color    = Color( 0.0, 0.0, 0.0 ),
        const Color&        _emissive = Color( 0.0, 0.0, 0.0 ),
        const f64           _refraction = 1.0
    )
    : MaterialBase( MATERIAL_TYPE_REFRACT, _color, _emissive )
    , refractivity( _refraction )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    RefractionMaterial
    (
        const Color&        _color,
        const Color&        _emissive,
        const f64           _refraction,
        const char*         _filename,
        const TextureSampler& _sampler = TextureSampler()
    )
    : MaterialBase( MATERIAL_TYPE_REFRACT, _color, _emissive, _filename, _sampler )
    , refractivity( _refraction )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      屈折率を取得します.
    //--------------------------------------------------------------------------------
    virtual f64 GetRefractivity() const
    { return refractivity; }
};


//////////////////////////////////////////////////////////////////////////////////////
// Crystal structure
//////////////////////////////////////////////////////////////////////////////////////
struct Crystal : public RefractionMaterial
{
    //--------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------
    Crystal()
    : RefractionMaterial( REFRACTIVITY_CRYSTAL )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Crystal
    (
        const Color& _color,
        const Color& _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : RefractionMaterial( _color, _emissive, REFRACTIVITY_CRYSTAL )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Crystal
    (
        const Color&         _color,
        const char*          _filename,
        const TextureSampler _sampler = TextureSampler(),
        const Color&         _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : RefractionMaterial( _color, _emissive, REFRACTIVITY_CRYSTAL, _filename, _sampler )
    { /* DO_NOTHING */ }
};


//////////////////////////////////////////////////////////////////////////////////////
// Diamon structure
//////////////////////////////////////////////////////////////////////////////////////
struct Diamond : public RefractionMaterial
{
    //--------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------
    Diamond()
    : RefractionMaterial( REFRACTIVITY_DIAMOND )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Diamond
    (
        const Color& _color,
        const Color& _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : RefractionMaterial( _color, _emissive, REFRACTIVITY_DIAMOND )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //--------------------------------------------------------------------------------
    Diamond
    (
        const Color&         _color,
        const char*          _filename,
        const TextureSampler _sampler = TextureSampler(),
        const Color&         _emissive = Color( 0.0, 0.0, 0.0 )
    )
    : RefractionMaterial( _color, _emissive, REFRACTIVITY_DIAMOND, _filename, _sampler )
    { /* DO_NOTHING */ }
};

#if 0
////////////////////////////////////////////////////////////////////////////////////////
//// PlasticMaterial structure
////////////////////////////////////////////////////////////////////////////////////////
//struct PlasticMaterial : public MaterialBase
//{
//    f64     power;      //!< 鏡面反射強度です.
//
//    //--------------------------------------------------------------------------------
//    //! @brief
//    //--------------------------------------------------------------------------------
//    PlasticMaterial()
//    : MaterialBase( MATERIAL_TYPE_PLASTIC )
//    , power       ( 0.0 )
//    {
//    }
//
//    //--------------------------------------------------------------------------------
//    //! @brief
//    //--------------------------------------------------------------------------------
//    PlasticMaterial
//    (
//        const Color& _color,
//        const f64    _power,
//        const Color& _emissive
//    )
//    : MaterialBase( MATERIAL_TYPE_PLASTIC, _color, _emissive )
//    , power( _power )
//    {
//    }
//
//    //--------------------------------------------------------------------------------
//    //! @brief
//    //--------------------------------------------------------------------------------
//    PlasticMaterial
//    (
//        const Color&        _color,
//        const f64           _power,
//        const char*         _filename,
//        const Color&        _emissive  = Color( 0.0, 0.0, 0.0 ),
//        const TextureSampler& _sampler = TextureSampler()
//    )
//    : MaterialBase( MATERIAL_TYPE_CLAY, _color, _emissive, _filename, _sampler )
//    , power( _power )
//    {
//    }
//
//    //--------------------------------------------------------------------------------
//    //! @brief
//    //--------------------------------------------------------------------------------
//    virtual f64 GetPower() const
//    { return power; }
//};
#endif


////////////////////////////////////////////////////////////////////////////////////////
// MeshMaterial structure
////////////////////////////////////////////////////////////////////////////////////////
struct MeshMaterial : public IMaterial, public IRefractionMaterial
{
    Color           diffuse;            //!< 拡散反射色です.
    Color           emissive;           //!< 自己発光色です.
    f64             refractivity;       //!< 屈折率です.
    f64             roughness;          //!< 面の粗さです.
    Texture2D       diffuseMap;         //!< ディフューズマップです.
    TextureSampler  diffuseSmp;         //!< ディフューズマップのサンプラーです.
    MATERIAL_TYPE   type;               //!< マテリアルタイプです.

    MeshMaterial()
    : diffuse       ()
    , emissive      ()
    , refractivity  ( 1.0 )
    , roughness     ( 0.0 )
    , diffuseMap    ()
    , diffuseSmp    ()
    , type          ( MATERIAL_TYPE_MATTE )
    {
    }

    MeshMaterial
    (
        Color Kd,
        Color Ke,
        f64   Re,
        f64   Kr
    )
    : diffuse       ( Kd )
    , emissive      ( Ke )
    , refractivity  ( Re )
    , roughness     ( Kr )
    , diffuseMap    ()
    , diffuseSmp    ()
    {
        if ( refractivity == 1.0 )
        {
            if ( roughness != 0.0 )
            { type = MATERIAL_TYPE_CLAY; }
            else
            { type = MATERIAL_TYPE_MATTE; }
        }
        else
        { type = MATERIAL_TYPE_REFRACT; }
    }

    MeshMaterial
    (
        Color       Kd,
        Color       Ke,
        f64         Re,
        f64         Kr,
        const char* map_Kd,
        TextureSampler& sampler = TextureSampler()
    )
    : diffuse       ( Kd )
    , emissive      ( Ke )
    , refractivity  ( Re )
    , roughness     ( Kr )
    , diffuseMap    ( map_Kd )
    , diffuseSmp    ( sampler )
    {
        if ( refractivity == 1.0 )
        {
            if ( roughness != 0.0 )
            { type = MATERIAL_TYPE_CLAY; }
            else
            { type = MATERIAL_TYPE_MATTE; }
        }
        else
        { type = MATERIAL_TYPE_REFRACT; }
    }

    //--------------------------------------------------------------------------------
    //! @brief      マテリアルタイプを取得します.
    //--------------------------------------------------------------------------------
    virtual MATERIAL_TYPE GetType() const
    { return type; }

    //--------------------------------------------------------------------------------
    //! @brief      自己発行カラーを取得します.
    //--------------------------------------------------------------------------------
    virtual Color GetEmissive() const
    { return emissive; }

    //--------------------------------------------------------------------------------
    //! @brief      マテリアルカラーを取得します.
    //--------------------------------------------------------------------------------
    virtual Color GetColor() const
    { return diffuse; }

    //--------------------------------------------------------------------------------
    //! @brief      テクスチャカラーを取得します.
    //--------------------------------------------------------------------------------
    virtual Color GetTextureColor( const Vector2& texcoord ) const
    { return diffuseMap.Sample( diffuseSmp, texcoord );  }

    //--------------------------------------------------------------------------------
    //! @brief      屈折率を取得します.
    //--------------------------------------------------------------------------------
    virtual f64 GetRefractivity() const
    { return refractivity; }

};


} // namespace s3d

#endif //__S3D_MATERIAL_H__
