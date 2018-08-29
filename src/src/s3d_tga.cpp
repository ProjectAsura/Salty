//-------------------------------------------------------------------------------------------------
// File : s3d_tga.cpp
// Desc : Targa Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <s3d_tga.h>


namespace /* anonymous */ {

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

} // namespace /* anonymous */


namespace s3d {

//-------------------------------------------------------------------------------------------------
//      TGAファイルを読み込みます.
//-------------------------------------------------------------------------------------------------
bool LoadFromTGA
(
    const char* filename,
    s32&        width,
    s32&        height,
    s32&        component,
    f32**       ppPixel
)
{
    if ( filename == nullptr || ppPixel == nullptr )
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
    if ( ppPixel == nullptr )
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
    (*ppPixel) = new f32 [ width * height *4 ];

    switch( component )
    {
        case 1:
            {
                for( auto i=0, j=0; i<pixelCount; i++, j+=4 )
                {
                    (*ppPixel)[j + 0] = static_cast<f32>( pPixels[i] ) / 255.0f;
                    (*ppPixel)[j + 1] = static_cast<f32>( pPixels[i] ) / 255.0f;
                    (*ppPixel)[j + 2] = static_cast<f32>( pPixels[i] ) / 255.0f;
                    (*ppPixel)[j + 3] = 1.0f;
                }
            }
            break;

        case 2:
            {
                for( auto i=0, j=0; i<pixelCount-2; i+=2, j+=4 )
                {
                    (*ppPixel)[j + 0] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 1] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 2] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 3] = static_cast<f32>( pPixels[i + 1] ) / 255.0f;;
                }

            }
            break;

        case 3:
            {
                for( auto i=0, j=0; i<pixelCount-3; i+3, j+=4 )
                {
                    (*ppPixel)[j + 0] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 1] = static_cast<f32>( pPixels[i + 1] ) / 255.0f;
                    (*ppPixel)[j + 2] = static_cast<f32>( pPixels[i + 2] ) / 255.0f;
                    (*ppPixel)[j + 3] = 1.0f;
                }
            }
            break;

        case 4:
            {
                for( auto i=0, j=0; i<pixelCount-4; i+=4, j+=4 )
                {
                    (*ppPixel)[j + 0] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 1] = static_cast<f32>( pPixels[i + 1] ) / 255.0f;
                    (*ppPixel)[j + 2] = static_cast<f32>( pPixels[i + 2] ) / 255.0f;
                    (*ppPixel)[j + 3] = static_cast<f32>( pPixels[i + 3] ) / 255.0f;
                }
            }
            break;
    }

    // 不要なメモリを解放.
    SafeDeleteArray( pPixels );

    // 正常終了.
    return true;
}

} // namespace s3d
