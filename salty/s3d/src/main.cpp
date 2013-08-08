//-----------------------------------------------------------------------------------
// File : main.cpp
// Desc : Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------
#include <s3d_typedef.h>
#include <s3d_target.h>

using namespace s3d;

int main( int argc, char** argv )
{
    RenderTarget    target( 200, 150 );
    Color3          clearColor( 0.3, 0.3, 1.0 );

    target.Clear( clearColor );

    return 0;
}