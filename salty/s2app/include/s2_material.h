//---------------------------------------------------------------------------------------
// File : s2_material.h
// Desc : Material Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------

#ifndef __S2_MATERIAL_H__
#define __S2_MATERIAL_H__


//---------------------------------------------------------------------------------------
//! @brief      材質タイプ列挙子です.
//---------------------------------------------------------------------------------------
enum MATERIAL_TYPE
{
    MATERIAL_DIFFUSE = 0,    // でぃふゅーず
    MATERIAL_MIRROR,         // みらー
    MATERIAL_GLASS,          // がらす
    MATERIAL_LIGHT,          // らいと.
};

//---------------------------------------------------------------------------------------
//! @brief      マテリアル構造体です.
//---------------------------------------------------------------------------------------
struct MATERIAL
{
    COLOR         color;
    MATERIAL_TYPE type;

    MATERIAL()
    { /* DO_NOTHING */ }

    MATERIAL(
        const COLOR&          color_,
        const MATERIAL_TYPE   type_
    )
    : color     ( color_ )
    , type      ( type_ )
    { /* DO_NOTHING */ }

};

#endif//__S2_MATERIAL_H__
