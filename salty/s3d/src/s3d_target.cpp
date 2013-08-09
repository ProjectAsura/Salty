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

RenderTarget::~RenderTarget()
{
    Term();
}

void RenderTarget::Init( const u32 width, const u32 height )
{
    m_Width = width;
    m_Height = height;
    m_pFrameBuffer = new Color3[ m_Width * m_Height ];
    Color3 clearColor( 0.0, 0.0, 0.0 );
    Clear( clearColor );
}

void RenderTarget::Init( const u32 width, const u32 height, const Color3& clearColor )
{
    m_Width = width;
    m_Height = height;
    m_pFrameBuffer = new Color3[ m_Width * m_Height ];
    Clear( clearColor);
}

void RenderTarget::Term()
{
    if ( m_pFrameBuffer )
    {
        delete [] m_pFrameBuffer;
        m_pFrameBuffer = nullptr;
    }
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

Color3* RenderTarget::GetFrameBuffer()
{ return m_pFrameBuffer; }

u32 RenderTarget::GetWidth() const
{ return m_Width; }

u32 RenderTarget::GetHeight() const
{ return m_Height; }


} // namespace s3d

