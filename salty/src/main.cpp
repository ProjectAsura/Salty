//--------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : Sample Aplication.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------------

#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

//--------------------------------------------------------------------------------------------
// Include
//--------------------------------------------------------------------------------------------
#include <crtdbg.h>
#include "SaltyApp.h"

//--------------------------------------------------------------------------------------------
//! @brief      メインエントリーポイントです.
//--------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif//defined(DEBUG) || defined(_DEBUG)
    {
        SaltyApp app;

        app.Run();
    }

    return 0;
}