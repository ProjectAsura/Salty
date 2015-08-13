//--------------------------------------------------------------------------------------
// File : s3d_hdr.h
// Desc : HDR File Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------
#pragma once 

//--------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------
#include <s3d_typedef.h>


namespace s3d {

//--------------------------------------------------------------------------------------
//! @brief      HDRファイルに保存します.
//!
//! @param[in]      filename        ファイル名です.
//! @param[in]      width           画像の横幅です.
//! @param[in]      height          画像の縦幅です.
//! @param[in]      gamma           ガンマ値です.
//! @param[in]      exposure        露光値です.
//! @param[in]      pPixels         ピクセルデータです.
//--------------------------------------------------------------------------------------
bool SaveToHDR(
    const char* filename,
    const s32   width,
    const s32   height,
    const f32   gamma,
    const f32   expsoure,
    const f32*  pPixel );

//--------------------------------------------------------------------------------------
//! @brief      HDRファイルから読み込みします.
//!
//! @param[in]      filename        ファイル名です.
//! @param[out]     width           画像の横幅です.
//! @param[out]     height          画像の縦幅です.
//! @param[out]     gamma           ガンマ値です.
//! @param[out]     exposure        露光値です.
//! @param[out]     ppPixels        ピクセルデータです.
//--------------------------------------------------------------------------------------
bool LoadFromHDR(
    const char* filename,
    s32&        width,
    s32&        height,
    f32&        gamma,
    f32&        exposure,
    f32**       ppPixel );

} // namespace s3d
