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
//      コンストラクタです.
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
//      デストラクタです.
//---------------------------------------------------------------------------------------
SampleApplication::~SampleApplication()
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------
//      初期化時の処理です.
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

        // 頂点シェーダをコンパイル.
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

        // 頂点シェーダを生成.
        hr = m_pDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVS );
        if ( FAILED( hr ) )
        {
            pVSBlob->Release();
            pVSBlob = nullptr;

            ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );
            return false;
        }

        // どせいさんの初期化.
        {
            // リソースメッシュをロードします.
            asdx::ResMesh resMesh;
            if ( !resMesh.LoadFromFile( "../res/dosei/dosei.msh" ) )
            {
                ELOG( "Error : Mesh Load Failed." );
                return false;
            }

            // 描画メッシュの初期化.
            if ( !m_Dosei.Init( m_pDevice, resMesh, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), "../res/dosei/", "../res/dummy/" ) )
            {
                ELOG( "Error : TinyMesh::Init() Failed." );
                return false;
            }

            // リソースメッシュを解放.
            resMesh.Release();
        }

        // 地面の初期化.
        {
            // リソースメッシュをロードします.
            asdx::ResMesh resMesh;
            if ( !resMesh.LoadFromFile( "../res/ground/ground.msh" ) )
            {
                ELOG( "Error : Mesh Load Failed." );
                return false;
            }

            // 描画メッシュの初期化.
            if ( !m_Ground.Init( m_pDevice, resMesh, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), "../res/ground/", "../res/dummy/" ) )
            {
                ELOG( "Error : TinyMesh::Init() Failed." );
                return false;
            }

            // リソースメッシュを解放.
            resMesh.Release();
        }

        // シェーダコードを解放.
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

        // ピクセルシェーダをコンパイル.
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

        // ピクセルシェーダを生成.
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
        // 変換パラメータバッファの設定.
        D3D11_BUFFER_DESC bd;
        ZeroMemory( &bd, sizeof( D3D11_BUFFER_DESC ) );
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof( TransformParam );
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;

        // 変換パラメータバッファを生成.
        hr = m_pDevice->CreateBuffer( &bd, nullptr, &m_pTB );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
            return false;
        }

        // アスペクト比を算出.
        float aspectRatio = (float)m_Width/(float)m_Height;

        // カメラの設定.
        m_Camera.GetCamera().SetPosition( asdx::FLOAT3( -100.0f, 0.0f, 0.0f ) );
        m_Camera.GetCamera().SetTarget( asdx::FLOAT3( 0.0f, 0.0f, 0.0f ) );
        m_Camera.GetCamera().SetUpward( asdx::FLOAT3( 0.0f, 1.0f, 0.0f ) );
        m_Camera.GetCamera().Preset();
        m_Camera.GetCamera().Update();

        // 変換パラメータを設定.
        m_TransformParam.Light = asdx::FLOAT4( -100.0f, 100.0f, 0.0f, 1.0f );
        m_TransformParam.Proj  = asdx::CreatePerspectiveFieldOfView( asdx::F_PIDIV4, aspectRatio, 0.1f, 100000.0f );
        m_TransformParam.View  = m_Camera.GetView();
        m_TransformParam.World.Identity();
    }

    return true;
}

//---------------------------------------------------------------------------------------
//      終了時の処理です.
//---------------------------------------------------------------------------------------
void SampleApplication::OnTerm()
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

    // モデルの終了処理.
    m_Dosei.Term();
    m_Ground.Term();

    // 軸レンダラーの終了処理
    m_AxisRenderer.Term();

    // 変換パラメータバッファを解放.
    if ( m_pTB )
    {
        m_pTB->Release();
        m_pTB = nullptr;
    }
}

//---------------------------------------------------------------------------------------
//      フレーム遷移時の処理です.
//---------------------------------------------------------------------------------------
void SampleApplication::OnFrameMove( double time, double elapsedTime )
{
    // ビュー行列を更新.
    m_TransformParam.View = m_Camera.GetView();
}

//---------------------------------------------------------------------------------------
//      テキスト描画時の処理です.
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
//      メッシュ描画時の処理です.
//---------------------------------------------------------------------------------------
void SampleApplication::OnDrawMesh()
{
    // シェーダの設定.
    m_pDeviceContext->VSSetShader( m_pVS,   nullptr, 0 );
    m_pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->DSSetShader( nullptr, nullptr, 0 );
    m_pDeviceContext->PSSetShader( m_pPS,   nullptr, 0 );

    // 変換パラメータバッファを更新.
    m_TransformParam.World = asdx::CreateScale( 0.25f );
    m_pDeviceContext->UpdateSubresource( m_pTB, 0, nullptr, &m_TransformParam, 0, 0 );

    // 頂点シェーダに定数バッファを設定.
    m_pDeviceContext->VSSetConstantBuffers( 1, 1, &m_pTB );

    // どせいさんを描画.
    m_Dosei.Draw( m_pDeviceContext );

    // 変換パラメータバッファを更新.
    m_TransformParam.World = asdx::CreateTranslation( 0.0f, -10.0f, 0.0f );
    m_pDeviceContext->UpdateSubresource( m_pTB, 0, nullptr, &m_TransformParam, 0, 0 );

    // 頂点シェーダに定数バッファを設定.
    m_pDeviceContext->VSSetConstantBuffers( 1, 1, &m_pTB );

    // 地面を描画.
    m_Ground.Draw( m_pDeviceContext );

    // カメラの注視点軸を描画.
    m_AxisRenderer.Begin( m_pDeviceContext, m_Camera.GetView(), m_TransformParam.Proj );
    m_AxisRenderer.Render( m_pDeviceContext, m_Camera.GetCamera().GetTarget() );
    m_AxisRenderer.End( m_pDeviceContext );
}

//---------------------------------------------------------------------------------------
//      描画時の処理です.
//---------------------------------------------------------------------------------------
void SampleApplication::OnFrameRender( double time, double elapsedTime )
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

//---------------------------------------------------------------------------------------
//      リサイズイベントの処理です.
//---------------------------------------------------------------------------------------
void SampleApplication::OnResize( const asdx::ResizeEventParam& param )
{
    // 射影行列を更新.
    m_TransformParam.Proj = asdx::CreatePerspectiveFieldOfView( asdx::F_PIDIV4, param.AspectRatio, 0.1f, 1000.0f );
}

//---------------------------------------------------------------------------------------
//      キーイベントの処理です.
//---------------------------------------------------------------------------------------
void SampleApplication::OnKey( const asdx::KeyEventParam& param )
{
    // カメラ処理.
    m_Camera.OnKey( param.KeyCode, param.IsKeyDown, param.IsAltDown );
}

//---------------------------------------------------------------------------------------
//      マウスインベント時の処理です.
//---------------------------------------------------------------------------------------
void SampleApplication::OnMouse( const asdx::MouseEventParam& param )
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
