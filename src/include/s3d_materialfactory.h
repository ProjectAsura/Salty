//-------------------------------------------------------------------------------------------------
// File : s3d_materialfactory.h
// Desc : Material Factory.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_material.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// MaterialFactory class
///////////////////////////////////////////////////////////////////////////////////////////////////
class MaterialFactory
{
public:
    static IMaterial* CreateLambert(
        const Color4&           diffuse,
        const Texture2D*        pTexture = nullptr,
        const TextureSampler*   pSampler = nullptr,
        const Color4&           emissive = Color4(0.0f, 0.0f, 0.0f, 1.0f));

    static IMaterial* CreatePhong(
        const Color4&           specular,
        f32                     power,
        const Texture2D*        pTexture = nullptr,
        const TextureSampler*   pSampler = nullptr,
        const Color4&           emissive = Color4(0.0f, 0.0f, 0.0f, 1.0f));

    static IMaterial* CreateMirror(
        const Color4&           specular,
        const Texture2D*        pTexture = nullptr,
        const TextureSampler*   pSampler = nullptr,
        const Color4&           emissive = Color4(0.0f, 0.0f, 0.0f, 1.0f));

    static IMaterial* CreateGlass(
        const Color4&           specuar,
        f32                     ior,
        const Texture2D*        pTexture = nullptr,
        const TextureSampler*   pSampler = nullptr,
        const Color4&           emissive = Color4(0.0f, 0.0f, 0.0f, 1.0f));
};

} // namespace s3d
