//------------------------------------------------------------------------------
// File : SampleApp.h
// Desc : Sample Application.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __SAMPLE_APP_H__
#define __SAMPLE_APP_H__

//------------------------------------------------------------------------------
// Include
//------------------------------------------------------------------------------
#include <asdxApp.h>
#include <asdxResMesh.h>
#include <asdxMesh.h>
#include <asdxCameraUpdater.h>
#include <asdxAxisRenderer.h>
#include <asdxFont.h>


/////////////////////////////////////////////////////////////////////////////////////////
// TrasnsformParam structure
/////////////////////////////////////////////////////////////////////////////////////////
struct TransformParam
{
    asdx::MATRIX    World;          //!< ワールド行列.
    asdx::MATRIX    View;           //!< ビュー行列.
    asdx::MATRIX    Proj;           //!< 射影行列.
    asdx::FLOAT4    Light;          //!< ライト.
};


////////////////////////////////////////////////////////////////////////////////////////
// SampleApplication
////////////////////////////////////////////////////////////////////////////////////////
class SampleApplication : public asdx::Application
{
    //==================================================================================
    // list of friend classes and methods.
    //==================================================================================
    /* NOTHING */

private:
    //===================================================================================
    // private variables.
    //===================================================================================
    asdx::Font                  m_Font;             //!< フォントです.
    asdx::CameraUpdater         m_Camera;           //!< カメラです.
    ID3D11Buffer*               m_pTB;              //!< 変換パラメータバッファです.
    ID3D11VertexShader*         m_pVS;              //!< 頂点シェーダです.
    ID3D11PixelShader*          m_pPS;              //!< ピクセルシェーダです.
    TransformParam              m_TransformParam;   //!< 変換パラメータです.
    asdx::Mesh                  m_Dosei;            //!< どせいさん.
    asdx::Mesh                  m_Ground;           //!< 地面.
    asdx::AxisRenderer          m_AxisRenderer;

    //==================================================================================
    // private methods.
    //==================================================================================

    //----------------------------------------------------------------------------------
    //! @brief      テキスト描画時の処理です.
    //----------------------------------------------------------------------------------
    void OnDrawText();

    //----------------------------------------------------------------------------------
    //! @brief      メッシュ描画時の処理です.
    //----------------------------------------------------------------------------------
    void OnDrawMesh();

protected:
    //==================================================================================
    // protected varaibles
    //==================================================================================
    /* NOTHING */

    //==================================================================================
    // protected methods.
    //==================================================================================

    //----------------------------------------------------------------------------------
    //! @brief      初期化時の処理です.
    //!
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //----------------------------------------------------------------------------------
    bool OnInit();

    //----------------------------------------------------------------------------------
    //! @brief      終了時の処理です.
    //----------------------------------------------------------------------------------
    void OnTerm();

    //----------------------------------------------------------------------------------
    //! @brief      フレーム繊維時の処理です.
    //!
    //! @param [in]     time            アプリケーション開始してからの相対時間です.
    //! @param [in]     elapsedTime     前フレームからの経過時間です.
    //----------------------------------------------------------------------------------
    void OnFrameMove  ( double time, double elapsedTime );

    //----------------------------------------------------------------------------------
    //! @brief      フレーム描画時の処理です.
    //!
    //! @param [in]     time            アプリケーション開始してからの相対時間です.
    //! @param [in]     elapsedTime     前フレームからの経過時間です.
    //----------------------------------------------------------------------------------
    void OnFrameRender( double time, double elapsedTime );

    //----------------------------------------------------------------------------------
    //! @brief      リサイズ時の処理です.
    //!
    //! @param [in]     param           リサイズイベントパラメータです.
    //----------------------------------------------------------------------------------
    void OnResize     ( const asdx::ResizeEventParam& param );

    //----------------------------------------------------------------------------------
    //! @brief      キーイベント時の処理です.
    //!
    //! @param [in]     param           キーイベントパラメータです.
    //----------------------------------------------------------------------------------
    void OnKey        ( const asdx::KeyEventParam& param );

    //----------------------------------------------------------------------------------
    //! @brief      マウスイベント時の処理です.
    //!
    //! @param [in]     param           マウスイベントパラメータです.
    //----------------------------------------------------------------------------------
    void OnMouse      ( const asdx::MouseEventParam& param );

public:
    //==================================================================================
    // public variables.
    //==================================================================================
    /* NOTHING */

    //==================================================================================
    // public methods.
    //==================================================================================

    //----------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //----------------------------------------------------------------------------------
    SampleApplication();

    //----------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //----------------------------------------------------------------------------------
    virtual ~SampleApplication();
};



#endif//__SAMPLE_APP_H__
