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
#include <s3d_pt.h>

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
        s3d::PathTracer::Config config;

    #if 1
        // 本番用.
        config.Width          = 1280;
        config.Height         = 720;
        config.SampleCount    = 2048;
        config.SubSampleCount = 1;
        config.MaxBounceCount = 8;
    #else
        // デバッグ用.
        config.Width          = 320;
        config.Height         = 180;
        config.SampleCount    = 512;
        config.SubSampleCount = 1;
        config.MaxBounceCount = 16;
    #endif

        s3d::PathTracer renderer;

        // アプリケーション実行.
        renderer.Run( config );
    }

    return 0;
}
