//------------------------------------------------------------------------------------------
// File : asdxShader.h
// Desc : Shader Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

#ifndef __ASDX_SHADER_H__
#define __ASDX_SHADER_H__

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <d3d11.h>
#include <d3dcompiler.h>


namespace asdx {

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderHelper class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderHelper
{
    //======================================================================================
    // list of friend classes and methods.
    //======================================================================================
    /* NOTHING */

private:
    //======================================================================================
    // private variables.
    //======================================================================================
    /* NOTHING */

    //======================================================================================
    // private methods.
    //======================================================================================
    /* NOTHING */

protected:
    //======================================================================================
    // protected variables.
    //======================================================================================
    /* NOTHING */

    //======================================================================================
    // protected methods.
    //======================================================================================
    /* NOTHING */

public:
    //======================================================================================
    // public variables.
    //======================================================================================
    enum SHADER_MODEL_TYPE
    {
        // ShaderModel 1.1
        VS_1_1 = 0,     //!< Vertex Shader 1.1
        TX_1_0,         //!< Texture shader profile that legacy D3DX9 functions D3DXCreateTextureShader and D3DXFillTextureTX use

        // ShaderModel 2.0
        VS_2_0,         //!< Vertex Shader 2.0
        VS_2_A,         //!< Vertex Shader 2.a
        VS_2_SW,        //!< Vertex Shader 2.0 (SoftWare)
        PS_2_0,         //!< Pixel Shader 2.0
        PS_2_A,         //!< Pixel Shader 2.a
        PS_2_B,         //!< Pixel Shader 2.b
        PS_2_SW,        //!< Pixel Shader 2.0 (SoftWare)

        // ShaderModel 3.0
        VS_3_0,         //!< Vertex Shader 3.0
        VS_3_SW,        //!< Vertex Shader 3.0 (SoftWare)
        PS_3_0,         //!< Pixel Shader 3.0
        PS_3_SW,        //!< Pixel Shader 3.0 (SoftWare)

        // ShaderModel 4.0
        VS_4_0,         //!< Vertex Shader 4.0
        GS_4_0,         //!< Geometry Shader 4.0
        CS_4_0,         //!< Compute Shader 4.0
        PS_4_0,         //!< Pixel Shader 4.0

        // ShaderModel 4.1
        VS_4_1,         //!< Vertex Shader 4.1
        GS_4_1,         //!< Geometry Shader 4.1
        CS_4_1,         //!< Compute Shader 4.1
        PS_4_1,         //!< Pixel Shader 4.1

        // ShaderModel 5.0
        VS_5_0,         //!< Vertex Shader 5.0
        HS_5_0,         //!< Hull Shader 5.0
        DS_5_0,         //!< Domain Shader 5.0
        GS_5_0,         //!< Geometry Shader 5.0
        CS_5_0,         //!< Compute Shader 5.0
        PS_5_0,         //!< Pixel Shader 5.0

        // Legacy Effects
        FX_2_0,         //!< Effects (FX) for Direct3D 9 in D3DX9
        FX_4_0,         //!< Effects (FX) for Direct3D 10.0 in D3DX10
        FX_4_1,         //!< Effects (FX) for Direct3D 10.1 in D3DX10
        FX_5_0,         //!< Effects (FX) for Direct3D 11

        NUM_SHADER_MODEL_TYPE       //!< シェーダモデルタイプ数です
    };

    //======================================================================================
    // public methods.
    //======================================================================================

    //--------------------------------------------------------------------------------------
    //! @breif      ファイルからシェーダをコンパイルします.
    //!
    //! @param [in]     szFileName          シェーダファイル名.
    //! @param [in]     szEntryPoint        エントリーポイント名.
    //! @param [in]     eShaderModel        シェーダモデル.
    //! @param [out]    ppBlobOut           コンパイルされたコードの格納先.
    //! @return     コンパイルに成功したら，S_OKを返却します.
    //--------------------------------------------------------------------------------------
    static HRESULT CompileShaderFromFile(
        LPCWSTR             szFileName,
        LPCSTR              szEntryPoint,
        SHADER_MODEL_TYPE   eShaderModel,
        ID3DBlob**          ppBlobOut 
    );

    //--------------------------------------------------------------------------------------
    //! @brief      メモリからシェーダをコンパイルします.
    //!
    //! @param [in]     pSrcData            シェーダコード.
    //! @param [in]     srcDataSize         シェーダコードのサイズ.
    //! @param [in]     szEntryPoint        エントリーポイント名.
    //! @param [in]     eShaderModel        シェーダモデル.
    //! @param [out]    ppBlobOut           コンパイルされたコードの格納先.
    //! @return     コンパイルに成功したら, S_OKを返却します.
    //--------------------------------------------------------------------------------------
    static HRESULT CompileShaderFromMemory(
        LPCVOID             pSrcData,
        SIZE_T              srcDataSize,
        LPCSTR              szEntryPoint,
        SHADER_MODEL_TYPE   eShaderModel,
        ID3DBlob**          ppBlobOut 
    );
};


} // namespace asdx


#endif//__ASDX_SHADER_H__
