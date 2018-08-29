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
const s3d::TONE_MAPPING_TYPE  ToneMappingType = s3d::TONE_MAPPING_ACES_FILMIC;
float time1 = 0.0f;
float time2 = 0.0f;

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
{ /* DO_NOTHING */ }

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
    ILOG( "//  Renderer \"Salty\"" );
    ILOG( "//  Version : Alpha 3.5" );
    ILOG( "//  Author  : Pocol" );
    ILOG( "//=================================================================" );
    ILOG( " Configuration : " );
    ILOG( "     width      = %d", config.Width );
    ILOG( "     height     = %d", config.Height );
    ILOG( "     sample     = %d", config.SampleCount );
    ILOG( "     subsample  = %d", config.SubSampleCount );
    ILOG( "     max bounce = %d", config.MaxBounceCount );
    ILOG( "     CPU Core   = %d", config.CpuCoreCount );
    ILOG( "--------------------------------------------------------------------" );

    // コンフィグ設定.
    m_Config = config;
    
    // 1つは監視用に空けておく.
    if ( m_Config.CpuCoreCount > 1 )
    { m_Config.CpuCoreCount; }

    // レンダーターゲットを生成.
    m_RenderTarget = new Color4 [m_Config.Width * m_Config.Height];
    m_Intermediate = new Color4 [m_Config.Width * m_Config.Height];

    for( auto i=0; i<m_Config.Width * m_Config.Height; ++i )
    {
        m_RenderTarget[i] = Color4(0.0f, 0.0f, 0.0f, 0.0f);
        m_Intermediate[i] = Color4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // 時間監視スレッドを起動.
    std::thread thd( &PathTracer::Watcher, this, m_Config.MaxRenderingMin, m_Config.CaptureIntervalSec );

    // 画像出力用ディレクトリ作成.
    _mkdir( "./img" );

    // シーン生成.
    m_pScene = new TestScene( m_Config.Width, m_Config.Height );

    // 経路追跡を実行.
    TracePath();

    // 時間監視スレッドを終了.
    thd.join();

    // シーンを破棄.
    SafeDelete( m_pScene );

    // レンダーターゲット解放.
    SafeDeleteArray(m_RenderTarget);
    SafeDeleteArray(m_Intermediate);

    return m_IsFinish;
}

//-------------------------------------------------------------------------------------------------
//      レンダリング結果をキャプチャーします.
//-------------------------------------------------------------------------------------------------
void PathTracer::Capture( const char* filename )
{
#if 1
    // トーンマッピングを実行.
    ToneMapper::Map( ToneMappingType, m_Config.Width, m_Config.Height, m_RenderTarget, m_Intermediate );

    // BMPに出力する.
    SaveToBMP( filename, m_Config.Width, m_Config.Height, &m_Intermediate[0].x );
#else
    SaveToBMP( filename, m_Config.Width, m_Config.Height, &m_RenderTarget[0].x);
#endif
}

//-------------------------------------------------------------------------------------------------
//      レンダリング時間を監視します.
//-------------------------------------------------------------------------------------------------
void PathTracer::Watcher(f32 maxRenderingTimeMin, f32 captureIntervalSec)
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

        // レンダリング結果をキャプチャ.
        if ( sec >= captureIntervalSec )
        {
            // タイマー再スタート.
            captureTimer.Start();

            // ファイル保存.
            sprintf_s( filename, "img/%03d.bmp", counter );
            Capture( filename );

            counter++;

            ILOG( "Captured. %5.2lf min", min );
        }

        // シーン生成待ち.
        if (m_pScene == nullptr)
        {
            printf_s( "\rWaiting for create scene. min = %5.2lf min", min );
        }

        // 規定時間に達した場合.
        if ( min >= maxRenderingTimeMin )
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
    auto arg    = ShadingArg();
    auto raySet = MakeRaySet( input.pos, input.dir );

    Color4 W( 1.0f, 1.0f, 1.0f, 1.0f );
    Color4 L( 0.0f, 0.0f, 0.0f, 0.0f );

    // 乱数設定.
    arg.random = m_Random;

    for( auto depth=0; depth < m_Config.MaxBounceCount && !m_WatcherEnd ;++depth)
    {
        auto record = HitRecord();

        Timer timer;

        timer.Start();
        // 交差判定.
        if ( !m_pScene->Intersect( raySet, record ) )
        {
            L += Color4::Mul( W, m_pScene->SampleIBL( raySet.ray.dir ) );
            break;
        }
        timer.Stop();
        time1 += timer.GetElapsedTimeMsec();

        auto pos = raySet.ray.pos + raySet.ray.dir * record.distance;

        const auto shape    = record.pShape;
        const auto material = record.pMaterial;
        assert( shape    != nullptr );
        assert( material != nullptr );

        timer.Start();

        // 自己発光による放射輝度.
        L += Color4::Mul( W, material->GetEmissive() );

        // シェーディング引数を設定.
        arg.input = raySet.ray.dir;
        shape->CalcParam(pos, record.barycentric, &arg.normal, &arg.texcoord);

        // 直接光をサンプリング.
        if ( !record.pMaterial->HasDelta() )
        {
            auto color = material->GetBaseColor(arg.texcoord);
            L += Color4::Mul( W, Color4::Mul(NextEventEstimation( pos, arg.normal, arg.random ), color) );
        }

        // 色を求める.
        W = Color4::Mul( W, material->Shade( arg ) );
        timer.Stop();
        time2 += timer.GetElapsedTimeMsec();

        // ロシアンルーレットで打ち切るかどうか?
        if ( arg.dice )
        { break; }

        // 重みがゼロになったら以降の更新は無駄なので打ち切りにする.
        if ( (W.GetX() < FLT_EPSILON) &&
             (W.GetY() < FLT_EPSILON) &&
             (W.GetZ() < FLT_EPSILON) )
        { break; }

        // レイを更新.
        raySet = MakeRaySet( pos, arg.output );
    }

    // 乱数を更新.
    m_Random = arg.random;

    // 計算結果を返却.
    return L;
}

//-------------------------------------------------------------------------------------------------
//      シャドウレイを生成します.
//-------------------------------------------------------------------------------------------------
RaySet PathTracer::MakeShadowRaySet( const Vector3& position, Random& random )
{
    //auto phi = F_2PI * random.GetAsF32();
    //auto r  = random.GetAsF32();
    //auto x = r * std::cos( phi );
    //auto y = r * std::sin( phi );
    //auto dir = Vector3( x, y, SafeSqrt( 1.0f - (x * x) - (y * y) ) );
    //dir = Vector3::SafeUnitVector( dir );
    auto light = m_pScene->GetLight(random);
    auto dir = Vector3::SafeUnitVector(light->GetCenter() - position);

    return MakeRaySet( position, dir );
}

//-------------------------------------------------------------------------------------------------
//      直接光ライティングを行います.
//-------------------------------------------------------------------------------------------------
Color4 PathTracer::NextEventEstimation( const Vector3& position, const Vector3& normal, Random& random )
{
    //auto shadowRay = MakeShadowRaySet( position, random );
    auto light = m_pScene->GetLight(random);
    Vector3 light_pos;
    float   light_pdf;
    light->Sample(random, &light_pos, &light_pdf);

    auto light_dir  = light_pos - position;
    auto light_dist2 = Vector3::Dot(light_dir, light_dir);
    light_dir = Vector3::SafeUnitVector(light_dir);

    auto shadowRay = MakeRaySet( position, light_dir );

    HitRecord shadowRecord;

    // ライトにヒットした場合.
    if ( m_pScene->Intersect(shadowRay, shadowRecord) && light == shadowRecord.pShape )
    {
        auto p = shadowRay.ray.pos + shadowRay.ray.dir * shadowRecord.distance;
        Vector3 light_normal;
        Vector2 light_texcoord;
        light->CalcParam(p, shadowRecord.barycentric, &light_normal, &light_texcoord);
        auto dot0 = abs(Vector3::Dot(normal, light_dir));
        auto dot1 = abs(Vector3::Dot(light_normal, -light_dir));
        auto brdf_pdf = 1.0f / F_PI;

        auto G = (dot0 * dot1) / light_dist2;
        return shadowRecord.pMaterial->GetEmissive() * brdf_pdf * (G / light_pdf);
    }

    return Color4(0.0f, 0.0f, 0.0f, 0.0f);
    //return m_pScene->SampleIBL( shadowRay.ray.dir );
}

//-------------------------------------------------------------------------------------------------
//      経路を追跡します.
//-------------------------------------------------------------------------------------------------
void PathTracer::TracePath()
{
    ILOG( "\nPathTrace Start.");

    const auto sampleCount    = m_Config.SampleCount * m_Config.SubSampleCount  * m_Config.SubSampleCount;
    const auto invSampleCount = 1.0f / static_cast<f32>( sampleCount );

    const auto rate     = 1.0f / static_cast<f32>( m_Config.SubSampleCount );
    const auto halfRate = rate * 0.5f;

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
            #pragma omp parallel for schedule(dynamic, 1) num_threads(m_Config.CpuCoreCount)
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

    printf_s("time1 %f\n", time1);
    printf_s("time2 %f\n", time2);

    ILOG( "\nPathTrace End.");
}

} // namespace s3d
