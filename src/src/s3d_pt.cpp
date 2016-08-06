//-------------------------------------------------------------------------------------------------
// File : s3d_pt.cpp
// Desc : Path Tracer Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <cstdio>
#include <thread>
#include <mutex>
#include <direct.h>

#include <s3d_pt.h>
#include <s3d_logger.h>
#include <s3d_timer.h>
#include <s3d_tonemapper.h>
#include <s3d_bmp.h>
#include <s3d_hdr.h>
#include <s3d_camera.h>
#include <s3d_shape.h>
#include <s3d_material.h>
#include <s3d_testScene.h> // for Debug.


#undef min
#undef max

namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
// Global Variables.
//-------------------------------------------------------------------------------------------------
std::mutex      g_Mutex;
const s3d::TONE_MAPPING_TYPE  ToneMappingType = s3d::TONE_MAPPING_REINHARD;


//-------------------------------------------------------------------------------------------------
//! @brief      CPUコアの数を取得します.
//-------------------------------------------------------------------------------------------------
s32 GetCPUCoreCount()
{
    s32 numCore = 1;
    
#if S3D_NDEBUG // リリースビルド時のみ有効化.
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
#endif//S3D_NDEBUG

    return numCore;
}

} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// PathTracer class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
PathTracer::PathTracer()
: m_RenderTarget( nullptr )
, m_Intermediate( nullptr )
, m_pScene      ( nullptr )
, m_IsFinish    ( false )
, m_WatcherEnd  ( false )
{
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
PathTracer::~PathTracer()
{
    SafeDeleteArray( m_RenderTarget );
    SafeDeleteArray( m_Intermediate );
    SafeDelete( m_pScene );
}

//-------------------------------------------------------------------------------------------------
//      レンダリングを実行します.
//-------------------------------------------------------------------------------------------------
bool PathTracer::Run( const Config& config )
{
    // 起動画面.
    ILOG( "//=================================================================" );
    ILOG( "//  Path Tarcer \"Salty\"" );
    ILOG( "//  Version : Alpha 3.0" );
    ILOG( "//  Author  : Pocol" );
    ILOG( "//=================================================================" );
    ILOG( " Configuration : " );
    ILOG( "     width      = %d", config.Width );
    ILOG( "     height     = %d", config.Height );
    ILOG( "     sample     = %d", config.SampleCount );
    ILOG( "     subsample  = %d", config.SubSampleCount );
    ILOG( "     max bounce = %d", config.MaxBounceCount );
    ILOG( "--------------------------------------------------------------------" );

    // 画像出力用ディレクトリ作成.
    _mkdir( "./img" );

    // コンフィグ設定.
    m_Config = config;

    m_pScene = new TestScene( m_Config.Width, m_Config.Height );

    // 経路追跡を実行.
    TracePath();

    SafeDelete( m_pScene );

    return m_IsFinish;
}

//-------------------------------------------------------------------------------------------------
//      レンダリング結果をキャプチャーします.
//-------------------------------------------------------------------------------------------------
void PathTracer::Capture( const char* filename )
{
    // トーンマッピングを実行.
    ToneMapper::Map( ToneMappingType, m_Config.Width, m_Config.Height, m_RenderTarget, m_Intermediate );

    // BMPに出力する.
    SaveToBMP( filename, m_Config.Width, m_Config.Height, &m_Intermediate[0].x );
}

//-------------------------------------------------------------------------------------------------
//      レンダリング時間を監視します.
//-------------------------------------------------------------------------------------------------
void PathTracer::Watcher()
{
    auto counter = 0;
    char filename[256];
    memset( filename, 0, 256 );

    Timer renderingTimer;
    renderingTimer.Start();

    Timer captureTimer;
    captureTimer.Start();

    while( true )
    {
        captureTimer.Stop();
        auto sec = captureTimer.GetElapsedTimeSec();

        renderingTimer.Stop();
        auto min = renderingTimer.GetElapsedTimeMin();

        // 約30秒ごとにレンダリング結果をキャプチャ.
        if ( sec >= 29.9 )
        {
            sprintf_s( filename, "img/%03d.bmp", counter );
            Capture( filename );
            counter++;

            ILOG( "Captured. %5.2lf min", min );

            // タイマー再スタート.
            captureTimer.Start();
        }

        // 15分の規定時間に達した場合.
        if ( min > 14.9 )
        {
            sprintf_s( filename, "img/%03d.bmp", counter );
            ILOG( "Rendering Imcompleted..." );
            break;
        }

        // レンダリングが正常終了した場合.
        if ( m_IsFinish )
        {
            sprintf_s( filename, "img/%03d.bmp", counter );
            ILOG( "Rendering Completed !!!" );
            ILOG( "Rendering Time %lf min", min );
            break;
        }

        // 1 sec 寝かせる.
        Sleep( 1000 );
    }

    Capture( "img/final.bmp" );

    m_WatcherEnd = true;
}

//-------------------------------------------------------------------------------------------------
//      指定方向からの放射輝度推定を行います.
//-------------------------------------------------------------------------------------------------
Color4 PathTracer::Radiance( const Ray& input )
{
    ShadingArg arg = ShadingArg();

    Ray ray( input );

    Color4 W( 1.0f, 1.0f, 1.0f, 1.0f );
    Color4 L( 0.0f, 0.0f, 0.0f, 0.0f );

    // 乱数設定.
    arg.random = m_Random;

    for( auto depth=0; depth < m_Config.MaxBounceCount || !m_WatcherEnd ;++depth)
    {
        auto record = HitRecord();

        // 交差判定.
        if ( !m_pScene->Intersect( ray, record ) )
        {
            L += Color4::Mul( W, m_pScene->SampleIBL( ray.dir ) );
            break;
        }

        const auto shape    = record.pShape;
        const auto material = record.pMaterial;
        assert( shape    != nullptr );
        assert( material != nullptr );

        // 自己発光による放射輝度.
        L += Color4::Mul( W, material->GetEmissive() );

        // シェーディング引数を設定.
        arg.input    = ray.dir;
        arg.normal   = record.normal;
        arg.texcoord = record.texcoord;

        // 色を求める.
        W = Color4::Mul( W, material->Shade( arg ) );

        // 直接光をサンプリング.
        if ( !record.pMaterial->HasDelta() )
        { L += Color4::Mul( W, NextEventEstimation( ray, record ) );  }

        // ロシアンルーレットで打ち切るかどうか?
        if ( arg.dice )
        { break; }

        // 重みがゼロになったら以降の更新は無駄なので打ち切りにする.
        if ( IsZero(W.GetX()) && IsZero(W.GetY()) && IsZero(W.GetZ()) )
        { break; }

        // レイを更新.
        ray.Update( record.position, arg.output );
    }

    // 乱数を更新.
    m_Random = arg.random;

    // 計算結果を返却.
    return L;
}

//-------------------------------------------------------------------------------------------------
//      直接光ライティングを行います.
//-------------------------------------------------------------------------------------------------
Color4 PathTracer::NextEventEstimation( const Ray& ray, const HitRecord& record )
{
    auto phi = F_2PI * m_Random.GetAsF32();
    auto r  = m_Random.GetAsF32();
    auto x = r * std::cos( phi );
    auto y = r * std::sin( phi );
    auto dir = Vector3( x, y, SafeSqrt( 1.0f - (x * x) - (y * y) ) );
    dir = Vector3::UnitVector( dir );

    auto shadowRay = Ray( record.position, dir );

    HitRecord shadowRecord;
    if ( m_pScene->Intersect(shadowRay, shadowRecord) )
    { return Color4(0.0f, 0.0f, 0.0f, 0.0f); }

    return m_pScene->SampleIBL( shadowRay.dir );
}

//-------------------------------------------------------------------------------------------------
//      経路を追跡します.
//-------------------------------------------------------------------------------------------------
void PathTracer::TracePath()
{
    // CPUコア数を取得.
    auto coreCount = GetCPUCoreCount();
    ILOG( "Info : CPU Core = %d", coreCount );

    // 1つは監視用に空けておく.
    if ( coreCount > 1 )
    { coreCount--; }

    // レンダーターゲットを生成.
    m_RenderTarget = static_cast<Color4*>(_aligned_malloc( sizeof(Color4) * m_Config.Width * m_Config.Height, 16 ));
    m_Intermediate = static_cast<Color4*>(_aligned_malloc( sizeof(Color4) * m_Config.Width * m_Config.Height, 16 ));

    for( auto i=0; i<m_Config.Width * m_Config.Height; ++i )
    {
        m_RenderTarget[i] = Color4(0.0f, 0.0f, 0.0f, 0.0f);
        m_Intermediate[i] = Color4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    const auto sampleCount    = m_Config.SampleCount * m_Config.SubSampleCount  * m_Config.SubSampleCount;
    const auto invSampleCount = 1.0f / static_cast<f32>( sampleCount );

    const auto rate     = 1.0f / static_cast<f32>( m_Config.SubSampleCount );
    const auto halfRate = rate * 0.5f;

    // 時間監視スレッドを起動.
    std::thread thd( &PathTracer::Watcher, this );

    // 乱数初期化.
    m_Random.SetSeed( 3141592 );

    for ( auto sy=0; sy<m_Config.SubSampleCount && !m_WatcherEnd; ++sy )
    for ( auto sx=0; sx<m_Config.SubSampleCount && !m_WatcherEnd; ++sx )
    {
        const auto r1 = sx * rate + halfRate;
        const auto r2 = sy * rate + halfRate;

        for( auto s=0; s<m_Config.SampleCount && !m_WatcherEnd; ++s )
        {
            printf_s( "\r%5.2f%% Completed.", 100.f * ( sy * m_Config.SubSampleCount * m_Config.SampleCount + sx * m_Config.SampleCount + s ) / sampleCount );

        #if _OPENMP
            #pragma omp parallel for schedule(dynamic, 1) num_threads(coreCount)
        #endif
            for( auto y=0; y<m_Config.Height; ++y )
            for( auto x=0; x<m_Config.Width ; ++x )
            {
                auto ray = m_pScene->GetRay(
                    ( r1 + x ) / m_Config.Width  - 0.5f,
                    ( r2 + y ) / m_Config.Height - 0.5f );

                const auto idx = y * m_Config.Width + x;
                m_RenderTarget[ idx ] += Radiance( ray ) * invSampleCount;
            }
        }
    }

    // 正常終了フラグを立てる.
    g_Mutex.lock();
    m_IsFinish = true;
    g_Mutex.unlock();

    // 時間監視スレッドを終了.
    thd.join();

    // レンダーターゲット解放.
    if ( m_RenderTarget != nullptr )
    {
        _aligned_free( m_RenderTarget );
        m_RenderTarget = nullptr;
    }
    if ( m_Intermediate != nullptr )
    {
        _aligned_free( m_Intermediate );
        m_Intermediate = nullptr;
    }
}

} // namespace s3d
