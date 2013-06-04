//------------------------------------------------------------------------------------------
// File : DDSLoader.cpp
// Desc : Direct Draw Surface Loader Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include "DDSLoader.h"
#include <cstdio>
#include <cstring>
#include <cassert>
#include "MapData.h"


#ifndef DLOG
#define DLOG( x, ... )  printf_s( x, ##__VA_VARGS__ )
#endif//DLOG

#ifndef ELOG
#define ELOG( x, ... )  fprintf_s( stderr, "[File : %s, Line : %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG

//------------------------------------------------------------------------------------------
// Constant Values
//------------------------------------------------------------------------------------------

// dwFlags Value
static const unsigned int DDSD_CAPS         = 0x00000001;   // dwCaps/dwCaps2が有効.
static const unsigned int DDSD_HEIGHT       = 0x00000002;   // dwHeightが有効.
static const unsigned int DDSD_WIDTH        = 0x00000004;   // dwWidthが有効.
static const unsigned int DDSD_PITCH        = 0x00000008;   // dwPitchOrLinearSizeがPitchを表す.
static const unsigned int DDSD_PIXELFORMAT  = 0x00001000;   // dwPfSize/dwPfFlags/dwRGB～等の直接定義が有効.
static const unsigned int DDSD_MIPMAPCOUNT  = 0x00020000;   // dwMipMapCountが有効.
static const unsigned int DDSD_LINEARSIZE   = 0x00080000;   // dwPitchOrLinearSizeがLinerSizeを表す.
static const unsigned int DDSD_DEPTH        = 0x00800000;   // dwDepthが有効.

// dwPfFlags Value
static const unsigned int DDPF_ALPHAPIXELS      = 0x00000001;   // RGB以外にalphaが含まれている.
static const unsigned int DDPF_ALPHA            = 0x00000002;   // pixelはAlpha成分のみ.
static const unsigned int DDPF_FOURCC           = 0x00000004;   // dwFourCCが有効.
static const unsigned int DDPF_PALETTE_INDEXED4 = 0x00000008;   // Palette 16 colors.
static const unsigned int DDPF_PALETTE_INDEXED8 = 0x00000020;   // Palette 256 colors.
static const unsigned int DDPF_RGB              = 0x00000040;   // dwRGBBitCount/dwRBitMask/dwGBitMask/dwBBitMask/dwRGBAlphaBitMaskによってフォーマットが定義されていることを示す.
static const unsigned int DDPF_LUMINANCE        = 0x00020000;   // 1chのデータがRGB全てに展開される.
static const unsigned int DDPF_BUMPDUDV         = 0x00080000;   // pixelが符号付きであることを示す.

// dwCaps Value
static const unsigned int DDSCAPS_ALPHA     = 0x00000002;       // Alphaが含まれている場合.
static const unsigned int DDSCAPS_COMPLEX   = 0x00000008;       // 複数のデータが含まれている場合Palette/Mipmap/Cube/Volume等.
static const unsigned int DDSCAPS_TEXTURE   = 0x00001000;       // 常に1.
static const unsigned int DDSCAPS_MIPMAP    = 0x00400000;       // MipMapが存在する場合.

// dwCaps2 Value
static const unsigned int DDSCAPS2_CUBEMAP              = 0x00000200;   // CubeMapが存在する場合.
static const unsigned int DDSCAPS2_CUBEMAP_POSITIVE_X   = 0x00000400;   // CubeMap X+
static const unsigned int DDSCAPS2_CUBEMAP_NEGATIVE_X   = 0x00000800;   // CubeMap X-
static const unsigned int DDSCAPS2_CUBEMAP_POSITIVE_Y   = 0x00001000;   // CubeMap Y+
static const unsigned int DDSCAPS2_CUBEMAP_NEGATIVE_Y   = 0x00002000;   // CubeMap Y-
static const unsigned int DDSCAPS2_CUBEMAP_POSITIVE_Z   = 0x00004000;   // CubeMap Z+
static const unsigned int DDSCAPS2_CUBEMAP_NEGATIVE_Z   = 0x00008000;   // CubeMap Z-
static const unsigned int DDSCAPS2_VOLUME               = 0x00400000;   // VolumeTextureの場合.

// dwFourCC Value
static const unsigned int FOURCC_DXT1           = '1TXD';           // DXT1
static const unsigned int FOURCC_DXT2           = '2TXD';           // DXT2
static const unsigned int FOURCC_DXT3           = '3TXD';           // DXT3
static const unsigned int FOURCC_DXT4           = '4TXD';           // DXT4
static const unsigned int FOURCC_DXT5           = '5TXD';           // DXT5
static const unsigned int FOURCC_ATI2           = '2ITA';           // 3Dc ATI2
static const unsigned int FOURCC_A16B16G16R16   = 0x00000024;
static const unsigned int FOURCC_Q16W16V16U16   = 0x0000006e;
static const unsigned int FOURCC_R16F           = 0x0000006f;
static const unsigned int FOURCC_G16R16F        = 0x00000070;
static const unsigned int FOURCC_A16B16G16R16F  = 0x00000071;
static const unsigned int FOURCC_R32F           = 0x00000072;
static const unsigned int FOURCC_G32R32F        = 0x00000073;
static const unsigned int FOURCC_A32B32G32R32F  = 0x00000074;
static const unsigned int FOURCC_CxV8U8         = 0x00000075;
static const unsigned int FOURCC_Q8W8V8U8       = 0x0000003f;



////////////////////////////////////////////////////////////////////////////////////////////
// DDPixelFormat structure
////////////////////////////////////////////////////////////////////////////////////////////
typedef struct __DDPixelFormat
{
    unsigned int    size;
    unsigned int    flags;
    unsigned int    fourCC;
    unsigned int    bpp;
    unsigned int    maskR;
    unsigned int    maskG;
    unsigned int    maskB;
    unsigned int    maskA;
} DDPixelFormat;


////////////////////////////////////////////////////////////////////////////////////////////
// DDColorKey structure
////////////////////////////////////////////////////////////////////////////////////////////
typedef struct __DDColorKey
{
    unsigned int    low;
    unsigned int    hight;
} DDColorKey;


////////////////////////////////////////////////////////////////////////////////////////////
// DDSurfaceDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
typedef struct __DDSurfaceDesc
{
    unsigned int    size;
    unsigned int    flags;
    unsigned int    height;
    unsigned int    width;
    unsigned int    pitch;
    unsigned int    depth;
    unsigned int    mipMapLevels;
    unsigned int    alphaBitDepth;
    unsigned int    reserved;
    unsigned int    surface;

    DDColorKey      ckDestOverlay;
    DDColorKey      ckDestBit;
    DDColorKey      ckSrcOverlay;
    DDColorKey      ckSrcBit;

    DDPixelFormat   pixelFormat;
    unsigned int    caps;
    unsigned int    caps2;
    unsigned int    reservedCaps[ 2 ];

    unsigned int    textureStage;
} DDSurfaceDesc;

////////////////////////////////////////////////////////////////////////////////////////////
// NATIVE_TEXTURE_FORMAT enum
////////////////////////////////////////////////////////////////////////////////////////////
enum NATIVE_TEXTURE_FORMAT
{
    NATIVE_TEXTURE_FORMAT_ARGB_8888 = 0,
    NATIVE_TEXTURE_FORMAT_ABGR_8888,
    NATIVE_TEXTURE_FORMAT_XRGB_8888,
    NATIVE_TEXTURE_FORMAT_XBGR_8888,
    NATIVE_TEXTURE_FORMAT_BC1,
    NATIVE_TEXTURE_FORMAT_BC2,
    NATIVE_TEXTURE_FORMAT_BC3,
    NATIVE_TEXTURE_FORMAT_A8,
    NATIVE_TEXTURE_FORMAT_L8,
    NATIVE_TEXTURE_FORMAT_L8A8,
};

//-------------------------------------------------------------------------------------------
//      マスクをチェックします.
//-------------------------------------------------------------------------------------------
bool CheckMask
(
    const DDPixelFormat& pixelFormat,
    unsigned int maskR,
    unsigned int maskG,
    unsigned int maskB,
    unsigned int maskA
)
{
    if ( ( pixelFormat.maskR == maskR )
      && ( pixelFormat.maskG == maskG )
      && ( pixelFormat.maskB == maskB )
      && ( pixelFormat.maskA == maskA ) )
    { return true; }

    return false;
}

//-------------------------------------------------------------------------------------------
//      最大値を取得します.
//-------------------------------------------------------------------------------------------
template<typename T>
T Max( const T a, const T b )
{ return ( a > b ) ? a : b; }


//////////////////////////////////////////////////////////////////////////////////////////////
// DDSTexture class
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------------------
DDSTexture::DDSTexture()
: m_Width         ( 0 )
, m_Height        ( 0 )
, m_Format        ( TEXTURE_FORMAT_R8G8B8A8 )
, m_MipMapCount   ( 0 )
, m_pResourceData ( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------------------------
DDSTexture::~DDSTexture()
{
    Release();
}

//--------------------------------------------------------------------------------------------
//      メモリ解放処理です.
//--------------------------------------------------------------------------------------------
void DDSTexture::Release()
{
    if ( m_pResourceData )
    {
        for( unsigned int i=0; i<m_MipMapCount; ++i )
        {
            // メモリ解放.
            delete [] m_pResourceData[ i ].pPixelData;

            // 変数をリセット.
            m_pResourceData[ i ].pPixelData = nullptr;
            m_pResourceData[ i ].width      = 0;
            m_pResourceData[ i ].height     = 0;
            m_pResourceData[ i ].pitch      = 0;
            m_pResourceData[ i ].slicePitch = 0;
        }

        // メモリを解放.
        delete [] m_pResourceData;
        m_pResourceData = nullptr;
    }

    m_Width        = 0;
    m_Height       = 0;
    m_Format       = TEXTURE_FORMAT_R8G8B8A8;
    m_MipMapCount  = 0;
}

//-------------------------------------------------------------------------------------------
//      ファイルからロードします.
//-------------------------------------------------------------------------------------------
bool DDSTexture::LoadFromFile( const char* filename )
{
    FILE*           pFile;
    DDSurfaceDesc   ddsd;
    char            magic[ 4 ];
    unsigned int    width  = 0;
    unsigned int    height = 0;
    unsigned int    depth  = 0;
    unsigned int    nativeFormat = 0;

    // ファイルを開きます.
    errno_t err = fopen_s( &pFile, filename, "rb" );

    // ファイルオープンチェック.
    if ( err != 0 )
    {
        ELOG( "Error : File Open Failed. filename = %s", filename );
        return false;
    }

   // マジックを読み込む.
    fread( magic, sizeof(char), 4, pFile);

    // マジックをチェック.
    if ( ( magic[0] != 'D' ) 
      || ( magic[1] != 'D' )
      || ( magic[2] != 'S' )
      || ( magic[3] != ' ' ) )
    {
        // エラーログ出力.
        ELOG( "Error : Invalid File. filename = %s", filename );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // ミップマップ数を初期化.
    m_MipMapCount = 1;

    // サーフェイスデスクリプションを読み込み.
    fread( &ddsd, sizeof( DDSurfaceDesc ), 1, pFile );

    // 高さ有効.
    if ( ddsd.flags & DDSD_HEIGHT )
    { height = ddsd.height; }

    // 幅有効.
    if ( ddsd.flags & DDSD_WIDTH )
    { width = ddsd.width; }

    // 奥行有効.
    if ( ddsd.flags & DDSD_DEPTH )
    { depth = ddsd.depth; }

    m_Width  = width;
    m_Height = height;

    // ミップマップ数.有効
    if ( ddsd.flags & DDSD_MIPMAPCOUNT )
    { m_MipMapCount = ddsd.mipMapLevels; }

    // キューブマップとボリュームテクスチャのチェック.
    if ( ddsd.caps & DDSCAPS_COMPLEX )
    {
        // キューブマップの場合.
        if ( ddsd.caps2 & DDSCAPS2_CUBEMAP )
        {
            unsigned int surfaceCount = 0;

            // サーフェイス数をチェック.
            if ( ddsd.caps2 & DDSCAPS2_CUBEMAP_POSITIVE_X ) { surfaceCount++; }
            if ( ddsd.caps2 & DDSCAPS2_CUBEMAP_NEGATIVE_X ) { surfaceCount++; }
            if ( ddsd.caps2 & DDSCAPS2_CUBEMAP_POSITIVE_Y ) { surfaceCount++; }
            if ( ddsd.caps2 & DDSCAPS2_CUBEMAP_NEGATIVE_Y ) { surfaceCount++; }
            if ( ddsd.caps2 & DDSCAPS2_CUBEMAP_POSITIVE_Z ) { surfaceCount++; }
            if ( ddsd.caps2 & DDSCAPS2_CUBEMAP_NEGATIVE_Z ) { surfaceCount++; }
            
            // ファイルを閉じる.
            fclose( pFile );

            // エラーログ出力.
            ELOG( "Error : CubeMap Not Supported. surfaceCount = %d", surfaceCount );
    
            // 一応チェック.
            assert( surfaceCount == 6 );

            // 異常終了.
            return false;
        }
        // ボリュームテクスチャの場合.
        else if ( ddsd.caps2 & DDSCAPS2_VOLUME )
        {
            // ファイルを閉じる.
            fclose( pFile );

            // エラーログ出力.
            ELOG( "Error : Volume Texture Not Supported." );

            // 異常終了.
            return false;
        }
    }

    // サポートフォーマットのチェックフラグ.
    bool isSupportFormat = false;

    // ピクセルフォーマット有効.
    if ( ddsd.flags & DDSD_PIXELFORMAT )
    {
        // dwFourCC有効
        if ( ddsd.pixelFormat.flags & DDPF_FOURCC )
        {
            switch( ddsd.pixelFormat.fourCC )
            {
            case FOURCC_DXT1:
                {
                    m_Format        = TEXTURE_FORMAT_BC1;
                    nativeFormat    = NATIVE_TEXTURE_FORMAT_BC1;
                    isSupportFormat = true;
                }
                break;

            case FOURCC_DXT2:
                {
                    m_Format        = TEXTURE_FORMAT_BC2;
                    nativeFormat    = NATIVE_TEXTURE_FORMAT_BC2;
                    isSupportFormat = true;
                }
                break;

            case FOURCC_DXT3:
                {
                    m_Format        = TEXTURE_FORMAT_BC2;
                    nativeFormat    = NATIVE_TEXTURE_FORMAT_BC2;
                    isSupportFormat = true;
                }
                break;

            case FOURCC_DXT4:
                {
                    m_Format        = TEXTURE_FORMAT_BC3;
                    nativeFormat    = NATIVE_TEXTURE_FORMAT_BC3;
                    isSupportFormat = true;
                }
                break;

            case FOURCC_DXT5:
                {
                    m_Format        = TEXTURE_FORMAT_BC3;
                    nativeFormat    = NATIVE_TEXTURE_FORMAT_BC3;
                    isSupportFormat = true;
                }
                break;

            default:
                {
                    isSupportFormat = false;
                }
                break;
            }
        }
        else if ( ddsd.pixelFormat.flags & DDPF_RGB )
        {
            switch( ddsd.pixelFormat.bpp )
            {
            case 32:
                {
                    if ( CheckMask( ddsd.pixelFormat, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 ) )
                    {
                        // A8 R8 G8 B8
                        isSupportFormat = true;
                        m_Format = TEXTURE_FORMAT_R8G8B8A8;
                        nativeFormat = NATIVE_TEXTURE_FORMAT_ARGB_8888;
                    }

                    if ( CheckMask( ddsd.pixelFormat, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 ) )
                    {
                        // A8 B8 G8 R8
                        isSupportFormat = true;
                        m_Format = TEXTURE_FORMAT_R8G8B8A8;
                        nativeFormat = NATIVE_TEXTURE_FORMAT_ABGR_8888;
                    }

                    if ( CheckMask( ddsd.pixelFormat, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 ) )
                    {
                        // X8 R8 G8 B8
                        isSupportFormat = true;
                        m_Format = TEXTURE_FORMAT_R8G8B8A8;
                        nativeFormat = NATIVE_TEXTURE_FORMAT_XRGB_8888;
                    }

                    if ( CheckMask( ddsd.pixelFormat, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 ) )
                    {
                        // X8 B8 G8 R8
                        isSupportFormat = true;
                        m_Format = TEXTURE_FORMAT_R8G8B8A8;
                        nativeFormat = NATIVE_TEXTURE_FORMAT_XBGR_8888;
                    }

#if 0
                    //if ( CheckMask( ddsd.pixelFormat, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 ) )
                    //{
                    //    // R10 G10 B10 A2
                    //    /* NOT_SUPPORT */
                    //}

                    //if ( CheckMask( ddsd.pixelFormat, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 ) )
                    //{
                    //    // R16 G16
                    //    /* NOT_SUPPORT */
                    //}
                    //if ( CheckMask( ddsd.pixelFormat, 0xffffffff, 0x00000000,0x00000000,0x00000000 ) )
                    //{
                    //    // R32
                    //    /* NOT_SUPPORT */
                    //}
#endif
                }
                break;

            case 24:
                {
#if 0
                    /* NOT_SUPPORT */
#endif
                }
                break;

            case 16:
                {
#if 0
                    //if ( CheckMask( ddsd.pixelFormat, 0x7c00, 0x03e0, 0x001f, 0x8000 ) )
                    //{
                    //    // B5 G5 R5 A1
                    //    /* NOT_SUPPORT */
                    //}

                    //if ( CheckMask( ddsd.pixelFormat, 0xf800, 0x07e0, 0x001f, 0x0000 ) )
                    //{
                    //    // B5 G6 R5
                    //    /* NOT_SUPPORT */
                    //}
#endif
                }
                break;
            }
        }
        else if ( ddsd.pixelFormat.flags & DDPF_LUMINANCE )
        {
            switch( ddsd.pixelFormat.bpp )
            {
            case 8:
                {
                    if ( CheckMask( ddsd.pixelFormat, 0x000000ff, 0x00000000, 0x00000000, 0x00000000 ) )
                    {
                        // R8
                        isSupportFormat = true;
                        nativeFormat = NATIVE_TEXTURE_FORMAT_L8;
                        m_Format = TEXTURE_FORMAT_L8;
                    }
                }
                break;

            case 16:
                {
                    //if ( CheckMask( ddsd.pixelFormat, 0x0000ffff, 0x00000000, 0x00000000, 0x00000000 ) )
                    //{
                    //    // R16
                    //    /* NOT_SUPPORT */
                    //}

                    //if ( CheckMask( ddsd.pixelFormat, 0x000000ff, 0x00000000, 0x00000000, 0x0000ff00 ) )
                    //{
                    //    // R8 G8
                    //    /* NOT_SUPPORT */
                    //}
                }
                break;
            }
        }
        else if ( ddsd.pixelFormat.flags & DDPF_ALPHA )
        {
            if ( 8 == ddsd.pixelFormat.bpp )
            {
                // A8
                isSupportFormat = true;
                nativeFormat = NATIVE_TEXTURE_FORMAT_A8;
                m_Format = TEXTURE_FORMAT_A8;
            }
        }
    }

    // サポートフォーマットがあったかチェック.
    if ( !isSupportFormat )
    {
        // ファイルを閉じる.
        fclose( pFile );

        // エラーログ出力.
        ELOG( "Error : Unsupported Format." );

        // 異常終了.
        return false;
    }

    // 現在のファイル位置を記憶.
    long curr = ftell( pFile );

    // ファイル末端に移動.
    fseek( pFile, 0, SEEK_END );

    // ファイル末端位置を記憶.
    long end = ftell( pFile );

    // ファイル位置を元に戻す.
    fseek( pFile, curr, SEEK_SET );

    // ピクセルデータのサイズを算出.
    size_t pixelSize = end - curr;

    // ピクセルデータのメモリを確保.
    unsigned char* pPixelData = new unsigned char [pixelSize];

    // NULLチェック.
    if ( pPixelData == nullptr )
    {
        // ファイルを閉じる.
        fclose( pFile );

        // エラーログ出力.
        ELOG( "Error : Memory Allocate Failed." );

        // 異常終了.
        return false;
    }

    // ピクセルデータ読み込み.
    fread( pPixelData, sizeof( unsigned char ), pixelSize, pFile );

    // ファイルを閉じる.
    fclose( pFile );

    // リトルエンディアンなのでピクセルの並びを補正.
    switch( nativeFormat )
    {
        // 一括読み込みでくるっているので修正.
        case NATIVE_TEXTURE_FORMAT_ARGB_8888:
        case NATIVE_TEXTURE_FORMAT_XRGB_8888:
        {
            for( size_t i=0; i<pixelSize; i+=4 )
            {
                // BGRA -> RGBA
                unsigned char R = pPixelData[ i + 0 ];
                unsigned char B = pPixelData[ i + 2 ];
                pPixelData[ i + 0 ] = B;
                pPixelData[ i + 2 ] = R;
            }
        }
        break;
    }

    size_t w      = width;
    size_t h      = height;
    size_t offset = 0;

    // リソースデータのメモリを確保.
    m_pResourceData = new ResourceData[ m_MipMapCount ];

    // 各ミップレベルごとに処理.
    for( size_t i=0; i<m_MipMapCount; ++i )
    {
        size_t rowBytes = 0;
        size_t numRows  = 0;
        size_t numBytes = 0;

        // ブロック圧縮フォーマットの場合.
        if ( ( m_Format == TEXTURE_FORMAT_BC1 )
          || ( m_Format == TEXTURE_FORMAT_BC2 )
          || ( m_Format == TEXTURE_FORMAT_BC3 ) )
        {
            size_t bcPerBlock = 0;
            size_t blockWide  = 0;
            size_t blockHigh  = 0;

            // BC1, BC4の場合.
            if ( m_Format == TEXTURE_FORMAT_BC1 )
            { bcPerBlock = 8; }
            // BC2, BC3, BC5, BC6H, BC7の場合.
            else
            { bcPerBlock = 16; }

            // 横幅が1以上の場合.
            if ( w > 0 )
            { blockWide = Max< size_t >( 1, ( w + 3 ) / 4 ); }

            // 縦幅が異常の場合.
            if ( h > 0 )
            { blockHigh = Max< size_t >( 1, ( h + 3 ) / 4 ); }

            // 一行のバイト数.
            rowBytes = blockWide * bcPerBlock;

            // 行数.
            numRows  = blockHigh;
        }
        // ブロック圧縮フォーマット以外の場合.
        else
        {
            // 1ピクセル当たりのビット数.
            size_t bpp = ddsd.pixelFormat.bpp;

            // 一行のバイト数.
            rowBytes = ( w * bpp + 7 ) / 8;

            // 行数.
            numRows  = h;
        }

        // データ数 = (1行当たりのバイト数) * 行数.
        numBytes = rowBytes * numRows;

        // リソースデータを設定.
        m_pResourceData[ i ].width      = w;
        m_pResourceData[ i ].height     = h;
        m_pResourceData[ i ].pitch      = rowBytes;
        m_pResourceData[ i ].slicePitch = numBytes;
        m_pResourceData[ i ].pPixelData = new unsigned char [ numBytes ];

        // NULLチェック.
        if ( m_pResourceData[ i ].pPixelData == nullptr )
        {
            // エラーログ出力.
            ELOG( "Error : Memory Allocate Failed." );

            // 異常終了.
            return false;
        }

        // ピクセルデータをコピー.
        std::memcpy( m_pResourceData[ i ].pPixelData, pPixelData + offset, numBytes );

        // オフセットをカウントアップ.
        offset += numBytes;

        // 横幅，縦幅を更新.
        w = w >> 1;
        h = h >> 1;

        // クランプ処理.
        if ( w == 0 ) { w = 1; }
        if ( h == 0 ) { h = 1; }
    }

    // 不要になったメモリを解放.
    delete [] pPixelData;
    pPixelData = nullptr;

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------------
//      バイナリファイルに保存します.
//-------------------------------------------------------------------------------------------
bool DDSTexture::SaveToBinary( const char* filename )
{
    FILE* pFile;

    // ファイルを開く.
    errno_t err = fopen_s( &pFile, filename, "wb" );

    // ファイルオープンチェック.
    if ( err != 0 )
    {
        ELOG( "Error : File Open Failed. filename = %s", filename );
        return false;
    }

    MAP_FORMAT_TYPE pixelFormat;

    // ピクセルフォーマット設定.
    switch( m_Format )
    {
        case TEXTURE_FORMAT_A8:         { pixelFormat = MAP_FORMAT_A8; }        break;
        case TEXTURE_FORMAT_L8:         { pixelFormat = MAP_FORMAT_L8; }        break;
        case TEXTURE_FORMAT_R8G8B8A8:   { pixelFormat = MAP_FORMAT_R8G8B8A8; }  break;
        case TEXTURE_FORMAT_BC1:        { pixelFormat = MAP_FORMAT_BC1; }       break;
        case TEXTURE_FORMAT_BC2:        { pixelFormat = MAP_FORMAT_BC2; }       break;
        case TEXTURE_FORMAT_BC3:        { pixelFormat = MAP_FORMAT_BC3; }       break;
    }

    // ファイルマジック書き込み.
    fwrite( MAP_FILE_MAGIC, sizeof( unsigned char ) * 4, 1, pFile );

    // ファイルバージョン書き込み.
    fwrite( &MAP_FILE_VERSION, sizeof( unsigned int ), 1, pFile );

    // データヘッダ構造体サイズ書き込み.
    fwrite( &MAP_DATA_HEADER_STRUCT_SIZE, sizeof( unsigned int ), 1, pFile );

    // データヘッダ設定.
    MAP_DATA_HEADER dataHeader;
    dataHeader.width       = m_Width;
    dataHeader.height      = m_Height;
    dataHeader.format      = pixelFormat;
    dataHeader.mipMapCount = m_MipMapCount;

    // データヘッダ書き込み.
    fwrite( &dataHeader, sizeof( MAP_DATA_HEADER ), 1, pFile );

    // リソースデータ書き込み.
    for( unsigned int i=0; i<m_MipMapCount; ++i )
    {
        MAP_RESOURCE_HEADER resHeader;

        resHeader.width      = m_pResourceData[ i ].width;
        resHeader.height     = m_pResourceData[ i ].height;
        resHeader.pitch      = m_pResourceData[ i ].pitch;
        resHeader.slicePitch = m_pResourceData[ i ].slicePitch;

        // リソースデータヘッダを書き込み.
        fwrite( &resHeader, sizeof( MAP_RESOURCE_HEADER ), 1, pFile );

        // ピクセルデータを書き込み.
        fwrite( m_pResourceData[ i ].pPixelData, sizeof( unsigned char ), m_pResourceData[ i ].slicePitch, pFile );
    }

    // ファイルを閉じる.
    fclose( pFile );

    // 正常終了.
    return true;
}