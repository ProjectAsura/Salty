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
: emissive  ( 0.0f, 0.0f, 0.0f )
, color     ( 0.0f, 0.0f, 0.0f )
, texture   ()
, sampler   ()
, threshold ( 0.0 )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
MaterialBase::MaterialBase
(
    const Color& _color,
    const Color& _emissive = Color( 0.0f, 0.0f, 0.0f )
)
: emissive  ( _emissive )
, color     ( _color )
, texture   ()
, sampler   ()
{
    threshold = Max( color.x, color.y );
    threshold = Max( color.z, threshold );
}

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
MaterialBase::MaterialBase
(
    const Color&         _color,
    const Color&         _emissive,
    const char*          _filename,
    const TextureSampler& _sampler
)
: emissive  ( _emissive )
, color     ( _color )
, texture   ( _filename )
, sampler   ( _sampler )
{
    threshold = Max( color.x, color.y );
    threshold = Max( color.z, threshold );
}

//--------------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------------
MaterialBase::~MaterialBase()
{
    texture.Release();
}

//--------------------------------------------------------------------------------
//      自己発光色を取得します.
//--------------------------------------------------------------------------------
Color MaterialBase::GetEmissive() const
{ return emissive; }

//--------------------------------------------------------------------------------
//      ロシアンルーレットの閾値を取得します.
//--------------------------------------------------------------------------------
f32 MaterialBase::GetThreshold() const
{ return threshold; }

//--------------------------------------------------------------------------------
//     色を求めます.
//--------------------------------------------------------------------------------
Color MaterialBase::ComputeColor( ShadingArg& arg ) const
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
, roughness( 0.0 )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Matte::Matte
(
    const Color& _color,
    const f32    _roughness,
    const Color& _emissive
)
: MaterialBase( _color, _emissive )
, roughness( _roughness )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Matte::Matte
(
    const Color&        _color,
    const f32           _roughness,
    const char*         _filename,
    const Color&        _emissive,
    const TextureSampler& _sampler
)
: MaterialBase( _color, _emissive, _filename, _sampler )
, roughness( _roughness )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      色を求めます.
//--------------------------------------------------------------------------------
Color Matte::ComputeColor( ShadingArg& arg ) const
{
    // 補正済み法線データ (レイの入出を考慮済み).
    const Vector3 normalMod = ( Vector3::Dot ( arg.normal, arg.input ) < 0.0 ) ? arg.normal : -arg.normal;

    // normalModの方向を基準とした正規直交基底(w, u, v)を作る。
    // この基底に対する半球内で次のレイを飛ばす。
    OrthonormalBasis onb;
    onb.InitFromW( normalMod );

    // コサイン項を使った重点的サンプリング
    const f32 r1  = F_2PI * arg.random.GetAsF32();
    const f32 r2  = arg.random.GetAsF32();
    const f32 r2s = sqrtf(r2);
    Vector3 dir = Vector3::UnitVector(
          onb.u * cosf(r1) * r2s
        + onb.v * sinf(r1) * r2s
        + onb.w * sqrtf(1.0f - r2) );

    // 出射方向設定.
    arg.output = dir;

    // Oren-Nayer BRDF
    f32 s2 = roughness * roughness;
    f32 A = 1.0f - ( 0.5f * ( s2 / ( s2 + 0.33f ) ) );
    f32 B = 0.45f * ( s2 / ( s2 + 0.09f ) );

    f32 NV = Vector3::Dot( normalMod, arg.input );
    f32 NL = Vector3::Dot( normalMod, dir );

    Vector3 projI = Vector3::UnitVector( arg.input  - ( normalMod * NV ) );
    Vector3 projR = Vector3::UnitVector( dir - ( normalMod * NL ) );

    f32 cosPhai = Max( Vector3::Dot( projI, projR ), 0.0 );

    f32 ti    = acosf( NV );
    f32 to    = acosf( NL );
    f32 alpha = Max( ti, to );
    f32 beta  = Min( ti, to );
    f32 f     = A + B * cosPhai * sin( alpha ) * tan( beta );

    return Color::Mul( color, texture.Sample( sampler, arg.texcoord ) ) * f / arg.prob;
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
    const Color& _color,
    const Color& _emissive
)
: MaterialBase( _color, _emissive )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Mirror::Mirror
(
    const Color&         _color,
    const char*          _filename,
    const TextureSampler _sampler,
    const Color&         _emissive
)
: MaterialBase( _color, _emissive, _filename, _sampler )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      色を求めます.
//--------------------------------------------------------------------------------
Color Mirror::ComputeColor( ShadingArg& arg ) const
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


//////////////////////////////////////////////////////////////////////////////////
// Transparent structure
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
Transparent::Transparent()
: MaterialBase()
, ior( 1.0f )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Transparent::Transparent
(
    const f32    _ior,
    const Color& _color,
    const Color& _emissive
)
: MaterialBase( _color, _emissive )
, ior( _ior )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Transparent::Transparent
(
    const f32           _ior,
    const Color&        _color,
    const Color&        _emissive,
    const char*         _filename,
    const TextureSampler& _sampler
)
: MaterialBase( _color, _emissive, _filename, _sampler )
, ior( _ior )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      色を求めます.
//--------------------------------------------------------------------------------
Color Transparent::ComputeColor( ShadingArg& arg ) const
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
        arg.output = reflect;

        // 重み更新.
        return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) );
    }

    // 屈折ベクトル.
    Vector3 refract = Vector3::UnitVector(
        arg.input * nnt - arg.normal * ( ( into ) ? 1.0f : -1.0f ) * ( ddn * nnt + sqrt(cos2t) ) );

    // SchlickによるFresnelの反射係数の近似を使う
    const f32 a = nt - nc;
    const f32 b = nt + nc;
    const f32 R0 = (a * a) / (b * b);

    const f32 c = 1.0f - ( ( into ) ? -ddn : Vector3::Dot( refract, arg.normal ) );
    const f32 Re = R0 + (1.0f - R0) * powf(c, 5.0f); // 反射方向の光が反射してray.dirの方向に運ぶ割合。同時に屈折方向の光が反射する方向に運ぶ割合。
    const f32 Tr = ( 1.0f - Re );

    // 一定以上レイを追跡したら屈折と反射のどちらか一方を追跡する
    // ロシアンルーレットで決定する。
    const f32 P = 0.25f + 0.5f * Re;      // フレネルのグラフを参照.
                
    // 反射の場合.
    if ( arg.random.GetAsF32() < P )
    {
        arg.output = reflect;

        // 重み更新.
        return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) ) * Re / ( P * arg.prob );
    }
    // 屈折の場合.
    else
    {
        arg.output = refract;

        // 重み更新.
        return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) ) * Tr / ( ( 1.0f - P ) * arg.prob );
    }
}


//////////////////////////////////////////////////////////////////////////////////
// Glossy structure
//////////////////////////////////////////////////////////////////////////////////

Glossy::Glossy()
: emissive  ( 0.0f, 0.0f, 0.0f )
, diffuse   ( 0.0f, 0.0f, 0.0f )
, specular  ( 0.0f, 0.0f, 0.0f )
, texture   ()
, sampler   ()
, roughness ( 0.0f )
, fresnel   ( 0.0f )
, threshold ( 0.0f )
{ /* DO_NOTHING */ }

Glossy::Glossy
(
    const f32 _roughness,
    const f32 _fresnel,
    const Color& _diffuse,
    const Color& _specular,
    const Color& _emissive
)
: emissive  ( _emissive )
, diffuse   ( _diffuse )
, specular  ( _specular )
, roughness ( _roughness )
, fresnel   ( _fresnel )
, texture   ()
, sampler   ()
{
    threshold = Max( specular.x, specular.y );
    threshold = Max( specular.z, threshold );
}

Color Glossy::GetEmissive() const
{ return emissive; }

f32 Glossy::GetThreshold() const
{ return threshold; }

Color Glossy::ComputeColor( ShadingArg& arg ) const
{
    Vector3 reflect = Vector3::Reflect( arg.input, arg.normal );
    reflect.Normalize();

    OrthonormalBasis onb;
    onb.InitFromW( arg.normal );

    //for( ;; )
    //{
    //    f32 r1 = D_2PI * arg.random.GetAsF64();
    //    f32 r2 = 
    //}

    return Color( 0.0f, 0.0f, 0.0f );
}



} // namespace s3d
