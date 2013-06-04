//------------------------------------------------------------------------------------------
// File : asdxShader.cpp
// Desc : Shader Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <asdxShader.h>


namespace asdx {

static LPCSTR GetShaderModelString( const ShaderHelper::SHADER_MODEL_TYPE type )
{
    switch( type )
    {
    /*********************************/
    /*    ShaderModel 1.0            */
    /*********************************/
    case ShaderHelper::VS_1_1:
        { return "vs_1_1"; }
        break;

    case ShaderHelper::TX_1_0:
        { return "tx_1_0"; }
        break;

    /*********************************/
    /*    ShaderModel 2.0            */
    /*********************************/
    case ShaderHelper::VS_2_0:
        { return "vs_2_0"; }
        break;

    case ShaderHelper::VS_2_A:
        { return "vs_2_a"; }
        break;

    case ShaderHelper::VS_2_SW:
        { return "vs_2_sw"; }
        break;

    case ShaderHelper::PS_2_0:
        { return "ps_2_0"; }
        break;

    case ShaderHelper::PS_2_A:
        { return "ps_2_a"; }
        break;

    case ShaderHelper::PS_2_B:
        { return "ps_2_b"; }
        break;

    case ShaderHelper::PS_2_SW:
        { return "ps_2_sw"; }
        break;

    /*********************************/
    /*    ShaderModel 1.0            */
    /*********************************/
    case ShaderHelper::VS_3_0:
        { return "vs_3_0"; }
        break;

    case ShaderHelper::VS_3_SW:
        { return "vs_3_sw"; }
        break;

    case ShaderHelper::PS_3_0:
        { return "ps_3_0"; }
        break;

    case ShaderHelper::PS_3_SW:
        { return "ps_3_sw"; }
        break;

    /*********************************/
    /*    ShaderModel 1.0            */
    /*********************************/
    case ShaderHelper::VS_4_0:
        { return "vs_4_0"; }
        break;

    case ShaderHelper::GS_4_0:
        { return "gs_4_0"; }
        break;

    case ShaderHelper::CS_4_0:
        { return "cs_4_0"; }
        break;

    case ShaderHelper::PS_4_0:
        { return "ps_4_0"; }
        break;

    /*********************************/
    /*    ShaderModel 1.0            */
    /*********************************/
    case ShaderHelper::VS_5_0:
        { return "vs_5_0"; }
        break;

    case ShaderHelper::HS_5_0:
        { return "hs_5_0"; }
        break;

    case ShaderHelper::DS_5_0:
        { return "ds_5_0"; }
        break;

    case ShaderHelper::GS_5_0:
        { return "gs_5_0"; }
        break;

    case ShaderHelper::CS_5_0:
        { return "cs_5_0"; }
        break;

    case ShaderHelper::PS_5_0:
        { return "ps_5_0"; }
        break;

    /*********************************/
    /*    Legacy Effects             */
    /*********************************/
    case ShaderHelper::FX_2_0:
        { return "fx_2_0"; }
        break;

    case ShaderHelper::FX_4_0:
        { return "fx_4_0"; }
        break;

    case ShaderHelper::FX_4_1:
        { return "fx_4_1"; }
        break;

    case ShaderHelper::FX_5_0:
        { return "fx_5_0"; }
        break;
    };

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////
// ShaderHelper class
///////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------
//      ファイルからシェーダをコンパイルします.
//-----------------------------------------------------------------------------------------
HRESULT ShaderHelper::CompileShaderFromFile
(
    LPCWSTR                 szFileName,
    LPCSTR                  szEntryPoint,
    SHADER_MODEL_TYPE       eShaderModel,
    ID3DBlob**              ppBlobOut
)
{
    // リターンコードを初期化.
    HRESULT hr = S_OK;

    // コンパイルフラグ.
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif//defiend(DEBUG) || defined(_DEBUG)

#if defined(NDEBUG) || defined(_NDEBUG)
    dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif//defined(NDEBUG) || defined(_NDEBUG)

    ID3DBlob* pErrorBlob = nullptr;

    // ファイルからシェーダをコンパイル.
    hr = D3DCompileFromFile(
        szFileName,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        szEntryPoint,
        GetShaderModelString( eShaderModel ),
        dwShaderFlags,
        0,
        ppBlobOut,
        &pErrorBlob 
    );

    // エラーチェック.
    if ( FAILED( hr ) )
    {
        // エラーメッセージを出力.
        if ( pErrorBlob )
        { OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() ); }
    }

    // 解放処理.
    if ( pErrorBlob )
    {
        pErrorBlob->Release();
        pErrorBlob = nullptr;
    }

    // リターンコードを返却.
    return hr;
}

//-----------------------------------------------------------------------------------------
//      メモリからシェーダをコンパイルします.
//-----------------------------------------------------------------------------------------
HRESULT ShaderHelper::CompileShaderFromMemory
(
    LPCVOID             pSrcData,
    SIZE_T              srcDataSize,
    LPCSTR              szEntryPoint,
    SHADER_MODEL_TYPE   eShaderModel,
    ID3DBlob**          ppBlobOUt
)
{
    // リターンコードを初期化.
    HRESULT hr = S_OK;

    // コンパイルフラグ.
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif//defiend(DEBUG) || defined(_DEBUG)

#if defined(NDEBUG) || defined(_NDEBUG)
    dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif//defined(NDEBUG) || defined(_NDEBUG)

    ID3DBlob* pErrorBlob = nullptr;

    hr = D3DCompile( 
        pSrcData,
        srcDataSize,
        nullptr,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        szEntryPoint,
        GetShaderModelString( eShaderModel ),
        dwShaderFlags,
        0,
        ppBlobOUt,
        &pErrorBlob 
    );

    // エラーチェック.
    if ( FAILED( hr ) )
    {
        // エラーメッセージを出力.
        if ( pErrorBlob )
        { OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() ); }
    }

    // 解放処理.
    if ( pErrorBlob )
    {
        pErrorBlob->Release();
        pErrorBlob = nullptr;
    }

    // リターンコードを返却.
    return hr;
}

} // namespace asdx
