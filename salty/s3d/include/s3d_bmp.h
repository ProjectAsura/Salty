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
#include <s3d_color3.h>


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
bool SaveToBMP( const char* filename, const int width, const int height, const Color3* pPixel, const f64 gamma = 2.2 );

} // namespace s3d

#endif//__S3D_BMP_H__