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
//      バイリニア補間を適用してサンプリングします.
//---------------------------------------------------------------------------------------
Color4 Texture2D::Sample(const Vector2& texcoord) const
{
    // 浮動小数点形式で画像サイズにスケーリング.
    auto fx = texcoord.x * (m_Width - 1);
    auto fy = texcoord.y * (m_Height - 1);

    // 小数点以下を切り捨て.
    auto x0 = static_cast<s32>( floor( fx ) );
    auto y0 = static_cast<s32>( floor( fy ) );

    auto x1 = (x0 + 1);
    auto y1 = (y0 + 1);

    // 先に重み計算.
    auto x_w0 = x1 - fx;
    auto x_w1 = fx - x0;
    auto y_w0 = y1 - fy;
    auto y_w1 = fy - y0;

    x0 = abs(x0 % m_Width);
    x1 = abs(x1 % m_Width);
    y0 = abs(y0 % m_Height);
    y1 = abs(y1 % m_Height);

    return x_w0 * ( y_w0 * GetPixel( x0, y0 ) + y_w1 * GetPixel( x0, y1 ) )
         + x_w1 * ( y_w0 * GetPixel( x1, y0 ) + y_w1 * GetPixel( x1, y1 ) );
}

//---------------------------------------------------------------------------------------
//      アルファテストを行います.
//---------------------------------------------------------------------------------------
bool Texture2D::AlphaTest(const Vector2& texcoord, const f32 value ) const
{
    if ( m_ComponentCount != 4 )
    { return true; }

    if ( m_pPixels == nullptr )
    { return true; }

    Color4 result = Sample(texcoord);
    return ( result.GetW() >= value );
}

} // namespace s3d
