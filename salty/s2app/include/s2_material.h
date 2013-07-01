//---------------------------------------------------------------------------------------
// File : s2_material.h
// Desc : Material Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------

#ifndef __S2_MATERIAL_H__
#define __S2_MATERIAL_H__


//---------------------------------------------------------------------------------------
//! @brief      反射タイプ列挙子です.
//---------------------------------------------------------------------------------------
enum REFLECTION_TYPE
{
    REFLECTION_DIFFUSE = 0,    // でぃふゅーず
    REFLECTION_SPECULAR,       // すぺきゅら
    REFLECTION_REFRACTION,     // がらす
};

//---------------------------------------------------------------------------------------
//! @brief      マテリアル構造体です.
//---------------------------------------------------------------------------------------
struct MATERIAL
{
    COLOR           emission;
    COLOR           color;
    REFLECTION_TYPE type;

    MATERIAL()
    { /* DO_NOTHING */ }

    MATERIAL(
        const COLOR&            emission_,
        const COLOR&            color_,
        const REFLECTION_TYPE   type_
    )
    : emission  ( emission_ )
    , color     ( color_ )
    , type      ( type_ )
    { /* DO_NOTHING */ }

};

#endif//__S2_MATERIAL_H__