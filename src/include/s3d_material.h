//-------------------------------------------------------------------------------------------------
// File : s3d_material.h
// Desc : Material Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_math.h>
#include <s3d_texture.h>
#include <s3d_reference.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// ShadingArg structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ShadingArg
{
    Vector3     input;          //!< 入射方向.
    Vector3     output;         //!< 出射方向.
    Vector3     normal;         //!< 法線ベクトル.
    Vector2     texcoord;       //!< テクスチャ座標.
    Random      random;         //!< 乱数.
    bool        dice;           //!< 打ち切りかどうか?
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// IMaterial interface
///////////////////////////////////////////////////////////////////////////////////////////////////
struct IMaterial : IReference
{
    virtual ~IMaterial() {}
    virtual Color4  Shade      ( ShadingArg& ) const = 0;
    virtual Color4  GetEmissive() const = 0;
    virtual Color4  GetBaseColor(const Vector2& texcoord) const = 0;
    virtual bool    HasDelta   () const = 0;
};

} // namespace s3d
