//--------------------------------------------------------------------------------------------
// File : asdxCamera.cpp
// Desc : Camera Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------
#include <asdxCamera.h>


namespace /* anonymos */ {

//--------------------------------------------------------------------------------------------
//      �p�x�����߂܂�.
//--------------------------------------------------------------------------------------------
static inline 
float GetAngle( float sin, float cos )
{
    float angle = asinf( sin );

    if ( cos < 0.0f )
    { angle = asdx::F_PI - angle; }

    return angle;
}

//--------------------------------------------------------------------------------------------
//      �w�肳�ꂽ�ʒu�ƃ^�[�Q�b�g����p�x�ɕϊ����܂�.
//--------------------------------------------------------------------------------------------
static inline 
void ToAngle
( 
    const asdx::FLOAT3& position,
    const asdx::FLOAT3& target,
    asdx::FLOAT2&       angle
)
{
    asdx::FLOAT3 dir = target - position;
    if ( dir.LengthSquared() != 0.0f )
    { dir.Normalize(); }

    angle.x = GetAngle( dir.x, dir.z );
    angle.y = GetAngle( dir.y, dir.x );
}

//--------------------------------------------------------------------------------------------
//      �w�肳�ꂽ�p�x�E�����E�^�[�Q�b�g����ʒu�ɕϊ����܂�.
//--------------------------------------------------------------------------------------------
static inline
void ToPosition
(
    const asdx::FLOAT2& angle,
    const float         dist,
    const asdx::FLOAT3& target,
    asdx::FLOAT3&       position
)
{
    register float sinH = sinf( angle.x );
    register float cosH = cosf( angle.x );

    register float sinV = sinf( angle.y );
    register float cosV = cosf( angle.y );

    position.x = target.x + dist * ( -cosV * sinH );
    position.y = target.y + dist * ( -sinV );
    position.z = target.z + dist * ( -cosV * cosH );
}

//--------------------------------------------------------------------------------------------
//      �w�肳�ꂽ�p�x����A�b�v�x�N�g���ɕϊ����܂�.
//--------------------------------------------------------------------------------------------
static inline
void ToUpward
(
    const asdx::FLOAT2& angle,
    asdx::FLOAT3&       upward
)
{
    register float sinH = sinf( angle.x );
    register float cosH = cosf( angle.x );

    register float sinV = sinf( angle.y );
    register float cosV = cosf( angle.y );

    upward.x = -sinV * sinH;
    upward.y =  cosV;
    upward.z = -sinV * cosH;
}

} // namespace /* anonymos */ 



namespace asdx {

//////////////////////////////////////////////////////////////////////////////////////////////
// Camera class
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//--------------------------------------------------------------------------------------------
Camera::Camera()
: m_Param()
, m_Preset()
{ m_View.Identity(); }

//--------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//--------------------------------------------------------------------------------------------
Camera::~Camera()
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------------
//      �ʒu��ݒ肵�܂�.
//--------------------------------------------------------------------------------------------
void Camera::SetPosition( const asdx::FLOAT3& position )
{
    m_Param.Position = position;

    ClampDist();
    ComputeAngle();
}

//--------------------------------------------------------------------------------------------
//      �����_��ݒ肵�܂�.
//--------------------------------------------------------------------------------------------
void Camera::SetTarget( const asdx::FLOAT3& target )
{
    m_Param.Target = target;

    ClampDist();
    ComputeAngle();
}

//--------------------------------------------------------------------------------------------
//      ������x�N�g����ݒ肵�܂�.
//--------------------------------------------------------------------------------------------
void Camera::SetUpward( const asdx::FLOAT3& upward )
{ m_Param.Upward = upward; }

//--------------------------------------------------------------------------------------------
//      �c�C�X�g�p��ݒ肵�܂�.
//--------------------------------------------------------------------------------------------
void Camera::SetTwist( const float twist )
{ m_Param.Twist = twist; }

//--------------------------------------------------------------------------------------------
//      �������͈͂�ݒ肵�܂�.
//--------------------------------------------------------------------------------------------
void Camera::SetRange( const float minDist, const float maxDist )
{
    m_Param.MinDist = minDist;
    m_Param.MaxDist = maxDist;

    ClampDist();
    ComputeAngle();
}

//--------------------------------------------------------------------------------------------
//      �p�����[�^��ۑ����܂�.
//--------------------------------------------------------------------------------------------
void Camera::Preset()
{ m_Preset = m_Param; }

//--------------------------------------------------------------------------------------------
//      �p�����[�^�����Z�b�g���܂�.
//--------------------------------------------------------------------------------------------
void Camera::Reset()
{
    m_Param = m_Preset;

    ClampDist();
    ComputeAngle();
}

//--------------------------------------------------------------------------------------------
//      �r���[�s����擾���܂�.
//--------------------------------------------------------------------------------------------
asdx::MATRIX Camera::GetView() const
{ return m_View; }

//--------------------------------------------------------------------------------------------
//      �ʒu���擾���܂�.
//--------------------------------------------------------------------------------------------
asdx::FLOAT3 Camera::GetPosition() const
{ return m_Param.Position; }

//--------------------------------------------------------------------------------------------
//      �����_���擾���܂�.
//--------------------------------------------------------------------------------------------
asdx::FLOAT3 Camera::GetTarget() const
{ return m_Param.Target; }

//--------------------------------------------------------------------------------------------
//      ������x�N�g�����擾���܂�.
//--------------------------------------------------------------------------------------------
asdx::FLOAT3 Camera::GetUpward() const
{ return m_Param.Upward; }

//--------------------------------------------------------------------------------------------
//      �c�C�X�g�p���擾���܂�.
//--------------------------------------------------------------------------------------------
float Camera::GetTwist() const
{ return m_Param.Twist; }

//--------------------------------------------------------------------------------------------
//      ���ŏ��������擾���܂�.
//--------------------------------------------------------------------------------------------
float Camera::GetMinDist() const
{ return m_Param.MinDist; }

//--------------------------------------------------------------------------------------------
//      ���ő勗�����擾���܂�.
//--------------------------------------------------------------------------------------------
float Camera::GetMaxDist() const
{ return m_Param.MaxDist; }

//--------------------------------------------------------------------------------------------
//      �p�x���v�Z���܂�.
//--------------------------------------------------------------------------------------------
void Camera::ComputeAngle()
{
    // ��]�p���v�Z.
    ToAngle( m_Param.Position, m_Param.Target, m_Param.Rotate );

    // �p���E�`���g�p���v�Z.
    ToAngle( m_Param.Target, m_Param.Position, m_Param.PanTilt );
}

//--------------------------------------------------------------------------------------------
//      �������͈͓��ɐ������܂�.
//--------------------------------------------------------------------------------------------
void Camera::ClampDist()
{
    // �������Z�o.
    float dist = asdx::Distance( m_Param.Position, m_Param.Target );

    // �����x�N�g��
    asdx::FLOAT3 dir = m_Param.Position - m_Param.Target;

    // �ő勗���𒴂��Ȃ��悤�ɐ���.
    if ( dist > m_Param.MaxDist )
    {
        // �[�����Z�΍�.
        if ( dir.LengthSquared() != 0.0 )
        { dir.Normalize(); }

        m_Param.Position = m_Param.Target + dir * m_Param.MaxDist;
    }

    // �ŏ������������Ȃ��悤�ɐ���.
    if ( dist < m_Param.MinDist )
    {
        // �[�����Z�΍�.
        if ( dir.LengthSquared() != 0.0f )
        { dir.Normalize(); }

        m_Param.Position = m_Param.Target + dir * m_Param.MinDist;
    }
}

//--------------------------------------------------------------------------------------------
//      �r���[�s����X�V���܂�.
//--------------------------------------------------------------------------------------------
void Camera::Update()
{
    asdx::FLOAT3 upward = m_Param.Upward;

    // �c�C�X�g�p���[���łȂ��ꍇ.
    if ( m_Param.Twist != 0.0f )
    {
        // �����x�N�g�����쐬.
        asdx::FLOAT3 dir = m_Param.Target - m_Param.Position;
        if ( dir.LengthSquared() != 0.0f )
        { dir.Normalize(); }

        // �����x�N�g�����Ƃ�����]�s����쐬.
        asdx::MATRIX rotate = asdx::CreateFromAxisAngle( dir, m_Param.Twist );

        // �A�b�v�x�N�g������].
        upward = asdx::Transform( upward, rotate );
    }

    // �r���[�s����X�V.
    m_View = asdx::CreateLookAt( m_Param.Position, m_Param.Target, upward );
}

//--------------------------------------------------------------------------------------------
//      �J�����C�x���g����Ƀr���[�s����X�V���܂�.
//--------------------------------------------------------------------------------------------
void Camera::UpdateByEvent( const CameraEvent& camEvent )
{
    // ��]����.
    if ( camEvent.Flags & CameraEvent::EVENT_ROTATE )
    {
        // ��]�p�����Z.
        m_Param.Rotate += camEvent.Rotate;

        // �^�[�Q�b�g�܂ł̋������Z�o.
        float dist = asdx::Distance( m_Param.Position, m_Param.Target );

        // �ʒu�x�N�g�����X�V.
        ToPosition( m_Param.Rotate, dist, m_Param.Target, m_Param.Position );

        // �A�b�v�x�N�g�����X�V.
        ToUpward( m_Param.Rotate, m_Param.Upward );
    }

    // �h���[����.
    if ( camEvent.Flags & CameraEvent::EVENT_DOLLY )
    {
        // �����x�N�g�����쐬.
        asdx::FLOAT3 dir = m_Param.Position - m_Param.Target;
        float dist = asdx::Distance( m_Param.Position, m_Param.Target );

        // ���K��.
        if ( dist != 0.0f )
        {
            float invDist = 1.0f / dist;
            dir.x *= invDist;
            dir.y *= invDist;
            dir.z *= invDist;
        }

        // �h���[�ʂ����Z.
        dist += camEvent.Dolly;

        // �������͈͓��ɐ���.
        if ( m_Param.MinDist > dist )
        { dist = m_Param.MinDist; }
        if ( m_Param.MaxDist < dist )
        { dist = m_Param.MaxDist; }

        // �ʒu�x�N�g�����X�V.
        m_Param.Position = m_Param.Target + ( dir * dist );
    }

    // �g���b�N����.
    if ( camEvent.Flags & CameraEvent::EVENT_TRUCK )
    {
        m_Param.Position += camEvent.Truck;
        m_Param.Target   += camEvent.Truck;
    }

    // �p���E�`���g����.
    if ( camEvent.Flags & CameraEvent::EVENT_PANTILT )
    {
        // �p���E�`���g�p�����Z.
        m_Param.PanTilt += camEvent.PanTilt;

        // �^�[�Q�b�g�Ƃ̋������Z�o.
        float dist = asdx::Distance( m_Param.Target, m_Param.Position );

        // �^�[�Q�b�g�ʒu���X�V.
        ToPosition( m_Param.PanTilt, dist, m_Param.Position, m_Param.Target );

        // �A�b�v�x�N�g�����X�V.
        ToUpward( m_Param.PanTilt, m_Param.Upward );
    }

    // �c�C�X�g����.
    if ( camEvent.Flags & CameraEvent::EVENT_TWIST )
    { m_Param.Twist += camEvent.Twist; }

    // ���Z�b�g����.
    if ( camEvent.Flags & CameraEvent::EVENT_RESET )
    { Reset(); }

    // �r���[�s����X�V.
    Update();
}


} // namespace asdx

