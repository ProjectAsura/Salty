//-------------------------------------------------------------------------------------------------
// File : s3d_plastic.cpp
// Desc : Plastic Material.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#include <s3d_plastic.h>


namespace s3d {

Plastic::Plastic(const Color4& diffuse, const Color4& specular, f32 power, const Color4& emissive)
    : m_Count(1)
    , m_Diffuse(diffuse)
    , m_Specular(specular)
    , m_Power(power)
    , m_Emissive(emissive)
{
    m_Threshold[0] = diffuse.GetX();
    m_Threshold[0] = s3d::Max( m_Threshold[0], diffuse.GetY() );
    m_Threshold[0] = s3d::Max( m_Threshold[0], diffuse.GetZ() );
    m_Threshold[0] = s3d::Max( m_Threshold[0], 0.01f);    // Nan対策のため下駄をはかせる.

    m_Threshold[1] = specular.GetX();
    m_Threshold[1] = s3d::Max( m_Threshold[0], specular.GetY() );
    m_Threshold[1] = s3d::Max( m_Threshold[0], specular.GetZ() );
    m_Threshold[1] = s3d::Max( m_Threshold[0], 0.01f);    // Nan対策のため下駄をはかせる.
}

Plastic::~Plastic()
{ /* DO_NOTHING */ }

void Plastic::AddRef()
{ m_Count++; }

void Plastic::Release()
{
    m_Count--;
    if (m_Count == 0)
    { delete this; }
}

u32 Plastic::GetCount() const
{ return m_Count; }

Color4 Plastic::Shade( ShadingArg& arg ) const
{
    // 補正済み法線データ (レイの入出を考慮済み).
    auto cosine = Vector3::Dot( arg.normal, arg.input );
    const Vector3 normalMod = ( cosine < 0.0 ) ? arg.normal : -arg.normal;

    if ( cosine < 0.0f )
    { cosine = -cosine; }

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
        auto result = m_Diffuse  * R / P;
        arg.dice = ( arg.random.GetAsF32() >= m_Threshold[0] );

        return result;
    }
    else
    {
        // インポータンスサンプリング.
        const f32 phi = F_2PI * arg.random.GetAsF32();
        const f32 cosTheta = powf( 1.0f - arg.random.GetAsF32(), 1.0f / ( m_Power + 1.0f ) );
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
        arg.dice = ( arg.random.GetAsF32() >= m_Threshold[1] );

        return m_Specular * dots * ( 1.0f - R ) / ( 1.0f - P );
    }
}

Color4 Plastic::GetEmissive() const
{ return m_Emissive; }

bool Plastic::HasDelta() const
{ return false; }

IMaterial* Plastic::Create(const Color4& diffuse, const Color4& specular, f32 power)
{ return Plastic::Create(diffuse, specular, power, Color4(0.0f, 0.0f, 0.0f, 1.0f)); }

IMaterial* Plastic::Create(const Color4& diffuse, const Color4& specular, f32 power, const Color4& emissive)
{ return new(std::nothrow) Plastic(diffuse, specular, power, emissive); }

} // namespace s3d