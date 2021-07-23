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
        const Color3&   diffuse,
        const Texture*  pTexture = nullptr,
        const Color3&   emissive = Color3(0.0f, 0.0f, 0.0f));

    static IMaterial* CreatePhong(
        const Color3&   specular,
        f32             power,
        const Texture*  pTexture = nullptr,
        const Color3&   emissive = Color3(0.0f, 0.0f, 0.0f));

    static IMaterial* CreateMirror(
        const Color3&   specular,
        const Texture*  pTexture = nullptr,
        const Color3&   emissive = Color3(0.0f, 0.0f, 0.0f));

    static IMaterial* CreateGlass(
        const Color3&   specuar,
        f32             ior,
        const Texture*  pTexture = nullptr,
        const Color3&   emissive = Color3(0.0f, 0.0f, 0.0f));

    static IMaterial* CreatePlastic(
        const Color3&   diffuse,
        const Color3&   specular,
        f32             power,
        const Texture*  pTexture = nullptr,
        const Color3&   emissive = Color3(0.0f, 0.0f, 0.0f));
};

} // namespace s3d
