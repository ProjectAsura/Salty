//-------------------------------------------------------------------------------------------
// File : asdxApp.cpp
// Desc : Application Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <asdxApp.h>
#include <asdxLog.h>
#include <asdxUtil.h>
#include <list>



namespace /* anonymous */{

/////////////////////////////////////////////////////////////////////////////////////////////
// ApplicationList class
/////////////////////////////////////////////////////////////////////////////////////////////
class ApplicationList
{
    //=======================================================================================
    // list of friend classes and methods.
    //=======================================================================================
    /* NOTHING */

private:
    //=======================================================================================
    // private variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // private methods.
    //=======================================================================================
    /* NOTHING */

protected:
    //=======================================================================================
    // protected variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // protected methods.
    //=======================================================================================
    /* NOTHING */

public:
    //=======================================================================================
    // public variables.
    //=======================================================================================
    typedef std::list< asdx::Application* >                 List;
    typedef std::list< asdx::Application* >::iterator       ListItr;
    typedef std::list< asdx::Application* >::const_iterator ListCItr;
    List    m_List;         //!< ���X�g�ł�.

    //=======================================================================================
    // public methods.
    //=======================================================================================

    //---------------------------------------------------------------------------------------
    //! @brief      �R���X�g���N�^�ł�.
    //---------------------------------------------------------------------------------------
    ApplicationList()
    { m_List.clear(); }

    //---------------------------------------------------------------------------------------
    //! @brief      �f�X�g���N�^�ł�.
    //---------------------------------------------------------------------------------------
    ~ApplicationList()
    { m_List.clear(); }

    //---------------------------------------------------------------------------------------
    //! @brief      push_back()�̃��b�p�[�֐��ł�.
    //---------------------------------------------------------------------------------------
    void PushBack( asdx::Application* pApp )
    { m_List.push_back( pApp ); }

    //---------------------------------------------------------------------------------------
    //! @brief      push_front()�̃��b�p�[�֐��ł�.
    //---------------------------------------------------------------------------------------
    void PushFront( asdx::Application* pApp )
    { m_List.push_front( pApp ); }

    //---------------------------------------------------------------------------------------
    //! @brief      pop_back()�̃��b�p�[�֐��ł�.
    //---------------------------------------------------------------------------------------
    void PopBack()
    { m_List.pop_back(); }

    //---------------------------------------------------------------------------------------
    //! @brief      pop_front()�̃��b�p�[�֐��ł�.
    //---------------------------------------------------------------------------------------
    void PopFront()
    { m_List.pop_front(); }

    //---------------------------------------------------------------------------------------
    //! @brief      clear()�̃��b�p�[�֐��ł�.
    //---------------------------------------------------------------------------------------
    void Clear()
    { m_List.clear(); }

    //---------------------------------------------------------------------------------------
    //! @brief      remove()�̃��b�p�[�֐��ł�.
    //---------------------------------------------------------------------------------------
    void Remove( asdx::Application* pApp )
    { m_List.remove( pApp ); }

    //---------------------------------------------------------------------------------------
    //! @brief      begin()�̃��b�p�[�֐��ł�.
    //---------------------------------------------------------------------------------------
    ListItr Begin()
    { return m_List.begin(); }

    //---------------------------------------------------------------------------------------
    //! @brief      begin()�̃��b�p�[�֐��ł�(const��).
    //---------------------------------------------------------------------------------------
    ListCItr Begin() const
    { return m_List.begin(); }
    
    //---------------------------------------------------------------------------------------
    //! @brief      end()�̃��b�p�[�֐��ł�.
    //---------------------------------------------------------------------------------------
    ListItr End()
    { return m_List.end(); }

    //---------------------------------------------------------------------------------------
    //! @brief      end()�̃��b�p�[�֐��ł�(const��).
    //---------------------------------------------------------------------------------------
    ListCItr End() const
    { return m_List.end(); }
};


// �A�v���P�[�V�������X�g.
ApplicationList     g_AppList;

// �~���[�e�b�N�X.
CRITICAL_SECTION    g_CriticalSection;


/////////////////////////////////////////////////////////////////////////////////////////////
// Lock class
/////////////////////////////////////////////////////////////////////////////////////////////
class Lock
{
public:
    //---------------------------------------------------------------------------------------
    //! @brief      �R���X�g���N�^�ł�.
    //---------------------------------------------------------------------------------------
    inline Lock()
    { EnterCriticalSection( &g_CriticalSection ); }

    //---------------------------------------------------------------------------------------
    //! @brief      �f�X�g���N�^�ł�.
    //---------------------------------------------------------------------------------------
    inline ~Lock()
    { LeaveCriticalSection( &g_CriticalSection ); }
};


} // namespace /* anonymous */


namespace asdx  {

// �E�B���h�E�N���X���ł�.
#ifndef ASDX_WND_CLASSNAME
#define ASDX_WND_CLASSNAME      "asdxWindowClass"
#endif//ASDX_WND_CLAASNAME

///////////////////////////////////////////////////////////////////////////////////////////////
// Application class
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------    
Application::Application()
: m_hInst               ( nullptr )
, m_hWnd                ( nullptr )
, m_DriverType          ( D3D_DRIVER_TYPE_HARDWARE )
, m_FeatureLevel        ( D3D_FEATURE_LEVEL_11_0 )
, m_MultiSampleCount    ( 4 )
, m_MultiSampleQuality  ( 0 )
, m_SwapChainCount      ( 2 )
, m_SwapChainFormat     ( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB )
, m_DepthStencilFormat  ( DXGI_FORMAT_D24_UNORM_S8_UINT )
, m_pDevice             ( nullptr )
, m_pDeviceContext      ( nullptr )
, m_pSwapChain          ( nullptr )
, m_RenderTarget2D      ()
, m_DepthStencilBuffer  ()
, m_pRasterizerState    ( nullptr )
, m_pDepthStencilState  ( nullptr )
, m_pBlendState         ( nullptr )
, m_SampleMask          ( 0 )
, m_StencilRef          ( 0 )
, m_ClearColor          ( DirectX::Colors::CornflowerBlue )
, m_Width               ( 960 )
, m_Height              ( 540 )
, m_AspectRatio         ( 1.7777f )
, m_Title               ( "asdxApplication" )
, m_Timer               ()
, m_FrameCount          ( 0 )
, m_FPS                 ( 0.0f )
, m_LatestUpdateTime    ( 0.0f )
, m_IsStopRendering     ( false )
{ /* DO_NOHTING */ }

//---------------------------------------------------------------------------------------------
//      �����t���R���X�g���N�^�ł�.
//--------------------------------------------------------------------------------------------- 
Application::Application( LPSTR title )
: m_hInst               ( nullptr )
, m_hWnd                ( nullptr )
, m_DriverType          ( D3D_DRIVER_TYPE_HARDWARE )
, m_FeatureLevel        ( D3D_FEATURE_LEVEL_11_0 )
, m_MultiSampleCount    ( 4 )
, m_MultiSampleQuality  ( 0 )
, m_SwapChainCount      ( 2 )
, m_SwapChainFormat     ( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB )
, m_DepthStencilFormat  ( DXGI_FORMAT_D24_UNORM_S8_UINT )
, m_pDevice             ( nullptr )
, m_pDeviceContext      ( nullptr )
, m_pSwapChain          ( nullptr )
, m_RenderTarget2D      ()
, m_DepthStencilBuffer  ()
, m_pRasterizerState    ( nullptr )
, m_pDepthStencilState  ( nullptr )
, m_pBlendState         ( nullptr )
, m_ClearColor          ( DirectX::Colors::CornflowerBlue )
, m_Width               ( 960 )
, m_Height              ( 540 )
, m_AspectRatio         ( 1.7777f )
, m_Title               ( title )
, m_Timer               ()
, m_FrameCount          ( 0 )
, m_FPS                 ( 0.0f )
, m_LatestUpdateTime    ( 0.0f )
, m_IsStopRendering     ( false )
{ /* DO_NOHTING */ }

//---------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
Application::~Application()
{ TermApp(); }

//---------------------------------------------------------------------------------------------
//      �`���~�t���O��ݒ肵�܂�.
//---------------------------------------------------------------------------------------------
void Application::SetStopRendering( bool isStopRendering )
{
    Lock;
    m_IsStopRendering = isStopRendering;
}

//---------------------------------------------------------------------------------------------
//      �`���~�t���O���擾���܂�.
//---------------------------------------------------------------------------------------------
bool Application::IsStopRendering() const
{
    Lock;
    return m_IsStopRendering;
}

//---------------------------------------------------------------------------------------------
//      �t���[���J�E���g���擾���܂�.
//---------------------------------------------------------------------------------------------
DWORD Application::GetFrameCount() const
{
    Lock;
    return m_FrameCount;
}

//---------------------------------------------------------------------------------------------
//      FPS���擾���܂�.
//---------------------------------------------------------------------------------------------
FLOAT Application::GetFPS() const
{
    Lock;
    return m_FPS;
}

//---------------------------------------------------------------------------------------------
//      �A�v���P�[�V���������������܂�.
//---------------------------------------------------------------------------------------------
bool Application::InitApp()
{
    // �E�B���h�E�̏�����.
    if ( !InitWnd() )
    {
        DLOG( "Error : InitWnd() Failed." );
        return false;
    }

    // Direct3D�̏�����.
    if ( !InitD3D() )
    {
        DLOG( "Error : InitD3D() Failed." );
        return false;
    }

    // ����I��.
    return true;
}

//---------------------------------------------------------------------------------------------
//      �A�v���P�[�V�����̏I������.
//---------------------------------------------------------------------------------------------
void Application::TermApp()
{
    // Direct3D�̏I������.
    TermD3D();

    // �E�B���h�E�̏I������.
    TermWnd();
}

//---------------------------------------------------------------------------------------------
//      �E�B���h�E�̏���������.
//---------------------------------------------------------------------------------------------
bool Application::InitWnd()
{
    // �C���X�^���X�n���h�����擾.
    HINSTANCE hInst = GetModuleHandle( nullptr );
    if ( !hInst )
    {
        DLOG( "Error : GetModuleHandle() Failed. ");
        return false;
    }

    // �g���E�B���h�E�N���X�̐ݒ�.
    WNDCLASSEXA wc;
    wc.cbSize           = sizeof( WNDCLASSEXA );
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = MsgProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor          = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground    = (HBRUSH)( COLOR_WINDOW + 1 );
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = ASDX_WND_CLASSNAME;
    wc.hIconSm          = LoadIcon( NULL, IDI_APPLICATION );

    // �E�B���h�E�N���X��o�^���܂�.
    if ( !RegisterClassExA( &wc ) )
    {
        // �G���[���O�o��.
        DLOG( "Error : RegisterClassEx() Failed." );

        // �ُ�I��.
        return false; 
    }

    // �C���X�^���X�n���h����ݒ�.
    m_hInst = hInst;

    // ��`�̐ݒ�.
    RECT rc = { 0, 0, m_Width, m_Height };

    // �w�肳�ꂽ�N���C�A���g�̈���m�ۂ��邽�߂ɕK�v�ȃE�B���h�E���W���v�Z���܂�.
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

    // �E�B���h�E�𐶐����܂�.
    m_hWnd = CreateWindowA(
        ASDX_WND_CLASSNAME,
        m_Title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        ( rc.right - rc.left ),
        ( rc.bottom - rc.top ),
        NULL,
        NULL,
        hInst,
        NULL 
    );

    // �����`�F�b�N.
    if ( !m_hWnd )
    {
        // �G���[���O�o��.
        DLOG( "Error : CreateWindow() Failed." );

        // �ُ�I��.
        return false;
    }

    // �E�B���h�E��\�����܂�.
    ShowWindow( m_hWnd, SW_SHOWNORMAL );
    UpdateWindow( m_hWnd );

    // �t�H�[�J�X��ݒ肵�܂�.
    SetFocus( m_hWnd );

    // �A�v���P�[�V�������X�g�ɓo�^���܂�.
    g_AppList.PushBack( this );

    // �^�C�}�[���J�n���܂�.
    m_Timer.Start();

    // �J�n�������擾.
    m_LatestUpdateTime = m_Timer.GetElapsedTime();

    // ����I��.
    return true;
}

//---------------------------------------------------------------------------------------------
//      �E�B���h�E�̏I������.
//---------------------------------------------------------------------------------------------
void Application::TermWnd()
{
    // �^�C�}�[���~�߂܂�.
    m_Timer.Stop();

    // �E�B���h�E�N���X�̓o�^������.
    if ( m_hInst != nullptr )
    { UnregisterClassA( ASDX_WND_CLASSNAME, m_hInst ); }

    // �^�C�g�������N���A.
    m_Title = nullptr;

    // �n���h�����N���A.
    m_hInst = nullptr;
    m_hWnd  = nullptr;

    // �A�v���P�[�V�������X�g����폜���܂�.
    g_AppList.Remove( this );
}

//---------------------------------------------------------------------------------------------
//      Direct3D�̏���������.
//---------------------------------------------------------------------------------------------
bool Application::InitD3D()
{
    HRESULT hr = S_OK;

    // �E�B���h�E�T�C�Y���擾���܂�.
    RECT rc;
    GetClientRect( m_hWnd, &rc );
    UINT w = rc.right - rc.left;
    UINT h = rc.bottom - rc.top;

    // �擾�����T�C�Y��ݒ肵�܂�.
    m_Width       = w;
    m_Height      = h;

    // �A�X�y�N�g����Z�o���܂�.
    m_AspectRatio = (FLOAT)w / (FLOAT)h;

    // �f�o�C�X�����t���O.
    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif//defined(DEBUG) || deifned(_DEBUG)

    // �h���C�o�[�^�C�v.
    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTytpes = sizeof( driverTypes ) / sizeof( driverTypes[0] );

    // �@�\���x��.
    D3D_FEATURE_LEVEL featureLevels[] = {
        //D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = sizeof( featureLevels ) / sizeof( featureLevels[0] );

    // �X���b�v�`�F�C���̍\���ݒ�.
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof(DXGI_SWAP_CHAIN_DESC) );
    sd.BufferCount                          = m_SwapChainCount;
    sd.BufferDesc.Width                     = w;
    sd.BufferDesc.Height                    = h;
    sd.BufferDesc.Format                    = m_SwapChainFormat;
    sd.BufferDesc.RefreshRate.Numerator     = 60;
    sd.BufferDesc.RefreshRate.Denominator   = 1;
    sd.BufferUsage                          = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    sd.OutputWindow                         = m_hWnd;
    sd.SampleDesc.Count                     = m_MultiSampleCount;
    sd.SampleDesc.Quality                   = m_MultiSampleQuality;
    sd.Windowed                             = TRUE;

    for( UINT idx = 0; idx < numDriverTytpes; ++idx )
    {
        // �h���C�o�[�^�C�v�ݒ�.
        m_DriverType = driverTypes[ idx ];

        // �f�o�C�X�ƃX���b�v�`�F�C���̐���.
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr, 
            m_DriverType,
            nullptr,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &sd,
            &m_pSwapChain,
            &m_pDevice,
            &m_FeatureLevel,
            &m_pDeviceContext 
        );

        // ���������烋�[�v��E�o.
        if ( SUCCEEDED( hr ) )
        {
        #if defined(DEBUG) || defined(_DEBUG)
            ILOG( "*******************************************************************" );
            ILOG( " DriverType              : %s", GetDriverTypeStringA( m_DriverType ) );
            ILOG( " Feature Level           : %s", GetFeatureLevelStringA( m_FeatureLevel ) );
            ILOG( " SwapChain Format        : %s", GetFormatStringA( m_SwapChainFormat ) );
            ILOG( " DepthStencil Format     : %s", GetFormatStringA( m_DepthStencilFormat ) );
            UINT maxQuality = 0;
            m_pDevice->CheckMultisampleQualityLevels( m_SwapChainFormat, m_MultiSampleCount, &maxQuality );
            ILOG( " Cur MultiSample Count   : %d", m_MultiSampleCount );
            ILOG( " Cur MultiSample Quality : %d", m_MultiSampleQuality );
            ILOG( " Max MultiSample Quality : %d", maxQuality );
            ILOG( "*******************************************************************" );
        #endif
            break;
        }
    }

    // ���s���Ă��Ȃ����`�F�b�N.
    if ( FAILED( hr ) )
    {
        DLOG( "Error : D3D11CreateDeviceAndSwapChain() Failed." );
        return false;
    }

    // �����_�[�^�[�Q�b�g�𐶐�.
    if ( !m_RenderTarget2D.CreateFromBackBuffer( m_pDevice, m_pSwapChain ) )
    {
        DLOG( "Error : RenderTarget2D::CreateFromBackBuffer() Failed." );
        return false;
    }

    // �[�x�X�e���V���o�b�t�@�̍\���ݒ�.
    DepthStencilBuffer::Description desc;
    desc.Width              = w;
    desc.Height             = h;
    desc.MipLevels          = 1;
    desc.ArraySize          = 1;
    desc.Format             = m_DepthStencilFormat;
    desc.SampleDesc.Count   = m_MultiSampleCount;
    desc.SampleDesc.Quality = m_MultiSampleQuality;
    desc.CPUAccessFlags     = 0;
    desc.MiscFlags          = 0;

    // �[�x�X�e���V���o�b�t�@�𐶐�.
    if ( !m_DepthStencilBuffer.Create( m_pDevice, desc ) )
    {
        DLOG( "Error : DepthStencilBuffer::Create() Failed." );
        return false;
    }

    // �f�o�C�X�R���e�L�X�g�Ƀ����_�[�^�[�Q�b�g��ݒ�.
    ID3D11RenderTargetView* pRTV = m_RenderTarget2D.GetRTV();
    ID3D11DepthStencilView* pDSV = m_DepthStencilBuffer.GetDSV();
    m_pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );

    // �r���[�|�[�g�̐ݒ�.
    D3D11_VIEWPORT vp;
    vp.Width    = (FLOAT)w;
    vp.Height   = (FLOAT)h;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    // �f�o�C�X�R���e�L�X�g�Ƀr���[�|�[�g��ݒ�.
    m_pDeviceContext->RSSetViewports( 1, &vp );

    // ���X�^���C�U�[�X�e�[�g�̐ݒ�.
    {
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory( &rd, sizeof( D3D11_RASTERIZER_DESC ) );
        rd.FillMode              = D3D11_FILL_SOLID;
        rd.CullMode              = D3D11_CULL_BACK;
        rd.FrontCounterClockwise = FALSE;
        rd.DepthBias             = 0;
        rd.DepthBiasClamp        = 0;
        rd.DepthClipEnable       = FALSE;
        rd.SlopeScaledDepthBias  = 0;
        rd.ScissorEnable         = FALSE;
        rd.MultisampleEnable     = ( m_MultiSampleCount == 0 ) ? FALSE : TRUE;
        rd.AntialiasedLineEnable = FALSE;

        // ���X�^���C�U�[�X�e�[�g�𐶐�.
        hr = m_pDevice->CreateRasterizerState( &rd, &m_pRasterizerState );
        if ( FAILED( hr ) )
        {
            DLOG( "Error : ID3D11Device::CreateRasterizerState() Failed." );
            return false;
        }
        SetDebugObjectName( m_pRasterizerState, "asdxApp" );

        // �f�o�C�X�R���e�L�X�g�Ƀ��X�^���C�U�[�X�e�[�g��ݒ�.
        m_pDeviceContext->RSSetState( m_pRasterizerState );
    }

    // �[�x�X�e���V���X�e�[�g���擾.
    {
        m_pDeviceContext->OMGetDepthStencilState( &m_pDepthStencilState, &m_StencilRef );
    }

    // �u�����h�X�e�[�g���擾.
    {
        m_pDeviceContext->OMGetBlendState( &m_pBlendState, m_BlendFactor, &m_SampleMask );
    }

    if ( !OnInit() )
    {
        ELOG( "Error : OnInit() Failed." );
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------------------------
//      Direct3D�̏I������.
//---------------------------------------------------------------------------------------------
void Application::TermD3D()
{
    // �X�e�[�g���f�t�H���g�ɖ߂��܂�.
    if ( m_pDeviceContext )
    {
        m_pDeviceContext->ClearState();
        m_pDeviceContext->Flush();
    }

    // �I���������Ăяo��.
    OnTerm();

    // �`��^�[�Q�b�g�����.
    m_RenderTarget2D.Release();

    // �[�x�X�e���V���o�b�t�@�����.
    m_DepthStencilBuffer.Release();

    // ���X�^���C�U�[�X�e�[�g�����.
    if ( m_pRasterizerState )
    {
        m_pRasterizerState->Release();
        m_pRasterizerState = nullptr;
    }

    //�@�[�x�X�e���V���X�e�[�g�����.
    if ( m_pDepthStencilState )
    {
        m_pDepthStencilState->Release();
        m_pDepthStencilState = nullptr;
    }

    // �u�����h�X�e�[�g�����.
    if ( m_pBlendState )
    {
        m_pBlendState->Release();
        m_pBlendState = nullptr;
    }

    // �X���b�v�`�F�C�������.
    if ( m_pSwapChain )
    {
        m_pSwapChain->Release();
        m_pSwapChain = nullptr;
    }

    // �f�o�C�X�R���e�L�X�g�����.
    if ( m_pDeviceContext )
    {
        m_pDeviceContext->Release();
        m_pDeviceContext = nullptr;
    }

    // �f�o�C�X�����.
    if ( m_pDevice )
    {
        m_pDevice->Release();
        m_pDevice = nullptr;
    }
}

//---------------------------------------------------------------------------------------------
//      ���C�����[�v����.
//---------------------------------------------------------------------------------------------
void Application::MainLoop()
{
    MSG msg = { 0 };

    while( WM_QUIT != msg.message )
    {
        if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            // �f�o�C�X�E�f�o�C�X�R���e�L�X�g�E�X���b�v�`�F�C���̂����ꂩ��NULL�ł���΃X�L�b�v.
            if ( ( m_pDevice        == nullptr )
              || ( m_pDeviceContext == nullptr )
              || ( m_pSwapChain     == nullptr ) )
            { continue; }

            double time;
            double absTime;
            double elapsedTime;

            // ���Ԃ��擾.
            m_Timer.GetValues( time, absTime, elapsedTime );

            // 60 �t���[�����ƂɌo�ߎ��Ԃ��Z�o����FPS���X�V.
            if ( ( m_FrameCount % 60 ) == 0 )
            {
                // FPS���Z�o.
                m_FPS = 60.0f / (float)( time - m_LatestUpdateTime );

                // �X�V���Ԃ�ݒ�.
                m_LatestUpdateTime = time;
            }

            // �t���[���J�ڏ���.
            OnFrameMove( time, elapsedTime );

            // �`���~�t���O�������Ă��Ȃ��ꍇ.
            if ( !IsStopRendering() )
            {
                // �t���[���`�揈��.
                OnFrameRender( time, elapsedTime );

                // �t���[���J�E���g���C���N�������g.
                m_FrameCount++;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------
//      �A�v���P�[�V���������s���܂�.
//---------------------------------------------------------------------------------------------
void Application::Run()
{
    // �A�v���P�[�V�����̏���������.
    if ( InitApp() )
    {
        // ���C�����[�v����.
        MainLoop();
    }

    // �A�v���P�[�V�����̏I������.
    TermApp();
}

//---------------------------------------------------------------------------------------------
//      �L�[�C�x���g����.
//---------------------------------------------------------------------------------------------
void Application::KeyEvent( UINT nChar, bool isKeyDown, bool isAltDown )
{
    KeyEventParam param;
    param.KeyCode   = nChar;
    param.IsKeyDown = isKeyDown;
    param.IsAltDown = isAltDown;

    OnKey( param );
}

//---------------------------------------------------------------------------------------------
//      ���T�C�Y�C�x���g����.
//---------------------------------------------------------------------------------------------
void Application::ResizeEvent( UINT width, UINT height )
{
    m_Width       = ( width  > 0 ) ? width  : 1;
    m_Height      = ( height > 0 ) ? height : 1;
    m_AspectRatio = (FLOAT)m_Width / (FLOAT)m_Height;

    if ( ( m_pSwapChain     != nullptr )
      && ( m_pDeviceContext != nullptr ) )
    {
        ID3D11RenderTargetView* pNull = nullptr;
        m_pDeviceContext->OMSetRenderTargets( 1, &pNull, nullptr );

        // �`��^�[�Q�b�g�����.
        m_RenderTarget2D.Release();

        // �[�x�X�e���V���o�b�t�@�����.
        m_DepthStencilBuffer.Release();

        HRESULT hr = S_OK;

        // �o�b�t�@�����T�C�Y.
        hr = m_pSwapChain->ResizeBuffers( m_SwapChainCount, 0, 0, m_SwapChainFormat, 0 );
        if ( FAILED( hr ) )
        { DLOG( "Error : IDXGISwapChain::ResizeBuffer() Failed." ); }

        // �o�b�N�o�b�t�@����`��^�[�Q�b�g�𐶐�.
        if ( !m_RenderTarget2D.CreateFromBackBuffer( m_pDevice, m_pSwapChain ) )
        { DLOG( "Error : RenderTarget2D::CreateFromBackBuffer() Failed." ); }

        DepthStencilBuffer::Description desc;
        desc.Width              = m_Width;
        desc.Height             = m_Height;
        desc.MipLevels          = 1;
        desc.ArraySize          = 1;
        desc.Format             = m_DepthStencilFormat;
        desc.SampleDesc.Count   = m_MultiSampleCount;
        desc.SampleDesc.Quality = m_MultiSampleQuality;
        desc.CPUAccessFlags     = 0;
        desc.MiscFlags          = 0;

        if ( !m_DepthStencilBuffer.Create( m_pDevice, desc ) )
        { DLOG( "Error : DepthStencilBuffer::Create() Failed." ); }

        // �f�o�C�X�R���e�L�X�g�Ƀ����_�[�^�[�Q�b�g��ݒ�.
        ID3D11RenderTargetView* pRTV = m_RenderTarget2D.GetRTV();
        ID3D11DepthStencilView* pDSV = m_DepthStencilBuffer.GetDSV();
        m_pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );

        // �r���[�|�[�g�̐ݒ�.
        D3D11_VIEWPORT vp;
        vp.Width    = (FLOAT)m_Width;
        vp.Height   = (FLOAT)m_Height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;

        // �f�o�C�X�R���e�L�X�g�Ƀr���[�|�[�g��ݒ�.
        m_pDeviceContext->RSSetViewports( 1, &vp );
    }

    ResizeEventParam param;
    param.Width       = m_Width;
    param.Height      = m_Height;
    param.AspectRatio = m_AspectRatio;

    OnResize( param );
}

//---------------------------------------------------------------------------------------------
//      �}�E�X�C�x���g����.
//---------------------------------------------------------------------------------------------
void Application::MouseEvent
(
    int  x,
    int  y,
    int  wheelDelta,
    bool isLeftButtonDown,
    bool isRightButtonDown,
    bool isMiddleButtonDown,
    bool isSideButton1Down,
    bool isSideButton2Down
)
{
    MouseEventParam param;
    param.X                  = x;
    param.Y                  = y;
    param.WheelDelta         = wheelDelta;
    param.IsLeftButtonDown   = isLeftButtonDown;
    param.IsRightButtonDown  = isRightButtonDown;
    param.IsMiddleButtonDown = isMiddleButtonDown;
    param.IsSideButton1Down  = isSideButton1Down;
    param.IsSideButton2Down  = isSideButton2Down;

    OnMouse( param );
}

//---------------------------------------------------------------------------------------------
//      �E�B���h�E�v���V�[�W��.
//---------------------------------------------------------------------------------------------
LRESULT CALLBACK Application::MsgProc( HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp )
{
    PAINTSTRUCT ps;
    HDC         hdc;

    if ( ( uMsg == WM_KEYDOWN )
      || ( uMsg == WM_SYSKEYDOWN )
      || ( uMsg == WM_KEYUP )
      || ( uMsg == WM_SYSKEYUP ) )
    {
        bool isKeyDown = ( uMsg == WM_KEYDOWN  || uMsg == WM_SYSKEYDOWN );
        
        DWORD mask = ( 1 << 29 );
        bool isAltDown =( ( lp & mask ) != 0 );

        for( ApplicationList::ListItr itr = g_AppList.Begin(); itr != g_AppList.End(); itr++ )
        {
            (*itr)->KeyEvent( (UINT)wp, isKeyDown, isAltDown );
        }

        if ( (UINT)wp == VK_ESCAPE )
        { PostQuitMessage( 0 ); }
    }

    // �Â�WM_MOUSEWHEEL�̒�`.
    const UINT OLD_WM_MOUSEWHEEL = 0x020A;

    if ( ( uMsg == WM_LBUTTONDOWN )
      || ( uMsg == WM_LBUTTONUP )
      || ( uMsg == WM_LBUTTONDBLCLK )
      || ( uMsg == WM_MBUTTONDOWN )
      || ( uMsg == WM_MBUTTONUP )
      || ( uMsg == WM_MBUTTONDBLCLK )
      || ( uMsg == WM_RBUTTONDOWN )
      || ( uMsg == WM_RBUTTONUP )
      || ( uMsg == WM_RBUTTONDBLCLK )
      || ( uMsg == WM_XBUTTONDOWN )
      || ( uMsg == WM_XBUTTONUP )
      || ( uMsg == WM_XBUTTONDBLCLK )
      || ( uMsg == WM_MOUSEHWHEEL )             // ����WM_MOUSEWHEEL��0x020E��z��.
      || ( uMsg == WM_MOUSEMOVE )
      || ( uMsg == OLD_WM_MOUSEWHEEL ) )
    {
        int x = (short)LOWORD( lp );
        int y = (short)HIWORD( lp );

        int wheelDelta = 0;
        if ( ( uMsg == WM_MOUSEHWHEEL )
          || ( uMsg == OLD_WM_MOUSEWHEEL ) )
        {
            POINT pt;
            pt.x = x;
            pt.y = y;

            ScreenToClient( hWnd, &pt );
            x = pt.x;
            y = pt.y;

            wheelDelta += (short)HIWORD( wp );
        }

        int buttonState = LOWORD( wp );
        bool isLeftButtonDown   = ( ( buttonState & MK_LBUTTON  ) != 0 );
        bool isRightButtonDown  = ( ( buttonState & MK_RBUTTON  ) != 0 );
        bool isMiddleButtonDown = ( ( buttonState & MK_MBUTTON  ) != 0 );
        bool isSideButton1Down  = ( ( buttonState & MK_XBUTTON1 ) != 0 );
        bool isSideButton2Down  = ( ( buttonState & MK_XBUTTON2 ) != 0 );

        for( ApplicationList::ListItr itr = g_AppList.Begin(); itr != g_AppList.End(); itr++ )
        {
            (*itr)->MouseEvent(
                x,
                y,
                wheelDelta,
                isLeftButtonDown,
                isRightButtonDown,
                isMiddleButtonDown,
                isSideButton1Down,
                isSideButton2Down
            );
        }
    }

    switch( uMsg )
    {
    case WM_PAINT:
        {
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
        }
        break;

    case WM_DESTROY:
        { PostQuitMessage( 0 ); }
        break;

    case WM_SIZE:
        {
            UINT w = (UINT)LOWORD( lp );
            UINT h = (UINT)HIWORD( lp );
            for( ApplicationList::ListItr itr = g_AppList.Begin(); itr != g_AppList.End(); itr++ )
            {
                (*itr)->ResizeEvent( w, h );
            }
        }
        break;
    }

    return DefWindowProc( hWnd, uMsg, wp, lp );
}

//---------------------------------------------------------------------------------------------
//      ���������̏���.
//---------------------------------------------------------------------------------------------
bool Application::OnInit()
{ 
    /* DO_NOTHING */
    return true; 
}

//---------------------------------------------------------------------------------------------
//      �I�����̏���.
//---------------------------------------------------------------------------------------------
void Application::OnTerm()
{
    /* DO_NOTHING */
}

//---------------------------------------------------------------------------------------------
//      �t���[���J�ڎ��̏���.
//---------------------------------------------------------------------------------------------
void Application::OnFrameMove( double time, double elapsedTime )
{
    ASDX_UNUSED_VAR( time );
    ASDX_UNUSED_VAR( elapsedTime );

    /* DO_NOTHING */
}

//---------------------------------------------------------------------------------------------
//      �t���[���`�掚�̏���.
//---------------------------------------------------------------------------------------------
void Application::OnFrameRender( double time, double elapsedTime )
{
    ASDX_UNUSED_VAR( time );
    ASDX_UNUSED_VAR( elapsedTime );

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

    // �`�揈��.
    {
        /* DO_NOTHING */
    }

    // �R�}���h�����s���āC��ʂɕ\��.
    m_pSwapChain->Present( 0, 0 );
}

//---------------------------------------------------------------------------------------------
//      ���T�C�Y���̏���.
//---------------------------------------------------------------------------------------------
void Application::OnResize( const ResizeEventParam& param )
{
    ASDX_UNUSED_VAR( param );
    /* DO_NOTHING */
}

//---------------------------------------------------------------------------------------------
//      �L�[�C�x���g���̏���.
//---------------------------------------------------------------------------------------------
void Application::OnKey( const KeyEventParam& param )
{
    ASDX_UNUSED_VAR( param );
    /* DO_NOTHING */
}

//---------------------------------------------------------------------------------------------
//      �}�E�X�C�x���g���̏���.
//---------------------------------------------------------------------------------------------
void Application::OnMouse( const MouseEventParam& param )
{
    ASDX_UNUSED_VAR( param );
    /* DO_NOTHING */
}

} // namespace asdx
