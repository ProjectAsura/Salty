//----------------------------------------------------------------------------------------
// File : asdxUtil.h
// Desc : Utility Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------------

#ifndef __ASDX_UTIL_H__
#define __ASDX_UTIL_H__

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include <d3d11.h>


#ifndef SAFE_FREE
#define SAFE_FREE( p )          { free( (p) ); (p) = nullptr; }
#endif//SAFF_FREE 

#ifndef SAFE_DELETE
#define SAFE_DELETE( p )        { if ( (p) ) { delete (p); (p) = nullptr; } }           // ifをつけるのはoperator delete対策のため.
#endif//SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY( p )  { if ( (p) ) { delete [] (p); (p) = nullptr; } }        // ifをつけるのはoperator delete対策のため.
#endif//SAFE_DELETE_ARRAY

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( p )       { if ( (p) ) { (p)->Release(); (p) = nullptr; } }
#endif//SAFE_RELEASE


namespace asdx {

//----------------------------------------------------------------------------------------
//! @brief      デバッグオブジェクト名を設定します.
//!
//! @param [in]     resource        デバッグオブジェクト名を設定するリソース.
//! @param [in]     name            設定するデバッグオブジェクト名.
//----------------------------------------------------------------------------------------
template<UINT TNameLength> inline
void SetDebugObjectName( ID3D11DeviceChild* resource, const char (&name)[TNameLength] )
{
    #if defined(_DEBUG) || defined(DEBUG)
        resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
    #else
        UNREFERENCED_PARAMETER(resource);
        UNREFERENCED_PARAMETER(name);
    #endif
}

//---------------------------------------------------------------------------------------
//! @brief      機能レベルから対応する文字列を取得します.
//!
//! @parma [in]     value       機能レベル.
//! @return     機能レベルから対応する文字列を返却します.
//---------------------------------------------------------------------------------------
LPSTR   GetFeatureLevelStringA( const D3D_FEATURE_LEVEL value );

//---------------------------------------------------------------------------------------
//! @brief      機能レベルから対応する文字列を取得します.
//!
//! @parma [in]     value       機能レベル.
//! @return     機能レベルから対応する文字列を返却します.
//---------------------------------------------------------------------------------------
LPWSTR  GetFeatureLevelStringW( const D3D_FEATURE_LEVEL value );

//---------------------------------------------------------------------------------------
//! @brief      ドライバータイプから対応する文字列を取得します.
//!
//! @param [in]     value           ドライバータイプ.
//! @return     ドライバータイプから対応する文字列を返却します.
//---------------------------------------------------------------------------------------
LPSTR   GetDriverTypeStringA( const D3D_DRIVER_TYPE value );

//---------------------------------------------------------------------------------------
//! @brief      ドライバータイプから対応する文字列を取得します.
//!
//! @param [in]     value           ドライバータイプ.
//! @return     ドライバータイプから対応する文字列を返却します.
//---------------------------------------------------------------------------------------
LPWSTR  GetDriverTypeStringW( const D3D_DRIVER_TYPE value );

//---------------------------------------------------------------------------------------
//! @brief      DXGIフォーマットから対応する文字列を取得します.
//!
//! @param [in]     value           DXGIフォーマット.
//! @return     DXGIフォーマットから対応する文字列を返却します.
//---------------------------------------------------------------------------------------
LPSTR   GetFormatStringA( const DXGI_FORMAT value );

//---------------------------------------------------------------------------------------
//! @brief      DXGIフォーマットから対応する文字列を取得します.
//!
//! @param [in]     value           DXGIフォーマット.
//! @return     DXGIフォーマットから対応する文字列を返却します.
//---------------------------------------------------------------------------------------
LPWSTR  GetFormatStringW( const DXGI_FORMAT value );

//---------------------------------------------------------------------------------------
//! @brief      DXGIフォーマットから1ピクセルあたりのビット数を取得します.
//
//! @param [in]     format          DXGIフォーマット.
//! @return     1ピクセルあたりのビット数を返却します.
//---------------------------------------------------------------------------------------
size_t  GetBitsPerPixel( DXGI_FORMAT format );

//---------------------------------------------------------------------------------------
//! @brief      サーフェイス情報を取得します.
//!
//! @param [in]     width           画像の横幅.
//! @param [in]     height          画像の縦幅.
//! @param [in]     format          DXGIフォーマット.
//! @param [out]    outNumBytes     バイト数.
//! @param [out]    outRowBytes     1列あたりのバイト数.
//! @param [out]    outNumRows      列数.
//---------------------------------------------------------------------------------------
void    GetSurfaceInfo( size_t width, size_t height, DXGI_FORMAT format, size_t* outNumBytes, size_t* outRowBytes, size_t* outNumRows );

//---------------------------------------------------------------------------------------
//! @brief      スクリーンキャプチャーを行います.
//!
//! @param [in]     pDeviceContext      デバイスコンテキストです.
//! @param [in]     pSwapChain          スワップチェインです.
//! @param [out]    ppTexture           キャプチャーイメージを格納するテクスチャです.
//! @retval true    キャプチャーに成功.
//! @retval false   キャプチャーに失敗.
//---------------------------------------------------------------------------------------
bool ScreenCapture( ID3D11DeviceContext* pDeviceContext, IDXGISwapChain* pSwapChain, ID3D11Texture2D** ppTexture );

//---------------------------------------------------------------------------------------
//! @brief      テクスチャをBMPファイルとして保存します.
//!
//! @param [in]     pDeviceContext      デバイスコンテキストです.
//! @param [in]     pTexture            テクスチャです.
//! @param [in]     fileName            出力ファイル名です.
//! @retval true    保存に成功.
//! @retval false   保存に失敗.
//---------------------------------------------------------------------------------------
bool SaveTextureToBmpA( ID3D11DeviceContext* pDeviceContext, ID3D11Texture2D* pTexture, const char*    fileName );

//---------------------------------------------------------------------------------------
//! @brief      テクスチャをBMPファイルとして保存します.
//!
//! @param [in]     pDeviceContext      デバイスコンテキストです.
//! @param [in]     pTexture            テクスチャです.
//! @param [in]     fileName            出力ファイル名です.
//! @retval true    保存に成功.
//! @retval false   保存に失敗.
//---------------------------------------------------------------------------------------
bool SaveTextureToBmpW( ID3D11DeviceContext* pDeviceContext, ID3D11Texture2D* pTexture, const wchar_t* fileName );

//---------------------------------------------------------------------------------------
//! @brief      テクスチャをTGAファイルとして保存します.
//!
//! @param [in]     pDeviceContext      デバイスコンテキストです.
//! @param [in]     pTexture            テクスチャです.
//! @param [in]     fileName            出力ファイル名です.
//! @retval true    保存に成功.
//! @retval false   保存に失敗.
//---------------------------------------------------------------------------------------
bool SaveTextureToTgaA( ID3D11DeviceContext* pDeviceContext, ID3D11Texture2D* pTexture, const char*    filename );

//---------------------------------------------------------------------------------------
//! @brief      テクスチャをTGAファイルとして保存します.
//!
//! @param [in]     pDeviceContext      デバイスコンテキストです.
//! @param [in]     pTexture            テクスチャです.
//! @param [in]     fileName            出力ファイル名です.
//! @retval true    保存に成功.
//! @retval false   保存に失敗.
//---------------------------------------------------------------------------------------
bool SaveTextureToTgaW( ID3D11DeviceContext* pDeviceContext, ID3D11Texture2D* pTexture, const wchar_t* filename );

//---------------------------------------------------------------------------------------
//! @brief      テクスチャをBMPファイルとして保存します.
//!
//! @param [in]     fileName            出力ファイル名です.
//! @param [in]     width               テクスチャの横幅です.
//! @param [in]     height              テクスチャの縦幅です.
//! @param [in]     component           ピクセルを構成するチャンネル数です(RGB=3, RGBA=4).
//! @retval true    保存に成功.
//! @retval false   保存に失敗.
//---------------------------------------------------------------------------------------
bool SaveTextureToBmpA( const char*    filename, const int width, const int height, const int component, const unsigned char* pPixels );

//---------------------------------------------------------------------------------------
//! @brief      テクスチャをBMPファイルとして保存します.
//!
//! @param [in]     fileName            出力ファイル名です.
//! @param [in]     width               テクスチャの横幅です.
//! @param [in]     height              テクスチャの縦幅です.
//! @param [in]     component           ピクセルを構成するチャンネル数です(RGB=3, RGBA=4).
//! @retval true    保存に成功.
//! @retval false   保存に失敗.
//---------------------------------------------------------------------------------------
bool SaveTextureToBmpW( const wchar_t* filename, const int width, const int height, const int component, const unsigned char* pPixels );

//---------------------------------------------------------------------------------------
//! @brief      テクスチャをTGAファイルとして保存します.
//!
//! @param [in]     fileName            出力ファイル名です.
//! @param [in]     width               テクスチャの横幅です.
//! @param [in]     height              テクスチャの縦幅です.
//! @param [in]     component           ピクセルを構成するチャンネル数です(RGB=3, RGBA=4).
//! @retval true    保存に成功.
//! @retval false   保存に失敗.
//---------------------------------------------------------------------------------------
bool SaveTextureToTgaA( const char*    filename, const int width, const int height, const int component, const unsigned char* pPixels );

//---------------------------------------------------------------------------------------
//! @brief      テクスチャをTGAファイルとして保存します.
//!
//! @param [in]     fileName            出力ファイル名です.
//! @param [in]     width               テクスチャの横幅です.
//! @param [in]     height              テクスチャの縦幅です.
//! @param [in]     component           ピクセルを構成するチャンネル数です(RGB=3, RGBA=4).
//! @retval true    保存に成功.
//! @retval false   保存に失敗.
//---------------------------------------------------------------------------------------
bool SaveTextureToTgaW( const wchar_t* filename, const int width, const int height, const int component, const unsigned char* pPixels );





} // namespace asdx

#endif//__ASDX_UTIL_H__

