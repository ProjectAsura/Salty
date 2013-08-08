//------------------------------------------------------------------------------
// File : s3d_target.cpp
// Desc : Render Target Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------
#include <s3d_target.h>
#include <cassert>
#include <cmath>


namespace s3d {

RenderTarget::RenderTarget()
: m_Width       ( 0 )
, m_Height      ( 0 )
, m_pFrameBuffer( nullptr )
{ /* DO_NOTHING */ }

RenderTarget::RenderTarget( const u32 width, const u32 height )
: m_Width ( width )
, m_Height( height )
{
    m_pFrameBuffer = new Color3[ m_Width * m_Height ];
    Color3 clearColor( 0.0, 0.0, 0.0 );
    Clear( clearColor );
}

RenderTarget::RenderTarget( const u32 width, const u32 height, const Color3& clearColor )
: m_Width   ( width )
, m_Height  ( height )
{
    m_pFrameBuffer = new Color3[ m_Width * m_Height ];
    Clear( clearColor);
}

void RenderTarget::Clear( const Color3& clearColor )
{
    if ( m_pFrameBuffer != nullptr )
    {
        for( u32 i=0; i<m_Height; ++i )
        {
            for( u32 j=0; j<m_Width; ++j )
            {
                m_pFrameBuffer[ i * m_Width + j ] = clearColor;
            }
        }
    }
}

void RenderTarget::SetPixel( const u32 x, const u32 y, const Color3& value )
{
    assert( x >= m_Width );
    assert( y >= m_Height);
    assert( m_pFrameBuffer != nullptr );

    m_pFrameBuffer[ y * m_Width + x ] = value;
}

Color3 RenderTarget::GetPixel( const u32 x, const u32 y )
{
    assert( x >= m_Width );
    assert( y >= m_Height );
    assert( m_pFrameBuffer != nullptr );

    return m_pFrameBuffer[ y * m_Width + x ];
}

void RenderTarget::GammaCorrection( const f64 gamma )
{
    assert( m_Width > 0 );
    assert( m_Height > 0 );
    assert( m_pFrameBuffer != nullptr );

    register Color3 pixel;
    register f64    power = 1.0 / gamma;

    for( u32 i=0; i<m_Height; ++i )
    {
        for( u32 j=0; j<m_Width; ++j )
        {
            register u32 idx = i * m_Width + j;
            pixel = m_pFrameBuffer[ idx ];
            m_pFrameBuffer[ idx ] = Color3( pow( pixel.r, power ),
                                            pow( pixel.g, power ),
                                            pow( pixel.b, power ) );
        }
    }
}


bool RenderTarget::SaveToBMP( const char* filename )
{

    return false;
}


} // namespace s3d

