﻿//-------------------------------------------------------------------------------------------------
// File : s3d_lambert.cpp
// Desc : Lambert Material.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------------
#include <s3d_lambert.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Lambert class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
Lambert::Lambert(const Color4& diffuse, const Color4& emissive)
: m_Count   (1)
, m_Diffuse (diffuse)
, m_Emissive(emissive)
{
    m_Threshold = diffuse.GetX();
    m_Threshold = s3d::Max( m_Threshold, diffuse.GetY() );
    m_Threshold = s3d::Max( m_Threshold, diffuse.GetZ() );
    m_Threshold = s3d::Max( m_Threshold, 0.01f);    // Nan対策のため下駄をはかせる.
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
Lambert::~Lambert()
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void Lambert::AddRef()
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------------
void Lambert::Release()
{
    m_Count--;
    if ( m_Count == 0 )
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 Lambert::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      シェーディングします.
//-------------------------------------------------------------------------------------------------
Color4 Lambert::Shade( ShadingArg& arg ) const
{
    // normalModの方向を基準とした正規直交基底(w, u, v)を作る。
    // この基底に対する半球内で次のレイを飛ばす。
    Vector3 T, B;
    TangentSpace(arg.normal, T, B);

    // インポータンスサンプリング.
    const f32 phi = F_2PI * arg.random.GetAsF32();
    const f32 r = SafeSqrt( arg.random.GetAsF32() );
    const f32 x = r * cosf( phi );
    const f32 y = r * sinf( phi );
    const f32 z = SafeSqrt( 1.0f - ( x * x ) - ( y * y ) );

    arg.output = Vector3::SafeUnitVector( T * x + B * y + arg.normal * z );
    arg.dice   = ( arg.random.GetAsF32() >= m_Threshold );

    // 以下の処理の省略.
    //      pdf = cosine * F_1DIVPI;
    //      sample = m_Diffuse * cosine * F_1DIVPI;
    //      weight = sample / pdf;

    return m_Diffuse;
}

//-------------------------------------------------------------------------------------------------
//      エミッシブカラーを取得します.
//-------------------------------------------------------------------------------------------------
Color4 Lambert::GetEmissive() const
{ return m_Emissive; }

//-------------------------------------------------------------------------------------------------
//      デルタ関数をもつかどうか?
//-------------------------------------------------------------------------------------------------
bool Lambert::HasDelta() const
{ return false; }

//-------------------------------------------------------------------------------------------------
//      生成処理です.
//-------------------------------------------------------------------------------------------------
IMaterial* Lambert::Create(const Color4& diffuse)
{ return Lambert::Create(diffuse, Color4(0.0f, 0.0f, 0.0f, 1.0f)); }

//-------------------------------------------------------------------------------------------------
//      生成処理です.
//-------------------------------------------------------------------------------------------------
IMaterial* Lambert::Create(const Color4& diffuse, const Color4& emissive)
{ return new (std::nothrow) Lambert(diffuse, emissive); }

} // namespace s3d
