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
#define SAFE_DELETE( p )        { if ( (p) ) { delete (p); (p) = nullptr; } }           // if������̂�operator delete�΍�̂���.
#endif//SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY( p )  { if ( (p) ) { delete [] (p); (p) = nullptr; } }        // if������̂�operator delete�΍�̂���.
#endif//SAFE_DELETE_ARRAY

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( p )       { if ( (p) ) { (p)->Release(); (p) = nullptr; } }
#endif//SAFE_RELEASE


namespace asdx {

//----------------------------------------------------------------------------------------
//! @brief      �f�o�b�O�I�u�W�F�N�g����ݒ肵�܂�.
//!
//! @param [in]     resource        �f�o�b�O�I�u�W�F�N�g����ݒ肷�郊�\�[�X.
//! @param [in]     name            �ݒ肷��f�o�b�O�I�u�W�F�N�g��.
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
//! @brief      �@�\���x������Ή����镶������擾���܂�.
//!
//! @parma [in]     value       �@�\���x��.
//! @return     �@�\���x������Ή����镶�����ԋp���܂�.
//---------------------------------------------------------------------------------------
LPSTR   GetFeatureLevelStringA( const D3D_FEATURE_LEVEL value );

//---------------------------------------------------------------------------------------
//! @brief      �@�\���x������Ή����镶������擾���܂�.
//!
//! @parma [in]     value       �@�\���x��.
//! @return     �@�\���x������Ή����镶�����ԋp���܂�.
//---------------------------------------------------------------------------------------
LPWSTR  GetFeatureLevelStringW( const D3D_FEATURE_LEVEL value );

//---------------------------------------------------------------------------------------
//! @brief      �h���C�o�[�^�C�v����Ή����镶������擾���܂�.
//!
//! @param [in]     value           �h���C�o�[�^�C�v.
//! @return     �h���C�o�[�^�C�v����Ή����镶�����ԋp���܂�.
//---------------------------------------------------------------------------------------
LPSTR   GetDriverTypeStringA( const D3D_DRIVER_TYPE value );

//---------------------------------------------------------------------------------------
//! @brief      �h���C�o�[�^�C�v����Ή����镶������擾���܂�.
//!
//! @param [in]     value           �h���C�o�[�^�C�v.
//! @return     �h���C�o�[�^�C�v����Ή����镶�����ԋp���܂�.
//---------------------------------------------------------------------------------------
LPWSTR  GetDriverTypeStringW( const D3D_DRIVER_TYPE value );

//---------------------------------------------------------------------------------------
//! @brief      DXGI�t�H�[�}�b�g����Ή����镶������擾���܂�.
//!
//! @param [in]     value           DXGI�t�H�[�}�b�g.
//! @return     DXGI�t�H�[�}�b�g����Ή����镶�����ԋp���܂�.
//---------------------------------------------------------------------------------------
LPSTR   GetFormatStringA( const DXGI_FORMAT value );

//---------------------------------------------------------------------------------------
//! @brief      DXGI�t�H�[�}�b�g����Ή����镶������擾���܂�.
//!
//! @param [in]     value           DXGI�t�H�[�}�b�g.
//! @return     DXGI�t�H�[�}�b�g����Ή����镶�����ԋp���܂�.
//---------------------------------------------------------------------------------------
LPWSTR  GetFormatStringW( const DXGI_FORMAT value );

} // namespace asdx

#endif//__ASDX_UTIL_H__

