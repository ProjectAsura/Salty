﻿//-------------------------------------------------------------------------------------------------
// File : s3d_materialfactory.cpp
// Desc : Material Factory.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_materialfactory.h>
#include <s3d_lambert.h>
#include <s3d_phong.h>
#include <s3d_mirror.h>
#include <s3d_glass.h>
#include <s3d_plastic.h>
#include <s3d_texturedmaterial.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// MaterialFactory class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      Lambertマテリアルを生成します.
//-------------------------------------------------------------------------------------------------
IMaterial* MaterialFactory::CreateLambert
(
    const Color3&           diffuse,
    const Texture*        pTexture,
    const Color3&           emissive
)
{
    auto instance = Lambert::Create(diffuse, emissive);

    if (pTexture == nullptr)
    { return instance; }

    auto wrapper = TexturedMaterial::Create(pTexture, instance);
    SafeRelease(instance);

    return wrapper;
}

//-------------------------------------------------------------------------------------------------
//      Phongマテリアルを生成します.
//-------------------------------------------------------------------------------------------------
IMaterial* MaterialFactory::CreatePhong
(
    const Color3&           specular,
    f32                     power,
    const Texture*        pTexture,
    const Color3&           emissive
)
{
    auto instance = Phong::Create(specular, power, emissive);

    if (pTexture == nullptr)
    { return instance; }

    auto wrapper = TexturedMaterial::Create(pTexture, instance);
    SafeRelease(instance);

    return wrapper;
}

//-------------------------------------------------------------------------------------------------
//      Mirrorマテリアルを生成します.
//-------------------------------------------------------------------------------------------------
IMaterial* MaterialFactory::CreateMirror
(
    const Color3&           specular,
    const Texture*        pTexture,
    const Color3&           emissive
)
{
    auto instance = Mirror::Create(specular, emissive);

    if (pTexture == nullptr)
    { return instance; }

    auto wrapper = TexturedMaterial::Create(pTexture, instance);
    SafeRelease(instance);

    return wrapper;
}

//-------------------------------------------------------------------------------------------------
//      Glassマテリアルを生成します.
//-------------------------------------------------------------------------------------------------
IMaterial* MaterialFactory::CreateGlass
(
    const Color3&           specular,
    f32                     ior,
    const Texture*        pTexture,
    const Color3&           emissive
)
{
    auto instance = Glass::Create(specular, ior, emissive);

    if (pTexture == nullptr)
    { return instance; }

    auto wrapper = TexturedMaterial::Create(pTexture, instance);
    SafeRelease(instance);

    return wrapper;
}

//-------------------------------------------------------------------------------------------------
//      Plasticマテリアルを生成します.
//-------------------------------------------------------------------------------------------------
IMaterial* MaterialFactory::CreatePlastic
(
    const Color3&           diffuse,
    const Color3&           specular,
    f32                     power,
    const Texture*        pTexture,
    const Color3&           emissive
)
{
    auto instance = Plastic::Create(diffuse, specular, power, emissive);

    if (pTexture == nullptr)
    { return instance; }

    auto wrapper = TexturedMaterial::Create(pTexture, instance);
    SafeRelease(instance);

    return wrapper;
}

} // namespace s3d
