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
#include <s3d_tga.h>
#include <s3d_logger.h>

#include <cassert>
#include <cstdio>
#include <cstring>


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
    if ( ( filename != nullptr )
      && ( filename[0] != 0 )
      && ( filename[0] != '\0' ) )
    {
        bool result = LoadFromFile( filename );
        if ( !result )
        {
            ELOG( "Error : Load File Failed. filename = %s", filename );
        }
        assert( result == true );
        S3D_UNUSED_VAR( result );
    }
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
    m_pPixels = new f32 [ m_Size ];
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

    FILE* pFile;
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    { return false; }

    u16 tag;
    fread( &tag, sizeof(tag), 1, pFile );
    if ( tag == 'MB' )
    {
        fclose( pFile );

        // BMP読み込み.
        if ( LoadFromBMP( filename, w, h, &m_pPixels ) )
        {
            m_Width  = static_cast<u32>( w ); 
            m_Height = static_cast<u32>( h );
            m_Size   = m_Width * m_Height * 4;
            m_ComponentCount = 3;

            // 正常終了.
            return true;
        }
    }
    else
    {
        char fileTag[18];
        fseek( pFile, -18, SEEK_END );
        fread( fileTag, sizeof(char), 18, pFile );

        fclose( pFile );

        if ( strcmp( fileTag, "TRUEVISION-XFILE.") == 0 ||
             strcmp( fileTag, "TRUEVISION-TARGA.") == 0 )
        {
            s32 c = 0;
            if ( LoadFromTGA( filename, w, h, c, &m_pPixels ) )
            {
                m_Width  = static_cast<u32>( w );
                m_Height = static_cast<u32>( h );
                m_Size   = m_Width * m_Height * 4;
                m_ComponentCount = c;
                return true;
            }
        }
    }

   // 異常終了.
   return false;
}

//--------------------------------------------------------------------------------------
//      メモリ解放処理を行います.
//--------------------------------------------------------------------------------------
void Texture2D::Release()
{
    // メモリ解放.
    SafeDeleteArray( m_pPixels );

    // ゼロリセット.
    m_Width  = 0;
    m_Height = 0;
    m_Size   = 0;
}

//---------------------------------------------------------------------------------------
//      指定されたピクセルを取得します.
//---------------------------------------------------------------------------------------
Color4 Texture2D::GetPixel(s32 x, s32 y, const TextureSampler& sampler ) const
{ 
    auto w = static_cast<s32>(m_Width);
    auto h = static_cast<s32>(m_Height);

    switch( sampler.address )
    {
    case TEXTURE_ADDRESS_WRAP:
        {
            x = abs( x % w );
            y = abs( y % h );
        }
        break;

    case TEXTURE_ADDRESS_BORADER:
        {
            if ( x >= w || y >= h || x < 0 || y < 0 )
            { return sampler.boarderColor; }
        }
        break;

    case TEXTURE_ADDRESS_CLAMP:
    default:
        {
            x = Clamp( x, 0, w - 1 );
            y = Clamp( y, 0, h - 1 );
        }
        break;
    }

    auto idx = ( m_Width * 4 * y ) + ( x * 4 );
    assert( idx < m_Size );

    return Color4(
        m_pPixels[idx + 0],
        m_pPixels[idx + 1],
        m_pPixels[idx + 2],
        m_pPixels[idx + 3] );
}


//---------------------------------------------------------------------------------------
//      最近傍補間を適用してサンプリングします.
//---------------------------------------------------------------------------------------
Color4 Texture2D::NearestSample(const TextureSampler& sampler, const Vector2& texcoord) const
{
    auto x = static_cast<s32>( texcoord.x * m_Width  + 0.5f );
    auto y = static_cast<s32>( texcoord.y * m_Height + 0.5f );

    return GetPixel(x, y, sampler);
}

//---------------------------------------------------------------------------------------
//      バイリニア補間を適用してサンプリングします.
//---------------------------------------------------------------------------------------
Color4 Texture2D::BilinearSample(const TextureSampler& sampler, const Vector2& texcoord) const
{
    // 浮動小数点形式で画像サイズにスケーリング.
    auto fx = texcoord.x * m_Width;
    auto fy = texcoord.y * m_Height;

    // 小数点以下を切り捨て.
    auto x0 = static_cast<s32>( floor( fx ) );
    auto y0 = static_cast<s32>( floor( fy ) );

    auto x1 = x0 + 1;
    auto y1 = y0 + 1;

    return ( x1 - fx ) * ( ( y1 - fy ) * GetPixel( x0, y0, sampler ) + ( fy - y0 ) * GetPixel( x0, y1, sampler ) )
         + ( fx - x0 ) * ( ( y1 - fy ) * GetPixel( x1, y0, sampler ) + ( fy - y0 ) * GetPixel( x1, y1, sampler ) );    
}

//--------------------------------------------------------------------------------------
//      テクスチャフェッチします.
//--------------------------------------------------------------------------------------
Color4 Texture2D::Sample( const TextureSampler& sampler, const Vector2& location ) const
{
    // テクスチャが無ければ(1.0, 1.0, 1.0)を返しておく.
    if ( m_pPixels == nullptr )
    { return Color4( 1.0f, 1.0f, 1.0f, 1.0f ); }

    if ( sampler.filter == TEXTURE_FILTER_BILINEAR )
    { return BilinearSample( sampler, location ); }

    return NearestSample( sampler, location );
}

//---------------------------------------------------------------------------------------
//      アルファテストを行います.
//---------------------------------------------------------------------------------------
bool Texture2D::AlphaTest( const TextureSampler& sampler, const Vector2& texcoord, const f32 value ) const
{
    if ( m_ComponentCount != 4 )
    { return true; }

    if ( m_pPixels == nullptr )
    { return true; }

    Color4 result;
    if ( sampler.filter == TEXTURE_FILTER_BILINEAR )
    { result = BilinearSample( sampler, texcoord ); }
    else
    { result = NearestSample( sampler, texcoord ); }

    return ( result.w >= value );
}

} // namespace s3d
