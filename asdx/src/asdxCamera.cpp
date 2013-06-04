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
//      角度を求めます.
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
//      指定された位置とターゲットから角度に変換します.
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
//      指定された角度・距離・ターゲットから位置に変換します.
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
//      指定された角度からアップベクトルに変換します.
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
//      コンストラクタです.
//--------------------------------------------------------------------------------------------
Camera::Camera()
: m_Param()
, m_Preset()
{ m_View.Identity(); }

//--------------------------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------------------------
Camera::~Camera()
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------------
//      位置を設定します.
//--------------------------------------------------------------------------------------------
void Camera::SetPosition( const asdx::FLOAT3& position )
{
    m_Param.Position = position;

    ClampDist();
    ComputeAngle();
}

//--------------------------------------------------------------------------------------------
//      注視点を設定します.
//--------------------------------------------------------------------------------------------
void Camera::SetTarget( const asdx::FLOAT3& target )
{
    m_Param.Target = target;

    ClampDist();
    ComputeAngle();
}

//--------------------------------------------------------------------------------------------
//      上向きベクトルを設定します.
//--------------------------------------------------------------------------------------------
void Camera::SetUpward( const asdx::FLOAT3& upward )
{ m_Param.Upward = upward; }

//--------------------------------------------------------------------------------------------
//      ツイスト角を設定します.
//--------------------------------------------------------------------------------------------
void Camera::SetTwist( const float twist )
{ m_Param.Twist = twist; }

//--------------------------------------------------------------------------------------------
//      可動距離範囲を設定します.
//--------------------------------------------------------------------------------------------
void Camera::SetRange( const float minDist, const float maxDist )
{
    m_Param.MinDist = minDist;
    m_Param.MaxDist = maxDist;

    ClampDist();
    ComputeAngle();
}

//--------------------------------------------------------------------------------------------
//      パラメータを保存します.
//--------------------------------------------------------------------------------------------
void Camera::Preset()
{ m_Preset = m_Param; }

//--------------------------------------------------------------------------------------------
//      パラメータをリセットします.
//--------------------------------------------------------------------------------------------
void Camera::Reset()
{
    m_Param = m_Preset;

    ClampDist();
    ComputeAngle();
}

//--------------------------------------------------------------------------------------------
//      ビュー行列を取得します.
//--------------------------------------------------------------------------------------------
asdx::MATRIX Camera::GetView() const
{ return m_View; }

//--------------------------------------------------------------------------------------------
//      位置を取得します.
//--------------------------------------------------------------------------------------------
asdx::FLOAT3 Camera::GetPosition() const
{ return m_Param.Position; }

//--------------------------------------------------------------------------------------------
//      注視点を取得します.
//--------------------------------------------------------------------------------------------
asdx::FLOAT3 Camera::GetTarget() const
{ return m_Param.Target; }

//--------------------------------------------------------------------------------------------
//      上向きベクトルを取得します.
//--------------------------------------------------------------------------------------------
asdx::FLOAT3 Camera::GetUpward() const
{ return m_Param.Upward; }

//--------------------------------------------------------------------------------------------
//      ツイスト角を取得します.
//--------------------------------------------------------------------------------------------
float Camera::GetTwist() const
{ return m_Param.Twist; }

//--------------------------------------------------------------------------------------------
//      可動最小距離を取得します.
//--------------------------------------------------------------------------------------------
float Camera::GetMinDist() const
{ return m_Param.MinDist; }

//--------------------------------------------------------------------------------------------
//      可動最大距離を取得します.
//--------------------------------------------------------------------------------------------
float Camera::GetMaxDist() const
{ return m_Param.MaxDist; }

//--------------------------------------------------------------------------------------------
//      角度を計算します.
//--------------------------------------------------------------------------------------------
void Camera::ComputeAngle()
{
    // 回転角を計算.
    ToAngle( m_Param.Position, m_Param.Target, m_Param.Rotate );

    // パン・チルト角を計算.
    ToAngle( m_Param.Target, m_Param.Position, m_Param.PanTilt );
}

//--------------------------------------------------------------------------------------------
//      可動距離範囲内に制限します.
//--------------------------------------------------------------------------------------------
void Camera::ClampDist()
{
    // 距離を算出.
    float dist = asdx::Distance( m_Param.Position, m_Param.Target );

    // 方向ベクトル
    asdx::FLOAT3 dir = m_Param.Position - m_Param.Target;

    // 最大距離を超えないように制限.
    if ( dist > m_Param.MaxDist )
    {
        // ゼロ除算対策.
        if ( dir.LengthSquared() != 0.0 )
        { dir.Normalize(); }

        m_Param.Position = m_Param.Target + dir * m_Param.MaxDist;
    }

    // 最小距離を下回らないように制限.
    if ( dist < m_Param.MinDist )
    {
        // ゼロ除算対策.
        if ( dir.LengthSquared() != 0.0f )
        { dir.Normalize(); }

        m_Param.Position = m_Param.Target + dir * m_Param.MinDist;
    }
}

//--------------------------------------------------------------------------------------------
//      ビュー行列を更新します.
//--------------------------------------------------------------------------------------------
void Camera::Update()
{
    asdx::FLOAT3 upward = m_Param.Upward;

    // ツイスト角がゼロでない場合.
    if ( m_Param.Twist != 0.0f )
    {
        // 視線ベクトルを作成.
        asdx::FLOAT3 dir = m_Param.Target - m_Param.Position;
        if ( dir.LengthSquared() != 0.0f )
        { dir.Normalize(); }

        // 視線ベクトル軸とした回転行列を作成.
        asdx::MATRIX rotate = asdx::CreateFromAxisAngle( dir, m_Param.Twist );

        // アップベクトルを回転.
        upward = asdx::Transform( upward, rotate );
    }

    // ビュー行列を更新.
    m_View = asdx::CreateLookAt( m_Param.Position, m_Param.Target, upward );
}

//--------------------------------------------------------------------------------------------
//      カメライベントを基にビュー行列を更新します.
//--------------------------------------------------------------------------------------------
void Camera::UpdateByEvent( const CameraEvent& camEvent )
{
    // 回転処理.
    if ( camEvent.Flags & CameraEvent::EVENT_ROTATE )
    {
        // 回転角を加算.
        m_Param.Rotate += camEvent.Rotate;

        // ターゲットまでの距離を算出.
        float dist = asdx::Distance( m_Param.Position, m_Param.Target );

        // 位置ベクトルを更新.
        ToPosition( m_Param.Rotate, dist, m_Param.Target, m_Param.Position );

        // アップベクトルを更新.
        ToUpward( m_Param.Rotate, m_Param.Upward );
    }

    // ドリー処理.
    if ( camEvent.Flags & CameraEvent::EVENT_DOLLY )
    {
        // 視線ベクトルを作成.
        asdx::FLOAT3 dir = m_Param.Position - m_Param.Target;
        float dist = asdx::Distance( m_Param.Position, m_Param.Target );

        // 正規化.
        if ( dist != 0.0f )
        {
            float invDist = 1.0f / dist;
            dir.x *= invDist;
            dir.y *= invDist;
            dir.z *= invDist;
        }

        // ドリー量を加算.
        dist += camEvent.Dolly;

        // 可動距離範囲内に制限.
        if ( m_Param.MinDist > dist )
        { dist = m_Param.MinDist; }
        if ( m_Param.MaxDist < dist )
        { dist = m_Param.MaxDist; }

        // 位置ベクトルを更新.
        m_Param.Position = m_Param.Target + ( dir * dist );
    }

    // トラック処理.
    if ( camEvent.Flags & CameraEvent::EVENT_TRUCK )
    {
        m_Param.Position += camEvent.Truck;
        m_Param.Target   += camEvent.Truck;
    }

    // パン・チルト処理.
    if ( camEvent.Flags & CameraEvent::EVENT_PANTILT )
    {
        // パン・チルト角を加算.
        m_Param.PanTilt += camEvent.PanTilt;

        // ターゲットとの距離を算出.
        float dist = asdx::Distance( m_Param.Target, m_Param.Position );

        // ターゲット位置を更新.
        ToPosition( m_Param.PanTilt, dist, m_Param.Position, m_Param.Target );

        // アップベクトルを更新.
        ToUpward( m_Param.PanTilt, m_Param.Upward );
    }

    // ツイスト処理.
    if ( camEvent.Flags & CameraEvent::EVENT_TWIST )
    { m_Param.Twist += camEvent.Twist; }

    // リセット処理.
    if ( camEvent.Flags & CameraEvent::EVENT_RESET )
    { Reset(); }

    // ビュー行列を更新.
    Update();
}


} // namespace asdx

