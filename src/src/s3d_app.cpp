//---------------------------------------------------------------------------------
// File : s3d_app.cpp
// Desc : Application Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <s3d_app.h>
#include <s3d_bmp.h>
#include <s3d_math.h>
#include <s3d_rand.h>
#include <s3d_camera.h>
#include <s3d_shape.h>
#include <s3d_material.h>
#include <s3d_timer.h>
#include <s3d_mutex.h>
#include <s3d_onb.h>
#include <s3d_bvh.h>
#include <s3d_mesh.h>

#include <iostream>
#include <direct.h>
#include <ctime>
#include <process.h>


namespace /* anonymous */ {

//----------------------------------------------------------------------------
//! @brief      CPUコアの数を取得します.
//----------------------------------------------------------------------------
u32 GetNumCPUCore()
{
    u32 numCore = 1;
    
#if S3D_NDEBUG // リリースビルド時のみ有効化.
    HANDLE process = GetCurrentProcess();
    
    DWORD_PTR  processMask;
    DWORD_PTR  systemMask;
    BOOL succeeded = GetProcessAffinityMask( process, &processMask, &systemMask );
    if ( succeeded != 0 )
    {
        for( u64 i=1; i<32; ++i )
        {
            if ( processMask & (DWORD_PTR)( 1 << i ) )
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

//----------------------------------------------------------------------------------
// Macro
//----------------------------------------------------------------------------------

// デバッグログ.
#ifndef DLOG
    #if defined(DEBUG) || defined(_DEBUG)
        #define DLOG( x, ... ) printf_s( "[File: %s, Line: %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
    #else
        #define DLOG( x, ... )
    #endif
#endif//DLOG

// 情報ログ.
#ifndef ILOG
#define ILOG( x, ... ) printf_s( x"\n", ##__VA_ARGS__ )
#endif//ILOG

// エラーログ.
#ifndef ELOG
#define ELOG( x, ... ) fprintf_s( stderr, "[File: %s, Line: %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG


//----------------------------------------------------------------------------------
// Forward Declarations.
//----------------------------------------------------------------------------------
void TimeWatch( void* );
Color Radiance( const Ray &inRay, s3d::Random &rnd );

//----------------------------------------------------------------------------------
// Constant Values
//----------------------------------------------------------------------------------
//static const s32 MAX_DEPTH = 32;        //!< 打ち切り深度.


//----------------------------------------------------------------------------------
// Global Varaibles.
//----------------------------------------------------------------------------------
bool        g_WatcherEnd    = false;        //!< ウォッチーの終了フラグ.
bool        g_IsFinished    = false;        //!< レイトレ終了フラグ.
bool        g_IsRendered    = false;
bool        g_ForceExit     = false;
Color*      g_pRT           = nullptr;      //!< レンダーターゲット.
Config      g_Config;                       //!< 設定です.
Mutex       g_Mutex;                        //!< ミューテックス.

Texture2D   g_TextureWall( "./res/texture/wall.bmp" );
Texture2D   g_TextureTile( "./res/texture/tile.bmp" );
TextureSampler g_Sampler = TextureSampler();

// Lambert
Matte g_Matte[] = {
    Matte( Color( 0.75f, 0.75f, 0.75f ), Color( 0.0f, 0.0f, 0.0f ), &g_TextureWall, &g_Sampler ),
    Matte( Color( 0.75f, 0.75f, 0.75f ), Color( 0.0f, 0.0f, 0.0f ), &g_TextureTile, &g_Sampler ),
    Matte( Color( 0.0f,  0.0f,  0.0f  ), Color( 36.0f, 36.0f, 36.0f ) ),
};

// Mirror
Mirror g_Mirror[] = {
    Mirror( Color( 0.25f, 0.25f, 0.75f ) ),
    Mirror( Color( 0.75f, 0.75f, 0.25f ) ),
};

// Refraction (Crystal)
Glass g_Crystal[] = {
    Glass( 1.54f, Color( 0.75f, 0.25f, 0.25f ) ),
    Glass( 2.5f,  Color( 1.0f,  1.0f,  1.0f  ) ),
};

// Phong
Glossy g_Glossy[] = {
    Glossy( Color( 0.0f, 1.0f, 0.0f ), 100000.0f ),
};

// Materials
IMaterial* g_pMaterials[] = {
    &g_Matte[0],        // 0 : 白.
    &g_Matte[1],        // 1 : タイル.
    &g_Matte[2],        // 2 : 照明.
    &g_Mirror[0],       // 3 : ミラー.
    &g_Mirror[1],       // 4 : 黄色ミラー.
    &g_Crystal[0],      // 5 : 水晶.
    &g_Glossy[0],       // 6 : メタル(緑).
};

// レンダリングするシーンデータ
Sphere g_Spheres[] = {
    Sphere( 16.5f,  Vector3( 20.0f, 16.5f,  27.0f ), g_pMaterials[5] ),    // 水晶
    Sphere( 16.5f,  Vector3( 77.0f, 16.5f,  78.0f ), g_pMaterials[6] ),    // 鏡.
    Sphere( 15.0f,  Vector3( 50.0f, 100.0f, 81.6f ), g_pMaterials[2] ),    // 照明
};

// トライアングル.
Triangle g_Triangles[] = {

    // 上.
    Triangle( 
        Face3(
            Vertex( Vector3( 70.0, 50.0, 20.0 ), Vector2( 0.0, 0.0 ) ), 
            Vertex( Vector3( 50.0, 80.0, 10.0 ), Vector2( 0.5, 1.0 ) ),
            Vertex( Vector3( 30.0, 50.0, 20.0 ), Vector2( 1.0, 0.0 ) ) ),
        g_pMaterials[4]
    ), 

    // 左.
    Triangle( 
        Face3(
            Vertex( Vector3( 70.0, 30.0, 20.0 ), Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3( 50.0, 70.0, 10.0 ), Vector2( 0.5, 1.0 ) ),
            Vertex( Vector3( 30.0, 30.0, 20.0 ), Vector2( 1.0, 0.0 ) ) ),
        g_pMaterials[4]
    ), 

};

// 矩形.
Quad g_Quads[] = {

    // 左.
    Quad( 
        Face4(
            Vertex( Vector3( 0,  0.0,  250.0 ), Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3( 0, 100.0, 250.0 ), Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3( 0, 100.0, 0.0 ),   Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3( 0,  0.0,  0.0 ),   Vector2( 2.0, 0.0 ) ) ),
        g_pMaterials[0]
    ),

    // 奥.
    Quad(
        Face4(
            Vertex( Vector3( 0,   0.0,  0.0 ),   Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3( 0, 100.0,  0.0 ),   Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3( 100, 100.0,  0.0 ), Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3( 100,   0.0,  0.0 ), Vector2( 2.0, 0.0 ) ) ),
        g_pMaterials[0]
    ),

    // 手前.
    Quad(
        Face4(
            Vertex( Vector3(  100,    0.0, 250.0 ), Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3(  100,  100.0, 250.0 ), Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3(  0,  100.0,  250.0 ),  Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3(  0,    0.0,  250.0 ),  Vector2( 2.0, 0.0 ) ) ),
        g_pMaterials[0]
    ),

    // 右.
    Quad(
        Face4(
            Vertex( Vector3( 100,   0.0,  0.0 ),   Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3( 100, 100.0,  0.0 ),   Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3( 100, 100.0, 250.0 ),  Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3( 100,  0.0, 250.0 ),   Vector2( 2.0, 0.0 ) ) ),
        g_pMaterials[0]
    ),

    // 上
    Quad(
        Face4(
            Vertex( Vector3( 100, 100.0, 250.0 ),  Vector2( 2.0, 0.0 ) ),
            Vertex( Vector3( 100, 100.0,  0.0 ),   Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3( 0, 100.0,  0.0 ),     Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3( 0, 100.0, 250.0 ),    Vector2( 0.0, 0.0 ) ) ),
        g_pMaterials[0]
   ),

    // 下.
    Quad(
        Face4(
            Vertex( Vector3( 0, 0.0, 250.0 ),      Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3( 0, 0.0,  0.0 ),       Vector2( 0.0, 3.0 ) ),
            Vertex( Vector3( 100, 0.0,  0.0 ),     Vector2( 3.0, 3.0 ) ),
            Vertex( Vector3( 100, 0.0, 250.0 ),    Vector2( 3.0, 0.0 ) ) ),
        g_pMaterials[1]
   ),
};

Mesh g_Mesh;


// 境界ボリューム階層.
IShape* g_pBVH = nullptr;


////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------
//! @brief      シーンとの交差判定をおこないます.
//----------------------------------------------------------------------------------
S3D_INLINE
bool Intersect(const Ray &ray, HitRecord& record)
{
    HitRecord temp;

    // 交差判定.
    if ( g_pBVH->IsHit( ray, temp ) )
    {
        // 更新.
        record = temp;
    }

    return (record.pShape != nullptr);
}

//---------------------------------------------------------------------------------
//! @brief      指定方向からの放射輝度を求めます.
//---------------------------------------------------------------------------------
Color Radiance( const Ray &inRay, s3d::Random &rnd )
{
    HitRecord   record = HitRecord();
    ShadingArg  arg    = ShadingArg();

    Ray   ray( inRay );
    Color W  ( 1.0f, 1.0f, 1.0f );
    Color L  ( 0.0f, 0.0f, 0.0f );

    // 乱数設定.
    arg.random = rnd;

    for( s32 depth=0; /* NOTHING */; ++depth )
    {
        // シーンと交差判定
        if ( !Intersect( ray, record ) )
        { break; }

        // 衝突物体へのポインタ.
        const IShape* pShape = record.pShape;

        // マテリアルへのポインタ.
        const IMaterial* pMaterial = pShape->GetMaterial();

        // 自己発光による放射輝度.
        L += Color::Mul( W, pMaterial->GetEmissive() );

#if 0
        /* 衝突判定のデバッグ */
        //{
        //    // 乱数を更新.
        //    rnd = arg.random;
        //    return pMaterial->GetDebugColor();
        //}
#endif

        // 色の反射率最大のものを得る。ロシアンルーレットで使う。
        // ロシアンルーレットの閾値は任意だが色の反射率等を使うとより良い ... らしい。
        // Memo : 閾値を平均とかにすると，うまい具合にばらけず偏ったりしたので上記を守るのが一番良さげ.
        arg.prob = pMaterial->GetThreshold();

        // 最大深度以上になったら，打ち切るために閾値を急激に下げる.
        if ( depth > g_Config.MaxDepth )
        { arg.prob *= powf( 0.5f, static_cast<f32>( depth - g_Config.MaxDepth ) ); }

        // ロシアンルーレット!
        if ( arg.random.GetAsF32() >= arg.prob )
        { break; }

        // シェーディング引数を設定.
        arg.input    = ray.dir;
        arg.normal   = record.normal;
        arg.texcoord = record.texcoord;

        // 色を求める.
        W = Color::Mul( W, pMaterial->ComputeColor( arg ) );

        // レイを更新.
        ray.Update( record.position, arg.output );

        // 重みがゼロなら，以降の結果はゼロとなり無駄な処理になるので打ち切り.
        if ( ( W.x <= FLT_EPSILON )
          && ( W.y <= FLT_EPSILON )
          && ( W.z <= FLT_EPSILON ) )
        { break; }
    }

    // 乱数を更新.
    rnd = arg.random;

    // 計算結果を返却.
    return L;
}


//----------------------------------------------------------------------------------
//! @brief      経路追跡法を実行します.
//----------------------------------------------------------------------------------
void PathTrace
(
    const s32 width,
    const s32 height,
    const s32 samples,
    const s32 supersamples
) 
{
    u32 numCore = GetNumCPUCore();
    if ( numCore > 1 )
    {
        numCore--;
    }

    // カメラ更新.
    Camera camera;
    camera.Update( 
        Vector3( 50.0f, 52.0f, 220.0f ),
        Vector3( 50.0f, 50.0f, 180.0f ),
        Vector3( 0.0f, 1.0f, 0.0f ),
        F_PIDIV4,
        (f32)width / (f32)height,
        1.0f );

    // レンダーターゲットのメモリを確保.
    g_pRT = new Color[ width * height ];

    // レンダーターゲットをクリア.
    for( s32 i=0; i<width * height; ++i )
    { g_pRT[i] = Color( 0.0, 0.0, 0.0 ); }

    // 全サンプル数.
    const s32 numSamples = samples * supersamples * supersamples;
    const f32 invNumSamples = 1.0f / static_cast<f32>( numSamples );

    // 1サブサンプルあたり.
    const f32 rate = 1.0f / static_cast<f32>( supersamples );
    const f32 halfRate = rate / 2.0f;

    // 時間監視スレッドを走らせる.
    uintptr_t ret = _beginthread( TimeWatch, 0, nullptr );
    assert( ret != -1 );    // 失敗しちゃだめよ.

    // 乱数初期化.
    s3d::Random rnd( 3141592 );

    // supersamples x supersamples のスーパーサンプリング
    for ( s32 sy = 0; sy < supersamples; ++sy ) 
    for ( s32 sx = 0; sx < supersamples; ++sx ) 
    {
        const f32 r1 = sx * rate + halfRate;
        const f32 r2 = sy * rate + halfRate;

        // 一つのサブピクセルあたりsamples回サンプリングする
        for (s32 s = 0; s < samples; s ++)
        {
        #if _OPENMP
            #pragma omp parallel for schedule(dynamic, 1) num_threads(numCore)
        #endif
            // 縦方向のループ.
            for ( s32 y = 0; y < height; ++y ) 
            { 
                // 横方向のループ.
                for ( s32 x = 0; x < width; ++x ) 
                {
                    // ぶっ飛ばすレイを取得.
                    Ray ray = camera.GetRay(
                        ( r1 + x ) / width  - 0.5f,
                        ( r2 + y ) / height - 0.5f );

                    // ピクセルインデックス.
                    const s32 idx = ( ( height - 1 - y ) * width ) + x;

                    // ピクセルカラーを加算.
                    g_pRT[ idx ] += Radiance( ray, rnd ) * invNumSamples;
                }
            }
        }
    }

    g_Mutex.Lock();
    {
        // 終了フラグを立てる.
        g_IsFinished = true;
        g_IsRendered = true;
    }
    g_Mutex.Unlock();

    // スレッドの終了を待機する.
    for( u32 i=0; i<(UINT_MAX-10); ++i )    // 無限ループ対策.
    {
        // ウォッチャーが終了するまで待ち.
        if ( g_WatcherEnd )
        { break; }

        // オネンネしな！
        Sleep( 10 );
    }

    char    filename[256];
    tm      local_time;
    time_t  t = time( nullptr );
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
        SaveToBMP( filename, width, height, &g_pRT[0].x );
    }
    else
    {
        // 最終結果をBMPファイルに出力.
        SaveToBMP( "img/output.bmp", width, height, &g_pRT[0].x );
    }

    // メモリ解放.
    if ( g_pRT )
    {
        delete[] g_pRT;
        g_pRT = nullptr;
    }
}

//-----------------------------------------------------------------------------
//      時間監視メソッド.
//-----------------------------------------------------------------------------
void TimeWatch( void* )
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

        // レンダリング開始してからの時間を取得.
        timer.Stop();
        f64 min = timer.GetElapsedTimeMin();

        // 59.9秒以上立ったらキャプチャー.
        if ( sec > 59.9 )
        {
            char filename[256];
            tm local_time;
            time_t t = time( nullptr );
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

                // 結果をBMPファイルに出力.
                SaveToBMP( filename, g_Config.Width, g_Config.Height, &g_pRT[0].x );
            }
            else
            {
                // 最終結果をBMPファイルに出力.
                SaveToBMP( "img/frame/frame.bmp", g_Config.Width, g_Config.Height, &g_pRT[0].x );
            }

            printf_s( "Captured. %lf min\n", min );

            // タイマーを再スタート.
            captureTimer.Start();
        }

#if 0
        // 1時間以上たった
        if ( min >= 60.0 )
        {
            // 最後のフレームをキャプチャー.
            SaveToBMP( "img/final_frame.bmp", g_Config.Width, g_Config.Height, &g_pRT[0].x );
            printf_s( "Time Over...\n" );

            g_Mutex.Lock();
            {
                g_WatcherEnd = true;
            }
            g_Mutex.Unlock();

            // ループ脱出.
            break;
        }
#endif

        // レンダリングが正常終了している場合.
        if ( g_IsFinished )
        {
            printf_s( "Finished Rendering!!\n" );

            g_IsRendered = true;

            FILE* pFile;
            errno_t err = fopen_s( &pFile, "result.txt", "w" );

            if ( err == 0 )
            {
                fprintf( pFile, "Setting : \n" );
                fprintf( pFile, "    width      = %d\n", g_Config.Width );
                fprintf( pFile, "    height     = %d\n", g_Config.Height );
                fprintf( pFile, "    sample     = %d\n", g_Config.NumSamples );
                fprintf( pFile, "Rendering Time %lf (sec)\n", timer.GetElapsedTimeSec() );
                fprintf( pFile, "               %lf (min)\n", timer.GetElapsedTimeMin() );
                fprintf( pFile, "               %lf (hour)\n", timer.GetElapsedTimeHour() );
                fprintf( pFile, "Per Pixel      %lf (msec)\n", timer.GetElapsedTimeMsec() / ( g_Config.Width * g_Config.Height ) );

                fclose( pFile );
            }

            g_Mutex.Lock();
            {
                g_WatcherEnd = true;
            }
            g_Mutex.Unlock();

            // ループ脱出.
            break;
        }

        // 1 sec 寝かせる.
        Sleep( 1000 );
    }
}

//////////////////////////////////////////////////////////////////////////////////
// App class
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
App::App()
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------------
App::~App()
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      アプリケーションを実行します.
//--------------------------------------------------------------------------------
void App::Run( const Config& config )
{
    // 起動画面.
    ILOG( "//=================================================================" );
    ILOG( "//  File   : s3d.exe" );
    ILOG( "//  Desc   : Path Tarcer \"Salty\"" );
    ILOG( "//  Author : Pocol" );
    ILOG( "//=================================================================" );
    ILOG( " Configuration : " );
    ILOG( "     width      = %d", config.Width );
    ILOG( "     height     = %d", config.Height );
    ILOG( "     sample     = %d", config.NumSamples );
    ILOG( "--------------------------------------------------------------------" );

    // 画像出力用ディレクトリ作成.
    _mkdir( "./img" );
    _mkdir( "./img/frame" );

    // かっこ悪いけど, グローバルに格納.
    g_Config = config;

    const char filename[] = "res/mesh/dosei/dosei.smd";
    if ( !g_Mesh.LoadFromFile( filename ) )
    {
        printf_s( "Error : Load Mesh Failed.\n" );
        assert( false );
        g_Mesh.Release();
        return;
    }

    // シェイプリスト.
    IShape* pShapes[] = {
        &g_Quads[0],
        &g_Quads[1],
        &g_Quads[2],
        &g_Quads[3],
        &g_Quads[4],
        &g_Quads[5],

        &g_Mesh,

        &g_Spheres[0],
        //&g_Spheres[1],
        &g_Spheres[2],
        &g_Triangles[0],
    };


    // 物体の数を算出.
    u32 numShapes = sizeof( pShapes ) / sizeof( pShapes[0] );

    // BVH構築.
    g_pBVH = OBVH::BuildBranch( pShapes, numShapes );
    assert( g_pBVH != nullptr );

    // レイトレ！
    PathTrace( config.Width, config.Height, config.NumSamples, config.NumSubSamples );

    g_Mesh.Release();

    // メモリ解放.
    IDispose* pObj = dynamic_cast<IDispose*>( g_pBVH );
    if ( pObj != nullptr )
    { pObj->Dispose(); }

    if ( g_IsFinished )
    {
        MessageBoxA( nullptr, "レンダリングが正常終了しました!!", "レンダリング終了", MB_ICONINFORMATION | MB_OK );
    }
    else
    {
        MessageBoxA( nullptr, "制限時間内にレンダリングが終了しませんでした...", "レンダリング未完", MB_ICONWARNING | MB_OK );
    }

}


} // namespace s3d

