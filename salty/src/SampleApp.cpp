//---------------------------------------------------------------------------------------
// File : SampleApp.cpp
// Desc : Sample Application
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------
#include "SampleApp.h"
#include <asdxLog.h>
#include <asdxShader.h>
#include <asdxTimer.h>



/////////////////////////////////////////////////////////////////////////////////////////
// SampleApplication class
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//---------------------------------------------------------------------------------------
SampleApplication::SampleApplication()
: Application   ( "Sample" )
, m_Font         ()
, m_pTB         ( nullptr )
, m_pVS         ( nullptr )
, m_pPS         ( nullptr )
, m_Dosei       ()
, m_Ground      ()
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//---------------------------------------------------------------------------------------
SampleApplication::~SampleApplication()
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------
//      ���������̏����ł�.
//---------------------------------------------------------------------------------------
bool SampleApplication::OnInit()
{
    if ( !m_Font.Init( m_pDevice, "../res/font/SetoMini-P.fnt", (float)m_Width, (float)m_Height ) )
    {
        ELOG( "Error : asdx::Font::Init() Failed." );
        return false;
    }

    HRESULT hr = S_OK;
    {
        ID3DBlob* pVSBlob = nullptr;

        // ���_�V�F�[�_���R���p�C��.
        hr = asdx::ShaderHelper::CompileShaderFromFile( 
            L"../res/SimpleVS.fx",
            "VSFunc",
            asdx::ShaderHelper::VS_4_0,
            &pVSBlob
        );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : CompileShaderFromFile() Failed." );
            return false;
        }

        // ���_�V�F�[�_�𐶐�.
        hr = m_pDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVS );
        if ( FAILED( hr ) )
        {
            pVSBlob->Release();
            pVSBlob = nullptr;

            ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );
            return false;
        }

        // �ǂ�������̏�����.
        {
            // ���\�[�X���b�V�������[�h���܂�.
            asdx::ResMesh resMesh;
            if ( !resMesh.LoadFromFile( "../res/dosei/dosei.msh" ) )
            {
                ELOG( "Error : Mesh Load Failed." );
                return false;
            }

            // �`�惁�b�V���̏�����.
            if ( !m_Dosei.Init( m_pDevice, resMesh, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), "../res/dosei/", "../res/dummy/" ) )
            {
                ELOG( "Error : TinyMesh::Init() Failed." );
                return false;
            }

            // ���\�[�X���b�V�������.
            resMesh.Release();
        }

        // �n�ʂ̏�����.
        {
            // ���\�[�X���b�V�������[�h���܂�.
            asdx::ResMesh resMesh;
            if ( !resMesh.LoadFromFile( "../res/ground/ground.msh" ) )
            {
                ELOG( "Error : Mesh Load Failed." );
                return false;
            }

            // �`�惁�b�V���̏�����.
            if ( !m_Ground.Init( m_pDevice, resMesh, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), "../res/ground/", "../res/dummy/" ) )
            {
                ELOG( "Error : TinyMesh::Init() Failed." );
                return false;
            }

            // ���\�[�X���b�V�������.
            resMesh.Release();
        }

        // �V�F�[�_�R�[�h�����.
        pVSBlob->Release();
        pVSBlob = nullptr;
    }

    if ( !m_AxisRenderer.Init( m_pDevice ) )
    {
        ELOG( "Error : AxisRenderer::Init() Failed." );
        return false;
    }

    {
        ID3DBlob* pPSBlob = nullptr;

        // �s�N�Z���V�F�[�_���R���p�C��.
        hr = asdx::ShaderHelper::CompileShaderFromFile(
            L"../res/SimplePS.fx",
            "PSFunc",
            asdx::ShaderHelper::PS_4_0,
            &pPSBlob 
        );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : CompileShaderFromFile() Failed." );
            return false;
        }

        // �s�N�Z���V�F�[�_�𐶐�.
        hr = m_pDevice->CreatePixelShader(
            pPSBlob->GetBufferPointer(),
            pPSBlob->GetBufferSize(),
            nullptr,
            &m_pPS
        );

        pPSBlob->Release();
        pPSBlob = nullptr;

        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11CreatePixelShader() Failed." );
            return false;
        }
    }

    {
        // �ϊ��p�����[�^�o�b�t�@�̐ݒ�.
        D3D11_BUFFER_DESC bd;
        ZeroMemory( &bd, sizeof( D3D11_BUFFER_DESC ) );
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof( TransformParam );
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;

        // �ϊ��p�����[�^�o�b�t�@�𐶐�.
        hr = m_pDevice->CreateBuffer( &bd, nullptr, &m_pTB );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
            return false;
        }

        // �A�X�y�N�g����Z�o.
        float aspectRatio = (float)m_Width/(float)m_Height;

        // �J�����̐ݒ�.
        m_Camera.GetCamera().SetPosition( asdx::FLOAT3( -100.0f, 0.0f, 0.0f ) );
        m_Camera.GetCamera().SetTarget( asdx::FLOAT3( 0.0f, 0.0f, 0.0f ) );
        m_Camera.GetCamera().SetUpward( asdx::FLOAT3( 0.0f, 1.0f, 0.0f ) );
        m_Camera.GetCamera().Preset();
        m_Camera.GetCamera().Update();

        // �ϊ��p�����[�^��ݒ�.
        m_TransformParam.Light = asdx::FLOAT4( -100.0f, 100.0f, 0.0f, 1.0f );
        m_TransformParam.Proj  = asdx::CreatePerspectiveFieldOfView( asdx::F_PIDIV4, aspectRatio, 0.1f, 100000.0f );
        m_TransformParam.View  = m_Camera.GetView();
        m_TransformParam.World.Identity();
    }

    return true;
}

//---------------------------------------------------------------------------------------
//      �I�����̏����ł�.
//---------------------------------------------------------------------------------------
void SampleApplication::OnTerm()
{
    // �t�H���g�̏I������.
    m_Font.Term();

    // ���_�V�F�[�_�����.
    if ( m_pVS )
    {
        m_pVS->Release();
        m_pVS = nullptr;
    }

    // �s�N�Z���V�F�[�_�����.
    if ( m_pPS )
    {
        m_pPS->Release();
        m_pPS = nullptr;
    }

    // ���f���̏I������.
    m_Dosei.Term();
    m_Ground.Term();

    // �������_���[�̏I������
    m_AxisRenderer.Term();

    // �ϊ��p�����[�^�o�b�t�@�����.
    if ( m_pTB )
    {
        m_pTB->Release();
        m_pTB = nullptr;
    }
}

//---------------------------------------------------------------------------------------
//      �t���[���J�ڎ��̏����ł�.
//---------------------------------------------------------------------------------------
void SampleApplication::OnFrameMove( double time, double elapsedTime )
{
    // �r���[�s����X�V.
    m_TransformParam.View = m_Camera.GetView();
}

//---------------------------------------------------------------------------------------
//      �e�L�X�g�`�掞�̏����ł�.
//---------------------------------------------------------------------------------------
void SampleApplication::OnDrawText()
{
    m_Font.Begin( m_pDeviceContext );
    {
        m_Font.DrawStringArg( 10, 10, "FPS : %.2f", GetFPS() );
    }
    m_Font.End( m_pDeviceContext );
}

//---------------------------------------------------------------------------------------
//      ���b�V���`�掞�̏����ł�.
//---------------------------------------------------------------------------------------
void SampleApplication::OnDrawMesh()
{
    // �V�F�[�_�̐ݒ�.
    m_pDeviceContext->VSSetShader( m_pVS,   nullptr, 0 );
    m_pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->DSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->PSSetShader( m_pPS,   nullptr, 0 );

    // �ϊ��p�����[�^�o�b�t�@���X�V.
    m_TransformParam.World = asdx::CreateScale( 0.25f );
    m_pDeviceContext->UpdateSubresource( m_pTB, 0, nullptr, &m_TransformParam, 0, 0 );

    // ���_�V�F�[�_�ɒ萔�o�b�t�@��ݒ�.
    m_pDeviceContext->VSSetConstantBuffers( 1, 1, &m_pTB );

    // �ǂ��������`��.
    m_Dosei.Draw( m_pDeviceContext );

    // �ϊ��p�����[�^�o�b�t�@���X�V.
    m_TransformParam.World = asdx::CreateTranslation( 0.0f, -10.0f, 0.0f );
    m_pDeviceContext->UpdateSubresource( m_pTB, 0, nullptr, &m_TransformParam, 0, 0 );

    // ���_�V�F�[�_�ɒ萔�o�b�t�@��ݒ�.
    m_pDeviceContext->VSSetConstantBuffers( 1, 1, &m_pTB );

    // �n�ʂ�`��.
    m_Ground.Draw( m_pDeviceContext );

    // �J�����̒����_����`��.
    m_AxisRenderer.Begin( m_pDeviceContext, m_Camera.GetView(), m_TransformParam.Proj );
    m_AxisRenderer.Render( m_pDeviceContext, m_Camera.GetCamera().GetTarget() );
    m_AxisRenderer.End( m_pDeviceContext );
}

//---------------------------------------------------------------------------------------
//      �`�掞�̏����ł�.
//---------------------------------------------------------------------------------------
void SampleApplication::OnFrameRender( double time, double elapsedTime )
{
    // �����_�[�^�[�Q�b�g�r���[�E�[�x�X�e���V���r���[���擾.
    ID3D11RenderTargetView* pRTV = m_RenderTarget2D.GetRTV();
    ID3D11DepthStencilView* pDSV = m_DepthStencilBuffer.GetDSV();

    // NULL�`�F�b�N.
    if ( pRTV == nullptr )
    { return; }
    if ( pDSV == nullptr )
    { return; }
    
    // �o�̓}�l�[�W���ɐݒ�.
    m_pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );

    // �N���A����.
    m_pDeviceContext->ClearRenderTargetView( pRTV, m_ClearColor );
    m_pDeviceContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

    // �X�e�[�g��ݒ�.
    m_pDeviceContext->RSSetState( m_pRasterizerState );
    m_pDeviceContext->OMSetBlendState( m_pBlendState, m_BlendFactor, m_SampleMask );
    m_pDeviceContext->OMSetDepthStencilState( m_pDepthStencilState, m_StencilRef );

    // �`�揈��.
    {
        // ���b�V����`��.
        OnDrawMesh();

        // �e�L�X�g��`��.
        OnDrawText();
    }

    // �R�}���h�����s���āC��ʂɕ\��.
    m_pSwapChain->Present( 0, 0 );
}

//---------------------------------------------------------------------------------------
//      ���T�C�Y�C�x���g�̏����ł�.
//---------------------------------------------------------------------------------------
void SampleApplication::OnResize( const asdx::ResizeEventParam& param )
{
    // �ˉe�s����X�V.
    m_TransformParam.Proj = asdx::CreatePerspectiveFieldOfView( asdx::F_PIDIV4, param.AspectRatio, 0.1f, 1000.0f );
}

//---------------------------------------------------------------------------------------
//      �L�[�C�x���g�̏����ł�.
//---------------------------------------------------------------------------------------
void SampleApplication::OnKey( const asdx::KeyEventParam& param )
{
    // �J��������.
    m_Camera.OnKey( param.KeyCode, param.IsKeyDown, param.IsAltDown );
}

//---------------------------------------------------------------------------------------
//      �}�E�X�C���x���g���̏����ł�.
//---------------------------------------------------------------------------------------
void SampleApplication::OnMouse( const asdx::MouseEventParam& param )
{
    // �J��������.
    m_Camera.OnMouse( 
        param.X,
        param.Y,
        param.WheelDelta,
        param.IsLeftButtonDown,
        param.IsRightButtonDown,
        param.IsMiddleButtonDown,
        param.IsSideButton1Down,
        param.IsSideButton2Down
    );
}
