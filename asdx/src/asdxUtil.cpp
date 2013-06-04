//----------------------------------------------------------------------------------------
// File : asdxUtil.cpp
// Desc : Utility Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include "asdxUtil.h"


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



} // namespace asdx