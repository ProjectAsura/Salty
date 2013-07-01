//-----------------------------------------------------------------------------------
// File : saltyFrameBuffer.cpp
// Desc : Frame Buffer Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------
#include <saltyFrameBuffer.h>
#include <asdxMath.h>
#include <asdxUtil.h>


namespace /* anonymous */ {

// ガンマ値.
static const f32 GAMMA_VALUE     = 2.2f;

// 逆ガンマ値.
static const f32 INV_GAMMA_VALUE = 1.0f / GAMMA_VALUE;

}// namespace /* anonymous */

namespace salty {

/////////////////////////////////////////////////////////////////////////////////////
// FrameBuffer class
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------
FrameBuffer::FrameBuffer()
: m_Width  ( 0 )
, m_Height ( 0 )
, m_PixelCount( 0 )
, m_pPixels( nullptr )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------------
FrameBuffer::~FrameBuffer()
{
    Release();
}

//-----------------------------------------------------------------------------------
//      生成します.
//-----------------------------------------------------------------------------------
bool FrameBuffer::Create( const Desc& desc )
{
    m_Width      = desc.Width;
    m_Height     = desc.Height;
    m_PixelCount = m_Width * m_Height;

    m_pPixels = new Pixel [ m_PixelCount ];
    if ( !m_pPixels )
    {
        m_Width      = 0;
        m_Height     = 0;
        m_PixelCount = 0;
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------
//      解放します.
//-----------------------------------------------------------------------------------
void FrameBuffer::Release()
{
    if ( m_pPixels )
    {
        delete [] m_pPixels;
        m_pPixels = nullptr;
    }

    m_Width      = 0;
    m_Height     = 0;
    m_PixelCount = 0;
}

//-----------------------------------------------------------------------------------
//      クリアします.
//-----------------------------------------------------------------------------------
void FrameBuffer::Clear( f32 r, f32 g, f32 b, f32 a )
{
    if ( m_pPixels )
    {
        for( u32 idx = 0; idx < m_PixelCount; ++idx )
        {
            m_pPixels[idx].R = r;
            m_pPixels[idx].G = g;
            m_pPixels[idx].B = b;
            m_pPixels[idx].A = a;
        }
    }
}

//-----------------------------------------------------------------------------------
//      インデクサです.
//-----------------------------------------------------------------------------------
Pixel& FrameBuffer::operator[] ( u32 index )
{
    assert( index < m_PixelCount );
    return m_pPixels[ index ];
}

//-----------------------------------------------------------------------------------
//      インデクサです(const版).
//-----------------------------------------------------------------------------------
const Pixel& FrameBuffer::operator[] ( u32 index ) const
{
    assert( index < m_PixelCount );
    return m_pPixels[ index ];
}

//-----------------------------------------------------------------------------------
//      ピクセルを取得します.
//-----------------------------------------------------------------------------------
Pixel& FrameBuffer::GetPixel( u32 index )
{
    assert( index < m_PixelCount );
    return m_pPixels[ index ];
}

//-----------------------------------------------------------------------------------
//      ピクセルのR成分を取得します.
//-----------------------------------------------------------------------------------
f32& FrameBuffer::GetPixelR( u32 index )
{
    assert( index < m_PixelCount );
    return m_pPixels[ index ].R;
}

//-----------------------------------------------------------------------------------
//      ピクセルのG成分を取得します.
//-----------------------------------------------------------------------------------
f32& FrameBuffer::GetPixelG( u32 index )
{
    assert( index < m_PixelCount );
    return m_pPixels[ index ].G;
}

//-----------------------------------------------------------------------------------
//      ピクセルのB成分を取得します.
//-----------------------------------------------------------------------------------
f32& FrameBuffer::GetPixelB( u32 index )
{
    assert( index < m_PixelCount );
    return m_pPixels[ index ].B;
}

//-----------------------------------------------------------------------------------
//      ピクセルのA成分を取得します.
//-----------------------------------------------------------------------------------
f32& FrameBuffer::GetPixelA( u32 index )
{
    assert( index < m_PixelCount );
    return m_pPixels[ index ].A;
}

//-----------------------------------------------------------------------------------
//      横幅を取得します.
//-----------------------------------------------------------------------------------
u32 FrameBuffer::GetWidth() const
{ return m_Width; }

//-----------------------------------------------------------------------------------
//      縦幅を取得します.
//-----------------------------------------------------------------------------------
u32 FrameBuffer::GetHeight() const
{ return m_Height; }

//-----------------------------------------------------------------------------------
//      ピクセル数を取得します.
//-----------------------------------------------------------------------------------
u32 FrameBuffer::GetPixelCount() const
{ return m_PixelCount; }

//-----------------------------------------------------------------------------------
//      BMPファイルに出力します.
//-----------------------------------------------------------------------------------
bool FrameBuffer::ExportToBmpA( const char8* filename )
{
    u8* pPixels = new u8 [ m_PixelCount * 3 ];
    if ( !pPixels )
    {
        return false;
    }

    if ( m_pPixels )
    {
        int i = 0;
        for( u32 idx = 0; idx < m_PixelCount; ++idx )
        {
            pPixels[i + 0] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].R, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 1] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].G, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 2] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].B, INV_GAMMA_VALUE ) ) * 255.0f );
            i += 3;
        }
    }

    if ( !asdx::SaveTextureToBmpA( filename, m_Width, m_Height, 3, pPixels ) )
    {
        delete [] pPixels;
        pPixels = nullptr;

        return false;
    }

    delete [] pPixels;
    pPixels =  nullptr;

    return true;
}

//-----------------------------------------------------------------------------------
//      BMPファイルに出力します.
//-----------------------------------------------------------------------------------
bool FrameBuffer::ExportToBmpW( const char16* filename )
{
    u8* pPixels = new u8 [ m_PixelCount * 3 ];
    if ( !pPixels )
    {
        return false;
    }

    if ( m_pPixels )
    {
        int i = 0;
        for( u32 idx = 0; idx < m_PixelCount; ++idx )
        {
            pPixels[i + 0] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].R, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 1] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].G, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 2] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].B, INV_GAMMA_VALUE ) ) * 255.0f );
            i += 3;
        }
    }

    if ( !asdx::SaveTextureToBmpW( filename, m_Width, m_Height, 3, pPixels ) )
    {
        delete [] pPixels;
        pPixels = nullptr;

        return false;
    }

    delete [] pPixels;
    pPixels =  nullptr;

    return true;
}

//-----------------------------------------------------------------------------------
//      TGAファイルに出力します.
//-----------------------------------------------------------------------------------
bool FrameBuffer::ExportToTgaA( const char8* filename )
{
    u8* pPixels = new u8 [ m_PixelCount * 4 ];
    if ( !pPixels )
    {
        return false;
    }

    if ( m_pPixels )
    {
        int i=0;
        for( u32 idx = 0; idx < m_PixelCount; ++idx )
        {
            pPixels[i + 0] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].R, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 1] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].G, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 2] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].B, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 3] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].A, INV_GAMMA_VALUE ) ) * 255.0f );
            i += 4;
        }
    }

    if ( !asdx::SaveTextureToTgaA( filename, m_Width, m_Height, 4, pPixels ) )
    {
        delete [] pPixels;
        pPixels = nullptr;

        return false;
    }

    delete [] pPixels;
    pPixels =  nullptr;

    return true;
}

//-----------------------------------------------------------------------------------
//      TGAファイルに出力します.
//-----------------------------------------------------------------------------------
bool FrameBuffer::ExportToTgaW( const char16* filename )
{
    u8* pPixels = new u8 [ m_PixelCount * 4 ];
    if ( !pPixels )
    {
        return false;
    }

    if ( m_pPixels )
    {
        int i=0;
        for( u32 idx = 0; idx < m_PixelCount; ++idx )
        {
            pPixels[i + 0] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].R, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 1] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].G, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 2] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].B, INV_GAMMA_VALUE ) ) * 255.0f );
            pPixels[i + 3] = static_cast<u8>( asdx::Saturate( powf( m_pPixels[idx].A, INV_GAMMA_VALUE ) ) * 255.0f );
            i += 4;
        }
    }

    if ( !asdx::SaveTextureToTgaW( filename, m_Width, m_Height, 4, pPixels ) )
    {
        delete [] pPixels;
        pPixels = nullptr;

        return false;
    }

    delete [] pPixels;
    pPixels =  nullptr;

    return true;
}


} // namespace salty