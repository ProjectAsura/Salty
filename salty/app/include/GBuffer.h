//----------------------------------------------------------------------------------------
// File : GBuffer.h
// Desc : G-Buffer.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------------

#ifndef __GBUFFER_H__
#define __GBUFFER_H__

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include <asdxTypedef.h>
#include <asdxMath.h>
#include <asdxRenderTarget.h>
#include <asdxDepthStencilTarget.h>

namespace salty {

class GBuffer
{
public:
    ASDX_ALIGN(16)
    struct VSParam
    {
        asdx::Matrix    World;
        asdx::Matrix    View;
        asdx::Matrix    Proj;
        asdx::Vector4   CameraPos;
    };

    ASDX_ALIGN(16)
    struct PSParam
    {
        f32     Reflectivity;
        f32     Refractivity;
        f32     Roughness;
    };

private:
    asdx::RenderTarget2D        Color;
    asdx::RenderTarget2D        Position;
    asdx::RenderTarget2D        Normal;
    asdx::DepthStencilTarget    Depth;

protected:

public:
    GBuffer();
    virtual ~GBuffer();

    bool Init( ID3D11Device* pDevice );
    void Term();

    void BeginPass( ID3D11DeviceContext* pDeviceContext );
    void EndPass( ID3D11DeviceContext* pDeviceContext );

    void UpdateVSParam( ID3D11DeviceContext* pDeviceContext, VSParam& param );
    void UpdatePSParam( ID3D11DeviceContext* pDeviceContext, PSParam& param );

};


} // namespace salty

#endif//__GBUFFER_H__
