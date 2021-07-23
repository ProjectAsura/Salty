﻿//-------------------------------------------------------------------------------------------
// File : s3d_ibl.cpp
// Desc : Image Based Lighting Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <s3d_ibl.h>
#include <s3d_hdr.h>


namespace s3d {

/////////////////////////////////////////////////////////////////////////////////////////////
// IBL class
/////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------
IBL::IBL()
: m_Width   ( 0 )
, m_Height  ( 0 )
, m_Gamma   ( 0.0f )
, m_Exposure( 0.0f )
, m_pPixels ( nullptr )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------
IBL::~IBL()
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------
//      初期化処理を行います.
//-------------------------------------------------------------------------------------------
bool IBL::Init( const char* filename )
{
    return LoadFromHDR( filename, m_Width, m_Height, m_Gamma, m_Exposure, &m_pPixels );
}

//-------------------------------------------------------------------------------------------
//      終了処理を行います.
//-------------------------------------------------------------------------------------------
void IBL::Term()
{
    SafeDeleteArray( m_pPixels );
    m_Width    = 0;
    m_Height   = 0;
    m_Gamma    = 0.0f;
    m_Exposure = 0.0f;
}

//--------------------------------------------------------------------------------------------
//      バイリニアフィルタを適用してサンプリングします.
//--------------------------------------------------------------------------------------------
Color4 IBL::Sample(const Vector3& dir) const
{
    Vector2 uv;
    uv.x = 0.0f;
    const auto theta = acosf( dir.y );
    uv.y = theta * F_1DIVPI;

    if ( !IsZero(dir.x) && !IsZero(dir.z) )
    {
        const auto phi = atan2( dir.z, dir.x );
        uv.x = (dir.z < 0.0f) ? (phi + F_2PI) : phi;
        uv.x *= F_1DIV2PI;
    }

    // 浮動小数点形式で画像サイズにスケーリング.
    auto fx = uv.x * (m_Width - 1);
    auto fy = uv.y * (m_Height - 1);

    // 小数点以下を切り捨て.
    auto x0 = static_cast<s32>( floor( fx ) );
    auto y0 = static_cast<s32>( floor( fy ) );

    auto x1 = x0 + 1;
    auto y1 = y0 + 1;

    // 先に重み計算.
    auto x_w0 = x1 - fx;
    auto x_w1 = fx - x0;
    auto y_w0 = y1 - fy;
    auto y_w1 = fy - y0;

    x0 = abs(x0 % m_Width);
    x1 = abs(x1 % m_Height);
    y0 = abs(y0 % m_Height);
    y1 = abs(y1 % m_Height);

    return x_w0 * ( y_w0 * GetPixel( x0, y0 ) + y_w1 * GetPixel( x0, y1 ) )
         + x_w1 * ( y_w0 * GetPixel( x1, y0 ) + y_w1 * GetPixel( x1, y1 ) );
}

} // namespace s3d
