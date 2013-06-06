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
// ���͗v�f�ł�.
const D3D11_INPUT_ELEMENT_DESC QuadRenderer::INPUT_ELEMENTS[ QuadRenderer::NUM_INPUT_ELEMENT ] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


///////////////////////////////////////////////////////////////////////////////////////////////
// QuadRenderer class
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
QuadRenderer::QuadRenderer()
: m_pVB     ( nullptr )
, m_pIL     ( nullptr )
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
QuadRenderer::~QuadRenderer()
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      �������������s���܂�.
//---------------------------------------------------------------------------------------------
bool QuadRenderer::Init
(
    ID3D11Device*       pDevice,
    const void*         pShaderBytecode,
    const asdx::UINT    byteCodeLength
)
{
    assert( pDevice != nullptr );
    assert( pShaderBytecode != nullptr );
    assert( byteCodeLength != 0 );

    HRESULT hr = S_OK;

    // ���̓��C�A�E�g�𐶐�.
    hr = pDevice->CreateInputLayout( 
        INPUT_ELEMENTS,
        NUM_INPUT_ELEMENT,
        pShaderBytecode,
        byteCodeLength,
        &m_pIL );

    // �����`�F�b�N.
    if ( FAILED( hr ) )
    {
        // �G���[���O�o��.
        ELOG( "Error : ID3D11Device::CreateInputLayout() Failed." );

        // �ُ�I��.
        return false;
    }

    // ���_�f�[�^�̐ݒ�.
    QuadRenderer::Vertex vertices[] = {
        { asdx::FLOAT3( +1.0f,  +1.0f,  0.0f ), asdx::FLOAT2( 1.0f, 0.0f ) },
        { asdx::FLOAT3( +1.0f,  -1.0f,  0.0f ), asdx::FLOAT2( 1.0f, 1.0f ) },
        { asdx::FLOAT3( -1.0f,  -1.0f,  0.0f ), asdx::FLOAT2( 0.0f, 1.0f ) },

        { asdx::FLOAT3( +1.0f,  +1.0f,  0.0f ), asdx::FLOAT2( 1.0f, 0.0f ) },
        { asdx::FLOAT3( -1.0f,  -1.0f,  0.0f ), asdx::FLOAT2( 0.0f, 1.0f ) },
        { asdx::FLOAT3( -1.0f,  +1.0f,  0.0f ), asdx::FLOAT2( 0.0f, 0.0f ) },
    };

    // ���_�o�b�t�@�̐ݒ�.
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

        // ���_�o�b�t�@�𐶐�.
        hr = pDevice->CreateBuffer( &desc, &res, &m_pVB );

        // �����`�F�b�N.
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // �ُ�I��.
            return false;
        }

        m_Stride = sizeof( QuadRenderer::Vertex );
        m_Offset = 0;
    }

    // ����I��.
    return true;
}

//----------------------------------------------------------------------------------------------
//      �I�������ł�.
//----------------------------------------------------------------------------------------------
void QuadRenderer::Term()
{
    // ���_�o�b�t�@�����.
    if ( m_pVB )
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    // ���̓��C�A�E�g�����.
    if ( m_pIL )
    {
        m_pIL->Release();
        m_pIL = nullptr;
    }
}

//----------------------------------------------------------------------------------------------
//      �`�揈���ł�.
//----------------------------------------------------------------------------------------------
void QuadRenderer::Draw( ID3D11DeviceContext* pDeviceContext )
{
    assert( pDeviceContext != nullptr );

    // ���̓��C�A�E�g��ݒ�.
    pDeviceContext->IASetInputLayout( m_pIL );

    // ���_�o�b�t�@��ݒ�.
    pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, &m_Stride, &m_Offset );

    // �v���~�e�B�u�g�|���W�[��ݒ�.
    pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // �`��L�b�N.
    pDeviceContext->Draw( 6, 0 );
}

} // namespace asdx