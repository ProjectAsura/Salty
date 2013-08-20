﻿//-------------------------------------------------------------------------------------------
// File : Color3.h
// Desc : RGB Color Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

#ifndef __S3D_COLOR3_H__
#define __S3D_COLOR3_H__

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <s3d_typedef.h>
#include <cmath>


namespace s3d {

/////////////////////////////////////////////////////////////////////////////////////////////
// Color3 structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct Color3
{
public:
    f64 r;
    f64 g;
    f64 b;

    Color3();
    Color3( const f64, const f64, const f64 );
    Color3( const Color3& );

    Color3& operator =  ( const Color3& );
    Color3& operator += ( const Color3& );
    Color3& operator -= ( const Color3& );
    Color3& operator *= ( const Color3& );
    Color3& operator /= ( const Color3& );
    Color3& operator *= ( const f64 );
    Color3& operator /= ( const f64 );

    Color3 operator +   () const;
    Color3 operator -   () const;

    Color3 operator +   ( const Color3& )   const;
    Color3 operator -   ( const Color3& )   const;
    Color3 operator *   ( const Color3& )   const;
    Color3 operator /   ( const Color3& )   const;
    Color3 operator *   ( const f64 )       const;
    Color3 operator /   ( const f64 )       const;

    void Clamp();

    bool operator == ( const Color3& ) const;
    bool operator != ( const Color3& ) const;

    static Color3 GammaCorrection( const Color3&, const f64 );
    static f64    Dot            ( const Color3&, const Color3& );
};


} // namespace s3d


//-------------------------------------------------------------------------------------------
// Inlines
//-------------------------------------------------------------------------------------------
#include <s3d_color3.inl>


#endif//__S3D_COLOR3_H__