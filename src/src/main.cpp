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
#include <s3d_hdr.h>
#include <Windows.h>

//-------------------------------------------------------------------------------------------------
//! @brief      CPUコアの数を取得します.
//-------------------------------------------------------------------------------------------------
s32 GetCPUCoreCount()
{
#if S3D_NDEBUG // リリースビルド時のみ有効化.
    s32 numCore = 1;
    HANDLE process = GetCurrentProcess();
    
    DWORD_PTR  processMask;
    DWORD_PTR  systemMask;
    BOOL succeeded = GetProcessAffinityMask( process, &processMask, &systemMask );
    if ( succeeded != 0 )
    {
        for( u64 i=1; i<32; ++i )
        {
            if ( processMask & (DWORD_PTR)( 1ui64 << i ) )
            {
                ++numCore;
            }
        }
    }

    return numCore;

#else
    return 1;
#endif
}

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

        config.MaxRenderingSec    = 59.5f; // デノイズ時間を考慮してあとで調整.

    #if 1
        // 本番用.
        config.Width          = 960;
        config.Height         = 540;
//        config.SampleCount    = int(5.6);// * 18); // ローカル環境は1ピクセル1秒間に0.48レイなので12秒では5.6サンプル程度. 本番環境では72コアなので 18倍CPUがあるので18倍している.
        //config.SubSampleCount = 1;
        config.MaxBounceCount = 32;
        config.CpuCoreCount   = GetCPUCoreCount();
    #else
        // デバッグ用.
        config.Width          = 256;
        config.Height         = 256;
        //config.SampleCount    = 512;
        //config.SubSampleCount = 1;
        config.MaxBounceCount = 4;
        config.CpuCoreCount   = GetCPUCoreCount();
    #endif

        s3d::PathTracer renderer;

        // アプリケーション実行.
        renderer.Run( config );
    }

    return 0;
}
