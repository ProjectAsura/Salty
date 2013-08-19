//----------------------------------------------------------------------------
// File : s3d_material.cpp
// Desc : Material Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include <s3d_material.h>
#include <s3d_math.h>

#include <cstdio>


namespace /* anonymous */ {

//----------------------------------------------------------------------------
//      ロシアンルーレットに使用する閾値を求めます.
//----------------------------------------------------------------------------
S3D_INLINE
f64 ComputeThreshold( const s3d::Color3& color )
{
#if 1
    f64 result = color.r;
    result = ( result < color.g ) ? color.g : result;
    result = ( result < color.b ) ? color.b : result;
    return result;
#else
    return ( color.r + color.g + color.b ) / 3.0;
#endif
}

} // namespace /* anonymous */

namespace s3d {

//////////////////////////////////////////////////////////////////////////////
// Matte class
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------
Matte::Matte( const Color3& diffuse, const Color3& emissive  )
: m_Diffuse ( diffuse )
, m_Emissive( emissive )
{
    m_Threshold = ComputeThreshold( diffuse );
}

//----------------------------------------------------------------------------
//      コピーコンストラクタです.
//----------------------------------------------------------------------------
Matte::Matte( const Matte& value )
: m_Diffuse  ( value.m_Diffuse )
, m_Emissive ( value.m_Emissive )
, m_Threshold( value.m_Threshold )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------
//      ロシアンルーレットに使用する閾値を求めます.
//----------------------------------------------------------------------------
f64 Matte::GetThreshold() const
{ return m_Threshold; }

//----------------------------------------------------------------------------
//      自己発光による放射輝度を求めます.
//----------------------------------------------------------------------------
Color3 Matte::Le( ShadeArg& arg )
{
    S3D_UNUSED_VAR( arg );
    return m_Emissive;
}

//---------------------------------------------------------------------------
//      重み ( BRDF * cosθ / pdf の値 ) を求めます.
//---------------------------------------------------------------------------
Color3 Matte::Wr( ShadeArg& arg )
{
    Vector3 normal = arg.normal;
    if ( Vector3::Dot( normal, arg.input ) >= 0.0 )
    { normal = -normal; }

    // 正規直交基底を計算.
    OrthonormalBasis onb;
    //onb.InitFromW( normal );

    onb.w = normal;
    if ( fabs( normal.x ) > 1e-6 )
    {
        onb.u = Vector3::Cross( Vector3( 0.0, 1.0, 0.0 ), onb.w );
        onb.u.Normalize();
    }
    else
    {
        onb.u = Vector3::Cross( Vector3( 1.0, 0.0, 0.0 ), onb.w );
        onb.u.Normalize();
    }
    onb.v = Vector3::Cross( onb.w, onb.u );
    onb.v.Normalize();

    // サンプリング方向についての確率密度関数 pdf(ω) = ( cosθ / π )とする.
    const f64 r1  = D_2PI * arg.random.GetAsF64();
    const f64 r2  = arg.random.GetAsF64();
    const f64 r2s = sqrt( r2 );
    const f64  x  = r2s * cos( r1 );
    const f64  y  = r2s * sin( r1 );
    const f64  z  = sqrt( 1.0 - r2 );

    //const f64 r = sqrt( arg.random.GetAsF64() );
    //const f64 x = r * cos( r1 );
    //const f64 y = r * sin( r1 );
    //const f64 z = sqrt( 1.0 - x * x - y * y );

    // 出射方向を決定.
    arg.output = Vector3::UnitVector(
        onb.u * x + onb.v * y + onb.w * z );

    //========================================================================
    // レンダリング方程式に対するモンテカルロ積分を考えると,
    // outRadiance = Wr * inRadiance.
    // ここで，Wr = ( ( fr * cosθ ) / pdf(ω) ) * ( 1 / q ).
    // ただし, fr:BRDF, q:ロシアンルーレットの確率 とする.
    // すなわち, 
    //      Wr = ( ( ( ρ / π ) * cosθ ) / ( cosθ / π ) * ( 1 / q ).
    //             = ρ * ( 1 / q )
    //             = ρ / q
    // ただし, ρ:反射率 とする. つまり, このプログラムでは 色 となる.
    // 従って Wr = color / q.
    //========================================================================

    // 重みを返却する.
    assert( arg.probability > 0.0 );
    return m_Diffuse / arg.probability;
}

#if S3D_DEBUG
//----------------------------------------------------------------------------
//      カラーを取得します(デバッグ用途)
//----------------------------------------------------------------------------
Color3 Matte::GetColor() const
{ return m_Diffuse; }
#endif//S3D_DEBUG


//////////////////////////////////////////////////////////////////////////////
// Clay class
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------
Clay::Clay( const Color3& color, const f64 roughness, const Color3& emissive )
: m_Diffuse  ( color )
, m_Emissive ( emissive )
, m_Roughness( roughness )
{
    m_Threshold = ComputeThreshold( color );
}

//----------------------------------------------------------------------------
//      コピーコンストラクタです.
//----------------------------------------------------------------------------
Clay::Clay( const Clay& value )
: m_Diffuse  ( value.m_Diffuse )
, m_Emissive ( value.m_Emissive )
, m_Roughness( value.m_Roughness )
, m_Threshold( value.m_Threshold )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------
//      ロシアンルーレットに使用する閾値を取得します.
//----------------------------------------------------------------------------
f64 Clay::GetThreshold() const
{ return m_Threshold; }

//----------------------------------------------------------------------------
//      自己発光による放射輝度を求めます.
//----------------------------------------------------------------------------
Color3 Clay::Le( ShadeArg& arg )
{
    S3D_UNUSED_VAR( arg );
    return m_Emissive;
}

//---------------------------------------------------------------------------
//      重み ( BRDF * cosθ / pdf の値 ) を求めます.
//---------------------------------------------------------------------------
Color3 Clay::Wr( ShadeArg& arg )
{
    // 正規直交基底を計算.
    OrthonormalBasis onb;
    onb.InitFromW( arg.normal );

    // サンプリング方向についての確率密度関数 pdf(ω) = ( cosθ / π )とする.
    const f64 r1  = D_2PI * arg.random.GetAsF64();
    const f64 r = sqrt( arg.random.GetAsF64() );
    const f64 x = r * cos( r1 );
    const f64 y = r * sin( r1 );
    const f64 z = sqrt( 1.0 - x*x - y*y );

    // 出射方向を決定.
    arg.output = Vector3::UnitVector(
        onb.u * x + onb.v * y + onb.w * z );

    //========================================================================
    // レンダリング方程式に対するモンテカルロ積分をLambertと同様に考えると,
    // 従って Wr = color * ( A + B * cosφ * sinα * tanβ ) / q.
    // ただし, α = min( θi, θr ), β = max( θi, θr )とする.
    //========================================================================

    f64 s2 = m_Roughness * m_Roughness;
    f64 A = 1.0 - ( 0.5 * ( s2 / ( s2 + 0.33 ) ) );
    f64 B = 0.45 * ( s2 / ( s2 + 0.09 ) );

    f64 NV = Vector3::Dot( arg.normal, arg.input );
    f64 NL = Vector3::Dot( arg.normal, arg.output );

    Vector3 projI = Vector3::UnitVector( arg.input  - ( arg.normal * NV ) );
    Vector3 projR = Vector3::UnitVector( arg.output - ( arg.normal * NL ) );

    f64 cosPhai = Max( Vector3::Dot( projI, projR ), 0.0 );

    f64 ti    = acos( NV );
    f64 to    = acos( NL );
    f64 alpha = Max( ti, to );
    f64 beta  = Min( ti, to );
    f64 f     = A + B * cosPhai * sin( alpha ) * tan( beta );

    assert( arg.probability != 0.0 );
    return ( m_Diffuse * f ) / arg.probability;
}

#if S3D_DEBUG
//----------------------------------------------------------------------------
//      カラーを取得します(デバッグ用途)
//----------------------------------------------------------------------------
Color3 Clay::GetColor() const
{ return m_Diffuse; }
#endif//S3D_DEBUG


//////////////////////////////////////////////////////////////////////////////
// Plastic class
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Metal class
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Mirror class
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------
Mirror::Mirror( const Color3& specular, const Color3& emissive )
: m_Specular( specular )
, m_Emissive( emissive )
{
    m_Threshold = ComputeThreshold( specular );
}

//----------------------------------------------------------------------------
//      コピーコンストラクタです.
//----------------------------------------------------------------------------
Mirror::Mirror( const Mirror& value )
: m_Specular ( value.m_Specular )
, m_Threshold( value.m_Threshold )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------
//      ロシアンルーレットに使用する閾値を取得します.
//----------------------------------------------------------------------------
f64 Mirror::GetThreshold() const
{ return m_Threshold; }

//----------------------------------------------------------------------------
//      自己発光による放射輝度を求めます.
//----------------------------------------------------------------------------
Color3 Mirror::Le( ShadeArg& arg )
{
    S3D_UNUSED_VAR( arg );
    return m_Emissive;
}

//---------------------------------------------------------------------------
//      重み ( BRDF * cosθ / pdf の値 ) を求めます.
//---------------------------------------------------------------------------
Color3 Mirror::Wr( ShadeArg& arg )
{
    arg.output = Vector3::Reflect( arg.input, arg.normal );
    arg.output.Normalize();

    assert( arg.probability > 0.0 );
    return m_Specular / arg.probability;
}

#if S3D_DEBUG
//----------------------------------------------------------------------------
//      カラーを取得します(デバッグ用途)
//----------------------------------------------------------------------------
Color3 Mirror::GetColor() const
{ return m_Specular; }
#endif//S3D_DEBUG


//////////////////////////////////////////////////////////////////////////////
// Glass class
//////////////////////////////////////////////////////////////////////////////

Glass::Glass( const Color3& specular, const f64 refractivity, const Color3& emissive )
: m_Specular    ( specular )
, m_Refractivity( refractivity )
, m_Emissive    ( emissive )
{
    m_Threshold = ComputeThreshold( specular );
}

Glass::Glass( const Glass& value )
: m_Specular    ( value.m_Specular )
, m_Refractivity( value.m_Refractivity )
, m_Emissive    ( value.m_Emissive )
{ /* DO_NOTHING */ }

f64 Glass::GetThreshold() const
{ return m_Threshold; }

Color3 Glass::Le( ShadeArg& arg )
{
    S3D_UNUSED_VAR( arg );
    return m_Emissive;
}

Color3 Glass::Wr( ShadeArg& arg )
{
    //Vector3 normalOrg = arg.normal;
    //Vector3 normalMod = ( Vector3::Dot( arg.input, arg.normal ) < 0.0 ) ? arg.normal : -arg.normal;

    //OrthonormalBasis onb;
    //onb.w = normalMod;
    //if ( fabs( normalMod.x ) > 1e-6 )
    //{
    //    onb.u = Vector3::Cross( Vector3( 0.0, 1.0, 0.0 ), onb.w );
    //    onb.u.Normalize();
    //}
    //else
    //{
    //    onb.u = Vector3::Cross( Vector3( 1.0, 0.0, 0.0 ), onb.w );
    //    onb.u.Normalize();
    //}
    //onb.v = Vector3::Cross( onb.w, onb.u );
    //onb.v.Normalize();

    //// サンプリング方向についての確率密度関数 pdf(ω) = ( cosθ / π )とする.
    //const f64 r1  = D_2PI * arg.random.GetAsF64();
    //const f64 r2  = arg.random.GetAsF64();
    //const f64 r2s = sqrt( r2 );
    //const f64  x  = r2s * cos( r1 );
    //const f64  y  = r2s * sin( r1 );
    //const f64  z  = sqrt( 1.0 - r2 );

    ////const f64 r = sqrt( arg.random.GetAsF64() );
    ////const f64 x = r * cos( r1 );
    ////const f64 y = r * sin( r1 );
    ////const f64 z = sqrt( 1.0 - x * x - y * y );

    //// 出射方向を決定.
    //arg.output = Vector3::UnitVector(
    //    onb.u * x + onb.v * y + onb.w * z );
    //return m_Specular / arg.probability;



    Vector3 normalOrg = arg.normal;
    Vector3 normalMod = ( Vector3::Dot( arg.input, arg.normal ) < 0.0 ) ? arg.normal : -arg.normal;

    Vector3 reflect = Vector3::UnitVector( arg.input - normalOrg * 2.0 * Vector3::Dot( normalOrg, arg.input) );
    const bool into = ( Vector3::Dot( normalOrg, normalMod ) > 0.0 );    // レイがオブジェクトから出るのか、入るのか

    // Snellの法則
    const double nc    = 1.0; // 真空の屈折率
    const double nt    = m_Refractivity; // オブジェクトの屈折率
    const double nnt   = into ? nc / nt : nt / nc;
    const double ddn   = Vector3::Dot( arg.input, normalMod );
    const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);

    if (cos2t < 0.0)
    {
        Color3 result = m_Specular / arg.probability;

        // 全反射
        arg.output = reflect;
        //return result;
        return Color3( 0.0, 1.0, 0.0 );
    }

    // 屈折の方向
    Vector3 refract = Vector3::UnitVector(arg.input * nnt - normalOrg * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t)));

    // SchlickによるFresnelの反射係数の近似を使う
    const double a = nt - nc;
    const double b = nt + nc;
    const double R0 = (a * a) / (b * b);

    const double c  = 1.0 - (into ? -ddn : Vector3::Dot(refract, normalOrg));
    const double Re = R0 + (1.0 - R0) * pow(c, 5.0); // 反射方向の光が反射してray.dirの方向に運ぶ割合。同時に屈折方向の光が反射する方向に運ぶ割合。
    const double Tr = ( 1.0 - Re );

    // 一定以上レイを追跡したら屈折と反射のどちらか一方を追跡する
    // ロシアンルーレットで決定する。
    const double P = 0.25 + 0.5 * Re;

    f64 rnd = arg.random.GetAsF64();
    if (arg.random.GetAsF64() > P)
    {
        Color3 result = m_Specular * Re / ( P * arg.probability );

        // 反射
        arg.output = reflect;
        //return result;
        return Color3( 0.0, 0.0, 1.0 );
    }
    else
    {
        Color3 result = m_Specular * Tr / (( 1.0 - P ) * arg.probability );

        // 屈折
        arg.output = refract;
//        return result;
        return Color3( 1.0, 0.0, 0.0 );
    }
}

#if S3D_DEBUG
Color3 Glass::GetColor() const
{ return m_Specular; }
#endif//S3D_DEBUG


} // namespace s3d

