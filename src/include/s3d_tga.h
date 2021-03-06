﻿//-------------------------------------------------------------------------------------------------
// File : s3d_tga.h
// Desc : Targa Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_typedef.h>


namespace s3d {

//-------------------------------------------------------------------------------------------------
//! @brief      TGAファイルを読み込みます.
//!
//! @param [in]     filename        ファイル名.
//! @param [out]    width           画像の横幅です.
//! @param [out]    height          画像の縦幅です.
//! @param [out]    component       コンポーネント数(RGB=3, RGBA=4)
//! @param [out]    ppPixels        ピクセルデータです.
//! @retval true    読み込みに成功.
//! @retval false   読み込みに失敗.
//-------------------------------------------------------------------------------------------------
bool LoadFromTGA(
    const char* filename,
    s32&        width,
    s32&        height,
    s32&        component,
    f32**       ppPixel );

} // namespace s3d

