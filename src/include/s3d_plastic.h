//-------------------------------------------------------------------------------------------------
// File : s3d_plastic.h
// Desc : Plastic Material.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_material.h>
#include <atomic>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Plastic class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Plastic : IMaterial
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOHTING */

public:
    //=============================================================================================
    // public variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // public methods.
    //=============================================================================================

    static IMaterial* Create(const Color3& diffuse, const Color3& specular, f32 power);

    static IMaterial* Create(const Color3& diffuse, const Color3& specular, f32 power, const Color3& emissive);

    void AddRef() override;

    void Release() override;

    u32 GetCount() const override;

    Color3 Shade( ShadingArg& arg ) const override;

    Color3 GetEmissive() const override;

    bool HasDelta() const override;

    Color3 GetBaseColor(const Vector2&) const override
    { return m_Diffuse; }

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    std::atomic<u32>    m_Count;
    Color3              m_Diffuse;
    Color3              m_Specular;
    f32                 m_Power;
    Color3              m_Emissive;
    f32                 m_Threshold[2];

    //=============================================================================================
    // private methods.
    //=============================================================================================
    Plastic(const Color3& diffuse, const Color3& specular, f32 power, const Color3& emissive);

    ~Plastic();
};

} // namespace s3d
