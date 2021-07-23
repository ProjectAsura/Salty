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
Phong::Phong(const Color3& specular, f32 power, const Color3& emissive)
: m_Count   (1)
, m_Specular(specular)
, m_Power   (power)
, m_Emissive(emissive)
{
    m_Threshold = m_Specular.x;
    m_Threshold = s3d::Max( m_Threshold, m_Specular.y );
    m_Threshold = s3d::Max( m_Threshold, m_Specular.z );
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
Color3 Phong::Shade( ShadingArg& arg ) const
{
    // インポータンスサンプリング.
    auto s = SamplePhong(arg.random.GetAsF32(), arg.random.GetAsF32(), m_Power);

    auto n = (Vector3::Dot(arg.input, arg.normal) < 0.0f) ? arg.normal : -arg.normal;

    // 反射ベクトル.
    Vector3 w = Vector3::Reflect( arg.input, n );
    w.SafeNormalize();

    // 基底ベクトルを求める.
    Vector3 T, B;
    TangentSpace( w, T, B );

    // 出射方向.
    auto dir = Vector3::SafeUnitVector( T * s.x + B * s.y + w * s.z );
    auto cosine = abs(Vector3::Dot( dir, n ));

    arg.output = dir;
    arg.dice = (arg.random.GetAsF32() >= m_Threshold);

    return m_Specular * cosine;
}

//-------------------------------------------------------------------------------------------------
//      エミッシブカラーを取得します.
//-------------------------------------------------------------------------------------------------
Color3 Phong::GetEmissive() const
{ return m_Emissive; }

//-------------------------------------------------------------------------------------------------
//      デルタ関数をもつかどうか?
//-------------------------------------------------------------------------------------------------
bool Phong::HasDelta() const
{ return false; }

//-------------------------------------------------------------------------------------------------
//      生成処理です.
//-------------------------------------------------------------------------------------------------
IMaterial* Phong::Create(const Color3& specular, f32 power)
{ return Phong::Create(specular, power, Color3(0.0f, 0.0f, 0.0f)); }

//-------------------------------------------------------------------------------------------------
//      生成処理です.
//-------------------------------------------------------------------------------------------------
IMaterial* Phong::Create(const Color3& specular, f32 power, const Color3& emissive)
{ return new(std::nothrow) Phong(specular, power, emissive); }

} // namespace s3d
