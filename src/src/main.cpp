//-----------------------------------------------------------------------------
// File : main.cpp
// Desc : Application Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <s3d_app.h>


//-----------------------------------------------------------------------------
//! @brief      メインエントリーポイントです.
//-----------------------------------------------------------------------------
int main( int argc, char **argv ) 
{
    // アプリケーションの構成設定.
    s3d::App::Config config;

#if 0
    // 本番用.
    config.width         = 1280;
    config.height        = 720;
    config.numSamples    = 1024;
    config.numSubSamples = 4;
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

    return 0;
}
