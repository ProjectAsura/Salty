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
#include <s3d_onb.h>


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


//////////////////////////////////////////////////////////////////////////////////////
// ShadingArg structure
//////////////////////////////////////////////////////////////////////////////////////
struct ShadingArg
{
    Vector3     input;          //!< 入射方向.
    Vector3     output;         //!< 出射方向.
    Vector3     normal;         //!< 法線ベクトル.
    Vector2     texcoord;       //!< テクスチャ座標.
    Random      random;         //!< 乱数.
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

    virtual f64 GetThreshold() const = 0;

    //-------------------------------------------------------------------------------
    //! @brief      シェーディングします.
    //-------------------------------------------------------------------------------
    virtual Color ComputeColor( ShadingArg& ) const = 0;
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
    f64             threshold;      //!< 閾値です.

    //---------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------
    MaterialBase()
    : emissive  ( 0.0, 0.0, 0.0 )
    , color     ( 0.0, 0.0, 0.0 )
    , type      ( MATERIAL_TYPE_MATTE )
    , texture   ()
    , sampler   ()
    {
        threshold = Max( color.x, color.y );
        threshold = Max( color.z, threshold );
    }

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
    {
        threshold = Max( color.x, color.y );
        threshold = Max( color.z, threshold );
    }

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
    {
        threshold = Max( color.x, color.y );
        threshold = Max( color.z, threshold );
    }

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


    virtual f64 GetThreshold() const
    { return threshold; }

    virtual Color ComputeColor( ShadingArg& arg ) const
    {
        S3D_UNUSED_VAR( arg );
        return color;
    }
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

    virtual Color ComputeColor( ShadingArg& arg ) const
    {
        // 補正済み法線データ (レイの入出を考慮済み).
        const Vector3 normalMod = ( Vector3::Dot ( arg.normal, arg.input ) < 0.0 ) ? arg.normal : -arg.normal;

        OrthonormalBasis onb;
        onb.InitFromW( normalMod );

        // コサイン項を使った重点的サンプリング
        const f64 r1  = D_2PI * arg.random.GetAsF64();
        const f64 r2  = arg.random.GetAsF64();
        const f64 r2s = sqrt(r2);
        Vector3 dir = Vector3::UnitVector(
            onb.u * cos(r1) * r2s
            + onb.v * sin(r1) * r2s
            + onb.w * sqrt(1.0 - r2) );

        // 出射方向.
        arg.output = dir;

        //====================================================================
        // レンダリング方程式に対するモンテカルロ積分を考えると、
        // outgoing_radiance = weight * incoming_radiance。
        // ここで、weight = (ρ/π) * cosθ / pdf(ω) / R になる。
        // ρ/πは完全拡散面のBRDFでρは反射率、cosθはレンダリング方程式におけるコサイン項、
        // pdf(ω)はサンプリング方向についての確率密度関数。
        // Rはロシアンルーレットの確率。
        // 今、コサイン項に比例した確率密度関数によるサンプリングを行っているため、pdf(ω) = cosθ/π
        // よって、weight = ρ/ R。
        //=====================================================================

        // カラー返却
        return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) ) / threshold;
    }
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

    virtual Color ComputeColor( ShadingArg& arg ) const
    {
        // 補正済み法線データ (レイの入出を考慮済み).
        const Vector3 normalMod = ( Vector3::Dot ( arg.normal, arg.input ) < 0.0 ) ? arg.normal : -arg.normal;

       // normalModの方向を基準とした正規直交基底(w, u, v)を作る。
        // この基底に対する半球内で次のレイを飛ばす。
        OrthonormalBasis onb;
        onb.InitFromW( normalMod );

        // コサイン項を使った重点的サンプリング
        const f64 r1  = D_2PI * arg.random.GetAsF64();
        const f64 r2  = arg.random.GetAsF64();
        const f64 r2s = sqrt(r2);
        Vector3 dir = Vector3::UnitVector(
            onb.u * cos(r1) * r2s
            + onb.v * sin(r1) * r2s
            + onb.w * sqrt(1.0 - r2) );

        arg.output = dir;

        //========================================================================
        // レンダリング方程式に対するモンテカルロ積分をLambertと同様に考えると,
        // 従って Wr = color * ( A + B * cosφ * sinα * tanβ ) / q.
        // ただし, α = min( θi, θr ), β = max( θi, θr )とする.
        //========================================================================

        f64 s2 = roughness * roughness;
        f64 A = 1.0 - ( 0.5 * ( s2 / ( s2 + 0.33 ) ) );
        f64 B = 0.45 * ( s2 / ( s2 + 0.09 ) );

        f64 NV = Vector3::Dot( normalMod, arg.input );
        f64 NL = Vector3::Dot( normalMod, dir );

        Vector3 projI = Vector3::UnitVector( arg.input  - ( normalMod * NV ) );
        Vector3 projR = Vector3::UnitVector( dir - ( normalMod * NL ) );

        f64 cosPhai = Max( Vector3::Dot( projI, projR ), 0.0 );

        f64 ti    = acos( NV );
        f64 to    = acos( NL );
        f64 alpha = Max( ti, to );
        f64 beta  = Min( ti, to );
        f64 f     = A + B * cosPhai * sin( alpha ) * tan( beta );

        return Color::Mul( color, texture.Sample( sampler, arg.texcoord ) ) * f / threshold;
    }
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

    virtual Color ComputeColor( ShadingArg& arg ) const
    {
        // 補正済み法線データ (レイの入出を考慮済み).
        const Vector3 normalMod = ( Vector3::Dot ( arg.normal, arg.input ) < 0.0 ) ? arg.normal : -arg.normal;

        // ====================================================
        // 完全鏡面なのでレイの反射方向は決定的。
        // ロシアンルーレットの確率で除算するのは上と同じ。
        // ====================================================

        // 反射ベクトルを求める.
        Vector3 reflect = Vector3::Reflect( arg.input, normalMod );
        reflect.Normalize();

        arg.output = reflect;

        // 重み更新.
        return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) );
    }
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

    virtual Color ComputeColor( ShadingArg& arg ) const
    {
        // 補正済み法線データ (レイの入出を考慮済み).
        const Vector3 normalMod = ( Vector3::Dot ( arg.normal, arg.input ) < 0.0 ) ? arg.normal : -arg.normal;

        // 反射ベクトルを求める.
        Vector3 reflect = Vector3::Reflect( arg.input, arg.normal );
        reflect.Normalize();

        // レイがオブジェクトから出るのか? 入るのか?
        const bool into = ( Vector3::Dot( arg.normal, normalMod ) > 0.0 );

        // ===============
        // Snellの法則
        // ===============
                
        // 真空の屈折率
        const f64 nc    = 1.0;

        // オブジェクトの屈折率
        const f64 nt    = refractivity;

        const f64 nnt   = ( into ) ? ( nc / nt ) : ( nt / nc );
        const f64 ddn   = Vector3::Dot( arg.input, normalMod );
        const f64 cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);

        // 全反射
        if ( cos2t < 0.0 )
        {
            arg.output = reflect;

            // 重み更新.
            return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) );
        }

        // 屈折ベクトル.
        Vector3 refract = Vector3::UnitVector(
            arg.input * nnt - arg.normal * ( ( into ) ? 1.0 : -1.0 ) * ( ddn * nnt + sqrt(cos2t) ) );

        // SchlickによるFresnelの反射係数の近似を使う
        const f64 a = nt - nc;
        const f64 b = nt + nc;
        const f64 R0 = (a * a) / (b * b);

        const f64 c = 1.0 - ( ( into ) ? -ddn : Vector3::Dot( refract, arg.normal ) );
        const f64 Re = R0 + (1.0 - R0) * pow(c, 5.0); // 反射方向の光が反射してray.dirの方向に運ぶ割合。同時に屈折方向の光が反射する方向に運ぶ割合。
        const f64 Tr = ( 1.0 - Re );

        // 一定以上レイを追跡したら屈折と反射のどちらか一方を追跡する
        // ロシアンルーレットで決定する。
        const f64 P = 0.25 + 0.5 * Re;      // フレネルのグラフを参照.
                
        // 反射の場合.
        if ( arg.random.GetAsF64() < P )
        {
            arg.output = reflect;

            // 重み更新.
            return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) ) * Re / ( P * threshold );
        }
        // 屈折の場合.
        else
        {
            arg.output = refract;

            // 重み更新.
            return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) ) * Tr / ( ( 1.0 - P ) * threshold );
        }

    }
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
