//-----------------------------------------------------------------------------
// File : main.cpp
// Desc : Application Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <crtdbg.h>
#include <s3d_app.h>


//-----------------------------------------------------------------------------
//! @brief      メインエントリーポイントです.
//-----------------------------------------------------------------------------
int main( int argc, char **argv ) 
{
    // リークチェック.
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    {
        // アプリケーションの構成設定.
        s3d::App::Config config;

    #if 0
        // 本番用.
        config.width         = 1024;
        config.height        = 576;
        config.numSamples    = 1024;
        config.numSubSamples = 2;
    #else
        // デバッグ用.
        config.width         = 320;
        config.height        = 180;
        config.numSamples    = 512;
        config.numSubSamples = 2;
    #endif

        s3d::App app;

        // アプリケーション実行.
        app.Run( config );
    }

    return 0;
}
