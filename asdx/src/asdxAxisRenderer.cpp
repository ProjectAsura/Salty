//--------------------------------------------------------------------------------------------
// File : asdxCameraRenderer.cpp
// Desc : Camera Renderer Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------
#include <asdxAxisRenderer.h>
#include <asdxLog.h>
#include <asdxUtil.h>


namespace /* anonymous */ {

/////////////////////////////////////////////////////////////////////////////////////////////
// Vertex structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    asdx::FLOAT3 position;  //!< 位置座標です.
    asdx::FLOAT4 color;     //!< 頂点カラーです.
};

////////////////////////////////////////////////////////////////////////////////////////////
// CbPerOnceVS structure
////////////////////////////////////////////////////////////////////////////////////////////
struct CbPerOnceVS
{
    asdx::MATRIX view;      //!< ビュー行列です.
    asdx::MATRIX proj;      //!< 射影行列です.
};

///////////////////////////////////////////////////////////////////////////////////////////
// CbPerObjVS structure.
///////////////////////////////////////////////////////////////////////////////////////////
struct CbPerObjVS
{
    asdx::MATRIX world;     //!< ワールド行列です.
};

//-----------------------------------------------------------------------------------------
// Constant Values
//-----------------------------------------------------------------------------------------
static const size_t InputElementCount= 2;   //!< 入力要素数です.

//!< 入力要素です.
static const D3D11_INPUT_ELEMENT_DESC InputElements[ InputElementCount ] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "VTXCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

} // namespace /* anonymous */


namespace asdx {

//------------------------------------------------------------------------------------------
//! @brief      軸モデルを生成します.
//!
//! @param [in]     pDevice     デバイス.
//! @param [in]     length      軸の長さ.
//! @param [out]    ppVB        頂点バッファ.
//------------------------------------------------------------------------------------------
bool CreateAxis( ID3D11Device* pDevice, float length, ID3D11Buffer** ppVB )
{
    Vertex vertices[ 6 ];

    // X軸.
    {
        vertices[ 0 ].position = asdx::FLOAT3( 0.0f, 0.0f, 0.0f );
        vertices[ 0 ].color    = asdx::FLOAT4( 1.0f, 0.0f, 0.0f, 1.0f );

        vertices[ 1 ].position = asdx::FLOAT3( length, 0.0f, 0.0f );
        vertices[ 1 ].color    = asdx::FLOAT4( 1.0f, 0.0f, 0.0f, 1.0f );
    }

    // Y軸.
    {
        vertices[ 2 ].position = asdx::FLOAT3( 0.0f, 0.0f, 0.0f );
        vertices[ 2 ].color    = asdx::FLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );

        vertices[ 3 ].position = asdx::FLOAT3( 0.0f, length, 0.0f );
        vertices[ 3 ].color    = asdx::FLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );
    }

    // Z軸.
    {
        vertices[ 4 ].position = asdx::FLOAT3( 0.0f, 0.0f, 0.0f );
        vertices[ 4 ].color    = asdx::FLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );

        vertices[ 5 ].position = asdx::FLOAT3( 0.0f, 0.0f, length );
        vertices[ 5 ].color    = asdx::FLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );
    }

    // 頂点バッファの設定.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage          = D3D11_USAGE_DEFAULT;
    desc.ByteWidth      = sizeof( Vertex ) * 6;
    desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;

    // サブリソースの設定.
    D3D11_SUBRESOURCE_DATA res;
    ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
    res.pSysMem = vertices;

    // 頂点バッファ生成.
    HRESULT hr = pDevice->CreateBuffer( &desc, &res, ppVB );
    if ( FAILED( hr ) )
    {
        // エラーログ出力.
        ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

        // 異常終了.
        return false;
    }

    // 正常終了.
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// AxisRenderer class
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------------------
AxisRenderer::AxisRenderer()
: m_pVB     ( nullptr )
, m_pCB0    ( nullptr )
, m_pCB1    ( nullptr )
, m_pVS     ( nullptr )
, m_pPS     ( nullptr )
, m_pIL     ( nullptr )
, m_pDSS    ( nullptr )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------
AxisRenderer::~AxisRenderer()
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------
//      初期化処理です.
//-------------------------------------------------------------------------------------------
bool AxisRenderer::Init( ID3D11Device* pDevice )
{
    // 引数チェック.
    if ( pDevice == nullptr )
    {
        // エラーログ出力.
        ELOG( "Error : Invalid Argument." );

        // 異常終了.
        return false;
    }

    // 頂点バッファ生成.
    if ( !CreateAxis( pDevice, 5.0f, &m_pVB ) )
    {
        // エラーログ出力.
        ELOG( "Error : CreateAxis() Failed." );

        // 異常終了.
        return false;
    }

    // ビルド済みシェーダコード.
    #include "../res/shader/Compiled/LineShader_VSFunc.inc"
    #include "../res/shader/Compiled/LineShader_PSFunc.inc"

    HRESULT hr = S_OK;

    // 頂点シェーダ生成.
    hr = pDevice->CreateVertexShader( LineShader_VSFunc, sizeof( LineShader_VSFunc ), nullptr, &m_pVS );
    if ( FAILED( hr ) )
    {
        // エラーログ出力.
        ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );

        // 異常終了.
        return false;
    }
    SetDebugObjectName( m_pVS, "asdx::AxisRenderer" );

    // ピクセルシェーダ生成.
    hr = pDevice->CreatePixelShader( LineShader_PSFunc, sizeof( LineShader_PSFunc ), nullptr, &m_pPS );
    if ( FAILED( hr ) )
    {
        // エラーログ出力
        ELOG( "Error : ID3D11Device::CreatePixelShader() Failed." );

        // 異常終了.
        return false;
    }
    SetDebugObjectName( m_pPS, "asdx::AxisRenderer" );

    // 入力レイアウト生成.
    hr = pDevice->CreateInputLayout( InputElements, InputElementCount, LineShader_VSFunc, sizeof( LineShader_VSFunc ), &m_pIL );
    if ( FAILED( hr ) )
    {
        // エラーログ出力.
        ELOG( "Error : ID3D11Device::CreateInputLayout() Failed." );
        return false;
    }
    SetDebugObjectName( m_pIL, "asdx::AxisRenderer" );

    {
        // 定数バッファ0の設定.
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.ByteWidth      = sizeof( CbPerOnceVS );
        desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;

        // 定数バッファ0を生成.
        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pCB0 );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // 異常終了.
            return false;
        }
        SetDebugObjectName( m_pCB0, "asdx::AxisRenderer" );

        // 定数バッファ1の設定.
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.ByteWidth      = sizeof( CbPerObjVS );
        desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;

        // 定数バッファ1の生成.
        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pCB1 );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // 異常終了.
            return false;
        }
        SetDebugObjectName( m_pCB1, "asdx::AxisRenderer" );
    }


    {
        D3D11_DEPTH_STENCIL_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
        desc.DepthEnable    = FALSE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc      = D3D11_COMPARISON_NEVER;
        desc.StencilEnable  = FALSE;

        hr = pDevice->CreateDepthStencilState( &desc, &m_pDSS );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateDepthStencilState() Failed." );
            return false;
        }
        SetDebugObjectName( m_pDSS, "asdx::AxisRenderer" );
    }

    // 正常終了.
    return true;
}

//--------------------------------------------------------------------------------------------
//          終了処理です.
//--------------------------------------------------------------------------------------------
void AxisRenderer::Term()
{
    // 頂点バッファ解放.
    if ( m_pVB )
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    // 定数バッファ0解放.
    if ( m_pCB0 )
    {
        m_pCB0->Release();
        m_pCB0 = nullptr;
    }

    // 定数バッファ1解放.
    if ( m_pCB1 )
    {
        m_pCB1->Release();
        m_pCB1 = nullptr;
    }

    // 頂点シェーダ解放.
    if ( m_pVS )
    {
        m_pVS->Release();
        m_pVS = nullptr;
    }

    // ピクセルシェーダ解放.
    if ( m_pPS )
    {
        m_pPS->Release();
        m_pPS = nullptr;
    }

    // 入力レイアウト解放.
    if ( m_pIL )
    {
        m_pIL->Release();
        m_pIL = nullptr;
    }

    // 深度ステンシルステートを解放.
    if ( m_pDSS )
    {
        m_pDSS->Release();
        m_pDSS = nullptr;
    }
}

//-------------------------------------------------------------------------------------------
//          描画開始処理です.
//-------------------------------------------------------------------------------------------
void AxisRenderer::Begin
(
    ID3D11DeviceContext* pDeviceContext,
    const asdx::MATRIX& view,
    const asdx::MATRIX& proj
)
{
    // シェーダを設定.    
    pDeviceContext->VSSetShader( m_pVS, nullptr, 0 );
    pDeviceContext->PSSetShader( m_pPS, nullptr, 0 );
    pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->DSSetShader( nullptr, nullptr, 0 );

    // 変換パラメータを設定.
    CbPerOnceVS param;
    param.view = view;
    param.proj = proj;

    // 定数バッファ(スロット0)を更新.
    pDeviceContext->UpdateSubresource( m_pCB0, 0, nullptr, &param, 0, 0 );

    // 定数バッファ(スロット0)を設定.
    pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pCB0 );

    // 深度ステンシルステートを設定.
    //pDeviceContext->OMSetDepthStencilState( m_pDSS, 0 );

    UINT stride = sizeof( Vertex );
    UINT offset = 0;

    // 入力レイアウト設定.
    pDeviceContext->IASetInputLayout( m_pIL );

    // 頂点バッファ設定.
    pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, &stride, &offset  );

    // プリミティブトポロジー設定.
    pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
}

//-------------------------------------------------------------------------------------------
//          軸描画処理です.
//-------------------------------------------------------------------------------------------
void AxisRenderer::Render( ID3D11DeviceContext* pDeviceContext, const asdx::FLOAT3 position, const float scale )
{
    // 平行移動行列作成.
    asdx::MATRIX translateMtx = asdx::CreateTranslation( position );

    // 拡大行列作成.
    asdx::MATRIX scaleMtx     = asdx::CreateScale( scale );

    // 変換パラメータ設定.
    CbPerObjVS param;
    param.world = scaleMtx * translateMtx;

    // 定数バッファ(スロット1)を更新.
    pDeviceContext->UpdateSubresource( m_pCB1, 0, nullptr, &param, 0, 0 );

    // 定数バッファ(スロット1)を設定.
    pDeviceContext->VSSetConstantBuffers( 1, 1, &m_pCB1 );

    // 描画キック.
    pDeviceContext->Draw( 6, 0 );
}

//-------------------------------------------------------------------------------------------
//          描画終了処理です.
//-------------------------------------------------------------------------------------------
void AxisRenderer::End( ID3D11DeviceContext* pDeviceContext )
{
    pDeviceContext->VSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->PSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->DSSetShader( nullptr, nullptr, 0 );

    pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

} // namespace asdx