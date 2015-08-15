//-------------------------------------------------------------------------------------------------
// File : s3d_bmp.h
// Desc : Bitmap Utility Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_bmp.h>

#include <cstdio>
#include <cmath>
#include <cassert>


namespace s3d {

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
void WriteBmp( FILE* pFile, const s32 width, const s32 height, const f32* pPixel )
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
            s32 index = ( i * width * 3 ) + ( j * 3 );

            f32 r = pPixel[index + 0];
            f32 g = pPixel[index + 1];
            f32 b = pPixel[index + 2];

            if ( r > 1.0f ) { r = 1.0f; }
            if ( g > 1.0f ) { g = 1.0f; }
            if ( b > 1.0f ) { b = 1.0f; }

            if ( r < 0.0f ) { r = 0.0f; }
            if ( g < 0.0f ) { g = 0.0f; }
            if ( b < 0.0f ) { b = 0.0f; }

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

//-------------------------------------------------------------------------------------------------
//      BMPファイルに保存します.
//-------------------------------------------------------------------------------------------------
bool SaveToBMP( const char* filename, const s32 width, const s32 height, const f32* pPixel )
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
bool LoadFromBMP( const char* filename, s32& width, s32& height, f32** ppPixels )
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

    (*ppPixels) = new f32 [ size ];
    for( s32 i=0; i<size-3; i+=3)
    {
        (*ppPixels)[ i + 0 ] = static_cast<f32>( pTexels[ i + 2 ] ) / 255.0f;
        (*ppPixels)[ i + 1 ] = static_cast<f32>( pTexels[ i + 1 ] ) / 255.0f;
        (*ppPixels)[ i + 2 ] = static_cast<f32>( pTexels[ i + 0 ] ) / 255.0f;
    }

    delete [] pTexels;
    pTexels = nullptr;

    return true;
}

} // namespace s3d
