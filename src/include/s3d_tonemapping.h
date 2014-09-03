//------------------------------------------------------------------------------------------------
// File : s3d_tonemap.h
// Desc : Tone Mapping Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------------

#ifndef __S3D_TONE_MAPPING_H__
#define __S3D_TONE_MAPPING_H__

//------------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------------
#include <s3d_typedef.h>
#include <s3d_math.h>
#include <vector>


namespace s3d {

//------------------------------------------------------------------------------------------------
//! @brief      トーンマッピングを行います.
//!
//! @param[in]      width       画像の横幅です.
//! @param[in]      height      画像の縦幅です.
//! @param[in]      pPixels     ピクセルです.
//------------------------------------------------------------------------------------------------
std::vector<Vector3> ToneMapping( const s32 width, const s32 height, const Vector3* pPixels );


} // namespace s3d

#endif//__S3D_TONE_MAPPING_H__
