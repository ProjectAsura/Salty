﻿//-------------------------------------------------------------------------------------------------
// File : s3d_phong.cpp
// Desc : Phong Material.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_phong.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Phong class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
Phong::Phong(const Color4& specular, f32 power, const Color4& emissive)
: m_Count   (1)
, m_Specular(specular)
, m_Power   (power)
, m_Emissive(emissive)
{
    m_Threshold = m_Specular.GetX();
    m_Threshold = s3d::Max( m_Threshold, m_Specular.GetY() );
    m_Threshold = s3d::Max( m_Threshold, m_Specular.GetZ() );
    m_Threshold = s3d::Max( m_Threshold, 0.01f);    // Nan対策のため下駄をはかせる.
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
Phong::~Phong()
{ /* DO_NOTHING */}

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void Phong::AddRef()
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------------
void Phong::Release()
{
    m_Count--;
    if ( m_Count == 0 )
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 Phong::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      シェーディングします.
//-------------------------------------------------------------------------------------------------
Color4 Phong::Shade( ShadingArg& arg ) const
{
    // インポータンスサンプリング.
    const f32 phi = F_2PI * arg.random.GetAsF32();
    const f32 cosTheta = powf( 1.0f - arg.random.GetAsF32(), 1.0f / ( m_Power + 1.0f ) );
    const f32 sinTheta = SafeSqrt( 1.0f - ( cosTheta * cosTheta ) );
    const f32 x = cosf( phi ) * sinTheta;
    const f32 y = sinf( phi ) * sinTheta;
    const f32 z = cosTheta;

    auto n = (Vector3::Dot(arg.input, arg.normal) < 0.0f) ? arg.normal : -arg.normal;

    // 反射ベクトル.
    Vector3 w = Vector3::Reflect( arg.input, n );
    w.SafeNormalize();

    // 基底ベクトルを求める.
    Vector3 T, B;
    TangentSpace( w, T, B );

    // 出射方向.
    auto dir = Vector3::SafeUnitVector( T * x + B * y + w * z );
    auto cosine = abs(Vector3::Dot( dir, n ));

    arg.output = dir;
    arg.dice = (arg.random.GetAsF32() >= m_Threshold);

    return m_Specular * cosine;
}

//-------------------------------------------------------------------------------------------------
//      エミッシブカラーを取得します.
//-------------------------------------------------------------------------------------------------
Color4 Phong::GetEmissive() const
{ return m_Emissive; }

//-------------------------------------------------------------------------------------------------
//      デルタ関数をもつかどうか?
//-------------------------------------------------------------------------------------------------
bool Phong::HasDelta() const
{ return false; }

//-------------------------------------------------------------------------------------------------
//      生成処理です.
//-------------------------------------------------------------------------------------------------
IMaterial* Phong::Create(const Color4& specular, f32 power)
{ return Phong::Create(specular, power, Color4(0.0f, 0.0f, 0.0f, 1.0f)); }

//-------------------------------------------------------------------------------------------------
//      生成処理です.
//-------------------------------------------------------------------------------------------------
IMaterial* Phong::Create(const Color4& specular, f32 power, const Color4& emissive)
{ return new(std::nothrow) Phong(specular, power, emissive); }

} // namespace s3d
