﻿//---------------------------------------------------------------------------
// File : s3d_quad.h
// Desc : Quad Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------

#ifndef __S3D_QUAD_H__
#define __S3D_QUAD_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <s3d_shape.h>


namespace s3d {

/////////////////////////////////////////////////////////////////////////////
// Quad class 
/////////////////////////////////////////////////////////////////////////////
class Quad : public IShape
{
    //=======================================================================
    // list of friend classes and methods.
    //=======================================================================
    /* NOTHING */

public:
    //=======================================================================
    // public variables.
    //=======================================================================
    Vector3     p0;
    Vector3     p1;
    Vector3     p2;
    Vector3     p3;
    Vector3     normal;
    IMaterial*  pMaterial;

    //=======================================================================
    // public methods.
    //=======================================================================
    Quad( const Vector3&, const Vector3&, const Vector3&, const Vector3&, IMaterial* );
    Quad( const Quad& );

    bool        IsHit  ( const Ray&, const f64, const f64, ShadeRec& );
    BoundingBox GetAABB() const;

protected:
    //=======================================================================
    // protected variables.
    //=======================================================================
    /* NOTHING */

    //=======================================================================
    // protected methods.
    //=======================================================================
    Vector3 ComputeNormal( const Vector3&, const Vector3&, const Vector3& );
    void ComputeQuadNormal();
    bool IsHitTriangle(
        const Ray&,
        const Vector3&,
        const Vector3&,
        const Vector3&,
        const f64,
        const f64,
        ShadeRec& );

private:
    //=======================================================================
    // private variables.
    //=======================================================================
    /* NOTHING */

    //=======================================================================
    // private methods.
    //=======================================================================
    /* NOTHING */
};

} // namespace s3d

#endif//__S3D_QUAD_H__