//------------------------------------------------------------------------------
// File : s3d_ray.inl
// Desc : Ray Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __S3D_RAY_INL__
#define __S3D_RAY_INL__

namespace s3d {

S3D_INLINE
Ray::Ray()
{ /* DO_NOTHING */ }

S3D_INLINE
Ray::Ray( const Vector3& position, const Vector3& direction )
: pos( position )
, dir( direction )
{ /* DO_NOTHING */ }

S3D_INLINE
Ray::Ray( const Ray& value )
: pos( value.pos )
, dir( value.dir )
{ /* DO_NOTHING */ }

S3D_INLINE
void Ray::SetPos( const Vector3& value )
{ pos = value; }

S3D_INLINE
void Ray::SetDir( const Vector3& value )
{
    dir = value;
    invDir.x = 1.0 / value.x;
    invDir.y = 1.0 / value.y;
    invDir.z = 1.0 / value.z;

    signX = ( dir.x > 0.0 ) ? 0 : 1;
    signY = ( dir.y > 0.0 ) ? 0 : 1;
    signZ = ( dir.z > 0.0 ) ? 0 : 1;
}

S3D_INLINE
Vector3 Ray::GetPos() const
{ return pos; }

S3D_INLINE
Vector3 Ray::GetDir() const
{ return dir; }

S3D_INLINE
Vector3 Ray::GetInvDir() const
{ return invDir; }

S3D_INLINE
s32 Ray::GetSignX() const
{ return signX; }

S3D_INLINE
s32 Ray::GetSignY() const
{ return signY; }

S3D_INLINE
s32 Ray::GetSignZ() const
{ return signZ; }

S3D_INLINE
Vector3 Ray::Step( const f64 dist ) const
{ return pos + dir * dist; }

S3D_INLINE
Ray& Ray::operator = ( const Ray& value )
{
    pos    = value.pos;
    dir    = value.dir;
    invDir = value.invDir;
    signX  = value.signX;
    signY  = value.signY;
    signZ  = value.signZ;
    return (*this);
}

} // namespace s3d

#endif//__S3D_RAY_INL__
