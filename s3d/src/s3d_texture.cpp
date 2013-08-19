//------------------------------------------------------------------------------------
// File : s3d_texture.cpp
// Desc : Texture Modle.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------
#include <s3d_texture.h>
#include <s3d_bmp.h>

#include <cassert>
#include <cstdio>

namespace s3d {

////////////////////////////////////////////////////////////////////////////////////////
// Texture2D class
////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------------
Texture2D::Texture2D()
: m_Width       ( 0 )
, m_Height      ( 0 )
, m_Size        ( 0 )
, m_pPixels     ( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------------
Texture2D::Texture2D( const char* filename )
: m_Width       ( 0 )
, m_Height      ( 0 )
, m_Size        ( 0 )
, m_pPixels     ( nullptr )
{
    bool result = LoadFromFile( filename );
    assert( result == true );
    S3D_UNUSED_VAR( result );
}

//--------------------------------------------------------------------------------------
//      コピーコンストラクタです.
//--------------------------------------------------------------------------------------
Texture2D::Texture2D( const Texture2D& value )
: m_Width       ( value.m_Width )
, m_Height      ( value.m_Height )
, m_Size        ( value.m_Size )
, m_pPixels     ( nullptr )
{
    m_pPixels = new f64 [ m_Size ];
    assert( m_pPixels != nullptr );

    for( u32 i=0; i<m_Size; ++i )
    { m_pPixels[i] = value.m_pPixels[i]; }
}

//--------------------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------------------
Texture2D::~Texture2D()
{
    Release();
}

//--------------------------------------------------------------------------------------
//      ファイルからロードします.
//--------------------------------------------------------------------------------------
bool Texture2D::LoadFromFile( const char* filename )
{
   // NULLチェック.
   if ( filename == nullptr )
   { return false; }

   int w = 0;
   int h = 0;
   if ( LoadFromBMP( filename, w, h, &m_pPixels ) )
   {
       m_Width  = static_cast<u32>( w ); 
       m_Height = static_cast<u32>( h );
       m_Size   = m_Width * m_Height * 3;
       return true;
   }

   return false;
}

//--------------------------------------------------------------------------------------
//      メモリ解放処理を行います.
//--------------------------------------------------------------------------------------
void Texture2D::Release()
{
    if ( m_pPixels )
    {
        delete [] m_pPixels;
        m_pPixels = nullptr;
    }

    m_Width  = 0;
    m_Height = 0;
    m_Size   = 0;
}

//--------------------------------------------------------------------------------------
//      テクスチャフェッチします.
//--------------------------------------------------------------------------------------
Color Texture2D::Sample( const Vector2& texcoord ) const
{
    if ( m_pPixels == nullptr )
    { return Color( 1.0, 1.0, 1.0 ); }

    u32 x = static_cast<u32>( texcoord.x * m_Width );
    u32 y = static_cast<u32>( texcoord.y * m_Height );

    u32 idx = ( m_Width * 3 * y ) + ( x * 3 );
    assert( idx < m_Size );

    return Color( 
        m_pPixels[ idx + 0 ],
        m_pPixels[ idx + 1 ],
        m_pPixels[ idx + 2 ] );
}

} // namespace s3d
