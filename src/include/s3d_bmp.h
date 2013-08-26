﻿//-------------------------------------------------------------------------------------
// File : s3d_bmp.h
// Desc : Bitmap Utility Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------

#ifndef __S3D_BMP_H__
#define __S3D_BMP_H__

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include <s3d_typedef.h>


namespace s3d {

//----------------------------------------------------------------------------------------
//! @brief      BMPファイルに保存します.
//!
//! @param [in]     filename        ファイル名.
//! @param [in]     width           画像の横幅.
//! @param [in]     height          画像の縦幅.
//! @param [in]     pPixel          ピクセル.
//! @retval true    保存に成功.
//! @retval false   保存に失敗.
//----------------------------------------------------------------------------------------
bool SaveToBMP( const char* filename, const s32 width, const s32 height, const f32* pPixel, const f32 gamma = 2.2f );

//----------------------------------------------------------------------------------------
//! @brief      BMPファイルを読み込みます.
//!
//! @param [in]     filename        ファイル名.
//! @param [out]    width           画像の横幅です.
//! @param [out]    height          画像の縦幅です.
//! @param [out]    ppPixels        ピクセルデータです.
//! @retval true    読み込みに成功.
//! @retval false   読み込みに失敗.
//----------------------------------------------------------------------------------------
bool LoadFromBMP( const char* filename, s32& width, s32& height, f32** ppPixel );


} // namespace s3d

#endif//__S3D_BMP_H__
