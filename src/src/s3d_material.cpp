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
const f64 REFRACTIVITY_WATER   = 1.33;  // 水の屈折率 (1.3)
const f64 REFRACTIVITY_CRYSTAL = 1.54;  // 水晶の屈折率 (1.54).
const f64 REFRACTIVITY_DIAMOND = 2.42;  // ダイアモンドの屈折率 (2.42)

}// namespace /* anonymous */


namespace s3d {

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
MaterialBase::MaterialBase()
: emissive  ( 0.0, 0.0, 0.0 )
, color     ( 0.0, 0.0, 0.0 )
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
    const Color& _color,
    const Color& _emissive = Color( 0.0, 0.0, 0.0 )
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
f64 MaterialBase::GetThreshold() const
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
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Matte::Matte
(
    const Color& _color,
    const Color& _emissive = Color( 0.0, 0.0, 0.0 )
)
: MaterialBase( _color, _emissive )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Matte::Matte
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
Color Matte::ComputeColor( ShadingArg& arg ) const
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
    return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) ) / arg.prob;
}
 

//////////////////////////////////////////////////////////////////////////////////
// Clay structure
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
Clay::Clay()
: MaterialBase()
, roughness( 0.0 )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Clay::Clay
(
    const Color& _color,
    const f64    _roughness,
    const Color& _emissive
)
: MaterialBase( _color, _emissive )
, roughness( _roughness )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Clay::Clay
(
    const Color&        _color,
    const f64           _roughness,
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
Color Clay::ComputeColor( ShadingArg& arg ) const
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
// RefractionMaterial structure
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
RefractionMaterial::RefractionMaterial
( 
    const f64           _refractivity
)
: MaterialBase()
, refractivity( _refractivity )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
RefractionMaterial::RefractionMaterial
(
    const Color& _color,
    const Color& _emissive,
    const f64    _refraction
)
: MaterialBase( _color, _emissive )
, refractivity( _refraction )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
RefractionMaterial::RefractionMaterial
(
    const Color&        _color,
    const Color&        _emissive,
    const f64           _refraction,
    const char*         _filename,
    const TextureSampler& _sampler
)
: MaterialBase( _color, _emissive, _filename, _sampler )
, refractivity( _refraction )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      色を求めます.
//--------------------------------------------------------------------------------
Color RefractionMaterial::ComputeColor( ShadingArg& arg ) const
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
        return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) ) * Re / ( P * arg.prob );
    }
    // 屈折の場合.
    else
    {
        arg.output = refract;

        // 重み更新.
        return Vector3::Mul( color, texture.Sample( sampler, arg.texcoord ) ) * Tr / ( ( 1.0 - P ) * arg.prob );
    }
}


//////////////////////////////////////////////////////////////////////////////////
// Crystal structure
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
Crystal::Crystal()
: RefractionMaterial( REFRACTIVITY_CRYSTAL )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Crystal::Crystal
(
    const Color& _color,
    const Color& _emissive
)
: RefractionMaterial( _color, _emissive, REFRACTIVITY_CRYSTAL )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Crystal::Crystal
(
    const Color&         _color,
    const char*          _filename,
    const TextureSampler _sampler,
    const Color&         _emissive
)
: RefractionMaterial( _color, _emissive, REFRACTIVITY_CRYSTAL, _filename, _sampler )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
Diamond::Diamond()
: RefractionMaterial( REFRACTIVITY_DIAMOND )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Diamond::Diamond
(
    const Color& _color,
    const Color& _emissive
)
: RefractionMaterial( _color, _emissive, REFRACTIVITY_DIAMOND )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
Diamond::Diamond
(
    const Color&         _color,
    const char*          _filename,
    const TextureSampler _sampler,
    const Color&         _emissive
)
: RefractionMaterial( _color, _emissive, REFRACTIVITY_DIAMOND, _filename, _sampler )
{ /* DO_NOTHING */ }


//////////////////////////////////////////////////////////////////////////////////
// MeshMaterial structure
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
MeshMaterial::MeshMaterial()
: diffuse       ()
, emissive      ()
, refractivity  ( 1.0 )
, roughness     ( 0.0 )
, diffuseMap    ()
, diffuseSmp    ()
{ 
    threshold = Max( diffuse.x, diffuse.y );
    threshold = Max( diffuse.z, threshold );
}

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
MeshMaterial::MeshMaterial
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
    threshold = Max( diffuse.x, diffuse.y );
    threshold = Max( diffuse.z, threshold );
}

//--------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------
MeshMaterial::MeshMaterial
(
    Color       Kd,
    Color       Ke,
    f64         Re,
    f64         Kr,
    const char* map_Kd,
    TextureSampler& sampler
)
: diffuse       ( Kd )
, emissive      ( Ke )
, refractivity  ( Re )
, roughness     ( Kr )
, diffuseMap    ( map_Kd )
, diffuseSmp    ( sampler )
{
    threshold = Max( diffuse.x, diffuse.y );
    threshold = Max( diffuse.z, threshold );
}

//--------------------------------------------------------------------------------
//      自己発光カラーを取得します.
//--------------------------------------------------------------------------------
Color MeshMaterial::GetEmissive() const
{ return emissive; }

//--------------------------------------------------------------------------------
//      ロシアンルーレットの閾値を求めます.
//--------------------------------------------------------------------------------
f64 MeshMaterial::GetThreshold() const
{ return threshold; }

//--------------------------------------------------------------------------------
//      色を求めます.
//--------------------------------------------------------------------------------
Color MeshMaterial::ComputeColor( ShadingArg& arg ) const
{
    /* NOT_IMPLEMENT */
    return Color( 0.0, 0.0, 0.0 );
}

} // namespace s3d
