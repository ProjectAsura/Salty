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
    List    m_List;         //!< リストです.

    //=======================================================================================
    // public methods.
    //=======================================================================================

    //---------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------
    ApplicationList()
    { m_List.clear(); }

    //---------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------
    ~ApplicationList()
    { m_List.clear(); }

    //---------------------------------------------------------------------------------------
    //! @brief      push_back()のラッパー関数です.
    //---------------------------------------------------------------------------------------
    void PushBack( asdx::Application* pApp )
    { m_List.push_back( pApp ); }

    //---------------------------------------------------------------------------------------
    //! @brief      push_front()のラッパー関数です.
    //---------------------------------------------------------------------------------------
    void PushFront( asdx::Application* pApp )
    { m_List.push_front( pApp ); }

    //---------------------------------------------------------------------------------------
    //! @brief      pop_back()のラッパー関数です.
    //---------------------------------------------------------------------------------------
    void PopBack()
    { m_List.pop_back(); }

    //---------------------------------------------------------------------------------------
    //! @brief      pop_front()のラッパー関数です.
    //---------------------------------------------------------------------------------------
    void PopFront()
    { m_List.pop_front(); }

    //---------------------------------------------------------------------------------------
    //! @brief      clear()のラッパー関数です.
    //---------------------------------------------------------------------------------------
    void Clear()
    { m_List.clear(); }

    //---------------------------------------------------------------------------------------
    //! @brief      remove()のラッパー関数です.
    //---------------------------------------------------------------------------------------
    void Remove( asdx::Application* pApp )
    { m_List.remove( pApp ); }

    //---------------------------------------------------------------------------------------
    //! @brief      begin()のラッパー関数です.
    //---------------------------------------------------------------------------------------
    ListItr Begin()
    { return m_List.begin(); }

    //---------------------------------------------------------------------------------------
    //! @brief      begin()のラッパー関数です(const版).
    //---------------------------------------------------------------------------------------
    ListCItr Begin() const
    { return m_List.begin(); }
    
    //---------------------------------------------------------------------------------------
    //! @brief      end()のラッパー関数です.
    //---------------------------------------------------------------------------------------
    ListItr End()
    { return m_List.end(); }

    //---------------------------------------------------------------------------------------
    //! @brief      end()のラッパー関数です(const版).
    //---------------------------------------------------------------------------------------
    ListCItr End() const
    { return m_List.end(); }
};


// アプリケーションリスト.
ApplicationList     g_AppList;

// ミューテックス.
CRITICAL_SECTION    g_CriticalSection;


/////////////////////////////////////////////////////////////////////////////////////////////
// Lock class
/////////////////////////////////////////////////////////////////////////////////////////////
class Lock
{
public:
    //---------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------
    inline Lock()
    { EnterCriticalSection( &g_CriticalSection ); }

    //---------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------
    inline ~Lock()
    { LeaveCriticalSection( &g_CriticalSection ); }
};


} // namespace /* anonymous */


namespace asdx  {

// ウィンドウクラス名です.
#ifndef ASDX_WND_CLASSNAME
#define ASDX_WND_CLASSNAME      "asdxWindowClass"
#endif//ASDX_WND_CLAASNAME

///////////////////////////////////////////////////////////////////////////////////////////////
// Application class
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//      コンストラクタです.
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
, m_DepthStencilTarget  ()
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
, m_IsStandbyMode       ( false )
{ /* DO_NOHTING */ }

//---------------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------------------------- 
Application::Application( LPSTR title, UINT width, UINT height )
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
, m_DepthStencilTarget  ()
, m_pRasterizerState    ( nullptr )
, m_pDepthStencilState  ( nullptr )
, m_pBlendState         ( nullptr )
, m_ClearColor          ( DirectX::Colors::CornflowerBlue )
, m_Width               ( width )
, m_Height              ( height )
, m_AspectRatio         ( (float)width/(float)height )
, m_Title               ( title )
, m_Timer               ()
, m_FrameCount          ( 0 )
, m_FPS                 ( 0.0f )
, m_LatestUpdateTime    ( 0.0f )
, m_IsStopRendering     ( false )
, m_IsStandbyMode       ( false )
{ /* DO_NOHTING */ }

//---------------------------------------------------------------------------------------------
//      デストラクタです.
//---------------------------------------------------------------------------------------------
Application::~Application()
{ TermApp(); }

//---------------------------------------------------------------------------------------------
//      描画停止フラグを設定します.
//---------------------------------------------------------------------------------------------
void Application::SetStopRendering( bool isStopRendering )
{
    Lock;
    m_IsStopRendering = isStopRendering;
}

//---------------------------------------------------------------------------------------------
//      描画停止フラグを取得します.
//---------------------------------------------------------------------------------------------
bool Application::IsStopRendering() const
{
    Lock;
    return m_IsStopRendering;
}

//---------------------------------------------------------------------------------------------
//      フレームカウントを取得します.
//---------------------------------------------------------------------------------------------
DWORD Application::GetFrameCount() const
{
    Lock;
    return m_FrameCount;
}

//---------------------------------------------------------------------------------------------
//      FPSを取得します.
//---------------------------------------------------------------------------------------------
FLOAT Application::GetFPS() const
{
    Lock;
    return m_FPS;
}

//---------------------------------------------------------------------------------------------
//      アプリケーションを初期化します.
//---------------------------------------------------------------------------------------------
bool Application::InitApp()
{
    // ウィンドウの初期化.
    if ( !InitWnd() )
    {
        DLOG( "Error : InitWnd() Failed." );
        return false;
    }

    // Direct3Dの初期化.
    if ( !InitD3D() )
    {
        DLOG( "Error : InitD3D() Failed." );
        return false;
    }

    // 正常終了.
    return true;
}

//---------------------------------------------------------------------------------------------
//      アプリケーションの終了処理.
//---------------------------------------------------------------------------------------------
void Application::TermApp()
{
    // Direct3Dの終了処理.
    TermD3D();

    // ウィンドウの終了処理.
    TermWnd();
}

//---------------------------------------------------------------------------------------------
//      ウィンドウの初期化処理.
//---------------------------------------------------------------------------------------------
bool Application::InitWnd()
{
    // インスタンスハンドルを取得.
    HINSTANCE hInst = GetModuleHandle( nullptr );
    if ( !hInst )
    {
        DLOG( "Error : GetModuleHandle() Failed. ");
        return false;
    }

    // 拡張ウィンドウクラスの設定.
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

    // ウィンドウクラスを登録します.
    if ( !RegisterClassExA( &wc ) )
    {
        // エラーログ出力.
        DLOG( "Error : RegisterClassEx() Failed." );

        // 異常終了.
        return false; 
    }

    // インスタンスハンドルを設定.
    m_hInst = hInst;

    // 矩形の設定.
    RECT rc = { 0, 0, m_Width, m_Height };

    // 指定されたクライアント領域を確保するために必要なウィンドウ座標を計算します.
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    AdjustWindowRect( &rc, style, FALSE );

    // ウィンドウを生成します.
    m_hWnd = CreateWindowA(
        ASDX_WND_CLASSNAME,
        m_Title,
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        ( rc.right - rc.left ),
        ( rc.bottom - rc.top ),
        NULL,
        NULL,
        hInst,
        NULL 
    );

    // 生成チェック.
    if ( !m_hWnd )
    {
        // エラーログ出力.
        DLOG( "Error : CreateWindow() Failed." );

        // 異常終了.
        return false;
    }

    // ウィンドウを表示します.
    ShowWindow( m_hWnd, SW_SHOWNORMAL );
    UpdateWindow( m_hWnd );

    // フォーカスを設定します.
    SetFocus( m_hWnd );

    // アプリケーションリストに登録します.
    g_AppList.PushBack( this );

    // タイマーを開始します.
    m_Timer.Start();

    // 開始時刻を取得.
    m_LatestUpdateTime = m_Timer.GetElapsedTime();

    // 正常終了.
    return true;
}

//---------------------------------------------------------------------------------------------
//      ウィンドウの終了処理.
//---------------------------------------------------------------------------------------------
void Application::TermWnd()
{
    // タイマーを止めます.
    m_Timer.Stop();

    // ウィンドウクラスの登録を解除.
    if ( m_hInst != nullptr )
    { UnregisterClassA( ASDX_WND_CLASSNAME, m_hInst ); }

    // タイトル名をクリア.
    m_Title = nullptr;

    // ハンドルをクリア.
    m_hInst = nullptr;
    m_hWnd  = nullptr;

    // アプリケーションリストから削除します.
    g_AppList.Remove( this );
}

//---------------------------------------------------------------------------------------------
//      Direct3Dの初期化処理.
//---------------------------------------------------------------------------------------------
bool Application::InitD3D()
{
    HRESULT hr = S_OK;

    // ウィンドウサイズを取得します.
    RECT rc;
    GetClientRect( m_hWnd, &rc );
    UINT w = rc.right - rc.left;
    UINT h = rc.bottom - rc.top;

    // 取得したサイズを設定します.
    m_Width       = w;
    m_Height      = h;

    // アスペクト比を算出します.
    m_AspectRatio = (FLOAT)w / (FLOAT)h;

    // デバイス生成フラグ.
    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif//defined(DEBUG) || deifned(_DEBUG)

    // ドライバータイプ.
    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTytpes = sizeof( driverTypes ) / sizeof( driverTypes[0] );

    // 機能レベル.
    D3D_FEATURE_LEVEL featureLevels[] = {
        //D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = sizeof( featureLevels ) / sizeof( featureLevels[0] );

    // スワップチェインの構成設定.
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
        // ドライバータイプ設定.
        m_DriverType = driverTypes[ idx ];

        // デバイスとスワップチェインの生成.
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

        // 成功したらループを脱出.
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

    // 失敗していないかチェック.
    if ( FAILED( hr ) )
    {
        DLOG( "Error : D3D11CreateDeviceAndSwapChain() Failed." );
        return false;
    }

    // レンダーターゲットを生成.
    if ( !m_RenderTarget2D.CreateFromBackBuffer( m_pDevice, m_pSwapChain ) )
    {
        DLOG( "Error : RenderTarget2D::CreateFromBackBuffer() Failed." );
        return false;
    }

    // 深度ステンシルバッファの構成設定.
    DepthStencilTarget::Description desc;
    desc.Width              = w;
    desc.Height             = h;
    desc.MipLevels          = 1;
    desc.ArraySize          = 1;
    desc.Format             = m_DepthStencilFormat;
    desc.SampleDesc.Count   = m_MultiSampleCount;
    desc.SampleDesc.Quality = m_MultiSampleQuality;
    desc.CPUAccessFlags     = 0;
    desc.MiscFlags          = 0;

    // 深度ステンシルバッファを生成.
    if ( !m_DepthStencilTarget.Create( m_pDevice, desc ) )
    {
        DLOG( "Error : DepthStencilTarget::Create() Failed." );
        return false;
    }

    // デバイスコンテキストにレンダーターゲットを設定.
    ID3D11RenderTargetView* pRTV = m_RenderTarget2D.GetRTV();
    ID3D11DepthStencilView* pDSV = m_DepthStencilTarget.GetDSV();
    m_pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );

    // ビューポートの設定.
    D3D11_VIEWPORT vp;
    vp.Width    = (FLOAT)w;
    vp.Height   = (FLOAT)h;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    // デバイスコンテキストにビューポートを設定.
    m_pDeviceContext->RSSetViewports( 1, &vp );

    // ラスタライザーステートの設定.
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

        // ラスタライザーステートを生成.
        hr = m_pDevice->CreateRasterizerState( &rd, &m_pRasterizerState );
        if ( FAILED( hr ) )
        {
            DLOG( "Error : ID3D11Device::CreateRasterizerState() Failed." );
            return false;
        }
        SetDebugObjectName( m_pRasterizerState, "asdxApp" );

        // デバイスコンテキストにラスタライザーステートを設定.
        m_pDeviceContext->RSSetState( m_pRasterizerState );
    }

    // 深度ステンシルステートを取得.
    {
        m_pDeviceContext->OMGetDepthStencilState( &m_pDepthStencilState, &m_StencilRef );
    }

    // ブレンドステートを取得.
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
//      Direct3Dの終了処理.
//---------------------------------------------------------------------------------------------
void Application::TermD3D()
{
    // ステートをデフォルトに戻します.
    if ( m_pDeviceContext )
    {
        m_pDeviceContext->ClearState();
        m_pDeviceContext->Flush();
    }

    // 終了処理を呼び出し.
    OnTerm();

    // 描画ターゲットを解放.
    m_RenderTarget2D.Release();

    // 深度ステンシルバッファを解放.
    m_DepthStencilTarget.Release();

    // ラスタライザーステートを解放.
    if ( m_pRasterizerState )
    {
        m_pRasterizerState->Release();
        m_pRasterizerState = nullptr;
    }

    //　深度ステンシルステートを解放.
    if ( m_pDepthStencilState )
    {
        m_pDepthStencilState->Release();
        m_pDepthStencilState = nullptr;
    }

    // ブレンドステートを解放.
    if ( m_pBlendState )
    {
        m_pBlendState->Release();
        m_pBlendState = nullptr;
    }

    // スワップチェインを解放.
    if ( m_pSwapChain )
    {
        m_pSwapChain->Release();
        m_pSwapChain = nullptr;
    }

    // デバイスコンテキストを解放.
    if ( m_pDeviceContext )
    {
        m_pDeviceContext->Release();
        m_pDeviceContext = nullptr;
    }

    // デバイスを解放.
    if ( m_pDevice )
    {
        m_pDevice->Release();
        m_pDevice = nullptr;
    }
}

//---------------------------------------------------------------------------------------------
//      メインループ処理.
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
            // デバイス・デバイスコンテキスト・スワップチェインのいずれかがNULLであればスキップ.
            if ( ( m_pDevice        == nullptr )
              || ( m_pDeviceContext == nullptr )
              || ( m_pSwapChain     == nullptr ) )
            { continue; }

            double time;
            double absTime;
            double elapsedTime;

            // 時間を取得.
            m_Timer.GetValues( time, absTime, elapsedTime );

            // 60 フレームごとに経過時間を算出してFPSを更新.
            if ( ( m_FrameCount % 60 ) == 0 )
            {
                // FPSを算出.
                m_FPS = 60.0f / (float)( time - m_LatestUpdateTime );

                // 更新時間を設定.
                m_LatestUpdateTime = time;
            }

            // フレーム遷移処理.
            OnFrameMove( time, elapsedTime );

            // 描画停止フラグが立っていない場合.
            if ( !IsStopRendering() )
            {
                // フレーム描画処理.
                OnFrameRender( time, elapsedTime );

                // フレームカウントをインクリメント.
                m_FrameCount++;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------
//      アプリケーションを実行します.
//---------------------------------------------------------------------------------------------
void Application::Run()
{
    // アプリケーションの初期化処理.
    if ( InitApp() )
    {
        // メインループ処理.
        MainLoop();
    }

    // アプリケーションの終了処理.
    TermApp();
}

//---------------------------------------------------------------------------------------------
//      キーイベント処理.
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
//      リサイズイベント処理.
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

        // 描画ターゲットを解放.
        m_RenderTarget2D.Release();

        // 深度ステンシルバッファを解放.
        m_DepthStencilTarget.Release();

        HRESULT hr = S_OK;

        // バッファをリサイズ.
        hr = m_pSwapChain->ResizeBuffers( m_SwapChainCount, 0, 0, m_SwapChainFormat, 0 );
        if ( FAILED( hr ) )
        { DLOG( "Error : IDXGISwapChain::ResizeBuffer() Failed." ); }

        // バックバッファから描画ターゲットを生成.
        if ( !m_RenderTarget2D.CreateFromBackBuffer( m_pDevice, m_pSwapChain ) )
        { DLOG( "Error : RenderTarget2D::CreateFromBackBuffer() Failed." ); }

        DepthStencilTarget::Description desc;
        desc.Width              = m_Width;
        desc.Height             = m_Height;
        desc.MipLevels          = 1;
        desc.ArraySize          = 1;
        desc.Format             = m_DepthStencilFormat;
        desc.SampleDesc.Count   = m_MultiSampleCount;
        desc.SampleDesc.Quality = m_MultiSampleQuality;
        desc.CPUAccessFlags     = 0;
        desc.MiscFlags          = 0;

        if ( !m_DepthStencilTarget.Create( m_pDevice, desc ) )
        { DLOG( "Error : DepthStencilTarget::Create() Failed." ); }

        // デバイスコンテキストにレンダーターゲットを設定.
        ID3D11RenderTargetView* pRTV = m_RenderTarget2D.GetRTV();
        ID3D11DepthStencilView* pDSV = m_DepthStencilTarget.GetDSV();
        m_pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );

        // ビューポートの設定.
        D3D11_VIEWPORT vp;
        vp.Width    = (FLOAT)m_Width;
        vp.Height   = (FLOAT)m_Height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;

        // デバイスコンテキストにビューポートを設定.
        m_pDeviceContext->RSSetViewports( 1, &vp );
    }

    ResizeEventParam param;
    param.Width       = m_Width;
    param.Height      = m_Height;
    param.AspectRatio = m_AspectRatio;

    OnResize( param );
}

//---------------------------------------------------------------------------------------------
//      マウスイベント処理.
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
//      ウィンドウプロシージャ.
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

    // 古いWM_MOUSEWHEELの定義.
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
      || ( uMsg == WM_MOUSEHWHEEL )             // このWM_MOUSEWHEELは0x020Eを想定.
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
//      初期化時の処理.
//---------------------------------------------------------------------------------------------
bool Application::OnInit()
{ 
    /* DO_NOTHING */
    return true; 
}

//---------------------------------------------------------------------------------------------
//      終了時の処理.
//---------------------------------------------------------------------------------------------
void Application::OnTerm()
{
    /* DO_NOTHING */
}

//---------------------------------------------------------------------------------------------
//      フレーム遷移時の処理.
//---------------------------------------------------------------------------------------------
void Application::OnFrameMove( double time, double elapsedTime )
{
    ASDX_UNUSED_VAR( time );
    ASDX_UNUSED_VAR( elapsedTime );

    /* DO_NOTHING */
}

//---------------------------------------------------------------------------------------------
//      フレーム描画字の処理.
//---------------------------------------------------------------------------------------------
void Application::OnFrameRender( double time, double elapsedTime )
{
    ASDX_UNUSED_VAR( time );
    ASDX_UNUSED_VAR( elapsedTime );

    // レンダーターゲットビュー・深度ステンシルビューを取得.
    ID3D11RenderTargetView* pRTV = m_RenderTarget2D.GetRTV();
    ID3D11DepthStencilView* pDSV = m_DepthStencilTarget.GetDSV();

    // NULLチェック.
    if ( pRTV == nullptr )
    { return; }
    if ( pDSV == nullptr )
    { return; }
    
    // 出力マネージャに設定.
    m_pDeviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );

    // クリア処理.
    m_pDeviceContext->ClearRenderTargetView( pRTV, m_ClearColor );
    m_pDeviceContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

    // 描画処理.
    {
        /* DO_NOTHING */
    }

    // コマンドを実行して，画面に表示.
    Present( 0 );
}

//---------------------------------------------------------------------------------------------
//      コマンドを実行して，画面に表示します.
//---------------------------------------------------------------------------------------------
void Application::Present( UINT syncInterval )
{
    HRESULT hr = S_OK;

    // スタンバイモードかどうかチェック.
    if ( m_IsStandbyMode )
    {
        // テストする.
        hr = m_pSwapChain->Present( syncInterval, DXGI_PRESENT_TEST );

        // 表示領域がなければ画面更新せずに終了.
        if ( hr == DXGI_STATUS_OCCLUDED )
        { return; }

        // フラグを下す.
        m_IsStandbyMode = false;
    }

    // 画面更新する.
    hr = m_pSwapChain->Present( syncInterval, 0 );

    // 表示領域がなければスタンバイモードに入る.
    if ( hr == DXGI_STATUS_OCCLUDED )
    { m_IsStandbyMode = true; }
}

//---------------------------------------------------------------------------------------------
//      リサイズ時の処理.
//---------------------------------------------------------------------------------------------
void Application::OnResize( const ResizeEventParam& param )
{
    ASDX_UNUSED_VAR( param );
    /* DO_NOTHING */
}

//---------------------------------------------------------------------------------------------
//      キーイベント時の処理.
//---------------------------------------------------------------------------------------------
void Application::OnKey( const KeyEventParam& param )
{
    ASDX_UNUSED_VAR( param );
    /* DO_NOTHING */
}

//---------------------------------------------------------------------------------------------
//      マウスイベント時の処理.
//---------------------------------------------------------------------------------------------
void Application::OnMouse( const MouseEventParam& param )
{
    ASDX_UNUSED_VAR( param );
    /* DO_NOTHING */
}

} // namespace asdx
