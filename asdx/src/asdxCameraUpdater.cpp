//------------------------------------------------------------------------------------------
// File : asdxCameraUpdater.cpp
// Desc : Camera Update Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <asdxCameraUpdater.h>


namespace asdx {

////////////////////////////////////////////////////////////////////////////////////////////
// CameraUpdate class
////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------
//      コンストラクタです.
//------------------------------------------------------------------------------------------
CameraUpdater::CameraUpdater()
: m_Camera      ()
, m_MouseLeft   ()
, m_MouseRight  ()
, m_MouseMiddle ()
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------------------
//      デストラクタです.
//------------------------------------------------------------------------------------------
CameraUpdater::~CameraUpdater()
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------------------
//      初期設定を行います.
//------------------------------------------------------------------------------------------
void CameraUpdater::Init
(
    asdx::FLOAT3 cameraPos,
    asdx::FLOAT3 cameraAim,
    asdx::FLOAT3 cameraUp,
    float        nearClip,
    float        farClip
)
{
    m_Camera.SetPosition( cameraPos );
    m_Camera.SetTarget( cameraAim );
    m_Camera.SetUpward( cameraUp );
    m_Camera.SetRange( nearClip, farClip );

    m_Camera.Preset();
    m_Camera.Update();
}

//------------------------------------------------------------------------------------------
//      マウスの処理です.
//------------------------------------------------------------------------------------------
void CameraUpdater::OnMouse
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
    m_MouseLeft  .Update( x, y, isLeftButtonDown );
    m_MouseRight .Update( x, y, isRightButtonDown );
    m_MouseMiddle.Update( x, y, isMiddleButtonDown );

    CameraEvent e = MakeEventFromMouse( wheelDelta );
    m_Camera.UpdateByEvent( e );
}

//------------------------------------------------------------------------------------------
//      キーの処理です.
//------------------------------------------------------------------------------------------
void CameraUpdater::OnKey( unsigned int nChar, bool isKeyDown, bool isAltDown )
{
    CameraEvent e = MakeEventFromKey( nChar, isKeyDown, isAltDown );
    m_Camera.UpdateByEvent( e );
}

//------------------------------------------------------------------------------------------
//      カメラを取得します.
//------------------------------------------------------------------------------------------
Camera& CameraUpdater::GetCamera()
{ return m_Camera; }

//------------------------------------------------------------------------------------------
//      ビュー行列を取得します.
//------------------------------------------------------------------------------------------
asdx::MATRIX CameraUpdater::GetView() const
{ return m_Camera.GetView(); }

//------------------------------------------------------------------------------------------
//      マウス入力からカメライベントを生成します.
//------------------------------------------------------------------------------------------
CameraEvent CameraUpdater::MakeEventFromMouse( int wheelDelta )
{
    CameraEvent result;
    unsigned int flags = 0;
    const float dollyGain = 0.1f;
    const float angleGain = 0.01f;
    const float moveGain  = 1.0f;
    const float wheelGain = 20.0f;

    // ホイールでドリー.
    if ( wheelDelta > 0 )
    {
        flags |= CameraEvent::EVENT_DOLLY;
        result.Dolly = wheelGain;
    }
    else if ( wheelDelta < 0 )
    {
        flags |= CameraEvent::EVENT_DOLLY;
        result.Dolly = -wheelGain;
    }

    // 左ボタンドラッグで回転処理.
    if ( m_MouseLeft.isClick && ( !m_MouseRight.isClick ) && ( !m_MouseMiddle.isClick ) )
    {
        flags |= CameraEvent::EVENT_ROTATE;

        // 動いた差分を回転量とする.
        result.Rotate.x = ( m_MouseLeft.X - m_MouseLeft.prevX ) * angleGain;
        result.Rotate.y = ( m_MouseLeft.Y - m_MouseLeft.prevY ) * angleGain;
    }

    // 右ボタンドラッグで回転処理.
    if ( ( !m_MouseLeft.isClick ) && m_MouseRight.isClick && ( !m_MouseMiddle.isClick ) )
    {
        flags |= CameraEvent::EVENT_DOLLY;

        // 動いた差分をドリー量とする.
        result.Dolly = ( m_MouseRight.Y - m_MouseRight.prevY ) * dollyGain;
    }

    // 左＋右ボタンドラッグでパン・チルト処理.
    if ( m_MouseLeft.isClick && m_MouseRight.isClick )
    {
        flags |= CameraEvent::EVENT_PANTILT;

        // 動いた差分をパン・チルト角とする.
        result.PanTilt.x = ( m_MouseLeft.X - m_MouseLeft.prevX ) * angleGain;
        result.PanTilt.y = ( m_MouseLeft.Y - m_MouseLeft.prevY ) * angleGain;
    }

    // 中ボタンドラッグでトラック処理.
    if ( ( !m_MouseLeft.isClick ) && ( !m_MouseRight.isClick ) && m_MouseMiddle.isClick )
    {
        flags |= CameraEvent::EVENT_TRUCK;

        // 方向ベクトル算出.
        asdx::FLOAT3 dir = m_Camera.GetTarget() - m_Camera.GetPosition();
        if ( dir.LengthSquared() != 0.0f )
        { dir.Normalize(); }

        // 右ベクトル算出.
        asdx::FLOAT3 right = asdx::Cross( m_Camera.GetUpward(), dir );
        if ( right.LengthSquared() != 0.0f )
        { right.Normalize(); }

        asdx::FLOAT3 upward = m_Camera.GetUpward();

        // 動いた差分を算出.
        float rightGain  = ( m_MouseMiddle.X - m_MouseMiddle.prevX ) * moveGain;
        float upwardGain = ( m_MouseMiddle.Y - m_MouseMiddle.prevY ) * moveGain;

        // 係数をかける.
        right.x  *= rightGain;
        right.y  *= rightGain;
        right.z  *= rightGain;
        upward.x *= upwardGain;
        upward.y *= upwardGain;
        upward.z *= upwardGain;

        // カメラ空間で平行に動かす.
        result.Truck.x = ( right.x + upward.x );
        result.Truck.y = ( right.y + upward.y );
        result.Truck.z = ( right.z + upward.z );
    }

    // 左＋中ボタンドラッグでツイスト処理.
    if ( m_MouseLeft.isClick && ( !m_MouseRight.isClick ) && m_MouseMiddle.isClick )
    {
        flags |= CameraEvent::EVENT_TWIST;

        // 動いた差分量をツイスト角とする.
        result.Twist = ( m_MouseLeft.X - m_MouseLeft.prevX ) * angleGain;
    }

    // フラグを設定.
    result.Flags = flags;

    return result;
}

//------------------------------------------------------------------------------------------
//      キー入力からカメライベントを生成する.
//------------------------------------------------------------------------------------------
CameraEvent CameraUpdater::MakeEventFromKey( unsigned int nChar, bool isKeyDown, bool isAltDown )
{
    CameraEvent result;

    switch( nChar )
    {
    // Rキーでリセット.
    case 0x52:
        if ( isKeyDown )
        { result.Flags |= CameraEvent::EVENT_RESET; }
        break;

    // Tキーでツイスト
    case 0x54:
        if ( isAltDown && isKeyDown )
        {
            result.Flags |= CameraEvent::EVENT_TWIST;
            result.Twist = -0.1f;
        }
        else if ( !isAltDown && isKeyDown )
        {
            result.Flags |= CameraEvent::EVENT_TWIST;
            result.Twist = 0.1f;
        }
        break;
    }

    return result;
}

} // namespace asdx