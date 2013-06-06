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
//      �R���X�g���N�^�ł�.
//------------------------------------------------------------------------------------------
CameraUpdater::CameraUpdater()
: m_Camera      ()
, m_MouseLeft   ()
, m_MouseRight  ()
, m_MouseMiddle ()
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//------------------------------------------------------------------------------------------
CameraUpdater::~CameraUpdater()
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------------------
//      �����ݒ���s���܂�.
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
//      �}�E�X�̏����ł�.
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
//      �L�[�̏����ł�.
//------------------------------------------------------------------------------------------
void CameraUpdater::OnKey( unsigned int nChar, bool isKeyDown, bool isAltDown )
{
    CameraEvent e = MakeEventFromKey( nChar, isKeyDown, isAltDown );
    m_Camera.UpdateByEvent( e );
}

//------------------------------------------------------------------------------------------
//      �J�������擾���܂�.
//------------------------------------------------------------------------------------------
Camera& CameraUpdater::GetCamera()
{ return m_Camera; }

//------------------------------------------------------------------------------------------
//      �r���[�s����擾���܂�.
//------------------------------------------------------------------------------------------
asdx::MATRIX CameraUpdater::GetView() const
{ return m_Camera.GetView(); }

//------------------------------------------------------------------------------------------
//      �}�E�X���͂���J�����C�x���g�𐶐����܂�.
//------------------------------------------------------------------------------------------
CameraEvent CameraUpdater::MakeEventFromMouse( int wheelDelta )
{
    CameraEvent result;
    unsigned int flags = 0;
    const float dollyGain = 0.1f;
    const float angleGain = 0.01f;
    const float moveGain  = 1.0f;
    const float wheelGain = 20.0f;

    // �z�C�[���Ńh���[.
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

    // ���{�^���h���b�O�ŉ�]����.
    if ( m_MouseLeft.isClick && ( !m_MouseRight.isClick ) && ( !m_MouseMiddle.isClick ) )
    {
        flags |= CameraEvent::EVENT_ROTATE;

        // ��������������]�ʂƂ���.
        result.Rotate.x = ( m_MouseLeft.X - m_MouseLeft.prevX ) * angleGain;
        result.Rotate.y = ( m_MouseLeft.Y - m_MouseLeft.prevY ) * angleGain;
    }

    // �E�{�^���h���b�O�ŉ�]����.
    if ( ( !m_MouseLeft.isClick ) && m_MouseRight.isClick && ( !m_MouseMiddle.isClick ) )
    {
        flags |= CameraEvent::EVENT_DOLLY;

        // �������������h���[�ʂƂ���.
        result.Dolly = ( m_MouseRight.Y - m_MouseRight.prevY ) * dollyGain;
    }

    // ���{�E�{�^���h���b�O�Ńp���E�`���g����.
    if ( m_MouseLeft.isClick && m_MouseRight.isClick )
    {
        flags |= CameraEvent::EVENT_PANTILT;

        // �������������p���E�`���g�p�Ƃ���.
        result.PanTilt.x = ( m_MouseLeft.X - m_MouseLeft.prevX ) * angleGain;
        result.PanTilt.y = ( m_MouseLeft.Y - m_MouseLeft.prevY ) * angleGain;
    }

    // ���{�^���h���b�O�Ńg���b�N����.
    if ( ( !m_MouseLeft.isClick ) && ( !m_MouseRight.isClick ) && m_MouseMiddle.isClick )
    {
        flags |= CameraEvent::EVENT_TRUCK;

        // �����x�N�g���Z�o.
        asdx::FLOAT3 dir = m_Camera.GetTarget() - m_Camera.GetPosition();
        if ( dir.LengthSquared() != 0.0f )
        { dir.Normalize(); }

        // �E�x�N�g���Z�o.
        asdx::FLOAT3 right = asdx::Cross( m_Camera.GetUpward(), dir );
        if ( right.LengthSquared() != 0.0f )
        { right.Normalize(); }

        asdx::FLOAT3 upward = m_Camera.GetUpward();

        // �������������Z�o.
        float rightGain  = ( m_MouseMiddle.X - m_MouseMiddle.prevX ) * moveGain;
        float upwardGain = ( m_MouseMiddle.Y - m_MouseMiddle.prevY ) * moveGain;

        // �W����������.
        right.x  *= rightGain;
        right.y  *= rightGain;
        right.z  *= rightGain;
        upward.x *= upwardGain;
        upward.y *= upwardGain;
        upward.z *= upwardGain;

        // �J������Ԃŕ��s�ɓ�����.
        result.Truck.x = ( right.x + upward.x );
        result.Truck.y = ( right.y + upward.y );
        result.Truck.z = ( right.z + upward.z );
    }

    // ���{���{�^���h���b�O�Ńc�C�X�g����.
    if ( m_MouseLeft.isClick && ( !m_MouseRight.isClick ) && m_MouseMiddle.isClick )
    {
        flags |= CameraEvent::EVENT_TWIST;

        // �����������ʂ��c�C�X�g�p�Ƃ���.
        result.Twist = ( m_MouseLeft.X - m_MouseLeft.prevX ) * angleGain;
    }

    // �t���O��ݒ�.
    result.Flags = flags;

    return result;
}

//------------------------------------------------------------------------------------------
//      �L�[���͂���J�����C�x���g�𐶐�����.
//------------------------------------------------------------------------------------------
CameraEvent CameraUpdater::MakeEventFromKey( unsigned int nChar, bool isKeyDown, bool isAltDown )
{
    CameraEvent result;

    switch( nChar )
    {
    // R�L�[�Ń��Z�b�g.
    case 0x52:
        if ( isKeyDown )
        { result.Flags |= CameraEvent::EVENT_RESET; }
        break;

    // T�L�[�Ńc�C�X�g
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