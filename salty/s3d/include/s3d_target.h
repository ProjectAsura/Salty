﻿//------------------------------------------------------------------------------
// File : s3d_target.h
// Desc : Render Target Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __S3D_TARGET_H__
#define __S3D_TARGET_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <s3d_typedef.h>
#include <s3d_color3.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////
// RenderTarget class
////////////////////////////////////////////////////////////////////////////////
class RenderTarget
{
    //==========================================================================
    // list of friend classes and methods.
    //==========================================================================
    /* NOTHING */

public:
    //==========================================================================
    // public variables.
    //==========================================================================
    /* NOTHING */

    //==========================================================================
    // public methods.
    //==========================================================================
    RenderTarget();
    ~RenderTarget();

    void    Init( const u32, const u32 );
    void    Init( const u32, const u32 , const Color3& clearColor );
    void    Term();

    void    Clear           ( const Color3& );
    void    SetPixel        ( const u32, const u32, const Color3& );
    Color3  GetPixel        ( const u32, const u32 );
    Color3* GetFrameBuffer  ();
    u32     GetWidth        () const;
    u32     GetHeight       () const;

protected:
    //==========================================================================
    // protected variables.
    //==========================================================================
    /* NOTHING */

    //==========================================================================
    // protected methods.
    //==========================================================================
    /* NOTHING */

private:
    //==========================================================================
    // private variables.
    //==========================================================================
    u32      m_Width;
    u32      m_Height;
    Color3*  m_pFB;

    //==========================================================================
    // private methods.
    //==========================================================================
    RenderTarget    ( const RenderTarget& );    // アクセス禁止.
    void operator = ( const RenderTarget& );    // アクセス禁止.
};


} // namespace s3d


#endif//__S3D_TARGET_H__
