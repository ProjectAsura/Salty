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

#if 1
    // 本番用.
    config.width         = 640;
    config.height        = 360;
    config.numSamples    = 2048;
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
