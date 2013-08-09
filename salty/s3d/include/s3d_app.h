//------------------------------------------------------------------------------
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
private:
    class Impl;
    Impl* m_pImpl;

public:
    App();
    virtual ~App();

    void Run();
};


} // namespace s3d


#endif//__S3D_APP_H__