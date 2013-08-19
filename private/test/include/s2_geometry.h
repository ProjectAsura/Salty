//---------------------------------------------------------------------------------------
// File : s2_geometry.h
// Desc : Geometry Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------

#ifndef __S2_GEOMETRY_H__
#define __S2_GEOMETRY_H__


//---------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------
#include "s2_math.h"
#include "s2_material.h"


//---------------------------------------------------------------------------------------
// Forward Declarations.
//---------------------------------------------------------------------------------------
struct HITRECORD;


//---------------------------------------------------------------------------------------
//! @brief      形状インタフェースです.
//---------------------------------------------------------------------------------------
struct ISHAPE 
{
    virtual bool        Intersect( const RAY& ray, HITRECORD& record ) = 0;
    virtual MATERIAL*   GetMaterial() = 0;
};


//---------------------------------------------------------------------------------------
//! @brief      衝突判定の結果を記録する構造体です.
//---------------------------------------------------------------------------------------
struct HITRECORD
{
    ISHAPE*     pShape;
    double      distance;
    VEC3        position;
    VEC3        normal;

    HITRECORD()
    : pShape    ( nullptr )
    , distance  ( DBL_MAX )
    , normal    ( 0.0, 0.0, 0.0 )
    { /* DO_NOTHING */ }

    HITRECORD(
        ISHAPE* pShape_,
        double  distance_,
        VEC3    position_,
        VEC3    normal_
    )
    : pShape    ( pShape_ )
    , distance  ( distance_ )
    , position  ( position_ )
    , normal    ( normal_ )
    { /* DO_NOTHING */ }
};


//--------------------------------------------------------------------------------------
//! @brief      球構造体です.
//--------------------------------------------------------------------------------------
struct SPHERE : ISHAPE
{
    double          radius;
    VEC3            position;
    MATERIAL*       pMaterial;

    SPHERE()
    : radius    ( 0.0 )
    , pMaterial ( nullptr )
    { /* DO_NOTHING */ }

    SPHERE(
        const double radius_,
        const VEC3&  position_,
        MATERIAL*    material_
    )
    : radius    ( radius_ )
    , position  ( position_ )
    , pMaterial ( material_ )
    { /* DO_NOTHING */ }

    bool Intersect( const RAY &ray, HITRECORD& record )
    {
        VEC3 p           = position - ray.org;
        const double b   = Dot( p, ray.dir );
        const double det = b * b - Dot( p, p ) + radius * radius;
        const double EPS = 1e-6;

        if ( det >= 0.0 ) 
        {
            const double sqrt_det = sqrt(det);
            const double t1       = b - sqrt_det;
            const double t2       = b + sqrt_det;

            if ( t1 > EPS )
            {
                record.position = ray.org + t1 * ray.dir;
                record.distance = t1;
                record.normal   = Normalize( record.position - position );
                record.pShape   = this;
                return true;
            }
            
            if( t2 > EPS )
            {
                record.position = ray.org + t2 * ray.dir;
                record.distance = t2;
                record.normal   = Normalize( record.position - position );
                record.pShape   = this;
                return true;
            }
        }

        return false;
    }

    MATERIAL* GetMaterial()
    { return pMaterial; }
};

//--------------------------------------------------------------------------------------
//! @brief      三角形構造体です.
//--------------------------------------------------------------------------------------
struct TRIANGLE : public ISHAPE
{
    VEC3            vertex[3];
    VEC3            normal;
    MATERIAL*       pMaterial;

    TRIANGLE()
    : normal    ( 0.0, 0.0, 0.0 )
    , pMaterial ( nullptr )
    { /* DO_NOTHING */ }

    TRIANGLE(
        VEC3 v0,
        VEC3 v1,
        VEC3 v2,
        MATERIAL* pMaterial_
    )
    : pMaterial( pMaterial_ )
    {
        vertex[0] = v0;
        vertex[1] = v1;
        vertex[2] = v2;

        normal = ComputeNormal( v0, v1, v2 );
        normal.Normalize();
    }

    bool Intersect( const RAY& ray, HITRECORD& record )
    {
        double dist;
        if ( IntersectTriangle( ray, vertex[0], vertex[1], vertex[2], dist ) )
        {
            record.distance = dist;
            record.position = ray.org + Normalize( ray.dir ) * dist;
            record.normal   = Normalize( normal );
            record.pShape   = this;
            return true;
        }

        return false;
    }

    MATERIAL* GetMaterial()
    { return pMaterial; }
};


//--------------------------------------------------------------------------------------
//! @brief      四角形構造体です.
//--------------------------------------------------------------------------------------
struct QUAD : public ISHAPE
{
    VEC3            vertex[4];
    VEC3            normal;
    MATERIAL*       pMaterial;

    QUAD()
    : normal    ( 0.0, 0.0, 0.0 )
    , pMaterial ( nullptr )
    { /* DO_NOTHING */ }

    QUAD(
        VEC3 v0,
        VEC3 v1,
        VEC3 v2,
        VEC3 v3,
        MATERIAL* pMaterial_
    )
    : pMaterial( pMaterial_ )
    {
        vertex[0] = v0;
        vertex[1] = v1;
        vertex[2] = v2;
        vertex[3] = v3;

        normal = ComputeQuadNormal( v0, v1, v2, v3 );
        normal.Normalize();
    }

    bool Intersect( const RAY& ray, HITRECORD& record )
    {
        double distance = DBL_MAX;
        if ( IntersectTriangle( ray, vertex[0], vertex[1], vertex[2], distance ) )
        {
            record.distance = distance;
            record.normal   = Normalize( normal );
            record.position = ray.org + Normalize( ray.dir ) * distance;
            record.pShape   = this;
            return true;
        }
        
        if ( IntersectTriangle( ray, vertex[0], vertex[2], vertex[3], distance ) )
        {
            record.distance = distance;
            record.normal   = Normalize( normal );
            record.position = ray.org + Normalize( ray.dir ) * distance;
            record.pShape   = this;
            return true;
        }

        return false;
    }

    MATERIAL* GetMaterial()
    { return pMaterial; }
};


struct MESH : public ISHAPE
{
    VEC3*       pVertices;
    VEC3*       pNormals;
    MATERIAL*   pMaterial;
    unsigned int    numVertices;
    unsigned int    numNormals;
};


#endif//__S2_GEOMETRY_H__
