//------------------------------------------------------------------------------------
// File : s3d_texture.cpp
// Desc : Texture Modle.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------
#include <s3d_texture.h>
//#include <s3d_bmp.h>
//#include <s3d_tga.h>
#include <s3d_logger.h>

#include <vector>
#include <cassert>
#include <cstdio>
#include <cstring>


namespace s3d {
namespace {

//-------------------------------------------------------------------------------------------------
//! @brief      ビットマップファイルヘッダーです.
//-------------------------------------------------------------------------------------------------
#pragma pack( push, 1 )
struct BMP_FILE_HEADER
{
    unsigned short      Type;           // ファイルタイプ 'BM'
    unsigned int        Size;           // ファイルサイズ.
    unsigned short      Reserved1;      // 予約領域 (0固定).
    unsigned short      Reserved2;      // 予約領域 (0固定).
    unsigned int        OffBits;        // ファイル先頭から画像データまでのオフセット.
};
#pragma pack( pop )

//-------------------------------------------------------------------------------------------------
//! @brief      ビットマップの圧縮タイプです.
//-------------------------------------------------------------------------------------------------
enum BMP_COMPRESSION_TYPE
{
    BMP_COMPRESSION_RGB       = 0,      // 無圧縮.
    BMP_COMPRESSION_RLE8      = 1,      // RLE圧縮 8 bits/pixel.
    BMP_COMPRESSION_RLE4      = 2,      // RLE圧縮 4 bits/pixel.
    BMP_COMPRESSION_BITFIELDS = 3,      // ビットフィールド.
};

//-------------------------------------------------------------------------------------------------
//! @brief      ビットマップ情報ヘッダーです.
//-------------------------------------------------------------------------------------------------
#pragma pack( push, 1 )
struct BMP_INFO_HEADER
{
    unsigned int        Size;           // ヘッダサイズ (40固定).
    long                Width;          // 画像の横幅.
    long                Height;         // 画像の縦幅.
    unsigned short      Planes;         // プレーン数 (1固定).
    unsigned short      BitCount;       // 1ピクセルあたりのビット数.
    unsigned int        Compression;    // 圧縮形式.
    unsigned int        SizeImage;      // 画像データ部のサイズ.
    long                XPelsPerMeter;  // 横方向の解像度.
    long                YPelsPerMeter;  // 縦方向の解像度.
    unsigned int        ClrUsed;        // 格納されているパレット数.
    unsigned int        ClrImportant;   // 重要なパレットのインデックス.
};
#pragma pack( pop )



//-------------------------------------------------------------------------------------------------
//      BMPファイルヘッダを書き込みます.
//-------------------------------------------------------------------------------------------------
void WriteBmpFileHeader( BMP_FILE_HEADER& header, FILE* pFile )
{
    fwrite( &header.Type,       sizeof(unsigned short), 1, pFile );
    fwrite( &header.Size,       sizeof(unsigned int),   1, pFile );
    fwrite( &header.Reserved1,  sizeof(unsigned short), 1, pFile );
    fwrite( &header.Reserved2,  sizeof(unsigned short), 1, pFile );
    fwrite( &header.OffBits,    sizeof(unsigned int),   1, pFile );
}

//-------------------------------------------------------------------------------------------------
//      BMP情報ヘッダを書き込みます.
//-------------------------------------------------------------------------------------------------
void WriteBmpInfoHeader( BMP_INFO_HEADER& header, FILE* pFile )
{
    fwrite( &header.Size,           sizeof(unsigned int),   1, pFile );
    fwrite( &header.Width,          sizeof(long),           1, pFile );
    fwrite( &header.Height,         sizeof(long),           1, pFile );
    fwrite( &header.Planes,         sizeof(unsigned short), 1, pFile );
    fwrite( &header.BitCount,       sizeof(unsigned short), 1, pFile );
    fwrite( &header.Compression,    sizeof(unsigned int),   1, pFile );
    fwrite( &header.SizeImage,      sizeof(unsigned int),   1, pFile );
    fwrite( &header.XPelsPerMeter,  sizeof(long),           1, pFile );
    fwrite( &header.YPelsPerMeter,  sizeof(long),           1, pFile );
    fwrite( &header.ClrUsed,        sizeof(unsigned int),   1, pFile );
    fwrite( &header.ClrImportant,   sizeof(unsigned int),   1, pFile );
}

//-------------------------------------------------------------------------------------------------
//      BMPファイルに書き出します.
//-------------------------------------------------------------------------------------------------
void WriteBmp( FILE* pFile, const s32 width, const s32 height, const Color3* pPixel )
{
    BMP_FILE_HEADER fileHeader;
    BMP_INFO_HEADER infoHeader;

    fileHeader.Type      = 'MB';
    fileHeader.Size      = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER) + ( width * height * 3 );
    fileHeader.Reserved1 = 0;
    fileHeader.Reserved2 = 0;
    fileHeader.OffBits   = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER);

    infoHeader.Size          = 40;
    infoHeader.Width         = width;
    infoHeader.Height        = height;
    infoHeader.Planes        = 1;
    infoHeader.BitCount      = 24;
    infoHeader.Compression   = BMP_COMPRESSION_RGB;
    infoHeader.SizeImage     = 0;
    infoHeader.XPelsPerMeter = 0;
    infoHeader.YPelsPerMeter = 0;
    infoHeader.ClrUsed       = 0;
    infoHeader.ClrImportant  = 0;

    WriteBmpFileHeader( fileHeader, pFile );
    WriteBmpInfoHeader( infoHeader, pFile );

    for ( int i=0; i<height; ++i )
    {
        for( int j=0; j<width; ++j )
        {
            s32 index = ( i * width  ) + j;

            f32 r = pPixel[index].x;
            f32 g = pPixel[index].y;
            f32 b = pPixel[index].z;

            if ( r > 1.0f ) { r = 1.0f; }
            if ( g > 1.0f ) { g = 1.0f; }
            if ( b > 1.0f ) { b = 1.0f; }

            if ( r < 0.0f ) { r = 0.0f; }
            if ( g < 0.0f ) { g = 0.0f; }
            if ( b < 0.0f ) { b = 0.0f; }

            // sRGB OETF
            r = (r <= 0.0031308f) ? 12.92f * r : std::pow(1.055f * r, 1.0f / 2.4f) - 0.055f;
            g = (g <= 0.0031308f) ? 12.92f * g : std::pow(1.055f * g, 1.0f / 2.4f) - 0.055f;
            b = (b <= 0.0031308f) ? 12.92f * b : std::pow(1.055f * b, 1.0f / 2.4f) - 0.055f;

            u8 R = static_cast<u8>( r * 255.0f + 0.5f );
            u8 G = static_cast<u8>( g * 255.0f + 0.5f );
            u8 B = static_cast<u8>( b * 255.0f + 0.5f );

            {
                fwrite( &B, sizeof(u8), 1, pFile );
                fwrite( &G, sizeof(u8), 1, pFile );
                fwrite( &R, sizeof(u8), 1, pFile );
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// TGA_FORMA_TYPE enum
////////////////////////////////////////////////////////////////////////////////////////////////////
enum TGA_FORMAT_TYPE
{
    TGA_FORMAT_NONE             = 0,        //!< イメージなし.
    TGA_FORMAT_INDEXCOLOR       = 1,        //!< インデックスカラー(256色).
    TGA_FORMAT_FULLCOLOR        = 2,        //!< フルカラー
    TGA_FORMAT_GRAYSCALE        = 3,        //!< 白黒.
    TGA_FORMAT_RLE_INDEXCOLOR   = 9,        //!< RLE圧縮インデックスカラー.
    TGA_FORMAT_RLE_FULLCOLOR    = 10,       //!< RLE圧縮フルカラー.
    TGA_FORMAT_RLE_GRAYSCALE    = 11,       //!< RLE圧縮白黒.
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// TGA_HEADER structure
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct TGA_HEADER
{
    u8  IdFieldLength;      // IDフィードのサイズ(範囲は0～255).
    u8  HasColorMap;        // カラーマップ有無(0=なし, 1=あり)
    u8  Format;             // 画像形式.
    u16 ColorMapEntry;      // カラーマップエントリー.
    u16 ColorMapLength;     // カラーマップのエントリーの総数.
    u8  ColorMapEntrySize;  // カラーマップの1エントリー当たりのビット数.
    u16 OffsetX;            // 画像のX座標.
    u16 OffsetY;            // 画像のY座標.
    u16 Width;              // 画像の横幅.
    u16 Height;             // 画像の縦幅.
    u8  BitPerPixel;        // ビットの深さ.
    u8  ImageDescriptor;    // (0~3bit) : 属性, 4bit : 格納方向(0=左から右,1=右から左), 5bit : 格納方向(0=下から上, 1=上から下), 6~7bit : インタリーブ(使用不可).
};
#pragma pack( pop )


////////////////////////////////////////////////////////////////////////////////////////////////////
// TGA_FOOTER structure
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct TGA_FOOTER
{
    u32  OffsetExt;      // 拡張データへのオフセット(byte数) [オフセットはファイルの先頭から].
    u32  OffsetDev;      // ディベロッパーエリアへのオフセット(byte数)[オフセットはファイルの先頭から].
    char Tag[18];        // 'TRUEVISION-XFILE.\0'
};
#pragma pack( pop )


///////////////////////////////////////////////////////////////////////////////////////////////////
// TGA_EXTENSION structure
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct TGA_EXTENSION
{
    u16     Size;                       //!< サイズ.
    char    AuthorName[ 41 ];           //!< 著作者名.
    char    AuthorComment[ 324 ];       //!< 著作者コメント.
    u16     StampMonth;                 //!< タイムスタンプ　月(1-12).
    u16     StampDay;                   //!< タイムスタンプ　日(1-31).
    u16     StampYear;                  //!< タイムスタンプ　年(4桁, 例1989).
    u16     StampHour;                  //!< タイムスタンプ　時(0-23).
    u16     StampMinute;                //!< タイムスタンプ　分(0-59).
    u16     StampSecond;                //!< タイムスタンプ　秒(0-59).
    char    JobName[ 41 ];              //!< ジョブ名 (最後のバイトはゼロが必須).
    u16     JobHour;                    //!< ジョブ時間  時(0-65535)
    u16     JobMinute;                  //!< ジョブ時間　分(0-59)
    u16     JobSecond;                  //!< ジョブ時間　秒(0-59)
    char    SoftwareId[ 41 ];           //!< ソフトウェアID (最後のバイトはゼロが必須).
    u16     VersionNumber;              //!< ソフトウェアバージョン    VersionNumber * 100になる.
    u8      VersionLetter;              //!< ソフトウェアバージョン
    u32     KeyColor;                   //!< キーカラー.
    u16     PixelNumerator;             //!< ピクセル比分子　ピクセル横幅.
    u16     PixelDenominator;           //!< ピクセル比分母　ピクセル縦幅.
    u16     GammaNumerator;             //!< ガンマ値分子.
    u16     GammaDenominator;           //!< ガンマ値分母
    u32     ColorCorrectionOffset;      //!< 色補正テーブルへのオフセット.
    u32     StampOffset;                //!< ポステージスタンプ画像へのオフセット.
    u32     ScanLineOffset;             //!< スキャンラインオフセット.
    u8      AttributeType;              //!< アルファチャンネルデータのタイプ
};
#pragma pack( pop )



//-------------------------------------------------------------------------------------------------
//! @brief      8Bitインデックスカラー形式を解析します.
//!
//! @param[in]      pColorMap       カラーマップです.
//-------------------------------------------------------------------------------------------------
void Parse8Bits( FILE* pFile, u32 size, u8* pColorMap, u8* pPixels )
{
    u8 color = 0;
    for( u32 i=0; i<size; ++i )
    {
        color = (u8)fgetc( pFile );
        pPixels[ i * 3 + 2 ] = pColorMap[ color * 3 + 0 ];
        pPixels[ i * 3 + 1 ] = pColorMap[ color * 3 + 1 ];
        pPixels[ i * 3 + 0 ] = pColorMap[ color * 3 + 2 ];
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      16Bitフルカラー形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse16Bits( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        u16 color = fgetc( pFile ) + ( fgetc( pFile ) << 8 );
        pPixels[ i * 3 + 0 ] = (u8)(( ( color & 0x7C00 ) >> 10 ) << 3);
        pPixels[ i * 3 + 1 ] = (u8)(( ( color & 0x03E0 ) >>  5 ) << 3);
        pPixels[ i * 3 + 2 ] = (u8)(( ( color & 0x001F ) >>  0 ) << 3);
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      24Bitフルカラー形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse24Bits( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        pPixels[ i * 4 + 2 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 1 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 0 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 3 ] = 255;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      32Bitフルカラー形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse32Bits( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        pPixels[ i * 4 + 2 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 1 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 0 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 3 ] = (u8)fgetc( pFile );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief     8Bitグレースケール形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse8BitsGrayScale( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        pPixels[ i ] = (u8)fgetc( pFile );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      16Bitグレースケール形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse16BitsGrayScale( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        pPixels[ i * 2 + 0 ] = (u8)fgetc( pFile );
        pPixels[ i * 2 + 1 ] = (u8)fgetc( pFile );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      8BitRLE圧縮インデックスカラー形式を解析します.
//!
//! @param[in]  pColorMap       カラーマップです.
//-------------------------------------------------------------------------------------------------
void Parse8BitsRLE( FILE* pFile, u8* pColorMap, u32 size, u8* pPixels )
{
    u32 count  = 0;
    u8  color  = 0;
    u8  header = 0;
    u8* ptr    = pPixels;

    while( ptr < pPixels + size )   // size = width * height * 3.
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            color = (u8)fgetc( pFile );

            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                ptr[ 0 ] = pColorMap[ color * 3 + 2 ];
                ptr[ 1 ] = pColorMap[ color * 3 + 1 ];
                ptr[ 2 ] = pColorMap[ color * 3 + 0 ];
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                color = (u8)fgetc( pFile );

                ptr[ 0 ] = pColorMap[ color * 3 + 2 ];
                ptr[ 1 ] = pColorMap[ color * 3 + 1 ];
                ptr[ 2 ] = pColorMap[ color * 3 + 0 ];
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      16BitRLE圧縮フルカラー形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse16BitsRLE( FILE* pFile, u32 size, u8* pPixels )
{
    u32 count  = 0;
    u16 color  = 0;
    u8  header = 0;
    u8* ptr    = pPixels;

    while( ptr < pPixels + size )   // size = width * height * 3.
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            color = fgetc( pFile ) + ( fgetc( pFile ) << 8 ); 

            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                ptr[ 0 ] = (u8)(( ( color & 0x7C00 ) >> 10 ) << 3);
                ptr[ 1 ] = (u8)(( ( color & 0x03E0 ) >>  5 ) << 3);
                ptr[ 2 ] = (u8)(( ( color & 0x001F ) >>  0 ) << 3);
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                color = fgetc( pFile ) + ( fgetc( pFile ) << 8 );

                ptr[ 0 ] = (u8)(( ( color & 0x7C00 ) >> 10 ) << 3);
                ptr[ 1 ] = (u8)(( ( color & 0x03E0 ) >>  5 ) << 3);
                ptr[ 2 ] = (u8)(( ( color & 0x001F ) >>  0 ) << 3);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      24BitRLE圧縮フルカラー形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse24BitsRLE( FILE* pFile, u32 size, u8* pPixels )
{
    u32 count    = 0;
    u8  color[3] = { 0, 0, 0 };
    u8  header   = 0;
    u8* ptr      = pPixels;

    while( ptr < pPixels + size )   // size = width * height * 3.
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            fread( color, sizeof(u8), 3, pFile );

            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                ptr[ 0 ] = color[ 2 ];
                ptr[ 1 ] = color[ 1 ];
                ptr[ 2 ] = color[ 0 ];
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                ptr[ 2 ] = (u8)fgetc( pFile );
                ptr[ 1 ] = (u8)fgetc( pFile );
                ptr[ 0 ] = (u8)fgetc( pFile );
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      32BitRLE圧縮フルカラー形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse32BitsRLE( FILE* pFile, u32 size, u8* pPixels )
{
    u32 count    = 0;
    u8  color[4] = { 0, 0, 0, 0 };
    u8  header   = 0;
    u8* ptr      = pPixels;

    while( ptr < pPixels + size )   // size = width * height * 4.
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            fread( color, sizeof(u8), 4, pFile );

            for( u32 i=0; i<count; ++i, ptr+=4 )
            {
                ptr[ 0 ] = color[ 2 ];
                ptr[ 1 ] = color[ 1 ];
                ptr[ 2 ] = color[ 0 ];
                ptr[ 3 ] = color[ 3 ];
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=4 )
            {
                ptr[ 2 ] = (u8)fgetc( pFile );
                ptr[ 1 ] = (u8)fgetc( pFile );
                ptr[ 0 ] = (u8)fgetc( pFile );
                ptr[ 3 ] = (u8)fgetc( pFile );
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      8BitRLE圧縮グレースケール形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse8BitsGrayScaleRLE( FILE* pFile, u32 size, u8* pPixles )
{
    u32 count  = 0;
    u8  color  = 0;
    u8  header = 0;
    u8* ptr    = pPixles;

    while( ptr < pPixles + size ) // size = width * height
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            color = (u8)fgetc( pFile );

            for( u32 i=0; i<count; ++i, ptr++ )
            { (*ptr) = color; }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr++ )
            { (*ptr) = (u8)fgetc( pFile ); }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      16BitRLE圧縮グレースケール形式を解析します.
//-------------------------------------------------------------------------------------------------
void Parse16BitsGrayScaleRLE( FILE* pFile, u32 size, u8* pPixles )
{
    u32 count  = 0;
    u8  color  = 0;
    u8  alpha  = 0;
    u8  header = 0;
    u8* ptr    = pPixles;

    while( ptr < pPixles + size ) // size = width * height * 2
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            color = (u8)fgetc( pFile );
            alpha = (u8)fgetc( pFile );

            for( u32 i=0; i<count; ++i, ptr+=2 )
            {
                ptr[ 0 ] = color;
                ptr[ 1 ] = alpha;
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=2 )
            {
                ptr[ 0 ] = (u8)fgetc( pFile );
                ptr[ 1 ] = (u8)fgetc( pFile );
            }
        }
    }
}


template<typename T>
T Mini(const T& a, const T& b)
{ return ( a < b ) ? a : b; }

////////////////////////////////////////////////////////////////////////////////////////////
// RGBE structure
////////////////////////////////////////////////////////////////////////////////////////////
struct RGBE
{
    union
    {
        struct
        {
            u8 r;
            u8 g;
            u8 b;
            u8 e;
        };
        u8 v[4];
    };
};

//------------------------------------------------------------------------------------------
//      RGBE形式からVector3形式に変換します.
//------------------------------------------------------------------------------------------
S3D_INLINE
s3d::Vector3 RGBEToVec3( const RGBE& val )
{
    s3d::Vector3 result;
    if ( val.e )
    {
        auto f = ldexp( 1.0, val.e - (int)(128+8) );
        result.x = static_cast<f32>( val.r * f );
        result.y = static_cast<f32>( val.g * f );
        result.z = static_cast<f32>( val.b * f );
    }
    else 
    {
        result.x = 0.0f;
        result.y = 0.0f;
        result.z = 0.0f;
    }
    return result;
}

//------------------------------------------------------------------------------------------
//      Vector3形式からRGBE形式に変換します.
//------------------------------------------------------------------------------------------
S3D_INLINE
RGBE Vec3ToRGBE( const s3d::Vector3& val )
{
    RGBE result;
    double d = s3d::Max( val.x, s3d::Max( val.y, val.z ) );

    if ( d <= DBL_EPSILON )
    {
        result.r = 0;
        result.g = 0;
        result.b = 0;
        result.e = 0;
        return result;
    }

    int e;
    double m = frexp(d, &e); // d = m * 2^e
    d = m * 256.0 / d;

    result.r = (u32)(val.x * d);
    result.g = (u32)(val.y * d);
    result.b = (u32)(val.z * d);
    result.e = (u32)(e + 128);
    return result;
}

//------------------------------------------------------------------------------------------
//      HDRファイルのヘッダを書き込みます.
//------------------------------------------------------------------------------------------
void WriteHdrHeader( FILE* pFile, s32 width, s32 height, f32 gamma, f32 exposure )
{
    fprintf_s( pFile, "#?RADIANCE\n" );
    fprintf_s( pFile, "# Made with 0%% pure HDR Shop.\n" );
    fprintf_s( pFile, "FORMAT=32-bit_rle_rgbe\n" );
    fprintf_s( pFile, "GAMMA=%f\n", gamma );
    fprintf_s( pFile, "EXPOSURE=%lf\n", exposure );
    fprintf_s( pFile, "\n" );
    fprintf_s( pFile, "-Y %d +X %d\n", height, width ); // w行のデータがh列ある.
}

//------------------------------------------------------------------------------------------
//      HDRファイルのヘッダを読み込みします.
//------------------------------------------------------------------------------------------
bool ReadHdrHeader( FILE* pFile, s32& width, s32& height, f32& gamma, f32& exposure )
{
    char buf[ 256 ];
    fread( buf, sizeof(char), 2, pFile );

    if ( buf[0] != '#' || buf[1] != '?' )
    { return false; }

    auto valid = false;
    for( ;; )
    {
        if ( fgets( buf, 256, pFile ) == nullptr )
        { break; }

        if ( buf[0] == '\n' )
        { break; }
        else if ( buf[0] == '#' )
        { continue; }
        else
        {
            auto g = 1.0f;
            auto e = 1.0f;
            if ( sscanf_s( buf, "GAMMA=%f\n", &g ) != 0 ) 
            { gamma = g; }
            else if ( sscanf_s( buf, "EXPOSURE=%f\n", &e ) != 0 )
            { exposure = e; }
            else if ( strcmp( buf, "FORMAT=32-bit_rle_rgbe\n" ) == 0 )
            { valid = true; }
        }
    }

    if ( !valid )
    { return false; }

    if ( fgets( buf, 256, pFile ) != nullptr )
    {
        auto w = 0;
        auto h = 0;
        if ( sscanf_s( buf, "-Y %d +X %d\n", &h, &w ) != 0 )
        {
            width = w;
            height = h;
        }
        else if ( sscanf_s( buf, "+X %d -Y %d\n", &w, &h ) != 0 )
        {
            width = w;
            height = h;
        }
        else
        { return false; }
    }

    return true;
}

//------------------------------------------------------------------------------------------
//      旧形式のカラーを読み取ります.
//------------------------------------------------------------------------------------------
bool ReadOldColors( FILE* pFile, RGBE* pLine, s32 count )
{
    auto shift = 0;
    while( 0 < count )
    {
        pLine[0].r = getc( pFile );
        pLine[0].g = getc( pFile );
        pLine[0].b = getc( pFile );
        pLine[0].e = getc( pFile );

        if ( feof( pFile ) || ferror( pFile ) )
            return false;

        if ( pLine[0].r == 1
          && pLine[0].g == 1
          && pLine[0].b == 1 )
        {
            for( auto i=pLine[0].e << shift; i > 0; i-- )
            {
                pLine[0].r = pLine[-1].r;
                pLine[0].g = pLine[-1].g;
                pLine[0].b = pLine[-1].b;
                pLine[0].e = pLine[-1].e;
                pLine++;
                count--;
            }
            shift += 8;
        }
        else
        {
            pLine++;
            count--;
            shift = 0;
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------
//      カラーを読み取ります.
//------------------------------------------------------------------------------------------
bool ReadColor( FILE* pFile, RGBE* pLine, s32 count )
{
    if ( count < 8 || 0x7fff < count )
    { return ReadOldColors( pFile, pLine, count ); }

    auto i = getc( pFile );
    if ( i == EOF )
        return false;

    if ( i != 2 )
    {
        ungetc( i, pFile );
        return ReadOldColors( pFile, pLine, count );
    }

    pLine[0].g = getc( pFile );
    pLine[0].b = getc( pFile );

    if ( ( i = getc( pFile ) ) == EOF )
        return false;

    if ( pLine[0].g != 2 || pLine[0].b & 128 )
    {
        pLine[0].r = 2;
        pLine[0].e = i;
        return ReadOldColors( pFile, pLine + 1, count -1 );
    }

    if ( ( pLine[0].b << 8 | i ) != count )
        return false;

    for( i=0; i<4; ++i )
    {
        for( auto j=0; j<count; )
        {
            auto code = getc( pFile );
            if ( code == EOF )
                return false;

            if ( 128 < code )
            {
                code &= 127;
                auto val = getc( pFile );
                while( code-- )
                { pLine[j++].v[i] = val; }
            }
            else
            {
                while( code-- )
                { pLine[j++].v[i] = getc( pFile ); }
            }
        }
    }

    return ( feof( pFile ) ? false : true );
}

//------------------------------------------------------------------------------------------
//      HDRデータを読み取ります.
//------------------------------------------------------------------------------------------
bool ReadHdrData( FILE* pFile, const s32 width, const s32 height, Color3** ppPixels )
{
    auto pLines = new(std::nothrow) RGBE [ width * height ];
    if ( pLines == nullptr )
    { return false; }

    auto pixels = new (std::nothrow) Color3 [ width * height ];
    if ( pixels == nullptr )
    { return false; }

    for( auto y=0; y<height; ++y )
    {
        if ( !ReadColor( pFile, pLines, width ) )
        {
            SafeDeleteArray( pLines );
            SafeDeleteArray( pixels );
            return false;
        }

        for( auto x =0; x < width; x++ )
        {
            auto pix = RGBEToVec3( pLines[x] );
            auto idx = x + y * width;
            pixels[idx].x = pix.x;
            pixels[idx].y = pix.y;
            pixels[idx].z = pix.z;
        }
    }

    SafeDeleteArray( pLines );
    (*ppPixels) = pixels;

    return true;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////
// Texture2D class
////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------------
Texture::Texture()
: m_Width       ( 0 )
, m_Height      ( 0 )
, m_Size        ( 0 )
, m_pColors     ( nullptr )
, m_pAlphas     ( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------------------
Texture::~Texture()
{ Term(); }

//--------------------------------------------------------------------------------------
//      ファイルからロードします.
//--------------------------------------------------------------------------------------
bool Texture::Init( const char* filename, TEXTURE_FILE_FORMAT format )
{
    // NULLチェック.
    if ( filename == nullptr )
    { return false; }

    int w = 0;
    int h = 0;

    if (format == TFF_BMP)
    {
        if (!LoadFromBMP(filename, w, h, &m_pColors))
        { return false; }
    }
    else if (format == TFF_TGA)
    {
        if (!LoadFromTGA(filename, w, h, &m_pColors, &m_pAlphas))
        { return false; }
    }
    else if (format == TFF_HDR)
    {
        if (!LoadFromHDR(filename, w, h, &m_pColors))
        { return false; }
    }

    m_Width  = w;
    m_Height = h;
    m_Size   = w * h;
    return true;
}

//--------------------------------------------------------------------------------------
//      メモリ解放処理を行います.
//--------------------------------------------------------------------------------------
void Texture::Term()
{
    // メモリ解放.
    SafeDeleteArray( m_pColors );
    SafeDeleteArray( m_pAlphas );

    // ゼロリセット.
    m_Width  = 0;
    m_Height = 0;
    m_Size   = 0;
}

//---------------------------------------------------------------------------------------
//      カラー値をサンプリングします.
//---------------------------------------------------------------------------------------
Color3 Texture::SampleColor(const Vector2& texcoord) const
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

    // バイリニア補間.
    return x_w0 * ( y_w0 * GetColor( x0, y0 ) + y_w1 * GetColor( x0, y1 ) )
         + x_w1 * ( y_w0 * GetColor( x1, y0 ) + y_w1 * GetColor( x1, y1 ) );
}

//---------------------------------------------------------------------------------------
//      カラー値をサンプリングします(スフィアマップ用).
//---------------------------------------------------------------------------------------
Color3 Texture::SampleColor(const Vector3& dir) const
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

    return SampleColor(uv);
}

//---------------------------------------------------------------------------------------
//      アルファ値をサンプリングします.
//---------------------------------------------------------------------------------------
f32 Texture::SampleAlpha(const Vector2& texcoord) const
{
    if (m_pAlphas == nullptr)
    { return 1.0f; }

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

    // バイリニア補間.
    return x_w0 * ( y_w0 * GetAlpha( x0, y0 ) + y_w1 * GetAlpha( x0, y1 ) )
         + x_w1 * ( y_w0 * GetAlpha( x1, y0 ) + y_w1 * GetAlpha( x1, y1 ) );
}


//-------------------------------------------------------------------------------------------------
//      BMPファイルに保存します.
//-------------------------------------------------------------------------------------------------
bool SaveToBMP( const char* filename, const s32 width, const s32 height, const Color3* pPixel )
{
    FILE* pFile;
    errno_t err = fopen_s( &pFile, filename, "wb" );
    if ( err != 0 )
    { return false; }

    WriteBmp( pFile, width, height, pPixel );

    fclose( pFile );
    return true;
}

//-------------------------------------------------------------------------------------------------
//      BMPファイルから読み込みます.
//-------------------------------------------------------------------------------------------------
bool LoadFromBMP( const char* filename, s32& width, s32& height, Color3** ppPixels )
{
    FILE* pFile;
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    { return false; }

    BMP_FILE_HEADER fileHeader;
    
    fread( &fileHeader, sizeof(fileHeader), 1, pFile );
    if ( fileHeader.Type != 'MB' )
    {
        fclose( pFile );
        return false;
    }

    BMP_INFO_HEADER infoHeader;
    fread( &infoHeader, sizeof(infoHeader), 1, pFile );

    if ( infoHeader.BitCount != 24 )
    {
        fclose( pFile );
        return false;
    }

    width  = static_cast<s32>( infoHeader.Width );
    height = static_cast<s32>( infoHeader.Height );

    s32 size = width * height * 3;
    u8* pTexels = new u8 [ size ];
    assert( pTexels != nullptr );

    fread( pTexels, sizeof(u8), size, pFile );

    fclose( pFile );

    (*ppPixels) = new Color3 [ width * height];
    for( s32 i=0, j=0; i<size-3; i+=3, j++)
    {
        (*ppPixels)[ j ].x = static_cast<f32>( pTexels[ i + 2 ] ) / 255.0f;
        (*ppPixels)[ j ].y = static_cast<f32>( pTexels[ i + 1 ] ) / 255.0f;
        (*ppPixels)[ j ].z = static_cast<f32>( pTexels[ i + 0 ] ) / 255.0f;
    }

    delete [] pTexels;
    pTexels = nullptr;

    return true;
}


//-------------------------------------------------------------------------------------------------
//      TGAファイルを読み込みます.
//-------------------------------------------------------------------------------------------------
bool LoadFromTGA
(
    const char* filename,
    s32&        width,
    s32&        height,
    Color3**    ppColors,
    f32**       ppAlphas
)
{
    if ( filename == nullptr || ppColors == nullptr || ppAlphas == nullptr )
    { return false; }

    FILE* pFile;
    auto err = fopen_s( &pFile, filename, "rb ");
    if ( err != 0 )
    { return false; }

    TGA_FOOTER footer;
    long offset = sizeof(footer);
    fseek( pFile, -offset, SEEK_END );
    fread( &footer, sizeof(footer), 1, pFile );

    if ( strcmp( footer.Tag, "TRUEVISION-XFILE." ) != 0 &&
         strcmp( footer.Tag, "TRUEVISION-TARGA." ) != 0 )
    {
        fclose( pFile );
        return false;
    }

    // 拡張データがある場合は読み込み.
    if ( footer.OffsetExt != 0 )
    {
        TGA_EXTENSION extension;

        fseek( pFile, footer.OffsetExt, SEEK_SET );
        fread( &extension, sizeof(extension), 1, pFile );
    }

    // ディベロッパーエリアがある場合.
    if ( footer.OffsetDev != 0 )
    {
        /* NOT IMPLEMENT */
    }

    // ファイル先頭に戻す.
    fseek( pFile, 0, SEEK_SET );

    // ヘッダデータを読み込む.
    TGA_HEADER header;
    fread( &header, sizeof(header), 1, pFile );

    // フォーマット判定.
    u32 bytePerPixel;
    switch( header.Format )
    {
    // 該当なし.
    case TGA_FORMAT_NONE:
        {
            fclose( pFile );
            return false;
        }
        break;

    // グレースケール
    case TGA_FORMAT_GRAYSCALE:
    case TGA_FORMAT_RLE_GRAYSCALE:
        { 
            if ( header.BitPerPixel == 8 )
            { bytePerPixel = 1; }
            else
            { bytePerPixel = 2; }
        }
        break;

    // カラー.
    case TGA_FORMAT_INDEXCOLOR:
    case TGA_FORMAT_FULLCOLOR:
    case TGA_FORMAT_RLE_INDEXCOLOR:
    case TGA_FORMAT_RLE_FULLCOLOR:
        {
            if ( header.BitPerPixel <= 24 )
            { bytePerPixel = 3; }
            else
            { bytePerPixel = 4; }
        }
        break;

    // 上記以外.
    default:
        {
            fclose( pFile );
            return false;
        }
        break;
    }

    // IDフィールドサイズ分だけオフセットを移動させる.
    fseek( pFile, header.IdFieldLength, SEEK_CUR );

    // ピクセルサイズを決定してメモリを確保.
    auto size = header.Width * header.Height * bytePerPixel;
    auto pPixels = new u8 [ size ];
    if ( pPixels == nullptr )
    {
        fclose( pFile );
        return false;
    }

    // カラーマップを持つかチェック.
    u8* pColorMap = nullptr;
    if ( header.HasColorMap )
    {
        // カラーマップサイズを算出.
        u32 colorMapSize = header.ColorMapEntry * ( header.ColorMapEntrySize >> 3 );

        // メモリを確保.
        pColorMap = new u8 [ colorMapSize ];
        if ( pColorMap == nullptr )
        {
            SafeDeleteArray( pPixels );
            fclose( pFile );
            return false;
        }

        // がばっと読み込む.
        fread( pColorMap, sizeof(u8), colorMapSize, pFile );
    }

    // 幅・高さ・ビットの深さ・ハッシュキーを設定.
    width       = header.Width;
    height      = header.Height;

    s32 component = 0;

    // フォーマットに合わせてピクセルデータを解析する.
    switch( header.Format )
    {
    // パレット.
    case TGA_FORMAT_INDEXCOLOR:
        {
            component = 3;
            Parse8Bits( pFile, width * height, pColorMap, pPixels );
        }
        break;

    // フルカラー.
    case TGA_FORMAT_FULLCOLOR:
        {
            switch( header.BitPerPixel )
            {
            case 16:
                {
                    component = 3;
                    Parse16Bits( pFile, width * height, pPixels );
                }
                break;

            case 24:
                {
                    component = 3;
                    Parse24Bits( pFile, width * height, pPixels );
                }
                break;

            case 32:
                {
                    component = 4;
                    Parse32Bits( pFile, width * height, pPixels );
                }
                break;
            }
        }
        break;

    // グレースケール.
    case TGA_FORMAT_GRAYSCALE:
        {
            if ( header.BitPerPixel == 8 )
            {
                component = 1;
                Parse8BitsGrayScale( pFile, width * height, pPixels );
            }
            else
            {
                component = 2;
                Parse16BitsGrayScale( pFile, width * height, pPixels );
            }
        }
        break;

    // パレットRLE圧縮.
    case TGA_FORMAT_RLE_INDEXCOLOR:
        {
            component = 3;
            Parse8BitsRLE( pFile, pColorMap, width * height * 3, pPixels );
        }
        break;

    // フルカラーRLE圧縮.
    case TGA_FORMAT_RLE_FULLCOLOR:
        {
            switch( header.BitPerPixel )
            {
            case 16:
                {
                    component = 3;
                    Parse16BitsRLE( pFile, width * height * 3, pPixels );
                }
                break;

            case 24:
                {
                    component = 3;
                    Parse24BitsRLE( pFile, width * height * 3, pPixels );
                }
                break;

            case 32:
                {
                    component = 4;
                    Parse32BitsRLE( pFile, width * height * 4, pPixels );
                }
                break;
            }
        }
        break;

    // グレースケールRLE圧縮.
    case TGA_FORMAT_RLE_GRAYSCALE:
        {
            if ( header.BitPerPixel == 8 )
            {
                component = 1;
                Parse8BitsGrayScaleRLE( pFile, width * height, pPixels );
            }
            else
            {
                component = 2;
                Parse16BitsGrayScaleRLE( pFile, width * height * 2, pPixels );
            }
        }
        break;
    }

    // 不要なメモリを解放.
    SafeDeleteArray( pColorMap );

    // ファイルを閉じる.
    fclose( pFile );

    // Float に変換.
    auto pixelCount = width * height * component;
    (*ppColors) = new Color3 [ width * height ];

    if (component == 4 || component == 2)
    {
        (*ppAlphas) = new f32 [ width * height ];
    }

    switch( component )
    {
        case 1:
            {
                for( auto i=0; i<pixelCount; i++ )
                {
                    auto color = static_cast<f32>( pPixels[i] ) / 255.0f;
                    (*ppColors)[i].x = color;
                    (*ppColors)[i].y = color;
                    (*ppColors)[i].z = color;
                }
            }
            break;

        case 2:
            {
                for( auto i=0, j=0; i<pixelCount-2; i+=2, j++ )
                {
                    auto color = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    auto alpha = static_cast<f32>( pPixels[i + 1] ) / 255.0f;

                    (*ppColors)[j].x = color;
                    (*ppColors)[j].y = color;
                    (*ppColors)[j].z = color;
                    (*ppAlphas)[j]   = alpha;
                }

            }
            break;

        case 3:
            {
                for( auto i=0, j=0; i<pixelCount-3; i+3, j++ )
                {
                    (*ppColors)[j].x = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppColors)[j].y = static_cast<f32>( pPixels[i + 1] ) / 255.0f;
                    (*ppColors)[j].z = static_cast<f32>( pPixels[i + 2] ) / 255.0f;
                }
            }
            break;

        case 4:
            {
                for( auto i=0, j=0; i<pixelCount-4; i+=4, j++ )
                {
                    (*ppColors)[j].x = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppColors)[j].y = static_cast<f32>( pPixels[i + 1] ) / 255.0f;
                    (*ppColors)[j].z = static_cast<f32>( pPixels[i + 2] ) / 255.0f;
                    (*ppAlphas)[j]   = static_cast<f32>( pPixels[i + 3] ) / 255.0f;
                }
            }
            break;
    }

    // 不要なメモリを解放.
    SafeDeleteArray( pPixels );

    // 正常終了.
    return true;
}


//------------------------------------------------------------------------------------------
//      HDRファイルからデータをロードします.
//------------------------------------------------------------------------------------------
bool LoadFromHDR( const char* filename, s32& width, s32& height, Color3** ppPixels )
{
    FILE* pFile;

    auto err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        ELOG( "Error : LoadFromHDR() Failed. File Open Failed. filename = %s", filename );
        return false;
    }

    f32 gamma    = 1.0f;
    f32 exposure = 1.0f;
    if ( !ReadHdrHeader( pFile, width, height, gamma, exposure ) )
    {
        ELOG( "Error : LoadFromHDR() Failed. Header Read Failed. filename = %s", filename );
        return false;
    }

    if ( !ReadHdrData( pFile, width, height, ppPixels ) )
    {
        ELOG( "Error : LoadFromHDR() Failed. Data Read Failed. filename = %s", filename );
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
//      HDRファイルにデータをセーブします.
//------------------------------------------------------------------------------------------
bool SaveToHDR( const char* filename, const s32 width, const s32 height, const Color3* pPixels )
{
    FILE* pFile;
    auto err = fopen_s( &pFile, filename, "wb" );
    if ( err != 0 )
    {
        ELOG( "Error : SaveToHDR() Failed. File Open Failed. filename = %s", filename );
        return false;
    }

    WriteHdrHeader( pFile, width, height, 1.0f, 1.0f );

    std::vector<RGBE> line;
    line.resize(width);

    for( auto i = height-1; i >=0; --i )
    {

        for( auto j=0; j<width; ++j )
        {
            auto idx = j + i * width;
            line[j] = Vec3ToRGBE( pPixels[idx] );
        }

        fprintf_s( pFile, "%c%c", 0x02, 0x02 );
        fprintf_s( pFile, "%c%c", (width >>8) & 0xFF, width & 0xFF );

        for( auto k=0; k<4; ++k )
        {
            for( auto cursor = 0; cursor < width; )
            {
                const auto cursor_move = Mini( 127, width - cursor );
                fprintf_s( pFile, "%c", cursor_move );

                for( auto l = cursor; l < cursor + cursor_move; ++l )
                { fprintf_s( pFile, "%c", line[l].v[k] ); }
                cursor += cursor_move;
            }
        }

        line.clear();
    }

    fclose( pFile );

    return true;
}


} // namespace s3d
