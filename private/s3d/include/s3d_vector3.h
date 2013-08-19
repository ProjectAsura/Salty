//--------------------------------------------------------------------------------
// File : s3d_vector3.h
// Desc : Vector3 Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------

#ifndef __S3D_VECTOR3_H__
#define __S3D_VECTOR3_H__

//--------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------
#include <s3d_typedef.h>
#include <cmath>
#include <cassert>


namespace s3d {

//////////////////////////////////////////////////////////////////////////////////
// Vector3 structure
//////////////////////////////////////////////////////////////////////////////////
struct Vector3
{
public:
    union
    {
        struct 
        {
            f64     x;
            f64     y;
            f64     z;
        };
        f64 v[3];
    };

    Vector3();
    Vector3( const f64, const f64, const f64 );
    Vector3( const Vector3& );

    Vector3& operator =  ( const Vector3& );
    Vector3& operator += ( const Vector3& );
    Vector3& operator -= ( const Vector3& );
    Vector3& operator *= ( const f64 );
    Vector3& operator /= ( const f64 );

    Vector3 operator +  () const;
    Vector3 operator -  () const;
    Vector3 operator +  ( const Vector3& ) const;
    Vector3 operator -  ( const Vector3& ) const;
    Vector3 operator *  ( const f64 ) const;
    Vector3 operator /  ( const f64 ) const;

    bool operator ==    ( const Vector3& ) const;
    bool operator !=    ( const Vector3& ) const;

    void    Normalize   ();
    f64     Length      () const;
    f64     LengthSq    () const;

    static f64     Dot  ( const Vector3&, const Vector3& );
    static Vector3 Cross( const Vector3&, const Vector3& );
    static Vector3 Min  ( const Vector3&, const Vector3& );
    static Vector3 Max  ( const Vector3&, const Vector3& );
    static Vector3 UnitVector( const Vector3& );
    static Vector3 Reflect( const Vector3&, const Vector3& );
    static Vector3 Refract( const Vector3&, const Vector3&, const f64 );
};


} // namespace s3d


//----------------------------------------------------------------------------
// Inlines
//----------------------------------------------------------------------------
#include <s3d_vector3.inl>

#endif//__S3D_VECTOR3_H__
