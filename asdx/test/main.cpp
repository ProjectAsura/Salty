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
    asdx::MATRIX    World;      //!< ワールド行列です.
    asdx::MATRIX    View;       //!< ビュー行列です.
    asdx::MATRIX    Proj;       //!< 射影行列です.
    asdx::FLOAT4    Light;      //!< ライトベクトルです.
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
    asdx::Font              m_Font;             //!< フォントです.
    asdx::CameraUpdater     m_Camera;           //!< カメラです.
    asdx::ConstantBuffer    m_CB;               //!< 定数バッファです.
    asdx::AxisRenderer      m_AxisRenderer;     //!< 軸レンダラーです.
    ID3D11VertexShader*     m_pVS;              //!< 頂点シェーダです.
    ID3D11PixelShader*      m_pPS;              //!< ピクセルシェーダです.
    CBPerOnce               m_Param;            //!< 定数バッファのパラメータです.

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
//      コンストラクタです.
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
//      デストラクタです.
//---------------------------------------------------------------------------------------------
TestApplication::~TestApplication()
{
    assert( m_pVS == nullptr );
    assert( m_pPS == nullptr );
}

//---------------------------------------------------------------------------------------------
//      初期化時の処理です.
//---------------------------------------------------------------------------------------------
bool TestApplication::OnInit()
{
    // フォント初期化.
    if ( !m_Font.Init( m_pDevice, "../res/font/SetoMini-P.fnt", (float)m_Width, (float)m_Height ) )
    {
        ELOG( "Error : asdx::Font::Init() Failed." );
        return false;
    }

    // 軸レンダラー初期化.
    if ( !m_AxisRenderer.Init( m_pDevice ) )
    {
        ELOG( "Error : AxisRenderer::Init() Failed." );
        return false;
    }

    {
        // アスペクト比を算出.
        float aspectRatio = (float)m_Width/(float)m_Height;

        // カメラの設定.
        m_Camera.GetCamera().SetPosition( asdx::FLOAT3( 0.0f, 0.0f, -50.0f ) );
        m_Camera.GetCamera().SetTarget( asdx::FLOAT3( 0.0f, 0.0f, 0.0f ) );
        m_Camera.GetCamera().SetUpward( asdx::FLOAT3( 0.0f, 1.0f, 0.0f ) );
        m_Camera.GetCamera().Preset();
        m_Camera.GetCamera().Update();

        // 変換パラメータを設定.
        m_Param.Light = asdx::FLOAT4( -100.0f, 100.0f, 0.0f, 1.0f );
        m_Param.Proj  = asdx::CreatePerspectiveFieldOfView( asdx::F_PIDIV4, aspectRatio, 0.1f, 100000.0f );
        m_Param.View  = m_Camera.GetView();
        m_Param.World.Identity();
    }

    return true;
}

//--------------------------------------------------------------------------------------------
//      終了時の処理です.
//--------------------------------------------------------------------------------------------
void TestApplication::OnTerm()
{
    // フォントの終了処理.
    m_Font.Term();

    // 頂点シェーダを解放.
    if ( m_pVS )
    {
        m_pVS->Release();
        m_pVS = nullptr;
    }

    // ピクセルシェーダを解放.
    if ( m_pPS )
    {
        m_pPS->Release();
        m_pPS = nullptr;
    }

    // 軸レンダラーの終了処理
    m_AxisRenderer.Term();

    // 変換パラメータバッファを解放.
    m_CB.Release();
} 

//--------------------------------------------------------------------------------------------
//      フレーム遷移時の処理です.
//--------------------------------------------------------------------------------------------
void TestApplication::OnFrameMove( double time, double elapsedTime )
{
    // ビュー行列を更新.
    m_Param.View = m_Camera.GetView();
}

//--------------------------------------------------------------------------------------------
//      テキスト描画時の処理です.
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
//      メッシュ描画時の処理です.
//--------------------------------------------------------------------------------------------
void TestApplication::OnDrawMesh()
{

}

//--------------------------------------------------------------------------------------------
//      フレーム描画時の処理です.
//--------------------------------------------------------------------------------------------
void TestApplication::OnFrameRender( double time, double elapsedTime )
{
    // レンダーターゲットビュー・深度ステンシルビューを取得.
    ID3D11RenderTargetView* pRTV = m_RenderTarget2D.GetRTV();
    ID3D11DepthStencilView* pDSV = m_DepthStencilBuffer.GetDSV();

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

    // ステートを設定.
    m_pDeviceContext->RSSetState( m_pRasterizerState );
    m_pDeviceContext->OMSetBlendState( m_pBlendState, m_BlendFactor, m_SampleMask );
    m_pDeviceContext->OMSetDepthStencilState( m_pDepthStencilState, m_StencilRef );

    // 描画処理.
    {
        // メッシュを描画.
        OnDrawMesh();

        // テキストを描画.
        OnDrawText();
    }

    // コマンドを実行して，画面に表示.
    m_pSwapChain->Present( 0, 0 );
}

//--------------------------------------------------------------------------------------------
//      リサイズ時の処理です.
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
//      キーの処理です.
//--------------------------------------------------------------------------------------------
void TestApplication::OnKey( const asdx::KeyEventParam& param )
{
    // カメラ処理.
    m_Camera.OnKey( param.KeyCode, param.IsKeyDown, param.IsAltDown );
}

//--------------------------------------------------------------------------------------------
//      マウスの処理です.
//--------------------------------------------------------------------------------------------
void TestApplication::OnMouse( const asdx::MouseEventParam& param )
{
    // カメラ処理.
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
//! @brief      メインエントリーポイントです.
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