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

} // namespace asdx

#endif//__ASDX_UTIL_H__

