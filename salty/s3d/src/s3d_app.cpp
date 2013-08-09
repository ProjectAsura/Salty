//------------------------------------------------------------------------------
// File : s3d_app.cpp
// Desc : Salty Application.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <s3d_app.h>
#include <s3d_target.h>
#include <s3d_vector3.h>
#include <s3d_color3.h>
#include <s3d_matrix.h>
#include <s3d_onb.h>
#include <s3d_bmp.h>
#include <vector>


namespace s3d {

class App::Impl
{
private:
    RenderTarget m_RT;

protected:
    void Trace();
    void Shade();

    void TacePaths();
    void Combine();

    void PathTrace();
    void BiDirectionalPathTrace();

    bool Init();
    void Term();

public:
    Impl()
    {
    }

    ~Impl()
    {
    }

    void Run();
};


bool App::Impl::Init()
{
    m_RT.Init( 200, 150 );

    return false;
}

void App::Impl::Term()
{
    SaveToBMP( "FinalResult.bmp", m_RT.GetWidth(), m_RT.GetHeight(), m_RT.GetFrameBuffer() );
    m_RT.Term();
}

void App::Impl::Run()
{
    if ( Init() )
    {

    }

    Term();
}


App::App()
{
    m_pImpl = new App::Impl();
}

App::~App()
{
    delete m_pImpl;
}

void App::Run()
{
    m_pImpl->Run();
}





}// namespace s3d