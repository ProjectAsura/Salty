//-------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <cassert>
#include "s2_math.h"
#include "s2_geometry.h"
#include "s2_hdr.h"
#include "s2_timer.h"
#include "asdxMath.h"

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
static const int    MAX_DEPTH = 3;                  // 増やしても大して綺麗にならないので, 3～5ぐらいで設定.
static const double MAX_P     = 0.99;               // ずっとループしてしまうので，1.0にならないように制限.
static const COLOR  BG_COLOR( 0.0, 0.0, 0.0 );
static const VEC3   UNIT_X  ( 1.0, 0.0, 0.0 );
static const VEC3   UNIT_Y  ( 0.0, 1.0, 0.0 );


//------------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------------

// 乱数発生器.
static XORSHIFT g_Random( 99999 );

// 材質リスト.
static MATERIAL g_Material[] = {
    MATERIAL( COLOR(0.0, 0.0, 0.0), COLOR(1.0,  0.15,  0.15),  REFLECTION_DIFFUSE),    // 左
    MATERIAL( COLOR(0.0, 0.0, 0.0), COLOR(0.15, 0.15,  1.0),   REFLECTION_DIFFUSE),    // 右
    MATERIAL( COLOR(0.0, 0.0, 0.0), COLOR(0.15, 1.0,   0.15),  REFLECTION_DIFFUSE),    // 奥
    MATERIAL( COLOR(0.0, 0.0, 0.0), COLOR(1.0,  0.15,  1.0),   REFLECTION_DIFFUSE),    // 手前
    MATERIAL( COLOR(0.0, 0.0, 0.0), COLOR(0.25, 1.0,   0.75),  REFLECTION_DIFFUSE),    // 床
    MATERIAL( COLOR(0.0, 0.0, 0.0), COLOR(0.75, 0.75,  0.75),  REFLECTION_DIFFUSE),    // 天井
    MATERIAL( COLOR(0.0, 0.0, 0.0), COLOR(1.0,  1.0,   1.0),   REFLECTION_SPECULAR),   // 鏡
    MATERIAL( COLOR(0.0, 0.0, 0.0), COLOR(1.0,  1.0,   1.0),   REFLECTION_REFRACTION), //ガラス
    MATERIAL( COLOR(100, 100, 100), COLOR(0.0,  0.0,   0.0),   REFLECTION_DIFFUSE),    //照明
};

static double size = 200.0;

static QUAD     g_Quads[] = {
    QUAD(
        VEC3( 1,  0,  0 ),
        VEC3( 1,  82, 0 ),
        VEC3( 1,  82, 250 ),
        VEC3( 1,  0,  250 ),
        &g_Material[0]
    ),
    QUAD( 
        VEC3( 99, 0,  0 ),
        VEC3( 99, 82, 0 ),
        VEC3( 99, 82, 250 ),
        VEC3( 99, 0,  250 ),
        &g_Material[1]
    ),
    QUAD( 
        VEC3(  100,   0,  0 ),
        VEC3(  100,   82, 0 ),
        VEC3(  0, 82, 0 ),
        VEC3(  0, 0,  0 ),
        &g_Material[2]
    ),
    QUAD( 
        VEC3( 100, 0,  250 ),
        VEC3( 100, 82, 250 ),
        VEC3( 0,   82, 250 ),
        VEC3( 0,   0,  250 ),
        &g_Material[3]
    ),
    QUAD( 
        VEC3( 0,  0,  0 ),
        VEC3( 0,  0,  250 ),
        VEC3( 100, 0, 250 ),
        VEC3( 100, 0, 0 ),
        &g_Material[4]
    ),
    QUAD(
        VEC3(  size, -size, -size ),
        VEC3(  size,  size, -size ),
        VEC3( -size,  size, -size ),
        VEC3( -size, -size, -size ),
        &g_Material[5]
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
    SPHERE( 0.25, VEC3( 0.0, 0.0, 5.0 ), &g_Material[8] ),
};

static ISHAPE* g_Shapes[] = {
    //&g_Spheres[0],
    //&g_Spheres[1],
    //&g_Spheres[2],
    //&g_Spheres[3],
    //&g_Spheres[4],
    &g_Spheres[5],
    &g_Spheres[6],
    &g_Spheres[7],
    &g_Spheres[8],
    &g_Quads[0],
    &g_Quads[1],
    &g_Quads[2],
    &g_Quads[3],
    &g_Quads[4],
    //&g_Quads[5],
    //&g_Spheres1[0],
    //&g_Spheres1[1],
    //&g_Spheres1[2]
};

} // namespace /* anonymous */


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
        if ( g_Shapes[i]->Intersect(ray, result) )
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
//! @brief      放射輝度を求めます.
//!
//! @param [in]     inputRay        入射する光線です.
//! @return     放射輝度の値を返却します.
//------------------------------------------------------------------------------------------
COLOR Radiance( const RAY &inputRay ) 
{
    COLOR accColor      ( 0.0, 0.0, 0.0 );
    COLOR accReflectance( 1.0, 1.0, 1.0 );

    // 現在のレイを保存しておく
    RAY ray( inputRay );

    double  P = 1.0;         // ロシアンルーレットの確率
    HITRECORD   record;
    ISHAPE*     pShape    = nullptr;     // 衝突物体へのポインタ.
    MATERIAL*   pMaterial = nullptr;     // 衝突物体のマテリアルへのポインタ.

    for ( int depth = 0; ; ++depth ) 
    {
        // シーンとの交差判定を行う.
        if ( !IntersectScene( ray, record ) )
        {
            // ループ脱出.
            break;
        }

        // 衝突物体取得.
        pShape = record.pShape;

        // NULLチェック.
        assert( pShape != nullptr );

        // マテリアル取得.
        pMaterial = pShape->GetMaterial();
        assert( pMaterial != nullptr );

        // 交差位置を求める.
        const VEC3 position = record.position;

        // 法線ベクトルを求める.
        const VEC3 direction = record.normal;

        // 物体からのレイの入出を考慮した法線ベクトル.
        const VEC3 normal    = ( Dot( direction, ray.dir ) < 0.0 ) ? direction : -direction; // 交差位置の法線（物体からのレイの入出を考慮）

        // カラー計算
        accColor += ( accReflectance * pMaterial->emission );

        // 一定以上レイを追跡したらロシアンルーレットを実行し追跡を打ち切るかどうかを判断する
        if ( depth > MAX_DEPTH )
        {
            // 色の反射率最大のものを得る。ロシアンルーレットで使う。
            // ロシアンルーレットの閾値は任意だが色の反射率等を使うとより良い。
            P = Max( pMaterial->color.x, Max( pMaterial->color.y, pMaterial->color.z ) );
            P = Min( P, MAX_P );

            // ロシアンルーレット実行.
            if ( g_Random.GetAsF64() >= P )
            {
                // ループ脱出.
                break;
            }
        }

        // ゼロ除算対策.
        assert( P != 0.0 );

        switch( pMaterial->type )
        {
        case REFLECTION_DIFFUSE:
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

                // レイを更新.
                ray.org = position;
                ray.dir = dir;

                // リフレクタンスを計算.
                accReflectance = ( accReflectance * pMaterial->color ) / P;
            }
            break;

        case REFLECTION_SPECULAR:
            {
                // 完全鏡面なのでレイの反射方向は決定的。
                ray.org = position;

                // 反射ベクトル.
                ray.dir = ray.dir - normal * 2.0 * Dot(normal, ray.dir);
            }
            break;

        case REFLECTION_REFRACTION:
            {
                RAY reflect;
                reflect.org = position;

                // 反射ベクトル.
                reflect.dir = ray.dir - direction * 2.0 * Dot( direction, ray.dir );

                // レイがオブジェクトから出るのか、入るのかを判定.
                bool into = ( Dot( direction, normal) > 0.0 ); 

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
                    accReflectance = ( accReflectance * pMaterial->color ) / P;

                    // case文脱出.
                    break;
                }

                // 屈折ベクトル.
                VEC3 refract = ray.dir * nnt - direction * ( ( into ) ? 1.0 : -1.0 ) * ( ddn * nnt + sqrt( cos2t ) );
                refract.Normalize();

                // SchlickによるFresnelの反射係数の近似
                const double a    = nt - nc;
                const double b    = nt + nc;
                const double R0   = (a * a) / (b * b);
                const double c    = 1.0 - ( (into) ? -ddn : Dot(refract, direction) );
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
                    accReflectance = ( accReflectance * pMaterial->color ) * Re / ( prob * P );
                } 
                else 
                { 
                    // 屈折
                    ray.org = position;
                    ray.dir = refract;

                    // リフレクタンスを計算.
                    accReflectance = ( accReflectance * pMaterial->color ) * Tr / ( (1.0 - prob) * P );
                }
            } 
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
    // 構成設定.
    int width   = 320;
    int height  = 240;
    int samples = 32;//2048;      // 増やせば積分の精度があがるけど，重くなる.

    // カメラ設定.
    const VEC3 camera_position = VEC3(50.0, 50.0, 220.0);
    const VEC3 camera_target   = VEC3(50.0, 50.0, 180.0);
    const VEC3 camera_up       = VEC3(0.0, 1.0, 0.0);

    // アスペクト比.
    const double aspectRatio = (double)width / (double)height;

    // 行列演算.
    MTX44 view        = LookAt( camera_position, camera_target, camera_up );
    MTX44 proj        = PerspectiveFov( PI / 4.0f, aspectRatio, 1.0, 1000.0 );
    MTX44 viewProj    = view * proj;
    MTX44 invViewProj = Inverse( viewProj );

    // フレームバッファ.
    COLOR *pImage = new COLOR[ width * height ];

    // カメラからスクリーンへのレイ.
    RAY    ray;

    VEC3 pos( 0.0f, 0.0f, 0.0f );
    VEC3 wpos;

#ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic, 1) num_threads(8)
#endif
    // フレームバッファのクリア.
    for( int y=0; y<height; ++y )
    {
        for( int x=0; x<width; ++x )
        {
            int idx = y * width + x;
            pImage[ idx ] = BG_COLOR;
        }
    }

    // OpenMP使うとなぜかバグる.
    for ( int y=0; y<height; ++y ) 
    {
        printf_s( "Rendering ( %d sample ) %lf%%\n", samples, 100.0 * y / (height - 1) );

        for ( int x=0; x<width; ++x )
        {
            // ゼロクリア.
            COLOR accRadiance( 0.0, 0.0, 0.0 );

            // 1ピクセルあたりsamples回サンプリングする.
            for ( int s = 0; s < samples; ++s ) 
            {
                // テントフィルターによってサンプリング
                // ピクセル範囲で一様にサンプリングするのではなく、ピクセル中央付近にサンプルがたくさん集まるように偏りを生じさせる
                const double r1 = 2.0 * g_Random.GetAsF64();
                const double r2 = 2.0 * g_Random.GetAsF64();

                const double dx = ( r1 < 1.0 ) ? sqrt(r1) - 1.0 : 1.0 - sqrt(2.0 - r1);
                const double dy = ( r2 < 1.0 ) ? sqrt(r2) - 1.0 : 1.0 - sqrt(2.0 - r2);

                // 射影空間上でのピクセル位置.
                pos.x = -2.0f * ( (float)x + (float)dx ) / width  + 1.0f;
                pos.y = -2.0f * ( (float)y + (float)dy ) / height + 1.0f;

                // ワールド空間のスクリーン上のピクセル位置.
                wpos = TransformCoord( pos, invViewProj );

                // カメラ位置からスクリーン位置へレイを飛ばす.
                ray.org = camera_position;
                ray.dir = Normalize( wpos - camera_position );

                // 放射輝度を蓄積.
                accRadiance += ( Radiance( ray ) / samples );
            }

            // ピクセルカラーを設定.
            int idx = ( width * y ) + x;
            pImage[ idx ] += accRadiance;
        }

    }

    // HDRファイルに保存する.
    if ( !SaveToHDRFile( "result2.hdr", pImage, width, height ) )
    {
        // 異常終了.
        return -1;
    }

    // 正常終了.
    return 0;
}

