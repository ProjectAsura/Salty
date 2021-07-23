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
#include <s3d_tonemapper.h>
#include <s3d_bmp.h>
#include <s3d_hdr.h>
#include <s3d_camera.h>
#include <s3d_shape.h>
#include <s3d_material.h>
#include <s3d_testScene.h> // for Debug.
#include <s3d_denoiser.h>
#include <ppl.h>
#include <stb_image_write.h>

#if _OPENMP
#include <omp.h>
#endif


//#define DEBUG_MODE

#undef min
#undef max

using namespace concurrency;

namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
// Global Variables.
//-------------------------------------------------------------------------------------------------
const s3d::TONE_MAPPING_TYPE  ToneMappingType = s3d::TONE_MAPPING_ACES_FILMIC;

} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// PathTracer class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
PathTracer::PathTracer()
: m_Intermediate( nullptr )
, m_pScene      ( nullptr )
{
    m_RenderTarget = nullptr;
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
    m_Timer.Start();

    // 起動画面.
    ILOG( "//=================================================================" );
    ILOG( "//  Renderer \"Salty\"" );
    ILOG( "//  Version : Alpha 5.0" );
    ILOG( "//  Author  : Pocol" );
    ILOG( "//=================================================================" );
    ILOG( " Configuration : " );
    ILOG( "     width      = %d", config.Width );
    ILOG( "     height     = %d", config.Height );
    ILOG( "     max bounce = %d", config.MaxBounceCount );
    ILOG( "     CPU Core   = %d", config.CpuCoreCount );
    ILOG( "--------------------------------------------------------------------" );

    // コンフィグ設定.
    m_Config = config;

    Timer timer;
    timer.Start();

    // レンダーターゲットを生成.
    auto size = m_Config.Width * m_Config.Height;
    m_RenderTarget = new Color4 [size];
    m_Intermediate = new Color4 [size];

    // レンダーターゲットをクリア.
    parallel_for<size_t>(0, size, [&](size_t i)
    {
        m_RenderTarget[i] = Color4(0.0f, 0.0f, 0.0f, 0.0f);
        m_Intermediate[i] = Color4(0.0f, 0.0f, 0.0f, 0.0f);
    });

    // シーン生成.
    m_pScene = new TestScene( m_Config.Width, m_Config.Height );

    m_Updatable = true;

    timer.Stop();
    ILOG("Scene Construct : %lf [msec]", timer.GetElapsedTimeMsec());

    // 経路追跡を実行.
    TracePath();

    // シーンを破棄.
    SafeDelete( m_pScene );

    // レンダーターゲット解放.
    SafeDeleteArray(m_RenderTarget);
    SafeDeleteArray(m_Intermediate);

    //return m_IsFinish;
    return true;
}

//-------------------------------------------------------------------------------------------------
//      レンダリング結果をキャプチャーします.
//-------------------------------------------------------------------------------------------------
void PathTracer::Capture( const char* filename )
{
#if 1
    // ノイズ除去.
    //FilterNLM(m_Config.Width, m_Config.Height, 0.3f, m_RenderTarget, m_DenoisedTarget);

    // トーンマッピングを実行.
    ToneMapper::Map( ToneMappingType, m_Config.Width, m_Config.Height, m_RenderTarget, m_Intermediate );

    auto size = m_Config.Width * m_Config.Height;
    std::vector<uint8_t> outputs;
    outputs.resize(size * 3);
    parallel_for<size_t>(0, size, [&](size_t i)
    {
        auto r = m_Intermediate[i].x;
        auto g = m_Intermediate[i].y;
        auto b = m_Intermediate[i].z;

        if ( r > 1.0f ) { r = 1.0f; }
        if ( g > 1.0f ) { g = 1.0f; }
        if ( b > 1.0f ) { b = 1.0f; }

        if ( r < 0.0f ) { r = 0.0f; }
        if ( g < 0.0f ) { g = 0.0f; }
        if ( b < 0.0f ) { b = 0.0f; }

        // sRGB OETF
        r = (r <= 0.0031308f) ? 12.92f * r : std::pow(1.055f * r, 1.0f / 2.4f) - 0.055f;
        g = (g <= 0.0031308f) ? 12.92f * g : std::pow(1.055f * g, 1.0f / 2.4f) - 0.055f;
        b = (b <= 0.0031308f) ? 12.92f * b : std::pow(1.055f * b, 1.0f / 2.4f) - 0.055f;

        u8 R = static_cast<u8>( r * 255.0f + 0.5f );
        u8 G = static_cast<u8>( g * 255.0f + 0.5f );
        u8 B = static_cast<u8>( b * 255.0f + 0.5f );

        outputs[i * 3 + 0] = R;
        outputs[i * 3 + 1] = G;
        outputs[i * 3 + 2] = B;
    });

    // PNG出力.
    void* ptr = outputs.data();
    stbi_write_png(filename, m_Config.Width, m_Config.Height, 3, ptr, 0);

#else
    SaveToBMP( filename, m_Config.Width, m_Config.Height, &m_RenderTarget[0].x);
#endif
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

    for( auto depth=0; depth < m_Config.MaxBounceCount && m_Updatable ;++depth)
    {
        auto record = HitRecord();

        // 交差判定.
        if ( !m_pScene->Intersect( raySet, record ) )
        {
            L += Color4::Mul( W, m_pScene->SampleIBL( raySet.ray.dir ) );
            break;
        }

        auto pos = raySet.ray.pos + raySet.ray.dir * record.distance;

        const auto shape    = record.pShape;
        const auto material = record.pMaterial;
        assert( shape    != nullptr );
        assert( material != nullptr );

        // 自己発光による放射輝度.
        L += Color4::Mul( W, material->GetEmissive() );

        // シェーディング引数を設定.
        arg.input = raySet.ray.dir;
        shape->CalcParam(pos, record.barycentric, &arg.normal, &arg.texcoord);

        // 直接光をサンプリング.
        if ( !record.pMaterial->HasDelta() )
        {
            L += Color4::Mul( W, NextEventEstimation( pos, arg.normal, arg.texcoord, material, arg.random ) );
        }

        // 色を求める.
        W = Color4::Mul( W, material->Shade( arg ) );

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
//      直接光ライティングを行います.
//-------------------------------------------------------------------------------------------------
Color4 PathTracer::NextEventEstimation
(
    const Vector3&      position,
    const Vector3&      normal,
    const Vector2&      texcoord,
    const IMaterial*    pMaterial,
    PCG&                random
)
{
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
        auto cosShadow = abs(Vector3::Dot(normal, light_dir));
        auto cosLight  = abs(Vector3::Dot(light_normal, -light_dir));
        auto fs = pMaterial->GetBaseColor(texcoord) / F_PI;
        auto G = (cosShadow * cosLight) / light_dist2;

        auto brdf_pdf = abs(shadowRay.ray.dir.z / F_PI ) * cosLight / light_dist2;
        auto mis_weight = light_pdf / (brdf_pdf + light_pdf);

        return shadowRecord.pMaterial->GetEmissive() * fs * mis_weight * (G / light_pdf);
    }

    return Color4(0.0f, 0.0f, 0.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------
//      経路を追跡します.
//-------------------------------------------------------------------------------------------------
void PathTracer::TracePath()
{
    ILOG( "PathTrace Start.");

    const auto halfRate = 0.5f;

    // 乱数初期化.
    m_Random.SetSeed( 3141592 );

    auto sampleCount = 0;
    auto rayCount    = m_Config.Width * m_Config.Height;

    while(m_Updatable)
    {
    #if _OPENMP
        #pragma omp parallel for schedule(dynamic) num_threads(72)
    #endif
        for( auto y=0; y<m_Config.Height; ++y )
        {
        #if _OPENMP
            auto thread_id = omp_get_thread_num();
            auto group = thread_id % 2; // 0, 1でプロセッサグループを指定
            GROUP_AFFINITY mask;
            if (GetNumaNodeProcessorMaskEx(group, &mask))
            { SetThreadGroupAffinity(GetCurrentThread(), &mask, nullptr); }
        #endif

            for( auto x=0; x<m_Config.Width ; ++x )
            {
                auto ray = m_pScene->GetRay(
                    ( halfRate + x ) / m_Config.Width  - 0.5f,
                    ( halfRate + y ) / m_Config.Height - 0.5f );

                const auto idx = y * m_Config.Width + x;
                m_RenderTarget[ idx ] += Radiance( ray );
            }
        }

        sampleCount += rayCount;

        m_Timer.Stop();
        if (m_Timer.GetElapsedTimeSec() >= m_Config.MaxRenderingSec)
        {
            m_Updatable = false;
            break;
        }
    }

    m_Timer.Stop();
    auto sample_rate = (sampleCount / 1000.0) / m_Timer.GetElapsedTimeSec();
    ILOG( "Rendering Time %lf sec", m_Timer.GetElapsedTimeSec()); 
    ILOG( "%lf [Mrays/sec]", sample_rate);

    Capture("final.png");

    ILOG( "PathTrace End.");
}

} // namespace s3d
