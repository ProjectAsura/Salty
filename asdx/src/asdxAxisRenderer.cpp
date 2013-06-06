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
    asdx::FLOAT3 position;  //!< �ʒu���W�ł�.
    asdx::FLOAT4 color;     //!< ���_�J���[�ł�.
};

////////////////////////////////////////////////////////////////////////////////////////////
// CbPerOnceVS structure
////////////////////////////////////////////////////////////////////////////////////////////
struct CbPerOnceVS
{
    asdx::MATRIX view;      //!< �r���[�s��ł�.
    asdx::MATRIX proj;      //!< �ˉe�s��ł�.
};

///////////////////////////////////////////////////////////////////////////////////////////
// CbPerObjVS structure.
///////////////////////////////////////////////////////////////////////////////////////////
struct CbPerObjVS
{
    asdx::MATRIX world;     //!< ���[���h�s��ł�.
};

//-----------------------------------------------------------------------------------------
// Constant Values
//-----------------------------------------------------------------------------------------
static const size_t InputElementCount= 2;   //!< ���͗v�f���ł�.

//!< ���͗v�f�ł�.
static const D3D11_INPUT_ELEMENT_DESC InputElements[ InputElementCount ] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "VTXCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

} // namespace /* anonymous */


namespace asdx {

//------------------------------------------------------------------------------------------
//! @brief      �����f���𐶐����܂�.
//!
//! @param [in]     pDevice     �f�o�C�X.
//! @param [in]     length      ���̒���.
//! @param [out]    ppVB        ���_�o�b�t�@.
//------------------------------------------------------------------------------------------
bool CreateAxis( ID3D11Device* pDevice, float length, ID3D11Buffer** ppVB )
{
    Vertex vertices[ 6 ];

    // X��.
    {
        vertices[ 0 ].position = asdx::FLOAT3( 0.0f, 0.0f, 0.0f );
        vertices[ 0 ].color    = asdx::FLOAT4( 1.0f, 0.0f, 0.0f, 1.0f );

        vertices[ 1 ].position = asdx::FLOAT3( length, 0.0f, 0.0f );
        vertices[ 1 ].color    = asdx::FLOAT4( 1.0f, 0.0f, 0.0f, 1.0f );
    }

    // Y��.
    {
        vertices[ 2 ].position = asdx::FLOAT3( 0.0f, 0.0f, 0.0f );
        vertices[ 2 ].color    = asdx::FLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );

        vertices[ 3 ].position = asdx::FLOAT3( 0.0f, length, 0.0f );
        vertices[ 3 ].color    = asdx::FLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );
    }

    // Z��.
    {
        vertices[ 4 ].position = asdx::FLOAT3( 0.0f, 0.0f, 0.0f );
        vertices[ 4 ].color    = asdx::FLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );

        vertices[ 5 ].position = asdx::FLOAT3( 0.0f, 0.0f, length );
        vertices[ 5 ].color    = asdx::FLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );
    }

    // ���_�o�b�t�@�̐ݒ�.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage          = D3D11_USAGE_DEFAULT;
    desc.ByteWidth      = sizeof( Vertex ) * 6;
    desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;

    // �T�u���\�[�X�̐ݒ�.
    D3D11_SUBRESOURCE_DATA res;
    ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
    res.pSysMem = vertices;

    // ���_�o�b�t�@����.
    HRESULT hr = pDevice->CreateBuffer( &desc, &res, ppVB );
    if ( FAILED( hr ) )
    {
        // �G���[���O�o��.
        ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

        // �ُ�I��.
        return false;
    }

    // ����I��.
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// AxisRenderer class
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
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
//      �f�X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
AxisRenderer::~AxisRenderer()
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------
//      �����������ł�.
//-------------------------------------------------------------------------------------------
bool AxisRenderer::Init( ID3D11Device* pDevice )
{
    // �����`�F�b�N.
    if ( pDevice == nullptr )
    {
        // �G���[���O�o��.
        ELOG( "Error : Invalid Argument." );

        // �ُ�I��.
        return false;
    }

    // ���_�o�b�t�@����.
    if ( !CreateAxis( pDevice, 5.0f, &m_pVB ) )
    {
        // �G���[���O�o��.
        ELOG( "Error : CreateAxis() Failed." );

        // �ُ�I��.
        return false;
    }

    // �r���h�ς݃V�F�[�_�R�[�h.
    #include "../res/shader/Compiled/LineShader_VSFunc.inc"
    #include "../res/shader/Compiled/LineShader_PSFunc.inc"

    HRESULT hr = S_OK;

    // ���_�V�F�[�_����.
    hr = pDevice->CreateVertexShader( LineShader_VSFunc, sizeof( LineShader_VSFunc ), nullptr, &m_pVS );
    if ( FAILED( hr ) )
    {
        // �G���[���O�o��.
        ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );

        // �ُ�I��.
        return false;
    }
    SetDebugObjectName( m_pVS, "asdx::AxisRenderer" );

    // �s�N�Z���V�F�[�_����.
    hr = pDevice->CreatePixelShader( LineShader_PSFunc, sizeof( LineShader_PSFunc ), nullptr, &m_pPS );
    if ( FAILED( hr ) )
    {
        // �G���[���O�o��
        ELOG( "Error : ID3D11Device::CreatePixelShader() Failed." );

        // �ُ�I��.
        return false;
    }
    SetDebugObjectName( m_pPS, "asdx::AxisRenderer" );

    // ���̓��C�A�E�g����.
    hr = pDevice->CreateInputLayout( InputElements, InputElementCount, LineShader_VSFunc, sizeof( LineShader_VSFunc ), &m_pIL );
    if ( FAILED( hr ) )
    {
        // �G���[���O�o��.
        ELOG( "Error : ID3D11Device::CreateInputLayout() Failed." );
        return false;
    }
    SetDebugObjectName( m_pIL, "asdx::AxisRenderer" );

    {
        // �萔�o�b�t�@0�̐ݒ�.
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.ByteWidth      = sizeof( CbPerOnceVS );
        desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;

        // �萔�o�b�t�@0�𐶐�.
        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pCB0 );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // �ُ�I��.
            return false;
        }
        SetDebugObjectName( m_pCB0, "asdx::AxisRenderer" );

        // �萔�o�b�t�@1�̐ݒ�.
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.ByteWidth      = sizeof( CbPerObjVS );
        desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;

        // �萔�o�b�t�@1�̐���.
        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pCB1 );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // �ُ�I��.
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

    // ����I��.
    return true;
}

//--------------------------------------------------------------------------------------------
//          �I�������ł�.
//--------------------------------------------------------------------------------------------
void AxisRenderer::Term()
{
    // ���_�o�b�t�@���.
    if ( m_pVB )
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    // �萔�o�b�t�@0���.
    if ( m_pCB0 )
    {
        m_pCB0->Release();
        m_pCB0 = nullptr;
    }

    // �萔�o�b�t�@1���.
    if ( m_pCB1 )
    {
        m_pCB1->Release();
        m_pCB1 = nullptr;
    }

    // ���_�V�F�[�_���.
    if ( m_pVS )
    {
        m_pVS->Release();
        m_pVS = nullptr;
    }

    // �s�N�Z���V�F�[�_���.
    if ( m_pPS )
    {
        m_pPS->Release();
        m_pPS = nullptr;
    }

    // ���̓��C�A�E�g���.
    if ( m_pIL )
    {
        m_pIL->Release();
        m_pIL = nullptr;
    }

    // �[�x�X�e���V���X�e�[�g�����.
    if ( m_pDSS )
    {
        m_pDSS->Release();
        m_pDSS = nullptr;
    }
}

//-------------------------------------------------------------------------------------------
//          �`��J�n�����ł�.
//-------------------------------------------------------------------------------------------
void AxisRenderer::Begin
(
    ID3D11DeviceContext* pDeviceContext,
    const asdx::MATRIX& view,
    const asdx::MATRIX& proj
)
{
    // �V�F�[�_��ݒ�.    
    pDeviceContext->VSSetShader( m_pVS, nullptr, 0 );
    pDeviceContext->PSSetShader( m_pPS, nullptr, 0 );
    pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->DSSetShader( nullptr, nullptr, 0 );

    // �ϊ��p�����[�^��ݒ�.
    CbPerOnceVS param;
    param.view = view;
    param.proj = proj;

    // �萔�o�b�t�@(�X���b�g0)���X�V.
    pDeviceContext->UpdateSubresource( m_pCB0, 0, nullptr, &param, 0, 0 );

    // �萔�o�b�t�@(�X���b�g0)��ݒ�.
    pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pCB0 );

    // �[�x�X�e���V���X�e�[�g��ݒ�.
    //pDeviceContext->OMSetDepthStencilState( m_pDSS, 0 );

    UINT stride = sizeof( Vertex );
    UINT offset = 0;

    // ���̓��C�A�E�g�ݒ�.
    pDeviceContext->IASetInputLayout( m_pIL );

    // ���_�o�b�t�@�ݒ�.
    pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, &stride, &offset  );

    // �v���~�e�B�u�g�|���W�[�ݒ�.
    pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
}

//-------------------------------------------------------------------------------------------
//          ���`�揈���ł�.
//-------------------------------------------------------------------------------------------
void AxisRenderer::Render( ID3D11DeviceContext* pDeviceContext, const asdx::FLOAT3 position, const float scale )
{
    // ���s�ړ��s��쐬.
    asdx::MATRIX translateMtx = asdx::CreateTranslation( position );

    // �g��s��쐬.
    asdx::MATRIX scaleMtx     = asdx::CreateScale( scale );

    // �ϊ��p�����[�^�ݒ�.
    CbPerObjVS param;
    param.world = scaleMtx * translateMtx;

    // �萔�o�b�t�@(�X���b�g1)���X�V.
    pDeviceContext->UpdateSubresource( m_pCB1, 0, nullptr, &param, 0, 0 );

    // �萔�o�b�t�@(�X���b�g1)��ݒ�.
    pDeviceContext->VSSetConstantBuffers( 1, 1, &m_pCB1 );

    // �`��L�b�N.
    pDeviceContext->Draw( 6, 0 );
}

//-------------------------------------------------------------------------------------------
//          �`��I�������ł�.
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