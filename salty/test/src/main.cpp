//-------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <cassert>
#include <cstdio>
#include "s2_math.h"
#include "s2_geometry.h"
#include "s2_timer.h"
#include "s2_bmp.h"


#ifndef DLOG
    #if defined(DEBUG) || defined(_DEBUG)
        #define DLOG( x, ... )      printf_s( x"\n", ##__VA_ARGS__ )
    #else
        #define DLOG( x, ... )
    #endif
#endif//DLOG

#ifndef ILOG
#define ILOG( x, ... )              printf_s( x"\n", ##__VA_ARGS__ )
#endif//ILOG


namespace /* anonymous */ {

//------------------------------------------------------------------------------------------
// Constant Values
//------------------------------------------------------------------------------------------
//static const int    MIN_DEPTH = 8;
static const int    MAX_DEPTH = 32;
static const double MAX_P     = 0.99;               // ずっとループしてしまうので，1.0にならないように制限.
static const COLOR  BG_COLOR( 0.0, 0.0, 0.0 );
static const VEC3   UNIT_X  ( 1.0, 0.0, 0.0 );
static const VEC3   UNIT_Y  ( 0.0, 1.0, 0.0 );

// カメラ設定.
static const VEC3   CAMERA_POSITION = VEC3(50.0, 50.0, 220.0);
static const VEC3   CAMERA_TARGET   = VEC3(50.0, 50.0, 180.0);
static const VEC3   CAMERA_UPWARD   = VEC3(0.0, 1.0, 0.0);

// 構成設定.
static const int    WIDTH   = 320;
static const int    HEIGHT  = 240;
static const int    SAMPLES = 4096;      // 増やせば積分の精度があがるけど，重くなる.
static const double ASPECT_RATIO = (double)WIDTH / (double)HEIGHT;


//------------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------------

// 乱数発生器.
static XORSHIFT g_Random( 99999 );

// 材質リスト.
static MATERIAL g_Material[] = {
    MATERIAL( COLOR(1.0,  0.15,  0.15),  MATERIAL_DIFFUSE),   // 左
    MATERIAL( COLOR(0.15, 0.15,  1.0),   MATERIAL_DIFFUSE),  // 右
    MATERIAL( COLOR(0.15, 1.0,   0.15),  MATERIAL_DIFFUSE),  // 奥
    MATERIAL( COLOR(1.0,  0.15,  1.0),   MATERIAL_DIFFUSE),  // 手前
    MATERIAL( COLOR(0.25, 1.0,   0.75),  MATERIAL_DIFFUSE),  // 床
    MATERIAL( COLOR(0.75, 0.75,  0.75),  MATERIAL_DIFFUSE),  // 天井
    MATERIAL( COLOR(1.0,  1.0,   1.0),   MATERIAL_MIRROR),   // 鏡
    MATERIAL( COLOR(1.0,  1.0,   1.0),   MATERIAL_GLASS),    //ガラス
    MATERIAL( COLOR(100, 100, 100),      MATERIAL_LIGHT),    //照明
};

static double size = 200.0;

static QUAD     g_Quads[] = {
    // 左.
    QUAD(
        VEC3( 1,  0,  0 ),
        VEC3( 1,  82, 0 ),
        VEC3( 1,  82, 250 ),
        VEC3( 1,  0,  250 ),
        &g_Material[0]
    ),
    // 右.
    QUAD( 
        VEC3( 99, 82,  0 ),
        VEC3( 99, 0, 0 ),
        VEC3( 99, 0, 250 ),
        VEC3( 99, 82,  250 ),
        &g_Material[1]
    ),
    // 奥.
    QUAD( 
        VEC3(  100,    0, 0 ),
        VEC3(  100,   82, 0 ),
        VEC3(  0, 82, 0 ),
        VEC3(  0, 0,  0 ),
        &g_Material[2]
    ),
    // 手前
    QUAD( 
        VEC3( 100, 0,  250 ),
        VEC3( 100, 82, 250 ),
        VEC3( 0,   82, 250 ),
        VEC3( 0,   0,  250 ),
        &g_Material[3]
    ),
    // 床.
    QUAD( 
        VEC3( 0,  0,  0 ),
        VEC3( 0,  0,  250 ),
        VEC3( 100, 0, 250 ),
        VEC3( 100, 0, 0 ),
        &g_Material[4]
    ),
    // 天井.
    QUAD(
        VEC3(  100, 82, 0 ),
        VEC3(  100, 82, 250 ),
        VEC3(  0,   82, 250 ),
        VEC3(  0,   82, 0 ),
        &g_Material[5]
    ),
    // 照明.
    QUAD(
        VEC3( 70.0, 82, 62.0 ),
        VEC3( 70.0, 82, 102.0 ),
        VEC3( 30.0, 82, 102.0 ),
        VEC3( 30.0, 82, 62.0 ),
        &g_Material[8]
    ),
};

// 物体リスト.
static SPHERE   g_Spheres[] = {
    SPHERE( 1e5,  VEC3(  1e5+1,  40.8,       81.6    ), &g_Material[0] ),   // 左
    SPHERE( 1e5,  VEC3( -1e5+99, 40.8,       81.6    ), &g_Material[1] ),   // 右
    SPHERE( 1e5,  VEC3( 50,      40.8,       1e5     ), &g_Material[2] ),   // 奥
    SPHERE( 1e5,  VEC3( 50,      40.8,      -1e5+250 ), &g_Material[3] ),   // 手前
    SPHERE( 1e5,  VEC3( 50,      1e5,        81.6    ), &g_Material[4] ),   // 床
    SPHERE( 1e5,  VEC3( 50,     -1e5+81.6,   81.6    ), &g_Material[5] ),   // 天井
    SPHERE( 16.5, VEC3( 27,      16.5,       47      ), &g_Material[6] ),   // 鏡
    SPHERE( 16.5, VEC3( 73,      16.5,       78      ), &g_Material[7] ),   //ガラス
    SPHERE( 5.0,  VEC3( 50.0,    75.0,       81.6    ), &g_Material[8] ),   //照明
};

static SPHERE   g_Spheres1[] = {
    SPHERE( 1.0,  VEC3( -2.5, 0.0, 0.0 ), &g_Material[6] ),
    SPHERE( 1.0,  VEC3(  2.5, 0.0, 0.0 ), &g_Material[7] ),
    SPHERE( 0.25, VEC3(  0.0, 0.0, 5.0 ), &g_Material[8] ),
};

static ISHAPE* g_Shapes[] = {
#if 0
    //&g_Spheres[0],
    //&g_Spheres[1],
    //&g_Spheres[2],
    //&g_Spheres[3],
    //&g_Spheres[4],
    //&g_Spheres[5],
#endif
    &g_Spheres[6],
    &g_Spheres[7],
    //&g_Spheres[8],
    &g_Quads[0],
    &g_Quads[1],
    &g_Quads[2],
    &g_Quads[3],
    &g_Quads[4],
    &g_Quads[5],
    &g_Quads[6],
    //&g_Spheres1[0],
    //&g_Spheres1[1],
    //&g_Spheres1[2]
};

} // namespace /* anonymous */


//------------------------------------------------------------------------------------------
//! @brief      次にレイを飛ばす方向を求めます.
//!
//! @param [in]     normal      法線ベクトル.
//! @return     法線ベクトル方向を基準とした基底に対する半球内で次にレイを飛ばす方向を返却します.
//------------------------------------------------------------------------------------------
inline VEC3 NextRayDirection( const VEC3& normal )
{
    VEC3 w = normal;
    VEC3 u;
    VEC3 v;

    // normalの方向を基準とした正規直交基底(w, u, v)を作る。この基底に対する半球内で次のレイを飛ばす。
    if ( fabs( w.x ) > DBL_EPSILON )
    {
        u = Cross( UNIT_Y, w );
        u.Normalize();
    }
    else
    { 
        u = Cross( UNIT_X, w );
        u.Normalize();
    }
                    
    v = Cross( w, u );
    v.Normalize();

    // コサイン項を使った重点的サンプリング
    const double r1  = g_Random.GetAsF64() * 2.0 * PI;
    const double r2  = g_Random.GetAsF64();
    const double r2s = sqrt( r2 );

    // 方向ベクトル算出.
    VEC3 dir = u * cos( r1 ) * r2s
                + v * sin( r1 ) * r2s
                + w * sqrt( 1.0 - r2 );
    dir.Normalize();

    return dir;
}


//------------------------------------------------------------------------------------------
//! @brief      シーンとの交差判定を行います.
//!
//! @param [in]     ray         レイです.
//! @param [out]    distance    衝突地点までの距離です.
//! @param [out]    ppSphere    衝突したオブジェクトへのポインタ.
//! @retval true    衝突しました.
//! @retval false   衝突はありません.
//------------------------------------------------------------------------------------------
inline bool IntersectScene
(
    const RAY&  ray,
    HITRECORD&  record
)
{
    const int n = sizeof( g_Shapes ) / sizeof( ISHAPE* );

    record.distance = DBL_MAX;
    record.pShape   = nullptr;

    HITRECORD result;

    for ( int i = 0; i < n; ++i )
    {
        if ( g_Shapes[i]->Intersect( ray, result ) )
        {
            if ( result.distance > 0.0 && result.distance < record.distance ) 
            {
                record.distance = result.distance;
                record.position = result.position;
                record.normal   = result.normal;
                record.pShape   = result.pShape;
            }
        }
    }

    return ( record.distance < DBL_MAX );
}

//------------------------------------------------------------------------------------------
//! @brief
//------------------------------------------------------------------------------------------
bool ComputeReflectance
(
    COLOR&              accColor,
    COLOR&              accReflectance,
    RAY&                ray,
    const HITRECORD&    record,
    const int           depth
)
{
    // ロシアンルーレットの確率.
    double Probability = 1.0;

    // 衝突物体のマテリアル.
    MATERIAL* pMaterial = record.pShape->GetMaterial();

    // 物体からのレイの入出を考慮した法線ベクトル.
    const VEC3 normal = ( Dot( record.normal, ray.dir ) < 0.0 ) ? record.normal : -record.normal; // 交差位置の法線（物体からのレイの入出を考慮）

    // 一定以上レイを追跡したらロシアンルーレットを実行し追跡を打ち切るかどうかを判断する
    //if ( depth > MIN_DEPTH )
    {
        // 色の反射率最大のものを得る。ロシアンルーレットで使う。
        // ロシアンルーレットの閾値は任意だが色の反射率等を使うとより良い。
        Probability = Max( pMaterial->color.x, Max( pMaterial->color.y, pMaterial->color.z ) );
        Probability = Min( Probability, MAX_P );

        // ロシアンルーレット実行.
        if ( g_Random.GetAsF64() >= Probability )
        {
            // ループ脱出.
            return true;
        }
    }

    // ゼロ除算対策.
    assert( Probability != 0.0 );


    switch( pMaterial->type )
    {
    case MATERIAL_DIFFUSE:
        {
            // レイを更新.
            ray.org = record.position;
            ray.dir = NextRayDirection( normal );

            // リフレクタンスを計算.
            accReflectance = ( accReflectance * pMaterial->color ) / Probability;
        }
        break;

    case MATERIAL_MIRROR:
        {
            // 完全鏡面なのでレイの反射方向は決定的。
            ray.org = record.position;

            // 反射ベクトル.
            ray.dir = ray.dir - normal * 2.0 * Dot(normal, ray.dir);
        }
        break;

    case MATERIAL_GLASS:
        {
            RAY reflect;
            reflect.org = record.position;

            // 反射ベクトル.
            reflect.dir = ray.dir - record.normal * 2.0 * Dot( record.normal, ray.dir );

            // レイがオブジェクトから出るのか、入るのかを判定.
            bool into = ( Dot( record.normal, normal) > 0.0 ); 

            // Snellの法則
            const double nc    = 1.0; // 真空の屈折率
            const double nt    = 1.5; // オブジェクトの屈折率
            const double nnt   = ( into ) ? ( nc / nt ) : ( nt / nc );
            const double ddn   = Dot( ray.dir, normal );
            const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);

            if ( cos2t < 0.0 ) 
            {
                // 全反射した
                ray = reflect;
                accReflectance = ( accReflectance * pMaterial->color ) / Probability;

                // case文脱出.
                break;
            }

            // 屈折ベクトル.
            VEC3 refract = ray.dir * nnt - record.normal * ( ( into ) ? 1.0 : -1.0 ) * ( ddn * nnt + sqrt( cos2t ) );
            refract.Normalize();

            // SchlickによるFresnelの反射係数の近似
            const double a    = nt - nc;
            const double b    = nt + nc;
            const double R0   = (a * a) / (b * b);
            const double c    = 1.0 - ( (into) ? -ddn : Dot(refract, record.normal) );
            const double Re   = R0 + (1.0 - R0) * pow(c, 5.0);
            const double Tr   = 1.0 - Re; // 屈折光の運ぶ光の量
            const double prob = 0.25 + 0.5 * Re;

            // ゼロ除算対策.
            assert( prob != 0.0 );
            assert( prob != 1.0 );

            // 屈折と反射のどちらか一方を追跡する。（さもないと指数的にレイが増える）
            // ロシアンルーレットで決定する。
            if ( g_Random.GetAsF64() < Min( prob, MAX_P ) ) 
            { 
                // 反射
                ray = reflect;

                // リフレクタンスを計算.
                accReflectance = ( accReflectance * pMaterial->color ) * Re / ( prob * Probability );
            } 
            else 
            { 
                // 屈折
                ray.org = record.position;
                ray.dir = refract;

                // リフレクタンスを計算.
                accReflectance = ( accReflectance * pMaterial->color ) * Tr / ( (1.0 - prob) * Probability );
            }
        } 
        break;

    case MATERIAL_LIGHT:
        {
            // カラー計算
            accColor += ( accReflectance * pMaterial->color );

            // レイを更新.
            ray.org = record.position;
            ray.dir = NextRayDirection( normal );
        }
        break;
    }

    return false;
}


//------------------------------------------------------------------------------------------
//! @brief      放射輝度を求めます.
//!
//! @param [in]     inputRay        入射する光線です.
//! @return     放射輝度の値を返却します.
//------------------------------------------------------------------------------------------
inline COLOR Radiance( const RAY &inputRay ) 
{
    COLOR accColor      ( 0.0, 0.0, 0.0 );
    COLOR accReflectance( 1.0, 1.0, 1.0 );

    // 現在のレイを保存しておく
    RAY ray( inputRay );

    HITRECORD   record;

    // 反射回数を制限.
    for ( int depth = 0; /*depth < MAX_DEPTH*/; ++depth ) 
    {
        // シーンとの交差判定を行う.
        if ( !IntersectScene( ray, record ) )
        {
            // ループ脱出.
            break;
        }

        // リフレクタンスを計算.
        if ( ComputeReflectance( accColor, accReflectance, ray, record, depth ) )
        {
            // ループ脱出.
            break;
        }
    }

    return accColor;
}


//--------------------------------------------------------------------------------------------
//! @brief      メインエントリーポイントです.
//!
//! @param [in]     argc        コマンドライン引数の数.
//! @param [in]     argv        コマンドライン引数の値.
//! @return     システムへ値を返却します.
//--------------------------------------------------------------------------------------------
int main( int argc, char **argv )
{
    // 行列演算.
    MTX44 view        = LookAt( CAMERA_POSITION, CAMERA_TARGET, CAMERA_UPWARD );        // ビュー行列算出.
    MTX44 proj        = PerspectiveFov( PI / 4.0f, ASPECT_RATIO, 1.0, 1000.0 );      // 射影行列算出.
    MTX44 viewProj    = view * proj;            // ビュー射影行列算出.
    MTX44 invViewProj = Inverse( viewProj );    // 逆ビュー射影行列算出.

    // フレームバッファ.
    COLOR *pImage = new COLOR[ WIDTH * HEIGHT ];

    // カメラからスクリーンへのレイ.
    RAY    ray;

    VEC3 projPos ( 0.0f, 0.0f, 0.0f );
    VEC3 worldPos( 0.0f, 0.0f, 0.0f );

    Timer   clearTime;
    Timer   renderingTime;
    Timer   stopWatch;
    double  rapTime = 0.0;

    clearTime.Start();
#ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic, 1) num_threads(8)
#endif
    // フレームバッファのクリア.
    for( int y=0; y<HEIGHT; ++y )
    {
        for( int x=0; x<WIDTH; ++x )
        {
            int idx = y * WIDTH + x;
            pImage[ idx ] = BG_COLOR;
        }
    }
    clearTime.End();

    ILOG( "Start Rendering." );
    renderingTime.Start();
    stopWatch.Start();
    for ( int y=0; y<HEIGHT; ++y ) 
    {
        // 10秒ごとに表示させる.
        stopWatch.End();
        rapTime = stopWatch.GetElapsedTimeSec();
        if ( rapTime > 9.9f )
        {
            ILOG( "Rendering ( %d sample ) %lf%%", SAMPLES, 100.0 * y / (HEIGHT - 1) );
            stopWatch.Start();
        }

    #ifdef _OPENMP
        #pragma omp parallel for schedule(dynamic, 1) num_threads(8)
    #endif
        for ( int x=0; x<WIDTH; ++x )
        {
            // ゼロクリア.
            COLOR accRadiance( 0.0, 0.0, 0.0 );

            // 1ピクセルあたりsamples回サンプリングする.
            for ( int s = 0; s < SAMPLES; ++s ) 
            {
                // テントフィルターによってサンプリング
                // ピクセル範囲で一様にサンプリングするのではなく、ピクセル中央付近にサンプルがたくさん集まるように偏りを生じさせる
                const double r1 = 2.0 * g_Random.GetAsF64();
                const double r2 = 2.0 * g_Random.GetAsF64();

                const double dx = ( r1 < 1.0 ) ? sqrt(r1) - 1.0 : 1.0 - sqrt(2.0 - r1);
                const double dy = ( r2 < 1.0 ) ? sqrt(r2) - 1.0 : 1.0 - sqrt(2.0 - r2);

                // 射影空間上でのピクセル位置.
                projPos.x = -2.0f * ( (float)x + (float)dx ) / WIDTH  + 1.0f;
                projPos.y = -2.0f * ( (float)y + (float)dy ) / HEIGHT + 1.0f;

                // ワールド空間のスクリーン上のピクセル位置.
                worldPos = TransformCoord( projPos, invViewProj );

                // カメラ位置からスクリーン位置へレイを飛ばす.
                ray.org = CAMERA_POSITION;
                ray.dir = Normalize( worldPos - CAMERA_POSITION );

                // 放射輝度を蓄積.
                accRadiance += ( Radiance( ray ) / SAMPLES );
            }

            // ピクセルカラーを設定.
            int idx = ( WIDTH * y ) + x;
            pImage[ idx ] += accRadiance;
        }
    }
    renderingTime.End();
    ILOG( "End Rendering." );


    // BMPファイルに保存する.
    if ( !SaveToBMP( "result.bmp", WIDTH, HEIGHT, pImage ) )
    {
        // 異常終了.
        return -1;
    }

    // 計測結果を書き出し.
    FILE* fp;
    errno_t err = fopen_s( &fp, "result.txt", "w" );

    if ( err != 0 )
    {
        return -1;
    }

    fprintf_s( fp, "Clear Time     = %lf(sec)\n", clearTime.GetElapsedTimeSec() );
    fprintf_s( fp, "Rendering Time = %lf(sec)\n", renderingTime.GetElapsedTimeSec() );
    fprintf_s( fp, "Per Pixel Time = %lf(sec)\n", renderingTime.GetElapsedTimeSec() / ( WIDTH * HEIGHT ) );

    fclose( fp );

    // 正常終了.
    return 0;
}

