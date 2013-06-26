//----------------------------------------------------------------------------------------
// File : asdxUtil.cpp
// Desc : Utility Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include "asdxUtil.h"
#include <cstdio>
#include <cassert>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------
//      最大値を取得します.
//-------------------------------------------------------------------------------------------
template<typename T>
inline T Max(T a, T b)
{ return (a > b) ? a : b; }

//-------------------------------------------------------------------------------------------
//      最小値を取得します.
//-------------------------------------------------------------------------------------------
template<typename T>
inline T Min(T a, T b)
{ return (a < b) ? a : b; }

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

//----------------------------------------------------------------------------------------
//! @brief      Targaのカラーマップタイプです.
//----------------------------------------------------------------------------------------
enum TGA_COLORMAP_TYPE
{
    TGA_COLORMAP_DISABLE = 0,           // カラーマップなし.
    TGA_COLORMAP_ENABLE  = 1,           // カラーマップあり.
};

//----------------------------------------------------------------------------------------
//! @brief      Targaの画像タイプです.
//----------------------------------------------------------------------------------------
enum TGA_IMAGE_TYPE
{
    TGA_IMAGE_NO_IMAGE        = 0,      // イメージなし.
    TGA_IMAGE_INDEX_COLOR     = 1,      // インデックスカラー(256色).
    TGA_IMAGE_FULL_COLOR      = 2,      // フルカラー.
    TGA_IMAGE_MONOCHROME      = 3,      // 白黒.
    TGA_IMAGE_INDEX_COLOR_RLE = 9,      // インデックスカラーRLE圧縮.
    TGA_IMAGE_FULL_COLOR_RLE  = 10,     // フルカラーRLE圧縮.
    TGA_IMAGE_MONOCHROME_RLE  = 11      // 白黒RLE圧縮.
};

//----------------------------------------------------------------------------------------
//! @brief      Targaのファイルヘッダです.
//----------------------------------------------------------------------------------------
#pragma pack( push, 1 )
struct TGA_FILE_HEADER
{
    unsigned char       IdLength;           // IDフィールド (asdxではイメージID無し:0を設定).
    unsigned char       ColorMapType;       // カラーマップタイプ (asdxはカラーマップ無し:0を設定).
    unsigned char       ImageType;          // 画像タイプ.
    unsigned short      ColorMapIndex;      // カラーマップテーブルへのオフセット.
    unsigned short      ColorMapLength;     // カラーマップのエントリーの数.
    unsigned char       ColorMapSize;       // カラーマップの1ピクセルあたりのビット数.
    unsigned short      OriginX;            // 開始X位置.
    unsigned short      OriginY;            // 開始Y位置.
    unsigned short      Width;              // 画像の横幅.
    unsigned short      Height;             // 画像の縦幅.
    unsigned char       BitPerPixel;        // 1ピクセルあたりのビット数.
    unsigned char       Discripter;         // 記述子 [ bit0~3：属性, bit4:格納方向(0:左から右, 1:右から左), bit5, 格納方向(0:下から上, 1:上から下), bit6~7:0固定 ].
};
#pragma pack( pop )

//----------------------------------------------------------------------------------------
//! @brief      Targaのファイルフッタです.
//----------------------------------------------------------------------------------------
#pragma pack( push, 1 )
struct TGA_FILE_FOOTER
{
    unsigned int        ExtOffset;      // 拡張エリアまでのオフセット (asdxは使わないので0固定).
    unsigned int        DevOffset;      // ディベロッパーエリアまでのオフセット (asdxは使わないので0固定).
    unsigned char       Magic[17];      // "TRUEVISION-XFILE" 固定
    unsigned char       RFU1;           // "." 固定.
    unsigned char       RFU2;           // 0 固定.
};
#pragma pack( pop )

//----------------------------------------------------------------------------------------
//! @brief      ちゃっちぃテクスチャです.
//----------------------------------------------------------------------------------------
struct TinyTexture
{
    enum FORMAT_TYPE
    {
        FORMAT_RGB = 1,
        FORMAT_RGBA,
        FORMAT_BGR,
        FORMAT_BGRA,
    };

    int             Width;      //!< 画像の横幅.
    int             Height;     //!< 画像の縦幅.
    int             Format;     //!< フォーマットです.
    int             Size;       //!< ピクセルデータのサイズです.
    unsigned char*  pPixels;    //!< ピクセルデータ.

    //------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //------------------------------------------------------------------------------------
    TinyTexture()
    : Width  ( 0 )
    , Height ( 0 )
    , Format ( 0 )
    , Size   ( 0 )
    , pPixels( nullptr )
    {
        /* DO_NOTHING */
    }

    //------------------------------------------------------------------------------------
    //! @brief      初期化処理です.
    //!
    //! @param [in]     width       画像の横幅.
    //! @param [in]     height      画像の縦幅.
    //! @param [in]     format      画像フォーマットです.
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //------------------------------------------------------------------------------------
    bool Init( int width, int height, FORMAT_TYPE format )
    {
        int bpp = 0;
        switch( format )
        {
        case FORMAT_RGB:
            bpp = 3;
            break;

        case FORMAT_RGBA:
            bpp = 4;
            break;

        case FORMAT_BGR:
            bpp = 3;
            break;

        case FORMAT_BGRA:
            bpp = 4;
            break;

        default:
            return false;
        }

        if ( pPixels )
        {
            delete [] pPixels;
            pPixels = nullptr;
        }

        
        pPixels = new unsigned char [ width * height * bpp ];
        if ( pPixels == nullptr )
        {
            return false;
        }

        Width  = width;
        Height = height;
        Format = format;
        Size   = width * height * bpp;

        return true;
    }

    //-------------------------------------------------------------------------------------
    //! @brief      終了処理です.
    //-------------------------------------------------------------------------------------
    void Term()
    {
        if ( pPixels )
        {
            delete [] pPixels;
            pPixels = nullptr;
        }

        Width  = 0;
        Height = 0;
        Format = 0;
        Size   = 0;
    }
};


} // namespace /* anonymous */


namespace asdx {

//-------------------------------------------------------------------------------------------
//      機能レベルから対応する文字列を取得します.
//-------------------------------------------------------------------------------------------
LPSTR GetFeatureLevelStringA( const D3D_FEATURE_LEVEL value )
{
    switch( value )
    {
    case D3D_FEATURE_LEVEL_11_1:
        { return "D3D_FEATURE_LEVEL_11_1\0"; }
        break;

    case D3D_FEATURE_LEVEL_11_0:
        { return "D3D_FEATURE_LEVEL_11_0\0"; }
        break;

    case D3D_FEATURE_LEVEL_10_1:
        { return "D3D_FEATURE_LEVEL_10_1\0"; }
        break;

    case D3D_FEATURE_LEVEL_10_0:
        { return "D3D_FEATURE_LEVEL_10_0\0"; }
        break;

    case D3D_FEATURE_LEVEL_9_3:
        { return "D3D_FEATURE_LEVEL_9_3\0"; }
        break;

    case D3D_FEATURE_LEVEL_9_2:
        { return "D3D_FEATURE_LEVEL_9_2\0"; }
        break;

    case D3D_FEATURE_LEVEL_9_1:
        { return "D3D_FEATURE_LEVEL_9_1\0"; }
        break;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------
//      機能レベルから対応する文字列を取得します.
//-------------------------------------------------------------------------------------------
LPWSTR GetFeatureLevelStringW( const D3D_FEATURE_LEVEL value )
{
    switch( value )
    {
    case D3D_FEATURE_LEVEL_11_1:
        { return L"D3D_FEATURE_LEVEL_11_1\0"; }
        break;

    case D3D_FEATURE_LEVEL_11_0:
        { return L"D3D_FEATURE_LEVEL_11_0\0"; }
        break;

    case D3D_FEATURE_LEVEL_10_1:
        { return L"D3D_FEATURE_LEVEL_10_1\0"; }
        break;

    case D3D_FEATURE_LEVEL_10_0:
        { return L"D3D_FEATURE_LEVEL_10_0\0"; }
        break;

    case D3D_FEATURE_LEVEL_9_3:
        { return L"D3D_FEATURE_LEVEL_9_3\0"; }
        break;

    case D3D_FEATURE_LEVEL_9_2:
        { return L"D3D_FEATURE_LEVEL_9_2\0"; }
        break;

    case D3D_FEATURE_LEVEL_9_1:
        { return L"D3D_FEATURE_LEVEL_9_1\0"; }
        break;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------
//      ドライバータイプから対応する文字列を取得します.
//-------------------------------------------------------------------------------------------
LPSTR GetDriverTypeStringA( const D3D_DRIVER_TYPE value )
{
    switch( value )
    {
    case D3D_DRIVER_TYPE_HARDWARE:
        { return "D3D_DRIVER_TYPE_HARDWARE\0"; }
        break;

    case D3D_DRIVER_TYPE_WARP:
        { return "D3D_DRIVER_TYPE_WARP\0"; }
        break;

    case D3D_DRIVER_TYPE_REFERENCE:
        { return "D3D_DRIVER_TYPE_REFERENCE\0"; }
        break;

    case D3D_DRIVER_TYPE_SOFTWARE:
        { return "D3D_DRIVER_TYPE_SOFTWARE\0"; }
        break;

    case D3D_DRIVER_TYPE_NULL:
        { return "D3D_DRIVER_TYPE_NULL\0"; }
        break;

    case D3D_DRIVER_TYPE_UNKNOWN:
        { return "D3D_DRIVER_TYPE_UNKNOWN\0"; }
        break;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------
//      ドライバータイプから対応する文字列を取得します.
//-------------------------------------------------------------------------------------------
LPWSTR GetDriverTypeStringW( const D3D_DRIVER_TYPE value )
{
    switch( value )
    {
    case D3D_DRIVER_TYPE_HARDWARE:
        { return L"D3D_DRIVER_TYPE_HARDWARE\0"; }
        break;

    case D3D_DRIVER_TYPE_WARP:
        { return L"D3D_DRIVER_TYPE_WARP\0"; }
        break;

    case D3D_DRIVER_TYPE_REFERENCE:
        { return L"D3D_DRIVER_TYPE_REFERENCE\0"; }
        break;

    case D3D_DRIVER_TYPE_SOFTWARE:
        { return L"D3D_DRIVER_TYPE_SOFTWARE\0"; }
        break;

    case D3D_DRIVER_TYPE_NULL:
        { return L"D3D_DRIVER_TYPE_NULL\0"; }
        break;

    case D3D_DRIVER_TYPE_UNKNOWN:
        { return L"D3D_DRIVER_TYPE_UNKNOWN\0"; }
        break;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------
//      DXGIフォーマットから対応する文字列を取得します.
//-------------------------------------------------------------------------------------------
LPSTR GetFormatStringA( const DXGI_FORMAT value )
{
    switch( value )
    {
    //DXGI_FORMAT_UNKNOWN	                    = 0,
    case DXGI_FORMAT_UNKNOWN:
        { return "DXGI_FORMAT_UNKNOWN\0"; }
        break;

    //DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        { return "DXGI_FORMAT_R32G32B32A32_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        { return "DXGI_FORMAT_R32G32B32A32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32A32_UINT           = 3,
    case DXGI_FORMAT_R32G32B32A32_UINT:
        { return "DXGI_FORMAT_R32G32B32A32_UINT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32A32_SINT           = 4,
    case DXGI_FORMAT_R32G32B32A32_SINT:
        { return "DXGI_FORMAT_R32G32B32A32_SINT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        { return "DXGI_FORMAT_R32G32B32_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R32G32B32_FLOAT             = 6,
    case DXGI_FORMAT_R32G32B32_FLOAT:
        { return "DXGI_FORMAT_R32G32B32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32_UINT              = 7,
    case DXGI_FORMAT_R32G32B32_UINT:
        { return "DXGI_FORMAT_R32G32B32_UINT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32_SINT              = 8,
    case DXGI_FORMAT_R32G32B32_SINT:
        { return "DXGI_FORMAT_R32G32B32_SINT\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        { return "DXGI_FORMAT_R16G16B16A16_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        { return "DXGI_FORMAT_R16G16B16A16_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        { return "DXGI_FORMAT_R16G16B16A16_UNORM\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_UINT           = 12,
    case DXGI_FORMAT_R16G16B16A16_UINT:
        { return "DXGI_FORMAT_R16G16B16A16_UINT\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
    case DXGI_FORMAT_R16G16B16A16_SNORM:
        { return "DXGI_FORMAT_R16G16B16A16_SNORM\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_SINT           = 14,
    case DXGI_FORMAT_R16G16B16A16_SINT:
        { return "DXGI_FORMAT_R16G16B16A16_SINT\0"; }
        break;

    //DXGI_FORMAT_R32G32_TYPELESS             = 15,
    case DXGI_FORMAT_R32G32_TYPELESS:
        { return "DXGI_FORMAT_R32G32_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R32G32_FLOAT                = 16,
    case DXGI_FORMAT_R32G32_FLOAT:
        { return "DXGI_FORMAT_R32G32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32G32_UINT                 = 17,
    case DXGI_FORMAT_R32G32_UINT:
        { return "DXGI_FORMAT_R32G32_UINT\0"; }
        break;

    //DXGI_FORMAT_R32G32_SINT                 = 18,
    case DXGI_FORMAT_R32G32_SINT:
        { return "DXGI_FORMAT_R32G32_SINT\0"; }
        break;

    //DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
    case DXGI_FORMAT_R32G8X24_TYPELESS:
        { return "DXGI_FORMAT_R32G8X24_TYPELESS\0"; }

    //DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        { return "DXGI_FORMAT_D32_FLOAT_S8X24_UINT\0"; }
        break;

    //DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        { return "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        { return "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT\0"; }
        break;

    //DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        { return "DXGI_FORMAT_R10G10B10A2_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
    case DXGI_FORMAT_R10G10B10A2_UNORM:
        { return "DXGI_FORMAT_R10G10B10A2_UNORM\0"; }
        break;

    //DXGI_FORMAT_R10G10B10A2_UINT            = 25,
    case DXGI_FORMAT_R10G10B10A2_UINT:
        { return "DXGI_FORMAT_R10G10B10A2_UINT\0"; }
        break;

    //DXGI_FORMAT_R11G11B10_FLOAT             = 26,
    case DXGI_FORMAT_R11G11B10_FLOAT:
        { return "DXGI_FORMAT_R11G11B10_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        { return "DXGI_FORMAT_R8G8B8A8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        { return "DXGI_FORMAT_R8G8B8A8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        { return "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_UINT               = 30,
    case DXGI_FORMAT_R8G8B8A8_UINT:
        { return "DXGI_FORMAT_R8G8B8A8_UINT\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        { return "DXGI_FORMAT_R8G8B8A8_SNORM\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_SINT               = 32,
    case DXGI_FORMAT_R8G8B8A8_SINT:
        { return "DXGI_FORMAT_R8G8B8A8_SINT\0"; }
        break;

    //DXGI_FORMAT_R16G16_TYPELESS             = 33,
    case DXGI_FORMAT_R16G16_TYPELESS:
        { return "DXGI_FORMAT_R16G16_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R16G16_FLOAT                = 34,
    case DXGI_FORMAT_R16G16_FLOAT:
        { return "DXGI_FORMAT_R16G16_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R16G16_UNORM                = 35,
    case DXGI_FORMAT_R16G16_UNORM:
        { return "DXGI_FORMAT_R16G16_UNORM\0"; }
        break;

    //DXGI_FORMAT_R16G16_UINT                 = 36,
    case DXGI_FORMAT_R16G16_UINT:
        { return "DXGI_FORMAT_R16G16_UINT\0"; }
        break;

    //DXGI_FORMAT_R16G16_SNORM                = 37,
    case DXGI_FORMAT_R16G16_SNORM:
        { return "DXGI_FORMAT_R16G16_SNORM\0"; }
        break;

    //DXGI_FORMAT_R16G16_SINT                 = 38,
    case DXGI_FORMAT_R16G16_SINT:
        { return "DXGI_FORMAT_R16G16_SINT\0"; }
        break;

    //DXGI_FORMAT_R32_TYPELESS                = 39,
    case DXGI_FORMAT_R32_TYPELESS:
        { return "DXGI_FORMAT_R32_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_D32_FLOAT                   = 40,
    case DXGI_FORMAT_D32_FLOAT:
        { return "DXGI_FORMAT_D32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32_FLOAT                   = 41,
    case DXGI_FORMAT_R32_FLOAT:
        { return "DXGI_FORMAT_R32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32_UINT                    = 42,
    case DXGI_FORMAT_R32_UINT:
        { return "DXGI_FORMAT_R32_UINT\0"; }
        break;

    //DXGI_FORMAT_R32_SINT                    = 43,
    case DXGI_FORMAT_R32_SINT:
        { return "DXGI_FORMAT_R32_SINT\0"; }
        break;

    //DXGI_FORMAT_R24G8_TYPELESS              = 44,
    case DXGI_FORMAT_R24G8_TYPELESS:
        { return "DXGI_FORMAT_R24G8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        { return "DXGI_FORMAT_D24_UNORM_S8_UINT\0"; }
        break;

    //DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        { return "DXGI_FORMAT_R24_UNORM_X8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        { return "DXGI_FORMAT_X24_TYPELESS_G8_UINT\0"; }
        break;

    //DXGI_FORMAT_R8G8_TYPELESS               = 48,
    case DXGI_FORMAT_R8G8_TYPELESS:
        { return "DXGI_FORMAT_R8G8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R8G8_UNORM                  = 49,
    case DXGI_FORMAT_R8G8_UNORM:
        { return "DXGI_FORMAT_R8G8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R8G8_UINT                   = 50,
    case DXGI_FORMAT_R8G8_UINT:
        { return "DXGI_FORMAT_R8G8_UINT\0"; }
        break;

    //DXGI_FORMAT_R8G8_SNORM                  = 51,
    case DXGI_FORMAT_R8G8_SNORM:
        { return "DXGI_FORMAT_R8G8_SNORM\0"; }
        break;

    //DXGI_FORMAT_R8G8_SINT                   = 52,
    case DXGI_FORMAT_R8G8_SINT:
        { return "DXGI_FORMAT_R8G8_SINT\0"; }
        break;

    //DXGI_FORMAT_R16_TYPELESS                = 53,
    case DXGI_FORMAT_R16_TYPELESS:
        { return "DXGI_FORMAT_R16_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R16_FLOAT                   = 54,
    case DXGI_FORMAT_R16_FLOAT:
        { return "DXGI_FORMAT_R16_FLOAT\0"; }
        break;

    //DXGI_FORMAT_D16_UNORM                   = 55,
    case DXGI_FORMAT_D16_UNORM:
        { return "DXGI_FORMAT_D16_UNORM\0"; }
        break;

    //DXGI_FORMAT_R16_UNORM                   = 56,
    case DXGI_FORMAT_R16_UNORM:
        { return "DXGI_FORMAT_R16_UNORM\0"; }
        break;

    //DXGI_FORMAT_R16_UINT                    = 57,
    case DXGI_FORMAT_R16_UINT:
        { return "DXGI_FORMAT_R16_UINT\0"; }
        break;

    //DXGI_FORMAT_R16_SNORM                   = 58,
    case DXGI_FORMAT_R16_SNORM:
        { return "DXGI_FORMAT_R16_SNORM\0"; }
        break;

    //DXGI_FORMAT_R16_SINT                    = 59,
    case DXGI_FORMAT_R16_SINT:
        { return "DXGI_FORMAT_R16_SINT\0"; }
        break;

    //DXGI_FORMAT_R8_TYPELESS                 = 60,
    case DXGI_FORMAT_R8_TYPELESS:
        { return "DXGI_FORMAT_R8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R8_UNORM                    = 61,
    case DXGI_FORMAT_R8_UNORM:
        { return "DXGI_FORMAT_R8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R8_UINT                     = 62,
    case DXGI_FORMAT_R8_UINT:
        { return "DXGI_FORMAT_R8_UINT\0"; }
        break;

    //DXGI_FORMAT_R8_SNORM                    = 63,
    case DXGI_FORMAT_R8_SNORM:
        { return "DXGI_FORMAT_R8_SNORM\0"; }
        break;

    //DXGI_FORMAT_R8_SINT                     = 64,
    case DXGI_FORMAT_R8_SINT:
        { return "DXGI_FORMAT_R8_SINT\0"; }
        break;

    //DXGI_FORMAT_A8_UNORM                    = 65,
    case DXGI_FORMAT_A8_UNORM:
        { return "DXGI_FORMAT_A8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R1_UNORM                    = 66,
    case DXGI_FORMAT_R1_UNORM:
        { return "DXGI_FORMAT_R1_UNORM\0"; }
        break;

    //DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        { return "DXGI_FORMAT_R9G9B9E5_SHAREDEXP\0"; }
        break;

    //DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        { return "DXGI_FORMAT_R8G8_B8G8_UNORM\0"; }
        break;

    //DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        { return "DXGI_FORMAT_G8R8_G8B8_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC1_TYPELESS                = 70,
    case DXGI_FORMAT_BC1_TYPELESS:
        { return "DXGI_FORMAT_BC1_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC1_UNORM                   = 71,
    case DXGI_FORMAT_BC1_UNORM:
        { return "DXGI_FORMAT_BC1_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        { return "DXGI_FORMAT_BC1_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_BC2_TYPELESS                = 73,
    case DXGI_FORMAT_BC2_TYPELESS:
        { return "DXGI_FORMAT_BC2_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC2_UNORM                   = 74,
    case DXGI_FORMAT_BC2_UNORM:
        { return "DXGI_FORMAT_BC2_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        { return "DXGI_FORMAT_BC2_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_BC3_TYPELESS                = 76,
    case DXGI_FORMAT_BC3_TYPELESS:
        { return "DXGI_FORMAT_BC3_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC3_UNORM                   = 77,
    case DXGI_FORMAT_BC3_UNORM:
        { return "DXGI_FORMAT_BC3_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        { return "DXGI_FORMAT_BC3_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_BC4_TYPELESS                = 79,
    case DXGI_FORMAT_BC4_TYPELESS:
        { return "DXGI_FORMAT_BC4_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC4_UNORM                   = 80,
    case DXGI_FORMAT_BC4_UNORM:
        { return "DXGI_FORMAT_BC4_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC4_SNORM                   = 81,
    case DXGI_FORMAT_BC4_SNORM:
        { return "DXGI_FORMAT_BC4_SNORM\0"; }
        break;

    //DXGI_FORMAT_BC5_TYPELESS                = 82,
    case DXGI_FORMAT_BC5_TYPELESS:
        { return "DXGI_FORMAT_BC5_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC5_UNORM                   = 83,
    case DXGI_FORMAT_BC5_UNORM:
        { return "DXGI_FORMAT_BC5_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC5_SNORM                   = 84,
    case DXGI_FORMAT_BC5_SNORM:
        { return "DXGI_FORMAT_BC5_SNORM\0"; }
        break;

    //DXGI_FORMAT_B5G6R5_UNORM                = 85,
    case DXGI_FORMAT_B5G6R5_UNORM:
        { return "DXGI_FORMAT_B5G6R5_UNORM\0"; }
        break;

    //DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        { return "DXGI_FORMAT_B5G5R5A1_UNORM\0"; }
        break;

    //DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        { return "DXGI_FORMAT_B8G8R8A8_UNORM\0"; }
        break;

    //DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        { return "DXGI_FORMAT_B8G8R8X8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        { return "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM\0"; }
        break;

    //DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        { return "DXGI_FORMAT_B8G8R8A8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        { return "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        { return "DXGI_FORMAT_B8G8R8X8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        { return "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_BC6H_TYPELESS               = 94,
    case DXGI_FORMAT_BC6H_TYPELESS:
        { return "DXGI_FORMAT_BC6H_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC6H_UF16                   = 95,
    case DXGI_FORMAT_BC6H_UF16:
        { return "DXGI_FORMAT_BC6H_UF16\0"; }
        break;

    //DXGI_FORMAT_BC6H_SF16                   = 96,
    case DXGI_FORMAT_BC6H_SF16:
        { return "DXGI_FORMAT_BC6H_SF16\0"; }
        break;

    //DXGI_FORMAT_BC7_TYPELESS                = 97,
    case DXGI_FORMAT_BC7_TYPELESS:
        { return "DXGI_FORMAT_BC7_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC7_UNORM                   = 98,
    case DXGI_FORMAT_BC7_UNORM:
        { return "DXGI_FORMAT_BC7_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        { return "DXGI_FORMAT_BC7_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_AYUV                        = 100,
    case DXGI_FORMAT_AYUV:
        { return "DXGI_FORMAT_AYUV\0"; }
        break;

    //DXGI_FORMAT_Y410                        = 101,
    case DXGI_FORMAT_Y410:
        { return "DXGI_FORMAT_Y410\0"; }
        break;

    //DXGI_FORMAT_Y416                        = 102,
    case DXGI_FORMAT_Y416:
        { return "DXGI_FORMAT_Y416\0"; }
        break;

    //DXGI_FORMAT_NV12                        = 103,
    case DXGI_FORMAT_NV12:
        { return "DXGI_FORMAT_NV12\0"; }
        break;

    //DXGI_FORMAT_P010                        = 104,
    case DXGI_FORMAT_P010:
        { return "DXGI_FORMAT_P010\0"; }
        break;

    //DXGI_FORMAT_P016                        = 105,
    case DXGI_FORMAT_P016:
        { return "DXGI_FORMAT_P016\0"; }
        break;

    //DXGI_FORMAT_420_OPAQUE                  = 106,
    case DXGI_FORMAT_420_OPAQUE:
        { return "DXGI_FORMAT_420_OPAQUE\0"; }
        break;

    //DXGI_FORMAT_YUY2                        = 107,
    case DXGI_FORMAT_YUY2:
        { return "DXGI_FORMAT_YUY2\0"; }
        break;

    //DXGI_FORMAT_Y210                        = 108,
    case DXGI_FORMAT_Y210:
        { return "DXGI_FORMAT_Y210\0"; }
        break;

    //DXGI_FORMAT_Y216                        = 109,
    case DXGI_FORMAT_Y216:
        { return "DXGI_FORMAT_Y216\0"; }
        break;

    //DXGI_FORMAT_NV11                        = 110,
    case DXGI_FORMAT_NV11:
        { return "DXGI_FORMAT_NV11\0"; }
        break;

    //DXGI_FORMAT_AI44                        = 111,
    case DXGI_FORMAT_AI44:
        { return "DXGI_FORMAT_AI44\0"; }
        break;

    //DXGI_FORMAT_IA44                        = 112,
    case DXGI_FORMAT_IA44:
        { return "DXGI_FORMAT_IA44\0"; }
        break;

    //DXGI_FORMAT_P8                          = 113,
    case DXGI_FORMAT_P8:
        { return "DXGI_FORMAT_P8\0"; }
        break;

    //DXGI_FORMAT_A8P8                        = 114,
    case DXGI_FORMAT_A8P8:
        { return "DXGI_FORMAT_A8P8\0"; }
        break;

    //DXGI_FORMAT_B4G4R4A4_UNORM              = 115,
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        { return "DXGI_FORMAT_B4G4R4A4_UNORM\0"; }
        break;
   }

   return NULL;
}


//-------------------------------------------------------------------------------------------
//      DXGIフォーマットから対応する文字列を取得します.
//-------------------------------------------------------------------------------------------
LPWSTR GetFormatStringW( const DXGI_FORMAT value )
{
    switch( value )
    {
    //DXGI_FORMAT_UNKNOWN	                    = 0,
    case DXGI_FORMAT_UNKNOWN:
        { return L"DXGI_FORMAT_UNKNOWN\0"; }
        break;

    //DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        { return L"DXGI_FORMAT_R32G32B32A32_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        { return L"DXGI_FORMAT_R32G32B32A32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32A32_UINT           = 3,
    case DXGI_FORMAT_R32G32B32A32_UINT:
        { return L"DXGI_FORMAT_R32G32B32A32_UINT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32A32_SINT           = 4,
    case DXGI_FORMAT_R32G32B32A32_SINT:
        { return L"DXGI_FORMAT_R32G32B32A32_SINT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        { return L"DXGI_FORMAT_R32G32B32_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R32G32B32_FLOAT             = 6,
    case DXGI_FORMAT_R32G32B32_FLOAT:
        { return L"DXGI_FORMAT_R32G32B32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32_UINT              = 7,
    case DXGI_FORMAT_R32G32B32_UINT:
        { return L"DXGI_FORMAT_R32G32B32_UINT\0"; }
        break;

    //DXGI_FORMAT_R32G32B32_SINT              = 8,
    case DXGI_FORMAT_R32G32B32_SINT:
        { return L"DXGI_FORMAT_R32G32B32_SINT\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        { return L"DXGI_FORMAT_R16G16B16A16_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        { return L"DXGI_FORMAT_R16G16B16A16_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        { return L"DXGI_FORMAT_R16G16B16A16_UNORM\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_UINT           = 12,
    case DXGI_FORMAT_R16G16B16A16_UINT:
        { return L"DXGI_FORMAT_R16G16B16A16_UINT\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
    case DXGI_FORMAT_R16G16B16A16_SNORM:
        { return L"DXGI_FORMAT_R16G16B16A16_SNORM\0"; }
        break;

    //DXGI_FORMAT_R16G16B16A16_SINT           = 14,
    case DXGI_FORMAT_R16G16B16A16_SINT:
        { return L"DXGI_FORMAT_R16G16B16A16_SINT\0"; }
        break;

    //DXGI_FORMAT_R32G32_TYPELESS             = 15,
    case DXGI_FORMAT_R32G32_TYPELESS:
        { return L"DXGI_FORMAT_R32G32_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R32G32_FLOAT                = 16,
    case DXGI_FORMAT_R32G32_FLOAT:
        { return L"DXGI_FORMAT_R32G32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32G32_UINT                 = 17,
    case DXGI_FORMAT_R32G32_UINT:
        { return L"DXGI_FORMAT_R32G32_UINT\0"; }
        break;

    //DXGI_FORMAT_R32G32_SINT                 = 18,
    case DXGI_FORMAT_R32G32_SINT:
        { return L"DXGI_FORMAT_R32G32_SINT\0"; }
        break;

    //DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
    case DXGI_FORMAT_R32G8X24_TYPELESS:
        { return L"DXGI_FORMAT_R32G8X24_TYPELESS\0"; }

    //DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        { return L"DXGI_FORMAT_D32_FLOAT_S8X24_UINT\0"; }
        break;

    //DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        { return L"DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        { return L"DXGI_FORMAT_X32_TYPELESS_G8X24_UINT\0"; }
        break;

    //DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        { return L"DXGI_FORMAT_R10G10B10A2_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
    case DXGI_FORMAT_R10G10B10A2_UNORM:
        { return L"DXGI_FORMAT_R10G10B10A2_UNORM\0"; }
        break;

    //DXGI_FORMAT_R10G10B10A2_UINT            = 25,
    case DXGI_FORMAT_R10G10B10A2_UINT:
        { return L"DXGI_FORMAT_R10G10B10A2_UINT\0"; }
        break;

    //DXGI_FORMAT_R11G11B10_FLOAT             = 26,
    case DXGI_FORMAT_R11G11B10_FLOAT:
        { return L"DXGI_FORMAT_R11G11B10_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        { return L"DXGI_FORMAT_R8G8B8A8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        { return L"DXGI_FORMAT_R8G8B8A8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        { return L"DXGI_FORMAT_R8G8B8A8_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_UINT               = 30,
    case DXGI_FORMAT_R8G8B8A8_UINT:
        { return L"DXGI_FORMAT_R8G8B8A8_UINT\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        { return L"DXGI_FORMAT_R8G8B8A8_SNORM\0"; }
        break;

    //DXGI_FORMAT_R8G8B8A8_SINT               = 32,
    case DXGI_FORMAT_R8G8B8A8_SINT:
        { return L"DXGI_FORMAT_R8G8B8A8_SINT\0"; }
        break;

    //DXGI_FORMAT_R16G16_TYPELESS             = 33,
    case DXGI_FORMAT_R16G16_TYPELESS:
        { return L"DXGI_FORMAT_R16G16_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R16G16_FLOAT                = 34,
    case DXGI_FORMAT_R16G16_FLOAT:
        { return L"DXGI_FORMAT_R16G16_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R16G16_UNORM                = 35,
    case DXGI_FORMAT_R16G16_UNORM:
        { return L"DXGI_FORMAT_R16G16_UNORM\0"; }
        break;

    //DXGI_FORMAT_R16G16_UINT                 = 36,
    case DXGI_FORMAT_R16G16_UINT:
        { return L"DXGI_FORMAT_R16G16_UINT\0"; }
        break;

    //DXGI_FORMAT_R16G16_SNORM                = 37,
    case DXGI_FORMAT_R16G16_SNORM:
        { return L"DXGI_FORMAT_R16G16_SNORM\0"; }
        break;

    //DXGI_FORMAT_R16G16_SINT                 = 38,
    case DXGI_FORMAT_R16G16_SINT:
        { return L"DXGI_FORMAT_R16G16_SINT\0"; }
        break;

    //DXGI_FORMAT_R32_TYPELESS                = 39,
    case DXGI_FORMAT_R32_TYPELESS:
        { return L"DXGI_FORMAT_R32_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_D32_FLOAT                   = 40,
    case DXGI_FORMAT_D32_FLOAT:
        { return L"DXGI_FORMAT_D32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32_FLOAT                   = 41,
    case DXGI_FORMAT_R32_FLOAT:
        { return L"DXGI_FORMAT_R32_FLOAT\0"; }
        break;

    //DXGI_FORMAT_R32_UINT                    = 42,
    case DXGI_FORMAT_R32_UINT:
        { return L"DXGI_FORMAT_R32_UINT\0"; }
        break;

    //DXGI_FORMAT_R32_SINT                    = 43,
    case DXGI_FORMAT_R32_SINT:
        { return L"DXGI_FORMAT_R32_SINT\0"; }
        break;

    //DXGI_FORMAT_R24G8_TYPELESS              = 44,
    case DXGI_FORMAT_R24G8_TYPELESS:
        { return L"DXGI_FORMAT_R24G8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        { return L"DXGI_FORMAT_D24_UNORM_S8_UINT\0"; }
        break;

    //DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        { return L"DXGI_FORMAT_R24_UNORM_X8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        { return L"DXGI_FORMAT_X24_TYPELESS_G8_UINT\0"; }
        break;

    //DXGI_FORMAT_R8G8_TYPELESS               = 48,
    case DXGI_FORMAT_R8G8_TYPELESS:
        { return L"DXGI_FORMAT_R8G8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R8G8_UNORM                  = 49,
    case DXGI_FORMAT_R8G8_UNORM:
        { return L"DXGI_FORMAT_R8G8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R8G8_UINT                   = 50,
    case DXGI_FORMAT_R8G8_UINT:
        { return L"DXGI_FORMAT_R8G8_UINT\0"; }
        break;

    //DXGI_FORMAT_R8G8_SNORM                  = 51,
    case DXGI_FORMAT_R8G8_SNORM:
        { return L"DXGI_FORMAT_R8G8_SNORM\0"; }
        break;

    //DXGI_FORMAT_R8G8_SINT                   = 52,
    case DXGI_FORMAT_R8G8_SINT:
        { return L"DXGI_FORMAT_R8G8_SINT\0"; }
        break;

    //DXGI_FORMAT_R16_TYPELESS                = 53,
    case DXGI_FORMAT_R16_TYPELESS:
        { return L"DXGI_FORMAT_R16_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R16_FLOAT                   = 54,
    case DXGI_FORMAT_R16_FLOAT:
        { return L"DXGI_FORMAT_R16_FLOAT\0"; }
        break;

    //DXGI_FORMAT_D16_UNORM                   = 55,
    case DXGI_FORMAT_D16_UNORM:
        { return L"DXGI_FORMAT_D16_UNORM\0"; }
        break;

    //DXGI_FORMAT_R16_UNORM                   = 56,
    case DXGI_FORMAT_R16_UNORM:
        { return L"DXGI_FORMAT_R16_UNORM\0"; }
        break;

    //DXGI_FORMAT_R16_UINT                    = 57,
    case DXGI_FORMAT_R16_UINT:
        { return L"DXGI_FORMAT_R16_UINT\0"; }
        break;

    //DXGI_FORMAT_R16_SNORM                   = 58,
    case DXGI_FORMAT_R16_SNORM:
        { return L"DXGI_FORMAT_R16_SNORM\0"; }
        break;

    //DXGI_FORMAT_R16_SINT                    = 59,
    case DXGI_FORMAT_R16_SINT:
        { return L"DXGI_FORMAT_R16_SINT\0"; }
        break;

    //DXGI_FORMAT_R8_TYPELESS                 = 60,
    case DXGI_FORMAT_R8_TYPELESS:
        { return L"DXGI_FORMAT_R8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_R8_UNORM                    = 61,
    case DXGI_FORMAT_R8_UNORM:
        { return L"DXGI_FORMAT_R8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R8_UINT                     = 62,
    case DXGI_FORMAT_R8_UINT:
        { return L"DXGI_FORMAT_R8_UINT\0"; }
        break;

    //DXGI_FORMAT_R8_SNORM                    = 63,
    case DXGI_FORMAT_R8_SNORM:
        { return L"DXGI_FORMAT_R8_SNORM\0"; }
        break;

    //DXGI_FORMAT_R8_SINT                     = 64,
    case DXGI_FORMAT_R8_SINT:
        { return L"DXGI_FORMAT_R8_SINT\0"; }
        break;

    //DXGI_FORMAT_A8_UNORM                    = 65,
    case DXGI_FORMAT_A8_UNORM:
        { return L"DXGI_FORMAT_A8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R1_UNORM                    = 66,
    case DXGI_FORMAT_R1_UNORM:
        { return L"DXGI_FORMAT_R1_UNORM\0"; }
        break;

    //DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        { return L"DXGI_FORMAT_R9G9B9E5_SHAREDEXP\0"; }
        break;

    //DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        { return L"DXGI_FORMAT_R8G8_B8G8_UNORM\0"; }
        break;

    //DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        { return L"DXGI_FORMAT_G8R8_G8B8_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC1_TYPELESS                = 70,
    case DXGI_FORMAT_BC1_TYPELESS:
        { return L"DXGI_FORMAT_BC1_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC1_UNORM                   = 71,
    case DXGI_FORMAT_BC1_UNORM:
        { return L"DXGI_FORMAT_BC1_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        { return L"DXGI_FORMAT_BC1_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_BC2_TYPELESS                = 73,
    case DXGI_FORMAT_BC2_TYPELESS:
        { return L"DXGI_FORMAT_BC2_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC2_UNORM                   = 74,
    case DXGI_FORMAT_BC2_UNORM:
        { return L"DXGI_FORMAT_BC2_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        { return L"DXGI_FORMAT_BC2_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_BC3_TYPELESS                = 76,
    case DXGI_FORMAT_BC3_TYPELESS:
        { return L"DXGI_FORMAT_BC3_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC3_UNORM                   = 77,
    case DXGI_FORMAT_BC3_UNORM:
        { return L"DXGI_FORMAT_BC3_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        { return L"DXGI_FORMAT_BC3_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_BC4_TYPELESS                = 79,
    case DXGI_FORMAT_BC4_TYPELESS:
        { return L"DXGI_FORMAT_BC4_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC4_UNORM                   = 80,
    case DXGI_FORMAT_BC4_UNORM:
        { return L"DXGI_FORMAT_BC4_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC4_SNORM                   = 81,
    case DXGI_FORMAT_BC4_SNORM:
        { return L"DXGI_FORMAT_BC4_SNORM\0"; }
        break;

    //DXGI_FORMAT_BC5_TYPELESS                = 82,
    case DXGI_FORMAT_BC5_TYPELESS:
        { return L"DXGI_FORMAT_BC5_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC5_UNORM                   = 83,
    case DXGI_FORMAT_BC5_UNORM:
        { return L"DXGI_FORMAT_BC5_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC5_SNORM                   = 84,
    case DXGI_FORMAT_BC5_SNORM:
        { return L"DXGI_FORMAT_BC5_SNORM\0"; }
        break;

    //DXGI_FORMAT_B5G6R5_UNORM                = 85,
    case DXGI_FORMAT_B5G6R5_UNORM:
        { return L"DXGI_FORMAT_B5G6R5_UNORM\0"; }
        break;

    //DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        { return L"DXGI_FORMAT_B5G5R5A1_UNORM\0"; }
        break;

    //DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        { return L"DXGI_FORMAT_B8G8R8A8_UNORM\0"; }
        break;

    //DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        { return L"DXGI_FORMAT_B8G8R8X8_UNORM\0"; }
        break;

    //DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        { return L"DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM\0"; }
        break;

    //DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        { return L"DXGI_FORMAT_B8G8R8A8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        { return L"DXGI_FORMAT_B8G8R8A8_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        { return L"DXGI_FORMAT_B8G8R8X8_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        { return L"DXGI_FORMAT_B8G8R8X8_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_BC6H_TYPELESS               = 94,
    case DXGI_FORMAT_BC6H_TYPELESS:
        { return L"DXGI_FORMAT_BC6H_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC6H_UF16                   = 95,
    case DXGI_FORMAT_BC6H_UF16:
        { return L"DXGI_FORMAT_BC6H_UF16\0"; }
        break;

    //DXGI_FORMAT_BC6H_SF16                   = 96,
    case DXGI_FORMAT_BC6H_SF16:
        { return L"DXGI_FORMAT_BC6H_SF16\0"; }
        break;

    //DXGI_FORMAT_BC7_TYPELESS                = 97,
    case DXGI_FORMAT_BC7_TYPELESS:
        { return L"DXGI_FORMAT_BC7_TYPELESS\0"; }
        break;

    //DXGI_FORMAT_BC7_UNORM                   = 98,
    case DXGI_FORMAT_BC7_UNORM:
        { return L"DXGI_FORMAT_BC7_UNORM\0"; }
        break;

    //DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        { return L"DXGI_FORMAT_BC7_UNORM_SRGB\0"; }
        break;

    //DXGI_FORMAT_AYUV                        = 100,
    case DXGI_FORMAT_AYUV:
        { return L"DXGI_FORMAT_AYUV\0"; }
        break;

    //DXGI_FORMAT_Y410                        = 101,
    case DXGI_FORMAT_Y410:
        { return L"DXGI_FORMAT_Y410\0"; }
        break;

    //DXGI_FORMAT_Y416                        = 102,
    case DXGI_FORMAT_Y416:
        { return L"DXGI_FORMAT_Y416\0"; }
        break;

    //DXGI_FORMAT_NV12                        = 103,
    case DXGI_FORMAT_NV12:
        { return L"DXGI_FORMAT_NV12\0"; }
        break;

    //DXGI_FORMAT_P010                        = 104,
    case DXGI_FORMAT_P010:
        { return L"DXGI_FORMAT_P010\0"; }
        break;

    //DXGI_FORMAT_P016                        = 105,
    case DXGI_FORMAT_P016:
        { return L"DXGI_FORMAT_P016\0"; }
        break;

    //DXGI_FORMAT_420_OPAQUE                  = 106,
    case DXGI_FORMAT_420_OPAQUE:
        { return L"DXGI_FORMAT_420_OPAQUE\0"; }
        break;

    //DXGI_FORMAT_YUY2                        = 107,
    case DXGI_FORMAT_YUY2:
        { return L"DXGI_FORMAT_YUY2\0"; }
        break;

    //DXGI_FORMAT_Y210                        = 108,
    case DXGI_FORMAT_Y210:
        { return L"DXGI_FORMAT_Y210\0"; }
        break;

    //DXGI_FORMAT_Y216                        = 109,
    case DXGI_FORMAT_Y216:
        { return L"DXGI_FORMAT_Y216\0"; }
        break;

    //DXGI_FORMAT_NV11                        = 110,
    case DXGI_FORMAT_NV11:
        { return L"DXGI_FORMAT_NV11\0"; }
        break;

    //DXGI_FORMAT_AI44                        = 111,
    case DXGI_FORMAT_AI44:
        { return L"DXGI_FORMAT_AI44\0"; }
        break;

    //DXGI_FORMAT_IA44                        = 112,
    case DXGI_FORMAT_IA44:
        { return L"DXGI_FORMAT_IA44\0"; }
        break;

    //DXGI_FORMAT_P8                          = 113,
    case DXGI_FORMAT_P8:
        { return L"DXGI_FORMAT_P8\0"; }
        break;

    //DXGI_FORMAT_A8P8                        = 114,
    case DXGI_FORMAT_A8P8:
        { return L"DXGI_FORMAT_A8P8\0"; }
        break;

    //DXGI_FORMAT_B4G4R4A4_UNORM              = 115,
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        { return L"DXGI_FORMAT_B4G4R4A4_UNORM\0"; }
        break;
   }

   return NULL;
}

//-------------------------------------------------------------------------------------------
//      Typelessフォーマットでないタイプに確定します.
//-------------------------------------------------------------------------------------------
DXGI_FORMAT EnsureNotTypeless( DXGI_FORMAT fmt )
{
    // Assumes UNORM or FLOAT; doesn't use UINT or SINT
    switch( fmt )
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case DXGI_FORMAT_R32G32B32_TYPELESS:    return DXGI_FORMAT_R32G32B32_FLOAT;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_UNORM;
    case DXGI_FORMAT_R32G32_TYPELESS:       return DXGI_FORMAT_R32G32_FLOAT;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:  return DXGI_FORMAT_R10G10B10A2_UNORM;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_R16G16_TYPELESS:       return DXGI_FORMAT_R16G16_UNORM;
    case DXGI_FORMAT_R32_TYPELESS:          return DXGI_FORMAT_R32_FLOAT;
    case DXGI_FORMAT_R8G8_TYPELESS:         return DXGI_FORMAT_R8G8_UNORM;
    case DXGI_FORMAT_R16_TYPELESS:          return DXGI_FORMAT_R16_UNORM;
    case DXGI_FORMAT_R8_TYPELESS:           return DXGI_FORMAT_R8_UNORM;
    case DXGI_FORMAT_BC1_TYPELESS:          return DXGI_FORMAT_BC1_UNORM;
    case DXGI_FORMAT_BC2_TYPELESS:          return DXGI_FORMAT_BC2_UNORM;
    case DXGI_FORMAT_BC3_TYPELESS:          return DXGI_FORMAT_BC3_UNORM;
    case DXGI_FORMAT_BC4_TYPELESS:          return DXGI_FORMAT_BC4_UNORM;
    case DXGI_FORMAT_BC5_TYPELESS:          return DXGI_FORMAT_BC5_UNORM;
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:     return DXGI_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:     return DXGI_FORMAT_B8G8R8X8_UNORM;
    case DXGI_FORMAT_BC7_TYPELESS:          return DXGI_FORMAT_BC7_UNORM;
    default:                                return fmt;
    }
}

//-------------------------------------------------------------------------------------------
//      スクリーンキャプチャーを行います.
//-------------------------------------------------------------------------------------------
bool ScreenCapture
(
    ID3D11DeviceContext* pDeviceContext,
    IDXGISwapChain*      pSwapChain,
    ID3D11Texture2D**    ppTexture
)
{
    // NULLチェック.
    if ( pDeviceContext == nullptr  || pSwapChain == nullptr )
    { return false; }

    ID3D11Texture2D* pBuffer = nullptr;

    // バッファを取得.
    if ( FAILED( pSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBuffer ) ) )
    {
        // 異常終了.
        return false;
    }

    {
        // デバイスを取得.
        ID3D11Device* pDevice;
        pDeviceContext->GetDevice( &pDevice );

        D3D11_TEXTURE2D_DESC desc;

        // デスクリプションを取得.
        pBuffer->GetDesc(&desc);

        if ( desc.SampleDesc.Count > 1 )
        {
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;

            ID3D11Texture2D* pTemp;
            HRESULT hr = pDevice->CreateTexture2D( &desc, 0, &pTemp );
            if ( FAILED( hr ) )
            {
                pBuffer->Release();
                pDevice->Release();
                return false;
            }

            DXGI_FORMAT format = EnsureNotTypeless( desc.Format );

            UINT support = 0;
            hr = pDevice->CheckFormatSupport( format, &support );
            if ( FAILED( hr ) )
            {
                pBuffer->Release();
                pDevice->Release();
                pTemp->Release();
                return false;
            }

            if ( !( support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE ) )
            {
                pBuffer->Release();
                pDevice->Release();
                pTemp->Release();
                return false;
            }

            for( unsigned int i=0; i<desc.ArraySize; ++i )
            {
                for( unsigned int j=0; j<desc.MipLevels; ++j )
                {
                    unsigned int idx = D3D11CalcSubresource( j, i, desc.MipLevels );
                    pDeviceContext->ResolveSubresource( pTemp, idx, pBuffer, idx, format );
                }
            }

            desc.BindFlags = 0;
            desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;

            hr = pDevice->CreateTexture2D( &desc, 0, ppTexture );
            if ( FAILED( hr ) )
            {
                pTemp->Release();
                pBuffer->Release();
                pDevice->Release();
                return false;
            }

            pDeviceContext->CopyResource( (*ppTexture), pTemp );

            pTemp->Release();
        }
        else if ( (desc.Usage == D3D11_USAGE_STAGING) && (desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ) )
        {
            pBuffer->AddRef();
            (*ppTexture) = pBuffer;
        }
        else
        {
            desc.BindFlags = 0;
            desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;

            HRESULT hr = pDevice->CreateTexture2D( &desc, 0, ppTexture );
            if ( FAILED( hr ) )
            {
                pBuffer->Release();
                pDevice->Release();
                return false;
            }

            // リソースをコピー.
            pDeviceContext->CopyResource( (*ppTexture), pBuffer );
        }

        // デバイスを解放.
        pDevice->Release();
    }

    // いらないバッファを解放.
    pBuffer->Release();

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------------
//      1ピクセルあたりのビット数を取得します.
//-------------------------------------------------------------------------------------------
size_t GetBitsPerPixel( DXGI_FORMAT fmt )
{
    switch( fmt )
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return 32;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:

#ifdef DXGI_1_2_FORMATS
    case DXGI_FORMAT_B4G4R4A4_UNORM:
#endif
        return 16;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

    default:
        return 0;
    }
}

//-------------------------------------------------------------------------------------------
//      サーフェイス情報を取得します.
//-------------------------------------------------------------------------------------------
void GetSurfaceInfo
(
    size_t      width,
    size_t      height,
    DXGI_FORMAT fmt,
    size_t*     outNumBytes,
    size_t*     outRowBytes,
    size_t*     outNumRows
)
{
    size_t numBytes = 0;
    size_t rowBytes = 0;
    size_t numRows  = 0;

    bool bc     = false;
    bool packed = false;
    size_t bcnumBytesPerBlock = 0;
    switch (fmt)
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        bc=true;
        bcnumBytesPerBlock = 8;
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        bc = true;
        bcnumBytesPerBlock = 16;
        break;

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        packed = true;
        break;
    }

    if (bc)
    {
        size_t numBlocksWide = 0;
        if (width > 0)
        {
            numBlocksWide = Max<size_t>( 1, (width + 3) / 4 );
        }
        size_t numBlocksHigh = 0;
        if (height > 0)
        {
            numBlocksHigh = Max<size_t>( 1, (height + 3) / 4 );
        }
        rowBytes = numBlocksWide * bcnumBytesPerBlock;
        numRows = numBlocksHigh;
    }
    else if (packed)
    {
        rowBytes = ( ( width + 1 ) >> 1 ) * 4;
        numRows = height;
    }
    else
    {
        size_t bpp = GetBitsPerPixel( fmt );
        rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
        numRows = height;
    }

    numBytes = rowBytes * numRows;
    if (outNumBytes)
    {
        *outNumBytes = numBytes;
    }
    if (outRowBytes)
    {
        *outRowBytes = rowBytes;
    }
    if (outNumRows)
    {
        *outNumRows = numRows;
    }
}

//-------------------------------------------------------------------------------------------
//      ちゃっちぃテクスチャを生成します.
//-------------------------------------------------------------------------------------------
bool CreateTinyTexture
(
    ID3D11DeviceContext* pDeviceContext,
    ID3D11Texture2D*     pTexture,
    TinyTexture&         result
)
{
    if ( pDeviceContext == nullptr || pTexture == nullptr )
    {
        return false;
    }

    D3D11_TEXTURE2D_DESC desc;
    pTexture->GetDesc( &desc );

    TinyTexture::FORMAT_TYPE format;

    bool isValidFormat = false;
    switch( desc.Format )
    {
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        {
            isValidFormat = true;
            format = TinyTexture::FORMAT_RGBA;
        }
        break;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        {
            isValidFormat = true;
            format = TinyTexture::FORMAT_BGRA;
        }
        break;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
        {
            isValidFormat = true;
            format = TinyTexture::FORMAT_BGR;
        }
        break;
    }

    if ( !isValidFormat )
    {
        return false;
    }

    size_t rowPitch;
    size_t slicePitch;
    size_t rowCount;
    GetSurfaceInfo( desc.Width, desc.Height, desc.Format, &slicePitch, &rowPitch, &rowCount );

    result.Width   = desc.Width;
    result.Height  = desc.Height;
    result.Format  = format;
    result.Size    = slicePitch;
    result.pPixels = new unsigned char [slicePitch];
    assert( result.pPixels != nullptr );

    D3D11_MAPPED_SUBRESOURCE res;
    HRESULT hr = pDeviceContext->Map( pTexture, 0, D3D11_MAP_READ, 0, &res );
    if ( FAILED( hr ) )
    {
        result.Term();
        return false;
    }

    unsigned char* pSrc = static_cast<unsigned char*>( res.pData );

    if ( pSrc == nullptr )
    {
        result.Term();
        pDeviceContext->Unmap( pTexture, 0 );
        return false;
    }

    unsigned char* pDst = result.pPixels;

    size_t miniSize = Min<size_t>( rowPitch, res.RowPitch );

    for( size_t h=0; h<rowCount; ++h )
    {
        memcpy_s( pDst, rowPitch, pSrc, miniSize );

        pSrc += res.RowPitch;
        pDst += rowPitch;
    }

    pDeviceContext->Unmap( pTexture, 0 );

    return true;
}

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
//      BMPデータを書き込みます.
//-------------------------------------------------------------------------------------------
void WriteBmp( FILE* pFile, TinyTexture& texture )
{
    BMP_FILE_HEADER fileHeader;
    BMP_INFO_HEADER infoHeader;

    fileHeader.Type      = 'MB';
    fileHeader.Size      = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER) + texture.Size;
    fileHeader.Reserved1 = 0;
    fileHeader.Reserved2 = 0;
    fileHeader.OffBits   = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER);

    infoHeader.Size   = 40;
    infoHeader.Width  = texture.Width;
    infoHeader.Height = texture.Height;
    infoHeader.Planes = 1;
    switch( texture.Format )
    {
    case TinyTexture::FORMAT_RGB:
    case TinyTexture::FORMAT_BGR:
        infoHeader.BitCount = 24;
        break;

    case TinyTexture::FORMAT_RGBA:
    case TinyTexture::FORMAT_BGRA:
        infoHeader.BitCount = 32;
        break;
    }
    infoHeader.Compression   = BMP_COMPRESSION_RGB;
    infoHeader.SizeImage     = 0;
    infoHeader.XPelsPerMeter = 0;
    infoHeader.YPelsPerMeter = 0;
    infoHeader.ClrUsed       = 0;
    infoHeader.ClrImportant  = 0;

    WriteBmpFileHeader( fileHeader, pFile );
    WriteBmpInfoHeader( infoHeader, pFile );

    int bytePerPixel = infoHeader.BitCount / 8;

    for ( int i=texture.Height -1; i>=0; i-- )
    {
        for( int j=0; j<texture.Width; ++j )
        {
            int index = ( i * texture.Width * bytePerPixel ) + j * bytePerPixel;

            if ( bytePerPixel == 4 )
            {
                fwrite( &texture.pPixels[index + 2], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 1], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 0], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 3], sizeof(unsigned char), 1, pFile );
            }
            else if ( bytePerPixel == 3 )
            {
                fwrite( &texture.pPixels[index + 2], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 1], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 0], sizeof(unsigned char), 1, pFile );
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
//      TGAファイルヘッダを書き込みます.
//-------------------------------------------------------------------------------------------
void WriteTgaFileHeader( TGA_FILE_HEADER& header, FILE* pFile )
{
    fwrite( &header.IdLength,       sizeof(unsigned char),  1, pFile );
    fwrite( &header.ColorMapType,   sizeof(unsigned char),  1, pFile );
    fwrite( &header.ImageType,      sizeof(unsigned char),  1, pFile );
    fwrite( &header.ColorMapIndex,  sizeof(unsigned short), 1, pFile );
    fwrite( &header.ColorMapLength, sizeof(unsigned short), 1, pFile );
    fwrite( &header.ColorMapSize,   sizeof(unsigned char),  1, pFile );
    fwrite( &header.OriginX,        sizeof(unsigned short), 1, pFile );
    fwrite( &header.OriginY,        sizeof(unsigned short), 1, pFile );
    fwrite( &header.Width,          sizeof(unsigned short), 1, pFile );
    fwrite( &header.Height,         sizeof(unsigned short), 1, pFile );
    fwrite( &header.BitPerPixel,    sizeof(unsigned char),  1, pFile );
    fwrite( &header.Discripter,     sizeof(unsigned char),  1, pFile );
}

//-------------------------------------------------------------------------------------------
//      TGAファイルフッターを書き込みます.
//-------------------------------------------------------------------------------------------
void WriteTgaFileFooter( TGA_FILE_FOOTER& footer, FILE* pFile )
{
    fwrite( &footer.ExtOffset,  sizeof(unsigned char), 1,   pFile );
    fwrite( &footer.DevOffset,  sizeof(unsigned char), 1,   pFile );
    fwrite( &footer.Magic,      sizeof(unsigned char), 17,  pFile );
    fwrite( &footer.RFU1,       sizeof(unsigned char), 1,   pFile );
    fwrite( &footer.RFU2,       sizeof(unsigned char), 1,   pFile );
}

//-------------------------------------------------------------------------------------------
//      TGAデータを書き込みます.
//-------------------------------------------------------------------------------------------
void WriteTga( FILE* pFile, TinyTexture& texture )
{
    TGA_FILE_HEADER fileHeader;
    TGA_FILE_FOOTER fileFooter;

    memset( &fileHeader, 0, sizeof(TGA_FILE_HEADER) );
    memset( &fileFooter, 0, sizeof(TGA_FILE_FOOTER) );

    fileHeader.ImageType = TGA_IMAGE_FULL_COLOR;
    fileHeader.Width = static_cast<unsigned short>( texture.Width );
    fileHeader.Height = static_cast<unsigned short>( texture.Height );
    switch( texture.Format )
    {
    case TinyTexture::FORMAT_RGB:
    case TinyTexture::FORMAT_BGR:
        fileHeader.BitPerPixel = 24;
        break;

    case TinyTexture::FORMAT_RGBA:
    case TinyTexture::FORMAT_BGRA:
        fileHeader.BitPerPixel = 32;
        break;
    }

    unsigned char magic[17] = "TRUEVISION-XFILE";
    memcpy( fileFooter.Magic, magic, sizeof(unsigned char) * 17 );
    fileFooter.RFU1 = '.';

    int bytePerPixel = fileHeader.BitPerPixel / 8;

    WriteTgaFileHeader( fileHeader, pFile );
    for ( int i=texture.Height -1; i>=0; i-- )
    {
        for( int j=0; j<texture.Width; ++j )
        {
            int index = ( i * texture.Width * bytePerPixel ) + j * bytePerPixel;

            if ( bytePerPixel == 4 )
            {
                fwrite( &texture.pPixels[index + 2], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 1], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 0], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 3], sizeof(unsigned char), 1, pFile );
            }
            else if ( bytePerPixel == 3 )
            {
                fwrite( &texture.pPixels[index + 2], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 1], sizeof(unsigned char), 1, pFile );
                fwrite( &texture.pPixels[index + 0], sizeof(unsigned char), 1, pFile );
            }
        }
    }
    WriteTgaFileFooter( fileFooter, pFile );
}

//-------------------------------------------------------------------------------------------
//      テクスチャをBMPファイルに保存します.
//-------------------------------------------------------------------------------------------
bool SaveTextureToBmpA
(
    ID3D11DeviceContext* pDeviceContext,
    ID3D11Texture2D*     pTexture,
    const char*          fileName
)
{
    FILE* pFile;
    errno_t err = fopen_s( &pFile, fileName, "wb" );
    if ( err != 0 )
    {
        return false;
    }

    TinyTexture image;
    if ( !CreateTinyTexture( pDeviceContext, pTexture, image ) )
    {
        image.Term();
        fclose( pFile );
        return false;
    }

    WriteBmp( pFile, image );

    fclose( pFile );
    image.Term();

    return true;
}

//-------------------------------------------------------------------------------------------
//      テクスチャをBMPファイルに保存します.
//-------------------------------------------------------------------------------------------
bool SaveTextureToBmpW
(
    ID3D11DeviceContext* pDeviceContext,
    ID3D11Texture2D*     pTexture,
    const wchar_t*       fileName
)
{
    FILE* pFile;
    errno_t err = _wfopen_s( &pFile, fileName, L"wb" );
    if ( err != 0 )
    {
        return false;
    }

    TinyTexture image;
    if ( !CreateTinyTexture( pDeviceContext, pTexture, image ) )
    {
        image.Term();
        fclose( pFile );
        return false;
    }

    WriteBmp( pFile, image );

    fclose( pFile );
    image.Term();

    return true;
}

//-------------------------------------------------------------------------------------------
//      テクスチャをTGAファイルに保存します.
//-------------------------------------------------------------------------------------------
bool SaveTextureToTgaA
(
    ID3D11DeviceContext* pDeviceContext,
    ID3D11Texture2D*     pTexture,
    const char*          fileName
)
{
    FILE* pFile;
    errno_t err = fopen_s( &pFile, fileName, "wb" );
    if ( err != 0 )
    {
        return false;
    }

    TinyTexture image;
    if ( !CreateTinyTexture( pDeviceContext, pTexture, image ) )
    {
        image.Term();
        fclose( pFile );
        return false;
    }

    WriteTga( pFile, image );

    fclose( pFile );
    image.Term();

    return true;
}

//-------------------------------------------------------------------------------------------
//          テクスチャをTGAファイルに保存します.
//-------------------------------------------------------------------------------------------
bool SaveTextureToTgaW
(
    ID3D11DeviceContext* pDeviceContext,
    ID3D11Texture2D*     pTexture,
    const wchar_t*       fileName
)
{
    FILE* pFile;
    errno_t err = _wfopen_s( &pFile, fileName, L"wb" );
    if ( err != 0 )
    {
        return false;
    }

    TinyTexture image;
    if ( !CreateTinyTexture( pDeviceContext, pTexture, image ) )
    {
        image.Term();
        fclose( pFile );
        return false;
    }

    WriteTga( pFile, image );

    fclose( pFile );
    image.Term();

    return true;
}


} // namespace asdx