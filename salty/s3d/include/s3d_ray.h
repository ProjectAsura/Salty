//------------------------------------------------------------------------------
// File : s3d_ray.h
// Desc : Ray Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __S3D_RAY_H__
#define __S3D_RAY_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <s3d_vector3.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////
// Ray structure
////////////////////////////////////////////////////////////////////////////////
struct Ray
{
public:
    Ray();
    Ray( const Vector3&, const Vector3& );
    Ray( const Ray& );

    void    SetPos      ( const Vector3& );
    void    SetDir      ( const Vector3& );
    Vector3 GetPos      ( void ) const;
    Vector3 GetDir      ( void ) const;
    Vector3 GetInvDir   ( void ) const;
    s32     GetSignX    ( void ) const;
    s32     GetSignY    ( void ) const;
    s32     GetSignZ    ( void ) const;
    Vector3 Step        ( const f64 ) const;

    Ray& operator = ( const Ray& );

private:
    Vector3 pos;
    Vector3 dir;
    Vector3 invDir;
    s32     signX;
    s32     signY;
    s32     signZ;

};

} // namespace s3d


//------------------------------------------------------------------------------
// Inlines
//------------------------------------------------------------------------------
#include <s3d_ray.inl>

#endif//__S3D_RAY_H__
