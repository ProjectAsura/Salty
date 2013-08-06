//-------------------------------------------------------------------------------------------
// File : saltyApp.h
// Desc : Salty Application Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

#ifndef __SALTY_APP_H__
#define __SALTY_APP_H__

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <asdxApp.h>
#include <asdxResMesh.h>
#include <asdxMesh.h>
#include <asdxCameraUpdater.h>
#include <asdxQuadRenderer.h>
#include <asdxFont.h>


namespace salty {

////////////////////////////////////////////////////////////////////////////////////////////
// App class
////////////////////////////////////////////////////////////////////////////////////////////
class App : public asdx::Application
{
    //======================================================================================
    // list of friend classes and methods.
    //======================================================================================
    /* NOTHING */

private:
    asdx::Font              m_Font;
    asdx::CameraUpdater     m_CameraUpdater;
    asdx::Mesh              m_Dosei;
    asdx::Mesh              m_Ground;
    asdx::QuadRenderer      m_QuadRenderer;

    ID3D11Buffer*           m_pVSB;
    ID3D11VertexShader*     m_GeomertyVS;
    ID3D11PixelShader*      m_GeometryPS;

    ID3D11Buffer*           m_pRB;
    ID3D11VertexShader*     m_pReflectionVS;
    ID3D11PixelShader*      m_pReflectionPS;

protected:
    //==================================================================================
    // protected varaibles
    //==================================================================================
    /* NOTHING */




public:

};



} // namespace salty

#endif//__SALTY_APP_H__
