//-------------------------------------------------------------------------------------
// File : s3d_onb.h
// Desc : Orthonomral Basis Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------

#ifndef __S3D_ONB_H__
#define __S3D_ONB_H__

//-------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------
#include <s3d_vector3.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////
// OrthonormalBasis structure
///////////////////////////////////////////////////////////////////////////////////////
struct OrthonormalBasis
{
public:
    Vector3 u;
    Vector3 v;
    Vector3 w;

    OrthonormalBasis();
    OrthonormalBasis( const Vector3&, const Vector3&, const Vector3& );

    void InitFromU( const Vector3& );
    void InitFromV( const Vector3& );
    void InitFromW( const Vector3& );

    void InitFromUV( const Vector3&, const Vector3& );
    void InitFromVU( const Vector3&, const Vector3& );

    void InitFromUW( const Vector3&, const Vector3& );
    void InitFromWU( const Vector3&, const Vector3& );

    void InitFromVW( const Vector3&, const Vector3& );
    void InitFromWV( const Vector3&, const Vector3& );

    bool operator == ( const OrthonormalBasis& ) const;
    bool operator != ( const OrthonormalBasis& ) const;
};

} // namespace s3d


#endif//__S3D_ONB_H__
