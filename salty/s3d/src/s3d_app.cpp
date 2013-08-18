//------------------------------------------------------------------------------
// File : s3d_app.cpp
// Desc : Salty Application.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <s3d_app.h>
#include <s3d_target.h>
#include <s3d_color3.h>
#include <s3d_math.h>
#include <s3d_bmp.h>
#include <s3d_shape.h>
#include <s3d_triangle.h>
#include <s3d_sphere.h>
#include <s3d_quad.h>
#include <s3d_camera.h>
#include <s3d_material.h>
#include <s3d_timer.h>
#include <s3d_bvh.h>
#include <s3d_mutex.h>

#include <ctime>
#include <cassert>
#include <cstdio>
#include <cfloat>

#include <direct.h>
#include <process.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////
// App::Impl class
////////////////////////////////////////////////////////////////////////////////
class App::Impl
{
    //==========================================================================
    // list of friend classes and methods.
    //==========================================================================
    /* NOTHING */

private:
    //==========================================================================
    // private variables.
    //==========================================================================
    static const s32 MAX_DEPTH = 64;
    u32                     m_Width;
    u32                     m_Height;
    u32                     m_NumSamples;
    u32                     m_NumSubSamples;
    RenderTarget            m_RT;
    IShape**                m_ppShapes;
    IMaterial**             m_ppMaterials;
    u32                     m_NumShapes;
    u32                     m_NumMaterials;
    BVH                     m_BVH;
    Camera                  m_Camera;
    Timer                   m_Timer;
    Random                  m_Random;
    Color3                  m_ClearColor;
    bool                    m_IsFinished;
    bool                    m_WatcherEnd;
    Mutex                   m_Mutex;

    //===========================================================================
    // private methods.
    //===========================================================================

protected:
    //===========================================================================
    // protected variables.
    //===========================================================================

    //===========================================================================
    // protected methods.
    //===========================================================================
    Color3 Trace( const Ray& );
    bool   Intersect( const Ray&, ShadeRec& );
    void   PathTrace();
    void   TimeWatch();

    bool   Init();
    void   Term();

    static void WatcherFunc( void* );
    static void TileRender ( void* );

public:
    //============================================================================
    // public variables.
    //============================================================================
    /* NOTHING */

    //============================================================================
    // public methods.
    //============================================================================
    Impl()
    : m_Width        ( 0 )
    , m_Height       ( 0 )
    , m_NumSamples   ( 0 )
    , m_NumSubSamples( 0 )
    , m_RT           ()
    , m_ppShapes     ( nullptr )
    , m_ppMaterials  ( nullptr )
    , m_NumShapes    ( 0 )
    , m_NumMaterials ( 0 )
    , m_Random       ()
    , m_Camera       ()
    , m_Timer        ()
    , m_IsFinished   ( false )
    , m_Mutex        ()
    , m_WatcherEnd   ( false )
    { /* DO_NOTHING */ }

    ~Impl()
    { /* DO_NOTHING */ }

    void Run();
};


//-----------------------------------------------------------------------------
//      初期化処理です.
//-----------------------------------------------------------------------------
bool App::Impl::Init()
{
#if 0
    // 完成版向け.
    m_Width         = 1280;
    m_Height        = 720;
    m_NumSamples    = 1024;
    m_NumSubSamples = 4;
#else
    // デバッグ向け.
    m_Width         = 320;
    m_Height        = 180;
    m_NumSamples    = 512;
    m_NumSubSamples = 2;
#endif

    // ディレクトリ作成.
    _mkdir( "./img" );
    _mkdir( "./img/frame" );

    // 終了フラグ初期化.
    m_IsFinished = false;

    m_ClearColor = Color3( 0.0, 0.0, 0.0 );

    // 乱数の種を設定.
    m_Random.SetSeed( 314159265 );

    // レンダーターゲットを初期化.
    m_RT.Init( m_Width, m_Height, m_ClearColor );

    // アスペクト比を算出.
    f64 aspectRatio = static_cast<f64>( m_Width ) / static_cast<f64>( m_Height );

    // カメラ更新.
    m_Camera.Update( 
        Vector3( 50.0, 50.0, 220.0 ),
        Vector3( 50.0, 50.0, 180.0 ),
        Vector3( 0.0, 1.0, 0.0 ),
        D_PIDIV4, 
        aspectRatio,
        1.0,
        10000.0 );

    m_NumMaterials = 9;
    m_ppMaterials = new IMaterial* [ m_NumMaterials ];
    //m_ppMaterials[0] = new Clay  ( Color3( 0.75, 0.25, 0.25 ), 0.0 );
    //m_ppMaterials[1] = new Clay  ( Color3( 0.75, 0.75, 0.75 ), 0.0 );
    //m_ppMaterials[2] = new Clay  ( Color3( 0.0,  0.0,  0.0  ), 0.0 );
    //m_ppMaterials[3] = new Clay  ( Color3( 0.25, 0.25, 0.75 ), 0.0 );
    m_ppMaterials[0] = new Matte  ( Color3( 0.75, 0.25, 0.25 ) );
    m_ppMaterials[1] = new Matte  ( Color3( 0.75, 0.75, 0.75 ) );
    m_ppMaterials[2] = new Matte  ( Color3( 0.0,  0.0,  0.0  ) );
    m_ppMaterials[3] = new Matte  ( Color3( 0.25, 0.25, 0.75 ) );
    m_ppMaterials[4] = new Matte ( Color3( 0.0,  0.0,  0.0  ), Color3( 36.0, 36.0, 36.0  ) );
    m_ppMaterials[5] = new Matte ( Color3( 0.0,  0.0,  0.0  ), Color3( 1.0,  1.0,  0.15 ) );
    m_ppMaterials[6] = new Mirror( Color3( 1.0,  1.0,  1.0  ) );
    m_ppMaterials[7] = new Glass ( Color3( 0.99, 0.99, 0.99 ), 1.54 );
    m_ppMaterials[8] = new Clay ( Color3( 0.25, 0.75, 0.25 ), 0.0 );

    // シェイプリストに追加.
    m_NumShapes = 8;
    m_ppShapes = new IShape* [ m_NumShapes ];

    //// 左.
    //m_ppShapes[0] = new Quad( 
    //        Vector3( -50.0 + 50, -50.0, -50.0 ),
    //        Vector3( -50.0 + 50,  50.0, -50.0 ),
    //        Vector3( -50.0 + 50,  50.0,  50.0 ),
    //        Vector3( -50.0 + 50, -50.0,  50.0 ),
    //        m_ppMaterials[0] );

    //// 奥.
    //m_ppShapes[1] = new Quad(
    //        Vector3( -50.0 + 50, -50.0, 50.0 ),
    //        Vector3( -50.0 + 50,  50.0, 50.0 ),
    //        Vector3(  50.0 + 50,  50.0, 50.0 ),
    //        Vector3(  50.0 + 50, -50.0, 50.0 ),
    //        m_ppMaterials[1] );

    //// 手前.
    //m_ppShapes[2] = new Quad(
    //        Vector3(  50.0 + 50, -50.0, -50.0 ),
    //        Vector3(  50.0 + 50,  50.0, -50.0 ),
    //        Vector3( -50.0 + 50,  50.0, -50.0 ),
    //        Vector3( -50.0 + 50, -50.0, -50.0 ),
    //        m_ppMaterials[1] );

    //// 右.
    //m_ppShapes[3] = new Quad(
    //        Vector3( 50.0 +50, -50.0,  50.0 ),
    //        Vector3( 50.0 +50,  50.0,  50.0 ),
    //        Vector3( 50.0 +50,  50.0, -50.0 ),
    //        Vector3( 50.0 +50, -50.0, -50.0 ),
    //        m_ppMaterials[3] );

    //// 上
    //m_ppShapes[4] = new Quad(
    //        Vector3(  50.0 +50, 50.0, -50.0 ),
    //        Vector3(  50.0 +50, 50.0,  50.0 ),
    //        Vector3( -50.0 +50, 50.0,  50.0 ),
    //        Vector3( -50.0 +50, 50.0, -50.0 ),
    //        m_ppMaterials[8] );

    //// 下.
    //m_ppShapes[5] = new Quad(
    //        Vector3( -50.0 +50, -50.0, -50.0 ),
    //        Vector3( -50.0 +50, -50.0,  50.0 ),
    //        Vector3(  50.0 +50, -50.0,  50.0 ),
    //        Vector3(  50.0 +50, -50.0, -50.0 ),
    //        m_ppMaterials[1] );

    //// 球照明.
    //m_ppShapes[6] = new Sphere(
    //        Vector3( 0.0 + 50, 50.0, 0.0 ),
    //        10.0,
    //        m_ppMaterials[4] );

    //// ミラーボール.
    //m_ppShapes[7] = new Sphere(
    //    Vector3( 20.0, -30.0, 10.0 ),
    //    16.5,
    //    m_ppMaterials[6] );

    //// ガラス球
    //m_ppShapes[8] = new Sphere(
    //    Vector3( 20.0, -40.0, 0.0 ),
    //    10.0,
    //    m_ppMaterials[7] );

    //// トライフォース(上)
    //m_ppShapes[9] = new Triangle(
    //        Vector3( -10.0, 0.0,  0.0 ),
    //        Vector3(   0.0, 20.0, 0.0 ),
    //        Vector3(  10.0, 0.0,  0.0 ),
    //        m_ppMaterials[5] );

    //// トライフォース(左)
    //m_ppShapes[10] = new Triangle(
    //        Vector3( -20.0, -20.0, 0.0 ),
    //        Vector3( -10.0,   0.0, 0.0 ),
    //        Vector3(   0.0, -20.0, 0.0 ),
    //        m_ppMaterials[5] );

    //// トライフォース(右)
    //m_ppShapes[11] = new Triangle(
    //        Vector3(  0.0, -20.0, 0.0 ),
    //        Vector3( 10.0,   0.0, 0.0 ),
    //        Vector3( 20.0, -20.0, 0.0 ),
    //        m_ppMaterials[5] );

    m_ppShapes[0] = new Sphere( Vector3( 1e5+1, 40.8, 81.6 ), 1e5, m_ppMaterials[0] );
    m_ppShapes[1] = new Sphere( Vector3( -1e5+99, 40.8, 81.6 ), 1e5, m_ppMaterials[3] );
    m_ppShapes[2] = new Sphere( Vector3( 50.0, 40.8, 1e5 ), 1e5, m_ppMaterials[1] );
    m_ppShapes[3] = new Sphere( Vector3( 50.0, 40.8, -1e5+250), 1e5, m_ppMaterials[1] );
    m_ppShapes[4] = new Sphere( Vector3( 50.0, 1e5, 81.6 ), 1e5, m_ppMaterials[1] );
    m_ppShapes[5] = new Sphere( Vector3( 50.0, -1e5+81.6, 81.6 ), 1e5, m_ppMaterials[1] );
    m_ppShapes[6] = new Sphere( Vector3( 50.0, 90.0, 81.6 ), 15, m_ppMaterials[4] );
    m_ppShapes[7] = new Sphere( Vector3( 77,  16.5, 78.0 ), 16.5, m_ppMaterials[7] );

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理です.
//-----------------------------------------------------------------------------
void App::Impl::Term()
{
    char filename[256];
    time_t t = time( nullptr );
    tm local_time;
    errno_t err = localtime_s( &local_time, &t );

    if ( err == 0 )
    {
        // output_yyyymmdd_hhmmss.bmpという形式の名前にする.
        // ex) output_20130805_153034.bmp →　2013/08/05 15:30 34sec
        sprintf_s( filename, "img/output_%04d%02d%02d_%02d%02d%02d.bmp",
            local_time.tm_year + 1900,
            local_time.tm_mon + 1,
            local_time.tm_mday,
            local_time.tm_hour,
            local_time.tm_min,
            local_time.tm_sec );

        // 最終結果をBMPファイルに出力.
        SaveToBMP( filename, m_RT.GetWidth(), m_RT.GetHeight(), m_RT.GetFrameBuffer() );
    }
    else
    {
        // 最終結果をBMPファイルに出力.
        SaveToBMP( "img/output.bmp", m_RT.GetWidth(), m_RT.GetHeight(), m_RT.GetFrameBuffer() );
    }

    // レンダーターゲットの終了処理.
    m_RT.Term();

    for( u32 i=0; i<m_NumShapes; ++i )
    {
        delete m_ppShapes[i];
    }
    delete [] m_ppShapes;

    for( u32 i=0; i<m_NumMaterials;  ++i )
    {
        delete m_ppMaterials[i];
    }
    delete [] m_ppMaterials;
}

//-----------------------------------------------------------------------------
//      交差判定を行います.
//-----------------------------------------------------------------------------
bool App::Impl::Intersect( const Ray& ray, ShadeRec& record )
{
    ShadeRec temp;

    for( u32 i=0; i<m_NumShapes; ++i )
    {
        if ( m_ppShapes[i]->IsHit( ray, DBL_EPSILON, DBL_MAX, temp ) )
        {
            if ( temp.dist > 0.0 && temp.dist < record.dist )
            { record = temp; }
        }
    }

    return ( record.dist != DBL_MAX && record.pMaterial != nullptr );
}


//-----------------------------------------------------------------------------
//      光線を追跡する.
//-----------------------------------------------------------------------------
Color3 App::Impl::Trace( const Ray& inputRay )
{
    ShadeRec record;
    ShadeArg arg;
    Ray ray( inputRay );

    //========================================================
    // レンダリング方程式を展開していくと...
    //  L[0] = Le[0] + Lr[0]
    //       = Le[0] + Wr[0] * L[1]
    //       = Le[0] + Wr[0] * ( Le[1] + Wr[1] * L[2] )
    //       = Le[0] + Wr[0] * ( Le[1] + Wr[1] * ( Le[2] + Wr[2] * L[3] )
    //       = Le[0] + Wr[0] * ( Le[1] + Wr[1] * ( Le[2] + Wr[2] * ( Le[3] + Wr[3] * L[4] )
    //       = ...
    //
    // のようになる.
    //
    // 上記をまじめに解こうとすると, 再帰を使うことになるが
    // プログラムだと, スタックに乗っからなくなる(=スタックオーバーフロー)になる恐れがある.
    // そのため再帰を使わずに, for文でぐーるぐる回して求める.
    // 
    // 少し式をまとめると...
    //  L[0] = Le[0]
    //       + ( Wr[0] ) * Le[0] 
    //       + ( Wr[0] * Wr[1] ) * Le[2]
    //       + ( Wr[0] * Wr[1] * Wr[2] ) * Le[3]
    //       + ( Wr[0] * Wr[1] * Wr[2] * Wr[3] ) * Le[4]
    //       + ...
    //
    // うまくfor文にできるようにさらに式変形.
    // L[0] =   Wr[-1] * Le[0]
    //      + ( Wr[-1] * Wr[0] ) * Le[0]
    //      + ( Wr[-1] * Wr[0] * Wr[1] ) * Le[2]
    //      + ( Wr[-1] * Wr[0] * Wr[1] * Wr[2] ) * Le[3]
    //      + ...
    // ただし， Wr[-1] = 1.0とする.
    // 上記式をfor文に落とし込むと.
    //
    // W = 1.0;
    // L = 0.0;
    // for( i=0; ; ++i )
    // {
    //      L += W * Le[i];
    //      W *= Wr[i];
    // }
    //
    // ... のようになる.
    //========================================================

    Color3 L( 0.0, 0.0, 0.0 );
    Color3 W( 1.0, 1.0, 1.0 );

    for( s32 depth = 0; /*depth < 4*/; ++depth )
    {
        // 交差判定.
        if ( !Intersect( ray, record ) )
        { break; }

        // ロシアンルーレットで使用する値.
        f64 probability = Max( record.pMaterial->GetThreshold(), 0.99 );

        // 引数設定.
        arg.input       = ray.GetDir();
        arg.normal      = record.normal;
        arg.probability = probability;
        arg.random      = m_Random;

        // Lだけはロシアンルーレットの前に実行する必要あり!
        Color3 Ke = record.pMaterial->Le( arg );
        Color3 Lr = W * Ke;
        //printf_s( "depth = %d, Lr = %lf, %lf, %lf\n", depth, Lr.r, Lr.g, Lr.b );
        //printf_s( "depth = %d, L = %lf, %lf, %lf\n", depth, L.r, L.g, L.b );
//        L += ( W * record.pMaterial->Le( arg ) );
        L += Lr;

        if ( Color3::Dot( W, W ) <= 0.0 )
        { break; }

        // 最大深度以上になったら，打ち切るために閾値を急激に下げる.
        if ( depth > MAX_DEPTH )
        { arg.probability *= pow( 0.5, depth - MAX_DEPTH ); }

        if ( depth > 3 )
        {
            // ロシアンルーレット.
            if ( m_Random.GetAsF64() >= arg.probability )
            { break; }
        }
        else
        {
            arg.probability = 1.0;
        }

        //arg.probability = Max( arg.probability, D_EPSILON );

        // 重みを更新.
        W *= record.pMaterial->Wr( arg );
        //printf_s( "depth = %d, W = %lf, %lf, %lf\n", depth, W.r, W.g, W.b );

        // レイを更新.
        ray.Step( record.dist );        // 衝突点まで移動.
        ray.SetDir( arg.output );
    };

    // レンダリング方程式をモンテカルロ法で解いた結果を返却.
    // ( ここで返す結果は，サンプリング数で割っていないので注意!! ).
    return L;
}

//-----------------------------------------------------------------------------
//      経路追跡法.
//-----------------------------------------------------------------------------
void App::Impl::PathTrace()
{
    f64 amountX = ( 1.0 / m_Width )  * m_NumSubSamples;
    f64 amountY = ( 1.0 / m_Height ) * m_NumSubSamples;

    // 総サンプリング数.
    s32 numTotalSamples = m_NumSamples * m_NumSubSamples * m_NumSubSamples;

    // 時間監視スレッドを走らせる.
    uintptr_t ret = _beginthread( WatcherFunc, 0, this );
    assert( ret != -1 );    // 失敗しちゃだめよ.

    for( u32 i=0; i<m_Height; ++i )
    {
        printf_s( "process %lf%% completed.\n", 100.0 * i / ( m_Height - 1 ) );

        m_Random.SetSeed( m_Height - 1 );


    #ifdef _OPENMP
        #pragma omp parallel for schedule(dynamic, 1) num_threads(8)
    #endif
        for( s32 j=0; j<(s32)m_Width; ++j )
        {
            Color3 color( 0.0, 0.0, 0.0 );

            for( s32 s=0; s<(s32)m_NumSamples; ++s )
            {
                // サブサンプリング.
                //for ( s32 sy=0; sy<(s32)m_NumSubSamples; ++sy )
                //for ( s32 sx=0; sx<(s32)m_NumSubSamples; ++sx )
                {
                    // Jitterサンプル.
                    const f64 dx = ( m_Random.GetAsF64() * -0.5 ) * amountX;
                    const f64 dy = ( m_Random.GetAsF64() * -0.5 ) * amountY;

                    // 射影空間での位置座標.
                    const f64 x = 2.0 * ( static_cast<f64>( j ) + dx ) / m_Width  - 1.0;
                    const f64 y = 2.0 * ( static_cast<f64>( i ) + dy ) / m_Height - 1.0;

                    // スクリーンへ飛ばすレイを取得する.
                    Ray ray = m_Camera.GetRay( x, y );

                    // レイトレ!
                    color += Trace( ray );
                }
            }

            // サンプル数で割る.
            color /= numTotalSamples;
            color += m_RT.GetPixel( j, i );

            // レンダーターゲットに設定.
            m_RT.SetPixel( j, i, color );
        }
    }

    m_Mutex.Lock();
    {
        // 終了フラグを立てる.
        m_IsFinished = true;
    }
    m_Mutex.Unlock();

    // スレッドの終了を待機する.
    while( !m_WatcherEnd )
    { Sleep( 10 ); }
}

//-----------------------------------------------------------------------------
//      時間監視スレッド関数.
//-----------------------------------------------------------------------------
void App::Impl::WatcherFunc( void* pParam )
{
    // インスタンス取り出し.
    App::Impl* pImpl = reinterpret_cast<App::Impl*>( pParam );
    assert( pImpl != nullptr );

    // メソッド呼び出し.
    pImpl->TimeWatch();
}

//-----------------------------------------------------------------------------
//      時間監視メソッド.
//-----------------------------------------------------------------------------
void App::Impl::TimeWatch()
{
    // レンダリング開始時刻を記録.
    Timer timer;
    timer.Start();

    // 定期監視を開始.
    Timer captureTimer;
    captureTimer.Start();

    // 1時間以上経過するか, レンダリングが先に終了するまでループ.
    while( 1 )
    {
        // 経過時間を取得.
        captureTimer.Stop();
        f64 sec  = captureTimer.GetElapsedTimeSec();

        // 59.9秒以上立ったらキャプチャー.
        if ( sec > 59.9 )
        {
            char filename[256];
            time_t t = time( nullptr );
            tm local_time;
            errno_t err = localtime_s( &local_time, &t );

            if ( err == 0 )
            {
                // frame_yyyymmdd_hhmmss.bmpという形式の名前にする.
                // ex) result_20130805_153034.bmp →　2013/08/05 15:30 34sec
                sprintf_s( filename, "img/frame/frame_%04d%02d%02d_%02d%02d%02d.bmp",
                    local_time.tm_year + 1900,
                    local_time.tm_mon + 1,
                    local_time.tm_mday,
                    local_time.tm_hour,
                    local_time.tm_min,
                    local_time.tm_sec );

                // 最終結果をBMPファイルに出力.
                SaveToBMP( filename, m_RT.GetWidth(), m_RT.GetHeight(), m_RT.GetFrameBuffer() );
            }
            else
            {
                // 最終結果をBMPファイルに出力.
                SaveToBMP( "img/frame/frame.bmp", m_RT.GetWidth(), m_RT.GetHeight(), m_RT.GetFrameBuffer() );
            }

            // タイマーを再スタート.
            captureTimer.Start();
        }

        // レンダリング開始してからの時間を取得.
        timer.Stop();
        f64 hour = timer.GetElapsedTimeHour();

        // 1時間以上たった
        if ( hour >= 1.0 )
        {
            // 最後のフレームをキャプチャー.
            SaveToBMP( "img/final_frame.bmp", m_RT.GetWidth(), m_RT.GetHeight(), m_RT.GetFrameBuffer() );

            Mutex mutex;
            mutex.Lock();
            {
                m_WatcherEnd = true;
            }
            mutex.Unlock();

            // ループ脱出.
            break;
        }

        // レンダリングが正常終了している場合.
        if ( m_IsFinished )
        {
            printf_s( "Finished Rendering!!\n" );

            FILE* pFile;
            errno_t err = fopen_s( &pFile, "result.txt", "w" );

            if ( err == 0 )
            {
                fprintf( pFile, "Rendering Time %lf (sec)\n", timer.GetElapsedTimeSec() );
                fprintf( pFile, "               %lf (min)\n", timer.GetElapsedTimeMin() );
                fprintf( pFile, "               %lf (hour)\n", timer.GetElapsedTimeHour() );
                fprintf( pFile, "Per Pixel      %lf (msec)\n", timer.GetElapsedTimeMsec() / ( m_RT.GetWidth() * m_RT.GetHeight() ) );

                fclose( pFile );
            }

            Mutex mutex;
            mutex.Lock();
            {
                m_WatcherEnd = true;
            }
            mutex.Unlock();

            // ループ脱出.
            break;
        }

        // 100 msec 寝かせる.
        Sleep( 100 );
    }
}

//-----------------------------------------------------------------------------
//      アプリケーションを実行します.
//-----------------------------------------------------------------------------
void App::Impl::Run()
{
    // 初期化処理.
    if ( Init() )
    {
        // パストレ.
        PathTrace();
    }

    // 終了処理.
    Term();
}


///////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
App::App()
{
    m_pImpl = new App::Impl();
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
App::~App()
{
    delete m_pImpl;
}

//-----------------------------------------------------------------------------
//      アプリケーションを実行します.
//-----------------------------------------------------------------------------
void App::Run()
{
    m_pImpl->Run();
}





}// namespace s3d