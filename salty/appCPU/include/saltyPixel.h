//------------------------------------------------------------------------------
// File : saltyPixel.h
// Desc : Salty Pixel Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __SALTY_PIXEL_H__
#define __SALTY_PIXEL_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <asdxTypedef.h>
#include <asdxMath.h>

namespace salty {

////////////////////////////////////////////////////////////////////////////////
// Pixel structure
////////////////////////////////////////////////////////////////////////////////
struct Pixel
{
    f32  R;      //!< R成分です.
    f32  G;      //!< G成分です.
    f32  B;      //!< B成分です.
    f32  A;      //!< A成分です.

    Pixel()
    : R( 0.0f )
    , G( 0.0f )
    , B( 0.0f )
    , A( 0.0f )
    {
    }

    Pixel( const f32 r , const f32 g, const f32 b, const f32 a )
    : R( r )
    , G( g )
    , B( b )
    , A( a )
    {
    }

    Pixel operator + ( const Pixel& value ) const
    {
        return Pixel(
            R + value.R,
            G + value.G,
            B + value.B,
            A + value.A );
    }


    Pixel operator + ( const asdx::Vector4& value ) const
    {
        return Pixel(
            R + value.x,
            G + value.y,
            B + value.z,
            A + value.w );
    }

    Pixel operator - ( const Pixel& value ) const
    {
        return Pixel(
            R - value.R,
            G - value.G,
            B - value.B,
            A - value.A );
    }

    Pixel operator - ( const asdx::Vector4& value ) const
    {
        return Pixel(
            R - value.x,
            G - value.y,
            B - value.z,
            A - value.w );
    }

    Pixel& operator += ( const Pixel& value )
    {
        R += value.R;
        G += value.G;
        B += value.B;
        A += value.A;
        return (*this);
    }

    Pixel& operator += ( const asdx::Vector4& value )
    {
        R += value.x;
        G += value.y;
        B += value.z;
        A += value.w;
        return (*this);
    }

    Pixel& operator -= ( const Pixel& value )
    {
        R -= value.R;
        G -= value.G;
        B -= value.B;
        A -= value.A;
        return (*this);
    }

    Pixel& operator -= ( const asdx::Vector4& value )
    {
        R -= value.x;
        G -= value.y;
        B -= value.z;
        A -= value.w;
        return (*this);
    }
};


} // namespace salty

#endif//__SALTY_PIXEL_H__