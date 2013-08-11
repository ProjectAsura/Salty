﻿//------------------------------------------------------------------------------
// File : s3d_app.h
// Desc : Salty Application.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __S3D_APP_H__
#define __S3D_APP_H__


namespace s3d {

////////////////////////////////////////////////////////////////////////////////
// App class
////////////////////////////////////////////////////////////////////////////////
class App 
{
    //==========================================================================
    // list of friend clases and methods.
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
    App();
    virtual ~App();

    void Run();

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
    class Impl;
    Impl* m_pImpl;

    //==========================================================================
    // private methods.
    //==========================================================================
    App             ( const App& );     // アクセス禁止.
    void operator = ( const App& );     // アクセス禁止.
};


} // namespace s3d


#endif//__S3D_APP_H__