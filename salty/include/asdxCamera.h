﻿//-------------------------------------------------------------------------------------------
// File : asdxCamera.h
// Desc : Camera Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

#ifndef __ASDX_CAMERA_H__
#define __ASDX_CAMERA_H__

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <asdxMath.h>


namespace asdx {

/////////////////////////////////////////////////////////////////////////////////////////////
// CameraEvent struct
/////////////////////////////////////////////////////////////////////////////////////////////
struct CameraEvent
{
    /////////////////////////////////////////////////////////////////////////////////////////
    // EVENT_TYPE enum
    /////////////////////////////////////////////////////////////////////////////////////////
    enum EVENT_TYPE
    {
        EVENT_ROTATE  = 0x000001 << 0,     //!< ターゲットを中心に回転.
        EVENT_DOLLY   = 0x000001 << 1,     //!< ドリー.
        EVENT_TRUCK   = 0x000001 << 2,     //!< 平行移動.
        EVENT_TWIST   = 0x000001 << 3,     //!< ツイスト.
        EVENT_PANTILT = 0x000001 << 4,     //!< パン・チルト.
        EVENT_RESET   = 0x000001 << 5,     //!< リセット.
    };

    asdx::Vector2   Rotate;     //!< 回転角です.
    asdx::Vector2   PanTilt;    //!< パン・チルト角.
    asdx::Vector3   Truck;      //!< トラック量です.
    f32             Twist;      //!< ツイスト角です.
    f32             Dolly;      //!< ドリー量です.
    u32             Flags;      //!< フラグです.

    //---------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------
    CameraEvent()
    : Rotate    ( 0.0f, 0.0f )
    , PanTilt   ( 0.0f, 0.0f )
    , Truck     ( 0.0f, 0.0f, 0.0f )
    , Twist     ( 0.0f )
    , Dolly     ( 0.0f )
    , Flags     ( 0 )
    { /* DO_NOTHING */ }
};


/////////////////////////////////////////////////////////////////////////////////////////////
// Camera class
/////////////////////////////////////////////////////////////////////////////////////////////
class Camera
{
    //=======================================================================================
    // list of friend classes and methods.
    //=======================================================================================
    /* NOTHING */

public:
    //=======================================================================================
    // public variables
    //=======================================================================================
    struct Param
    {
        asdx::Vector3 Position;      //!< カメラの位置です.
        asdx::Vector3 Target;        //!< カメラの注視点です.
        asdx::Vector3 Upward;        //!< カメラの上向きベクトルです.
        asdx::Vector2 Rotate;        //!< 回転角です.
        asdx::Vector2 PanTilt;       //!< パン・チルト角です.
        f32           Twist;         //!< ツイスト角です.
        f32           MinDist;       //!< ターゲットとの最小距離です.
        f32           MaxDist;       //!< ターゲットとの最大距離です.

        //-----------------------------------------------------------------------------------
        //! @brief      コンストラクタです.
        //-----------------------------------------------------------------------------------
        Param()
        : Position  ( 0.0f, 0.0f, 5.0f )
        , Target    ( 0.0f, 0.0f, 0.0f )
        , Upward    ( 0.0f, 1.0f, 0.0f )
        , Rotate    ( 0.0f, 0.0f )
        , PanTilt   ( 0.0f, 0.0f )
        , Twist     ( 0.0f )
        , MinDist   ( 0.01f )
        , MaxDist   ( 1000.0f )
        { /* DO_NOTHING */ }
    };

    //=======================================================================================
    // public methods.
    //=======================================================================================

    //---------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------
    Camera();

    //---------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------
    ~Camera();

    //---------------------------------------------------------------------------------------
    //! @brief      カメラ位置を設定します.
    //!
    //! @param [in]     position        設定するカメラ位置です.
    //---------------------------------------------------------------------------------------
    void SetPosition( const asdx::Vector3& position );

    //---------------------------------------------------------------------------------------
    //! @brief      カメラ注視点を設定します.
    //!
    //! @param [in]     target          設定するカメラ注視点です.
    //---------------------------------------------------------------------------------------
    void SetTarget( const asdx::Vector3& target );

    //---------------------------------------------------------------------------------------
    //! @brief      カメラ上向きベクトルを設定します.
    //!
    //! @param [in]     upward          設定するカメラ上向きベクトルです.
    //---------------------------------------------------------------------------------------
    void SetUpward( const asdx::Vector3& upward );

    //---------------------------------------------------------------------------------------
    //! @brief      ツイスト角を設定します.
    //!
    //! @param [in]     twist           設定するツイスト角です.
    //---------------------------------------------------------------------------------------
    void SetTwist( const f32 twist );

    //---------------------------------------------------------------------------------------
    //! @brief      可動距離範囲を設定します.
    //!
    //! @param [in]     minDist         設定するカメラ注視点との最小距離.
    //! @param [in]     maxDist         設定するカメラ注視点との最大距離.
    //---------------------------------------------------------------------------------------
    void SetRange( const f32 minDist, const f32 maxDist );

    //---------------------------------------------------------------------------------------
    //! @brief      カメラパラメータを記憶します.
    //---------------------------------------------------------------------------------------
    void Preset();

    //---------------------------------------------------------------------------------------
    //! @brief      カメラパラメータをリセットします.
    //---------------------------------------------------------------------------------------
    void Reset();

    //---------------------------------------------------------------------------------------
    //! @brief      ビュー行列を更新します.
    //---------------------------------------------------------------------------------------
    void Update();

    //---------------------------------------------------------------------------------------
    //! @brief      カメライベントを基にビュー行列を更新します.
    //---------------------------------------------------------------------------------------
    void UpdateByEvent( const CameraEvent& camEvent );

    //---------------------------------------------------------------------------------------
    //! @brief      ビュー行列を取得します.
    //!
    //! @return     ビュー行列を返却します.
    //---------------------------------------------------------------------------------------
    asdx::Matrix GetView() const;

    //---------------------------------------------------------------------------------------
    //! @brief      カメラ位置を取得します.
    //!
    //! @return     カメラ位置を返却します.
    //---------------------------------------------------------------------------------------
    asdx::Vector3 GetPosition() const;

    //---------------------------------------------------------------------------------------
    //! @brief      カメラ注視点を取得します.
    //!
    //! @return     カメラ注視点を返却します.
    //---------------------------------------------------------------------------------------
    asdx::Vector3 GetTarget() const;

    //---------------------------------------------------------------------------------------
    //! @brief      カメラ上向きベクトルを取得します.
    //!
    //! @return     カメラ上向きベクトルを返却します.
    //---------------------------------------------------------------------------------------
    asdx::Vector3 GetUpward() const;

    //---------------------------------------------------------------------------------------
    //! @brief      ツイスト角を取得します.
    //!
    //! @return     ツイスト角を返却します.
    //---------------------------------------------------------------------------------------
    f32 GetTwist() const;

    //---------------------------------------------------------------------------------------
    //! @brief      可動最小距離(カメラ注視点との最小距離)を取得します.
    //!
    //! @return     可動最小距離を返却します.
    //---------------------------------------------------------------------------------------
    f32 GetMinDist() const;

    //---------------------------------------------------------------------------------------
    //! @brief      可動最大距離(カメラ注視点との最大距離)を取得します.
    //!
    //! @return     可動最大距離を返却します.
    //---------------------------------------------------------------------------------------
    f32 GetMaxDist() const;


protected:
    //=======================================================================================
    // protected variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // protected methods.
    //=======================================================================================
    /* NOTHING */


private:
    //=======================================================================================
    // private variables
    //=======================================================================================
    Param           m_Param;        //!< 計算用カメラパラメータです.
    Param           m_Preset;       //!< リセット用カメラパラメータです.
    asdx::Matrix    m_View;         //!< ビュー行列です.

    //=======================================================================================
    // praviate methods.
    //=======================================================================================

    //---------------------------------------------------------------------------------------
    //! @brief      角度を計算します.
    //---------------------------------------------------------------------------------------
    void ComputeAngle();

    //---------------------------------------------------------------------------------------
    //! @brief      可動距離範囲内に制限します.
    //---------------------------------------------------------------------------------------
    void ClampDist();
};


} // namespace asdx

#endif//__ASDX_CAMERA_H__
