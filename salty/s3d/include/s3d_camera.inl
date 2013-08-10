//-----------------------------------------------------------------------------
// File : s3d_camera.inl
// Desc : Camera Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __S3D_CAMERA_INL__
#define __S3D_CAMERA_INL__

namespace s3d {

////////////////////////////////////////////////////////////////////////////////
// Camera class
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//      コンストラクタです.
//------------------------------------------------------------------------------
S3D_INLINE
Camera::Camera()
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------
//      コピーコンストラクタです.
//------------------------------------------------------------------------------
S3D_INLINE
Camera::Camera( const Camera& value )
: m_Position    ( value.m_Position )
, m_Target      ( value.m_Target )
, m_Upward      ( value.m_Upward )
, m_FieldOfView ( value.m_FieldOfView )
, m_AspectRatio ( value.m_AspectRatio )
, m_NearClip    ( value.m_NearClip )
, m_FarClip     ( value.m_FarClip )
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------
//      更新処理を行います.
//------------------------------------------------------------------------------
S3D_INLINE
void Camera::Update
(
    const Vector3&  position,
    const Vector3&  target,
    const Vector3&  upward,
    const f64       fieldOfView,
    const f64       aspectRatio,
    const f64       nearClip,
    const f64       farClip
)
{
    m_Position = position;
    m_Target   = target;
    m_Upward   = upward;

    m_FieldOfView = fieldOfView;
    m_AspectRatio = aspectRatio;
    m_NearClip    = nearClip;
    m_FarClip     = farClip;

    m_View = Matrix::LookAt( position, target, upward );
    m_Proj = Matrix::PersFov( fieldOfView, aspectRatio, nearClip, farClip );

    Matrix viewProj = m_View * m_Proj;
    m_InvViewProj = Matrix::Invert( viewProj );
}

//------------------------------------------------------------------------------
//      レイを取得します.
//------------------------------------------------------------------------------
S3D_INLINE
Ray Camera::GetRay( const f64 x, const f64 y )
{
    register f64 X = ( x * m_InvViewProj._11 ) + ( y * m_InvViewProj._21 ) + m_InvViewProj._41;
    register f64 Y = ( x * m_InvViewProj._12 ) + ( y * m_InvViewProj._22 ) + m_InvViewProj._42;
    register f64 Z = ( x * m_InvViewProj._13 ) + ( y * m_InvViewProj._23 ) + m_InvViewProj._43;
    register f64 W = ( x * m_InvViewProj._14 ) + ( y * m_InvViewProj._24 ) + m_InvViewProj._44;

    Vector3 pos( X / W, Y / W, Z / W );
    Vector3 dir = Vector3::UnitVector( pos - m_Position);

    return Ray( m_Position, dir );
}

} // namespace s3d


#endif//__S3D_CAMERA_INL__