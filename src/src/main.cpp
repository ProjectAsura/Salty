//-----------------------------------------------------------------------------
// File : main.cpp
// Desc : Application Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif//
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif
#include <s3d_app.h>


//-----------------------------------------------------------------------------
//! @brief      メインエントリーポイントです.
//-----------------------------------------------------------------------------
int main( int argc, char **argv ) 
{
#if S3D_DEBUG
    // リークチェック.
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
    {
        // アプリケーションの構成設定.
        s3d::Config config;

    #if 0
        // 本番用.
        config.Width         = 1024;
        config.Height        = 576;
        config.NumSamples    = 4096;
        config.NumSubSamples = 2;
        config.MaxDepth      = 32;
    #else
        // デバッグ用.
        config.Width         = 320;
        config.Height        = 180;
        config.NumSamples    = 512;
        config.NumSubSamples = 2;
        config.MaxDepth      = 32;
    #endif

        s3d::App app;

        // アプリケーション実行.
        app.Run( config );
    }

    return 0;
}
