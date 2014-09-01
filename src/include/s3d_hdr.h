//--------------------------------------------------------------------------------------
// File : s3d_hdr.h
// Desc : HDR File Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------

#ifndef __S3D_HDR_H__
#define __S3D_HDR_H__

//--------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------
#include <s3d_typedef.h>


namespace s3d {

bool SaveToHDR( const char* filename, const s32 width, const s32 height,  const f32 gamma, const f32 expsoure, const f32* pPixel );

bool LoadFromHDR( const char* filename, s32& widh, s32&height, f32& gamma, f32& exposure, f32** ppPixel );

} // namespace s3d


#endif//__S3D_HDR_H__
