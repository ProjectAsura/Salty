//---------------------------------------------------------------------------------------------
// File : asdxQuadRenderer.cpp
// Desc : Asura Quad Renderer Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------
#include <cassert>
#include <asdxQuadRenderer.h>
#include <asdxLog.h>


namespace asdx {

//---------------------------------------------------------------------------------------------
// Constant values
//---------------------------------------------------------------------------------------------
// 入力要素です.
const D3D11_INPUT_ELEMENT_DESC QuadRenderer::INPUT_ELEMENTS[ QuadRenderer::NUM_INPUT_ELEMENT ] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


///////////////////////////////////////////////////////////////////////////////////////////////
// QuadRenderer class
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//      コンストラクタです.
//---------------------------------------------------------------------------------------------
QuadRenderer::QuadRenderer()
: m_pVB     ( nullptr )
, m_pIL     ( nullptr )
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      デストラクタです.
//---------------------------------------------------------------------------------------------
QuadRenderer::~QuadRenderer()
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      初期化処理を行います.
//---------------------------------------------------------------------------------------------
bool QuadRenderer::Init
(
    ID3D11Device* pDevice,
    const void*   pShaderBytecode,
    const u32     byteCodeLength
)
{
    assert( pDevice != nullptr );
    assert( pShaderBytecode != nullptr );
    assert( byteCodeLength != 0 );

    HRESULT hr = S_OK;

    // 入力レイアウトを生成.
    hr = pDevice->CreateInputLayout( 
        INPUT_ELEMENTS,
        NUM_INPUT_ELEMENT,
        pShaderBytecode,
        byteCodeLength,
        &m_pIL );

    // 生成チェック.
    if ( FAILED( hr ) )
    {
        // エラーログ出力.
        ELOG( "Error : ID3D11Device::CreateInputLayout() Failed." );

        // 異常終了.
        return false;
    }

    // 頂点データの設定.
    QuadRenderer::Vertex vertices[] = {
        { asdx::Vector3( +1.0f,  +1.0f,  0.0f ), asdx::Vector2( 1.0f, 0.0f ) },
        { asdx::Vector3( +1.0f,  -1.0f,  0.0f ), asdx::Vector2( 1.0f, 1.0f ) },
        { asdx::Vector3( -1.0f,  -1.0f,  0.0f ), asdx::Vector2( 0.0f, 1.0f ) },

        { asdx::Vector3( +1.0f,  +1.0f,  0.0f ), asdx::Vector2( 1.0f, 0.0f ) },
        { asdx::Vector3( -1.0f,  -1.0f,  0.0f ), asdx::Vector2( 0.0f, 1.0f ) },
        { asdx::Vector3( -1.0f,  +1.0f,  0.0f ), asdx::Vector2( 0.0f, 0.0f ) },
    };

    // 頂点バッファの設定.
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.ByteWidth      = sizeof( QuadRenderer::Vertex ) * 6;
        desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA res;
        ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
        res.pSysMem = vertices;

        // 頂点バッファを生成.
        hr = pDevice->CreateBuffer( &desc, &res, &m_pVB );

        // 生成チェック.
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // 異常終了.
            return false;
        }

        m_Stride = sizeof( QuadRenderer::Vertex );
        m_Offset = 0;
    }

    // 正常終了.
    return true;
}

//----------------------------------------------------------------------------------------------
//      終了処理です.
//----------------------------------------------------------------------------------------------
void QuadRenderer::Term()
{
    // 頂点バッファを解放.
    if ( m_pVB )
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    // 入力レイアウトを解放.
    if ( m_pIL )
    {
        m_pIL->Release();
        m_pIL = nullptr;
    }
}

//----------------------------------------------------------------------------------------------
//      描画処理です.
//----------------------------------------------------------------------------------------------
void QuadRenderer::Draw( ID3D11DeviceContext* pDeviceContext )
{
    assert( pDeviceContext != nullptr );

    // 入力レイアウトを設定.
    pDeviceContext->IASetInputLayout( m_pIL );

    // 頂点バッファを設定.
    pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, &m_Stride, &m_Offset );

    // プリミティブトポロジーを設定.
    pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // 描画キック.
    pDeviceContext->Draw( 6, 0 );
}

} // namespace asdx