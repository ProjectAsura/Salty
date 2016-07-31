//------------------------------------------------------------------------------
// File : s3d_material.cpp
// Desc : Material Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <s3d_material.h>


namespace /* anonymous */ {

//-----------------------------------------------------------------------------------
// Constant Values
//-----------------------------------------------------------------------------------
const f32 REFRACTIVITY_WATER   = 1.33f;  // 水の屈折率 (1.3)
const f32 REFRACTIVITY_CRYSTAL = 1.54f;  // 水晶の屈折率 (1.54).
const f32 REFRACTIVITY_DIAMOND = 2.42f;  // ダイアモンドの屈折率 (2.42)

}// namespace /* anonymous */


namespace s3d {

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
MaterialBase::MaterialBase()
: emissive  ( 0.0f, 0.0f, 0.0f, 1.0f )
, color     ( 0.0f, 0.0f, 0.0f, 1.0f )
, pTexture  ( nullptr )
, pSampler  ( nullptr )
, threshold ( 0.0 )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
MaterialBase::MaterialBase
(
    const Color4& _color,
    const Color4& _emissive = Color4( 0.0f, 0.0f, 0.0f, 1.0f )
)
: emissive  ( _emissive )
, color     ( _color )
, pTexture  ( nullptr )
, pSampler  ( nullptr )
{
    threshold = Max( color.GetX(), color.GetY() );
    threshold = Max( color.GetZ(), threshold );
}

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
MaterialBase::MaterialBase
(
    const Color4&           _color,
    const Color4&           _emissive,
    const Texture2D*        _pTexture,
    const TextureSampler*   _pSampler
)
: emissive  ( _emissive )
, color     ( _color )
, pTexture  ( _pTexture )
, pSampler  ( _pSampler )
{
    threshold = Max( color.GetX(), color.GetY() );
    threshold = Max( color.GetZ(), threshold );
}

//--------------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------------
MaterialBase::~MaterialBase()
{
    pTexture = nullptr;
    pSampler = nullptr;
}

//--------------------------------------------------------------------------------
//      自己発光色を取得します.
//--------------------------------------------------------------------------------
Color4 MaterialBase::GetEmissive() const
{ return emissive; }

//--------------------------------------------------------------------------------
//      ロシアンルーレットの閾値を取得します.
//--------------------------------------------------------------------------------
f32 MaterialBase::GetThreshold() const
{ return threshold; }

//--------------------------------------------------------------------------------
//     色を求めます.
//--------------------------------------------------------------------------------
Color4 MaterialBase::Shade( ShadingArg& arg ) const
{
    S3D_UNUSED_VAR( arg );
    return color;
}

//////////////////////////////////////////////////////////////////////////////////
// Matte structure
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
Matte::Matte()
: MaterialBase()
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Matte::Matte
(
    const Color4& _color,
    const Color4& _emissive
)
: MaterialBase( _color, _emissive )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Matte::Matte
(
    const Color4&            _color,
    const Color4&            _emissive,
    const Texture2D*        _pTexture,
    const TextureSampler*   _pSampler
)
: MaterialBase( _color, _emissive, _pTexture, _pSampler )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      色を求めます.
//--------------------------------------------------------------------------------
Color4 Matte::Shade( ShadingArg& arg ) const
{
    // ========================
    // Lambert BRDF.
    // ========================

    // 補正済み法線データ (レイの入出を考慮済み).
    const Vector3 normalMod = ( Vector3::Dot ( arg.normal, arg.input ) < 0.0 ) ? arg.normal : -arg.normal;

    // normalModの方向を基準とした正規直交基底(w, u, v)を作る。
    // この基底に対する半球内で次のレイを飛ばす。
    OrthonormalBasis onb;
    onb.InitFromW( normalMod );

    // インポータンスサンプリング.
    const f32 phi = F_2PI * arg.random.GetAsF32();
    const f32 r = SafeSqrt( arg.random.GetAsF32() );
    const f32 x = r * cosf( phi );
    const f32 y = r * sinf( phi );
    const f32 z = SafeSqrt( 1.0f - ( x * x ) - ( y * y ) );

    // 出射方向.
    Vector3 dir = Vector3::UnitVector( onb.u * x + onb.v * y + onb.w * z );
    arg.output = dir;

    // 重み更新 (飛ぶ方向が不定なので確率で割る必要あり).
    if ( pTexture != nullptr && pSampler != nullptr )
    {
        return Color4::Mul( color, pTexture->Sample( (*pSampler), arg.texcoord ) ) / arg.prob;
    }

    return color / arg.prob;
}


//////////////////////////////////////////////////////////////////////////////////
// Mirror structure
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
Mirror::Mirror()
: MaterialBase()
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Mirror::Mirror
(
    const Color4& _color,
    const Color4& _emissive
)
: MaterialBase( _color, _emissive )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Mirror::Mirror
(
    const Color4&           _color,
    const Color4&           _emissive,
    const Texture2D*        _pTexture,
    const TextureSampler*   _pSampler
)
: MaterialBase( _color, _emissive, _pTexture, _pSampler )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      ロシアンルーレットに使用する閾値です.
//--------------------------------------------------------------------------------
float Mirror::GetThreshold() const
{ return 1.0f; }

//--------------------------------------------------------------------------------
//      色を求めます.
//--------------------------------------------------------------------------------
Color4 Mirror::Shade( ShadingArg& arg ) const
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

    // 出射方向.
    arg.output = reflect;

    // 重み更新 (飛ぶ方向が確定しているので，確率100%).
    if ( pTexture != nullptr && pSampler != nullptr )
    {
        return Color4::Mul( color, pTexture->Sample( (*pSampler), arg.texcoord ) );
    }

    return color;
}

//////////////////////////////////////////////////////////////////////////////////
// Dielectric structure
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
Dielectric::Dielectric()
: MaterialBase()
, ior( 1.0f )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Dielectric::Dielectric
(
    const f32     _ior,
    const Color4& _color,
    const Color4& _emissive
)
: MaterialBase( _color, _emissive )
, ior( _ior )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Dielectric::Dielectric
(
    const f32               _ior,
    const Color4&           _color,
    const Color4&           _emissive,
    const Texture2D*        _pTexture,
    const TextureSampler*   _pSampler
)
: MaterialBase( _color, _emissive, _pTexture, _pSampler )
, ior( _ior )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      ロシアンルーレットに使用する閾値です.
//--------------------------------------------------------------------------------
float Dielectric::GetThreshold() const 
{ return 1.0f; }

//--------------------------------------------------------------------------------
//      色を求めます.
//--------------------------------------------------------------------------------
Color4 Dielectric::Shade( ShadingArg& arg ) const
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
    const f32 nc    = 1.0f;

    // オブジェクトの屈折率
    const f32 nt    = ior;

    const f32 nnt   = ( into ) ? ( nc / nt ) : ( nt / nc );
    const f32 ddn   = Vector3::Dot( arg.input, normalMod );
    const f32 cos2t = 1.0f - nnt * nnt * (1.0f - ddn * ddn);

    // 全反射
    if ( cos2t < 0.0f )
    {
        // 出射方向.
        arg.output = reflect;

        // 重み更新 (飛ぶ方向が確定しているので，確率100%).
        if ( pTexture != nullptr && pSampler != nullptr )
        {
            return Color4::Mul( color, pTexture->Sample( (*pSampler), arg.texcoord ) );
        }

        return color;
    }

    // 屈折ベクトル.
    Vector3 refract = Vector3::UnitVector(
        arg.input * nnt - arg.normal * ( ( into ) ? 1.0f : -1.0f ) * ( ddn * nnt + SafeSqrt(cos2t) ) );

    // SchlickによるFresnelの反射係数の近似を使う
    const f32 a = nt - nc;
    const f32 b = nt + nc;
    const f32 R0 = (a * a) / (b * b);

    const f32 c = 1.0f - ( ( into ) ? -ddn : Vector3::Dot( refract, arg.normal ) );
    const f32 Re = R0 + (1.0f - R0) * ( c * c * c * c * c ); // 反射方向の光が反射してray.dirの方向に運ぶ割合。同時に屈折方向の光が反射する方向に運ぶ割合。
    const f32 Tr = ( 1.0f - Re );

    // 一定以上レイを追跡したら屈折と反射のどちらか一方を追跡する
    // ロシアンルーレットで決定する。
    const f32 P = 0.25f + 0.5f * Re;      // フレネルのグラフを参照.

    // 反射の場合.
    if ( arg.random.GetAsF32() < P )
    {
        // 出射方向.
        arg.output = reflect;

        // 重み更新.
        if ( pTexture != nullptr && pSampler != nullptr )
        {
            return Color4::Mul( color, pTexture->Sample( (*pSampler), arg.texcoord ) ) * Re / ( P * arg.prob );
        }

        return color * Re / ( P * arg.prob );
    }
    // 屈折の場合.
    else
    {
        // 出射方向.
        arg.output = refract;

        // 重み更新.
        if ( pTexture != nullptr && pSampler != nullptr )
        {
            return Color4::Mul( color, pTexture->Sample( (*pSampler), arg.texcoord ) ) * Tr / ( ( 1.0f - P ) * arg.prob );
        }

        return color * Tr / ( ( 1.0f - P ) * arg.prob );
    }
}


//////////////////////////////////////////////////////////////////////////////////
// Glossy structure
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
Glossy::Glossy()
: emissive  ( 0.0f, 0.0f, 0.0f, 1.0f )
, specular  ( 0.0f, 0.0f, 0.0f, 1.0f )
, power     ( 0.0f )
, pTexture  ( nullptr )
, pSampler  ( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Glossy::Glossy
(
    const Color4& _specular,
    const f32     _power,
    const Color4& _emissive
)
: emissive  ( _emissive )
, specular  ( _specular )
, power     ( _power )
, pTexture  ( nullptr )
, pSampler  ( nullptr )
{
    threshold = Max( specular.GetX(), specular.GetY() );
    threshold = Max( specular.GetZ(), threshold );
}

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Glossy::Glossy
(
    const Color4&          _specular,
    const f32               _power,
    const Color4&           _emissive,
    const Texture2D*        _pTexture,
    const TextureSampler*   _pSampler
)
: emissive( _emissive )
, specular( _specular )
, power   ( _power )
, pTexture( _pTexture )
, pSampler( _pSampler )
{
    threshold = Max( specular.GetX(), specular.GetY() );
    threshold = Max( specular.GetZ(), threshold );
}

//--------------------------------------------------------------------------------
//      自己発光色を取得します.
//--------------------------------------------------------------------------------
Color4 Glossy::GetEmissive() const
{ return emissive; }

//--------------------------------------------------------------------------------
//      ロシアンルーレットの閾値を取得します.
//--------------------------------------------------------------------------------
f32 Glossy::GetThreshold() const
{ return threshold; }

//---------------------------------------------------------------------------------
//      色を取得します.
//---------------------------------------------------------------------------------
Color4 Glossy::Shade( ShadingArg& arg ) const
{
    // 補正済み法線データ (レイの入出を考慮済み).
    const Vector3 normalMod = ( Vector3::Dot ( arg.normal, arg.input ) < 0.0 ) ? arg.normal : -arg.normal;

    Vector3 dir;
    f32 dots = 0.0f;

    {
        // インポータンスサンプリング.
        const f32 phi = F_2PI * arg.random.GetAsF32();
        const f32 cosTheta = powf( 1.0f - arg.random.GetAsF32(), 1.0f / ( power + 1.0f ) );
        const f32 sinTheta = SafeSqrt( 1.0f - ( cosTheta * cosTheta ) );
        const f32 x = cosf( phi ) * sinTheta;
        const f32 y = sinf( phi ) * sinTheta;
        const f32 z = cosTheta;

        // 反射ベクトル.
        Vector3 w = Vector3::Reflect( arg.input, normalMod );
        w.Normalize();

        // 基底ベクトルを求める.
        OrthonormalBasis onb;
        onb.InitFromW( w );

        // 出射方向.
        dir = Vector3::UnitVector( onb.u * x + onb.v * y + onb.w * z );

        // 出射方向と法線ベクトルの内積を求める.
        dots = Vector3::Dot( dir, normalMod );
    }

    // 出射方向を設定.
    arg.output = dir;

    // 重み更新.
    if ( pTexture != nullptr && pSampler != nullptr )
    {
        return Color4::Mul( specular, pTexture->Sample( (*pSampler), arg.texcoord ) ) * dots;
    }

    return specular * dots;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Plastric structure
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
Plastic::Plastic()
: diffuse    ( 0.0f, 0.0f, 0.0f, 1.0f )
, specular   ( 0.0f, 0.0f, 0.0f, 1.0f )
, power      ( 1.0f )
, emissive   ( 0.0f, 0.0f, 0.0f, 1.0f )
, pDiffuseMap( nullptr )
, pDiffuseSmp( nullptr )
, threshold  ( 0.0f )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-------------------------------------------------------------------------------------------------
Plastic::Plastic
(
    const Color4&           dif,
    const Color4&           spe,
    const f32               pow,
    const Color4&           emi,
    const Texture2D*        tex,
    const TextureSampler*   smp
)
: diffuse    ( dif )
, specular   ( spe )
, power      ( pow )
, emissive   ( emi )
, pDiffuseMap( tex )
, pDiffuseSmp( smp )
{
    threshold = Max( diffuse.GetX(), diffuse.GetY() );
    threshold = Max( diffuse.GetZ(), threshold );
    threshold = Max( specular.GetX(), threshold );
    threshold = Max( specular.GetY(), threshold );
    threshold = Max( specular.GetZ(), threshold );
}

//-------------------------------------------------------------------------------------------------
//      自己発光カラーを取得します.
//-------------------------------------------------------------------------------------------------
Color4 Plastic::GetEmissive() const
{
    return emissive;
}

//-------------------------------------------------------------------------------------------------
//      ロシアンルーレットの閾値を取得します.
//-------------------------------------------------------------------------------------------------
f32 Plastic::GetThreshold() const
{
    return threshold;
}

//-------------------------------------------------------------------------------------------------
//      シェーディングします.
//-------------------------------------------------------------------------------------------------
Color4 Plastic::Shade( ShadingArg& arg ) const
{
    // 補正済み法線データ (レイの入出を考慮済み).
    auto cosine = Vector3::Dot( arg.normal, arg.input );
    const Vector3 normalMod = ( cosine < 0.0 ) ? arg.normal : -arg.normal;

    if ( cosine < 0.0f ) cosine = -cosine;

    auto temp1 = 1.0f - cosine;
    const auto R0 = 0.5f;
    auto R = R0 + ( 1.0f - R0 ) * temp1 * temp1 * temp1 * temp1 * temp1;
    auto P = ( R + 0.5f ) / 2.0f;

    if ( arg.random.GetAsF32() <= P )
    {
        // normalModの方向を基準とした正規直交基底(w, u, v)を作る。
        // この基底に対する半球内で次のレイを飛ばす。
        OrthonormalBasis onb;
        onb.InitFromW( normalMod );

        // インポータンスサンプリング.
        const f32 phi = F_2PI * arg.random.GetAsF32( );
        const f32 r = SafeSqrt( arg.random.GetAsF32( ) );
        const f32 x = r * cosf( phi );
        const f32 y = r * sinf( phi );
        const f32 z = SafeSqrt( 1.0f - ( x * x ) - ( y * y ) );

        // 出射方向.
        Vector3 dir = Vector3::UnitVector( onb.u * x + onb.v * y + onb.w * z );
        arg.output = dir;

        // 重み更新 (飛ぶ方向が不定なので確率で割る必要あり).
        Color4 result;
        if ( pDiffuseMap != nullptr )
        { result = Color4::Mul( diffuse, pDiffuseMap->Sample( ( *pDiffuseSmp ), arg.texcoord ) ) * R / P; }
        else
        { result = diffuse  * R / P; }

        return result;
    }
    else
    {
        // インポータンスサンプリング.
        const f32 phi = F_2PI * arg.random.GetAsF32();
        const f32 cosTheta = powf( 1.0f - arg.random.GetAsF32(), 1.0f / ( power + 1.0f ) );
        const f32 sinTheta = SafeSqrt( 1.0f - ( cosTheta * cosTheta ) );
        const f32 x = cosf( phi ) * sinTheta;
        const f32 y = sinf( phi ) * sinTheta;
        const f32 z = cosTheta;

        // 反射ベクトル.
        Vector3 w = Vector3::Reflect( arg.input, normalMod );
        w.Normalize();

        // 基底ベクトルを求める.
        OrthonormalBasis onb;
        onb.InitFromW( w );

        // 出射方向.
        auto dir = Vector3::UnitVector( onb.u * x + onb.v * y + onb.w * z );

        // 出射方向と法線ベクトルの内積を求める.
        auto dots = Vector3::Dot( dir, normalMod );

        arg.output = dir;

        return specular * dots * ( 1.0f - R ) / ( 1.0f - P );
    }
}

} // namespace s3d
