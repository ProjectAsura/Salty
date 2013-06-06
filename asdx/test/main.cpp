//---------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : Test Application.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------------

#define _CRTDBG_MAP_ALLOC

//---------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------
#include <crtdbg.h>
#include <asdxApp.h>
#include <asdxLog.h>
#include <asdxFont.h>
#include <asdxCameraUpdater.h>
#include <asdxConstantBuffer.h>
#include <asdxAxisRenderer.h>


///////////////////////////////////////////////////////////////////////////////////////////////
// CBPerOnce structure
///////////////////////////////////////////////////////////////////////////////////////////////
struct CBPerOnce
{
    asdx::MATRIX    World;      //!< ���[���h�s��ł�.
    asdx::MATRIX    View;       //!< �r���[�s��ł�.
    asdx::MATRIX    Proj;       //!< �ˉe�s��ł�.
    asdx::FLOAT4    Light;      //!< ���C�g�x�N�g���ł�.
};


///////////////////////////////////////////////////////////////////////////////////////////////
// TestApplication class
///////////////////////////////////////////////////////////////////////////////////////////////
class TestApplication : public asdx::Application
{
    //=========================================================================================
    // list of friend classes and methods.
    //=========================================================================================
    /* NOTHING */

private:
    //=========================================================================================
    // private variables.
    //=========================================================================================
    asdx::Font              m_Font;             //!< �t�H���g�ł�.
    asdx::CameraUpdater     m_Camera;           //!< �J�����ł�.
    asdx::ConstantBuffer    m_CB;               //!< �萔�o�b�t�@�ł�.
    asdx::AxisRenderer      m_AxisRenderer;     //!< �������_���[�ł�.
    ID3D11VertexShader*     m_pVS;              //!< ���_�V�F�[�_�ł�.
    ID3D11PixelShader*      m_pPS;              //!< �s�N�Z���V�F�[�_�ł�.
    CBPerOnce               m_Param;            //!< �萔�o�b�t�@�̃p�����[�^�ł�.

    //=========================================================================================
    // private methods.
    //=========================================================================================
    void OnDrawText();
    void OnDrawMesh();

protected:
    //=========================================================================================
    // protected variables
    //=========================================================================================
    /* NOTHING */

    //=========================================================================================
    // protected methods
    //=========================================================================================
    bool OnInit();
    void OnTerm();
    void OnFrameMove  ( double time, double elapsedTime );
    void OnFrameRender( double time, double elapsedTime );
    void OnResize( const asdx::ResizeEventParam& param );
    void OnKey   ( const asdx::KeyEventParam& param );
    void OnMouse ( const asdx::MouseEventParam& param );

public:
    //=========================================================================================
    // public variables
    //=========================================================================================
    /* NOTHING */

    //=========================================================================================
    // public methods
    //=========================================================================================
    TestApplication();
    virtual ~TestApplication();
};

//---------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
TestApplication::TestApplication()
: Application   ( "Test" )
, m_Font        ()
, m_Camera      ()
, m_AxisRenderer()
, m_pVS         ( nullptr )
, m_pPS         ( nullptr )
, m_Param       ()
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
TestApplication::~TestApplication()
{
    assert( m_pVS == nullptr );
    assert( m_pPS == nullptr );
}

//---------------------------------------------------------------------------------------------
//      ���������̏����ł�.
//---------------------------------------------------------------------------------------------
bool TestApplication::OnInit()
{
    // �t�H���g������.
    if ( !m_Font.Init( m_pDevice, "../res/font/SetoMini-P.fnt", (float)m_Width, (float)m_Height ) )
    {
        ELOG( "Error : asdx::Font::Init() Failed." );
        return false;
    }

    // �������_���[������.
    if ( !m_AxisRenderer.Init( m_pDevice ) )
    {
        ELOG( "Error : AxisRenderer::Init() Failed." );
        return false;
    }

    {
        // �A�X�y�N�g����Z�o.
        float aspectRatio = (float)m_Width/(float)m_Height;

        // �J�����̐ݒ�.
        m_Camera.GetCamera().SetPosition( asdx::FLOAT3( 0.0f, 0.0f, -50.0f ) );
        m_Camera.GetCamera().SetTarget( asdx::FLOAT3( 0.0f, 0.0f, 0.0f ) );
        m_Camera.GetCamera().SetUpward( asdx::FLOAT3( 0.0f, 1.0f, 0.0f ) );
        m_Camera.GetCamera().Preset();
        m_Camera.GetCamera().Update();

        // �ϊ��p�����[�^��ݒ�.
        m_Param.Light = asdx::FLOAT4( -100.0f, 100.0f, 0.0f, 1.0f );
        m_Param.Proj  = asdx::CreatePerspectiveFieldOfView( asdx::F_PIDIV4, aspectRatio, 0.1f, 100000.0f );
        m_Param.View  = m_Camera.GetView();
        m_Param.World.Identity();
    }

    return true;
}

//--------------------------------------------------------------------------------------------
//      �I�����̏����ł�.
//--------------------------------------------------------------------------------------------
void TestApplication::OnTerm()
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

    // �������_���[�̏I������
    m_AxisRenderer.Term();

    // �ϊ��p�����[�^�o�b�t�@�����.
    m_CB.Release();
} 

//--------------------------------------------------------------------------------------------
//      �t���[���J�ڎ��̏����ł�.
//--------------------------------------------------------------------------------------------
void TestApplication::OnFrameMove( double time, double elapsedTime )
{
    // �r���[�s����X�V.
    m_Param.View = m_Camera.GetView();
}

//--------------------------------------------------------------------------------------------
//      �e�L�X�g�`�掞�̏����ł�.
//--------------------------------------------------------------------------------------------
void TestApplication::OnDrawText()
{
    m_Font.Begin( m_pDeviceContext );
    {
        m_Font.DrawStringArg( 10, 10, "FPS : %.2f", GetFPS() );
    }
    m_Font.End( m_pDeviceContext );
}

//--------------------------------------------------------------------------------------------
//      ���b�V���`�掞�̏����ł�.
//--------------------------------------------------------------------------------------------
void TestApplication::OnDrawMesh()
{

}

//--------------------------------------------------------------------------------------------
//      �t���[���`�掞�̏����ł�.
//--------------------------------------------------------------------------------------------
void TestApplication::OnFrameRender( double time, double elapsedTime )
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

//--------------------------------------------------------------------------------------------
//      ���T�C�Y���̏����ł�.
//--------------------------------------------------------------------------------------------
void TestApplication::OnResize( const asdx::ResizeEventParam& param )
{
    m_Param.Proj = asdx::CreatePerspectiveFieldOfView( 
                            asdx::F_PIDIV4,
                            param.AspectRatio,
                            0.1f,
                            1000.0f );
}

//--------------------------------------------------------------------------------------------
//      �L�[�̏����ł�.
//--------------------------------------------------------------------------------------------
void TestApplication::OnKey( const asdx::KeyEventParam& param )
{
    // �J��������.
    m_Camera.OnKey( param.KeyCode, param.IsKeyDown, param.IsAltDown );
}

//--------------------------------------------------------------------------------------------
//      �}�E�X�̏����ł�.
//--------------------------------------------------------------------------------------------
void TestApplication::OnMouse( const asdx::MouseEventParam& param )
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


//---------------------------------------------------------------------------------------------
//! @brief      ���C���G���g���[�|�C���g�ł�.
//---------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    {
        TestApplication app;
        app.Run();
    }

    return 0;
}