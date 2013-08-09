//-------------------------------------------------------------------------------------
// File : s3d_bmp.h
// Desc : Bitmap Utility Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include <cstdio>
#include <cmath>
#include <s3d_bmp.h>


namespace s3d {

//----------------------------------------------------------------------------------------
//! @brief      ビットマップファイルヘッダーです.
//----------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------
//! @brief      ビットマップの圧縮タイプです.
//----------------------------------------------------------------------------------------
enum BMP_COMPRESSION_TYPE
{
    BMP_COMPRESSION_RGB       = 0,      // 無圧縮.
    BMP_COMPRESSION_RLE8      = 1,      // RLE圧縮 8 bits/pixel.
    BMP_COMPRESSION_RLE4      = 2,      // RLE圧縮 4 bits/pixel.
    BMP_COMPRESSION_BITFIELDS = 3,      // ビットフィールド.
};

//----------------------------------------------------------------------------------------
//! @brief      ビットマップ情報ヘッダーです.
//----------------------------------------------------------------------------------------
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



//-------------------------------------------------------------------------------------------
//      BMPファイルヘッダを書き込みます.
//-------------------------------------------------------------------------------------------
void WriteBmpFileHeader( BMP_FILE_HEADER& header, FILE* pFile )
{
    fwrite( &header.Type,       sizeof(unsigned short), 1, pFile );
    fwrite( &header.Size,       sizeof(unsigned int),   1, pFile );
    fwrite( &header.Reserved1,  sizeof(unsigned short), 1, pFile );
    fwrite( &header.Reserved2,  sizeof(unsigned short), 1, pFile );
    fwrite( &header.OffBits,    sizeof(unsigned int),   1, pFile );
}

//-------------------------------------------------------------------------------------------
//      BMP情報ヘッダを書き込みます.
//-------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------
//      BMPファイルに書き出します.
//-------------------------------------------------------------------------------------------
void WriteBmp( FILE* pFile, const int width, const int height, const Color3* pPixel, const f64 gamma )
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
            int index = ( i * width ) + j;

            Color3 pixel = pPixel[index];

            pixel.r = pow( pixel.r, 1.0 / 2.2 );
            pixel.g = pow( pixel.g, 1.0 / 2.2 );
            pixel.b = pow( pixel.b, 1.0 / 2.2 );

            pixel.Clamp();

            unsigned char r = (unsigned char)( pixel.r * 255.0 );
            unsigned char g = (unsigned char)( pixel.g * 255.0 );
            unsigned char b = (unsigned char)( pixel.b * 255.0 );

            {
                fwrite( &b, sizeof(unsigned char), 1, pFile );
                fwrite( &g, sizeof(unsigned char), 1, pFile );
                fwrite( &r, sizeof(unsigned char), 1, pFile );
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
//      BMPファイルに保存します.
//-------------------------------------------------------------------------------------------
bool SaveToBMP( const char* filename, const int width, const int height, const Color3* pPixel, const f64 gamma )
{
    FILE* pFile;
    errno_t err = fopen_s( &pFile, filename, "wb" );
    if ( err != 0 )
    {
        return false;
    }

    WriteBmp( pFile, width, height, pPixel, gamma );

    fclose( pFile );
    return true;
}

} // namespace s3d
