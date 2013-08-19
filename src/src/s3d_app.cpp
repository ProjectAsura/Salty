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
#include <s3d_timer.h>
#include <s3d_mutex.h>
#include <s3d_onb.h>

#include <iostream>
#include <direct.h>


namespace s3d {

//----------------------------------------------------------------------------------
// Macro
//----------------------------------------------------------------------------------

// Debug Log
#ifndef DLOG
    #if defined(DEBUG) || defined(_DEBUG)
        #define DLOG( x, ... ) printf_s( "[File: %s, Line: %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
    #else
        #define DLOG( x, ... )
    #endif
#endif//DLOG

// Infomation Log
#ifndef ILOG
#define ILOG( x, ... ) printf_s( x"\n", ##__VA_ARGS__ )
#endif//ILOG

// Error Log
#ifndef ELOG
#define ELOG( x, ... ) fprintf_s( stderr, "[File: %s, Line: %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG


//----------------------------------------------------------------------------------
// Global Varaibles.
//----------------------------------------------------------------------------------

MaterialBase g_Materials[] = {
    MaterialBase( Color(0, 0, 0),  Color(0.75, 0.25, 0.25), MATERIAL_TYPE_MATTE ),
    MaterialBase( Color(0, 0, 0),  Color(0.25, 0.25, 0.75), MATERIAL_TYPE_MATTE ),
    MaterialBase( Color(0, 0, 0),  Color(0.75, 0.75, 0.75), MATERIAL_TYPE_MATTE ),
    MaterialBase( Color(0, 0, 0),  Color(0.75, 0.25, 0.75), MATERIAL_TYPE_MATTE ),
    MaterialBase( Color(0, 0, 0),  Color(0.75, 0.75, 0.75), MATERIAL_TYPE_MATTE ),
    MaterialBase( Color(0, 0 ,0),  Color(0.75, 0.75, 0.75), MATERIAL_TYPE_MATTE ),
    MaterialBase( Color(0, 0, 0),  Color(0.25, 0.75, 0.25), MATERIAL_TYPE_MATTE, "../res/texture/test.bmp" ),
    MaterialBase( Color(0, 0, 0),  Color(0.99, 0.99, 0.99), MATERIAL_TYPE_MIRROR ),
    MaterialBase( Color(0, 0, 0),  Color(0.99, 0.99, 0.99), MATERIAL_TYPE_CRYSTAL ),
    MaterialBase( Color(36,36,36), Color(0.0,  0.0,  0.0 ), MATERIAL_TYPE_MATTE ),
    MaterialBase( Color(0, 0, 0),  Color(0.75, 0.75, 0.25), MATERIAL_TYPE_MIRROR )
};

// レンダリングするシーンデータ
Sphere g_Spheres[10] = {
    Sphere( 1e5, Vector3(  1e5+1, 40.8, 81.6 ),  &g_Materials[0] ),    // 左
    Sphere( 1e5, Vector3( -1e5+99, 40.8, 81.6 ), &g_Materials[1] ),    // 右
    Sphere( 1e5, Vector3( 50, 40.8, 1e5 ),       &g_Materials[2] ),    // 奥
    Sphere( 1e5, Vector3( 50, 40.8, -1e5+250 ),  &g_Materials[3] ),    // 手前
    Sphere( 1e5, Vector3( 50, 1e5, 81.6 ),       &g_Materials[4] ),    // 床
    Sphere( 1e5, Vector3( 50, -1e5+81.6, 81.6 ), &g_Materials[5] ),    // 天井
    Sphere( 20,  Vector3( 65, 20, 20 ),          &g_Materials[6] ),    // 緑球
    Sphere( 16.5,Vector3( 27, 16.5, 27 ),        &g_Materials[7] ),    // 鏡
    Sphere( 16.5,Vector3( 77, 16.5, 78 ),        &g_Materials[8] ),    // 水晶.
    Sphere( 15.0,Vector3( 50.0, 90.0, 81.6 ),    &g_Materials[9] ),    // 照明
};

Triangle g_Triangles[] = {
    Triangle( Vector3( 70.0, 30.0, 20.0 ), Vector3( 50.0, 70.0, 10.0 ), Vector3( 30.0, 30.0, 20.0 ), &g_Materials[10] ), 
};

Quad g_Quads[] = {
    Quad( Vector3( 70.0, 30.0, 20.0 ), Vector3( 70.0, 70.0, 10.0 ), Vector3( 30.0, 70.0, 10.0 ), Vector3( 30.0, 30.0, 20.0 ), &g_Materials[8] ), 
};

// シェイプリスト.
IShape* g_pShapes[] = {
    &g_Spheres[0],
    &g_Spheres[1],
    &g_Spheres[2],
    &g_Spheres[3],
    &g_Spheres[4],
    &g_Spheres[5],
    &g_Spheres[6],
    &g_Spheres[7],
    &g_Spheres[8],
    &g_Spheres[9],
    &g_Triangles[0],
};

//----------------------------------------------------------------------------------
//! @brief      ロシアンルーレットに用いる閾値を求めます.
//----------------------------------------------------------------------------------
S3D_INLINE
f64 ComputeThreshold( const Color& value )
{
    f64 result = value.x;
    result = ( value.y > result ) ? value.y : result;
    result = ( value.z > result ) ? value.z : result;

    // 無限ループ対策
    result = Min( result, 0.99 );

    // ゼロ除算対策.
    result = Max( result, DBL_EPSILON );

    return result;
}

//----------------------------------------------------------------------------------
//! @brief      シーンとの交差判定をおこないます.
//----------------------------------------------------------------------------------
S3D_INLINE
bool Intersect(const Ray &ray, HitRecord& record)
{
    const f64 n = sizeof(g_pShapes) / sizeof(IShape*);

    // 初期化
    record = HitRecord();

    // 線形探索
    for (s32 i = 0; i < s32(n); i ++)
    {
        HitRecord temp;

        // 交差判定.
        if ( g_pShapes[i]->IsHit( ray, temp ) )
        {
            // 距離が0以上かつ，前回計算時よりも距離が近い場合.
            if ( temp.distance > 0.0 && temp.distance < record.distance )
            {
                // 更新.
                record = temp;
            }
        }
    }

    return (record.pShape != nullptr);
}

//---------------------------------------------------------------------------------
//! @brief      指定方向からの放射輝度を求めます.
//!
//! @note       このメソッドは edupt を基にしています.
//!             edupt の解説記事は，http://www.slideshare.net/h013/edupt-kaisetsu-22852235
//!             ソースコードは, https://github.com/githole/edupt
//!             をそれぞれ参照してください.
//---------------------------------------------------------------------------------
Color Radiance(const Ray &inRay, s3d::Random &rnd)
{
    HitRecord record = HitRecord();
    Ray ray( inRay );

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

    Color W( 1.0, 1.0, 1.0 );
    Color L( 0.0, 0.0, 0.0 );

    for( s32 depth=0; /* NOTHING */; ++depth )
    {
        // シーンと交差判定
        if ( !Intersect( ray, record ) )
        { break; }

        // 衝突物体へのポインタ.
        IShape* pShape = record.pShape;

        // マテリアルへのポインタ.
        IMaterial* pMaterial = pShape->GetMaterial();

        // 無補正法線データ.
        const Vector3 normalOrg = record.normal;

        // 補正済み法線データ (レイの入出を考慮済み).
        const Vector3 normalMod = ( Vector3::Dot ( normalOrg, ray.dir ) < 0.0 ) ? normalOrg : -normalOrg;

        // 自己発光による放射輝度.
        L += Vector3::Mul( W, pMaterial->GetEmissive() );

        // 色の反射率最大のものを得る。ロシアンルーレットで使う。
        // ロシアンルーレットの閾値は任意だが色の反射率等を使うとより良い ... らしい。
        // Memo : 閾値を平均とかにすると，うまい具合にばらけず偏ったりしたので上記を守るのが一番良さげ.
        const f64 prob = ComputeThreshold( pMaterial->GetColor() );

        // ロシアンルーレット!
        if ( rnd.GetAsF64() >= prob )
        { break; }

        // マテリアル計算.
        // TODO : 関数化で分岐処理をなくして，コードをすっきりさせる.
        switch ( pMaterial->GetType() )
        {
            // 完全拡散面
            case MATERIAL_TYPE_MATTE: 
            {
                #pragma region Lambert

                // normalModの方向を基準とした正規直交基底(w, u, v)を作る。
                // この基底に対する半球内で次のレイを飛ばす。
                OrthonormalBasis onb;
                onb.InitFromW( normalMod );

                // コサイン項を使った重点的サンプリング
                const f64 r1  = D_2PI * rnd.GetAsF64();
                const f64 r2  = rnd.GetAsF64();
                const f64 r2s = sqrt(r2);
                Vector3 dir = Vector3::UnitVector(
                    onb.u * cos(r1) * r2s
                  + onb.v * sin(r1) * r2s
                  + onb.w * sqrt(1.0 - r2) );

                //====================================================================
                // レンダリング方程式に対するモンテカルロ積分を考えると、
                // outgoing_radiance = weight * incoming_radiance。
                // ここで、weight = (ρ/π) * cosθ / pdf(ω) / R になる。
                // ρ/πは完全拡散面のBRDFでρは反射率、cosθはレンダリング方程式におけるコサイン項、
                // pdf(ω)はサンプリング方向についての確率密度関数。
                // Rはロシアンルーレットの確率。
                // 今、コサイン項に比例した確率密度関数によるサンプリングを行っているため、pdf(ω) = cosθ/π
                // よって、weight = ρ/ R。
                //=====================================================================

                // 重み更新.
                Color weight = Vector3::Mul( pMaterial->GetColor(), pMaterial->GetTextureColor( record.texcoord ) ) / prob;
                W = Vector3::Mul( W, weight );

                // レイを更新.
                ray.pos = record.position;
                ray.dir = dir;

                #pragma endregion
            }
            break;

            // 完全鏡面
            case MATERIAL_TYPE_MIRROR:
            {
                #pragma region Mirror
                // ====================================================
                // 完全鏡面なのでレイの反射方向は決定的。
                // ロシアンルーレットの確率で除算するのは上と同じ。
                // ====================================================

                // 反射ベクトルを求める.
                Vector3 reflect = Vector3::Reflect( ray.dir, normalMod );
                reflect.Normalize();

                // 重み更新.
                Color weight = Vector3::Mul( pMaterial->GetColor(), pMaterial->GetTextureColor( record.texcoord ) );
                W = Vector3::Mul( W, weight );

                // レイを更新.
                ray.pos = record.position;
                ray.dir = reflect;

                #pragma endregion
            }
            break;

            // 水晶.
            case MATERIAL_TYPE_CRYSTAL:
            {
                #pragma region Crystal

                // 反射ベクトルを求める.
                Vector3 reflect = Vector3::Reflect( ray.dir, normalOrg );
                reflect.Normalize();

                // レイがオブジェクトから出るのか? 入るのか?
                const bool into = ( Vector3::Dot( normalOrg, normalMod ) > 0.0 );

                // Snellの法則
                const f64 nc    = 1.0;                  // 真空の屈折率
                const f64 nt    = REFRACTITY_CRYSTAL;   // オブジェクトの屈折率
                const f64 nnt   = ( into ) ? ( nc / nt ) : ( nt / nc );
                const f64 ddn   = Vector3::Dot( ray.dir, normalMod );
                const f64 cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);

                // 全反射
                if ( cos2t < 0.0 )
                {
                    // 重み更新.
                    Color weight = Vector3::Mul( pMaterial->GetColor(), pMaterial->GetTextureColor( record.texcoord ) );
                    W = Vector3::Mul( W, weight );

                    // レイを更新.
                    ray.pos = record.position;
                    ray.dir = reflect;

                    // swith-case文脱出.
                    break;
                }

                // 屈折ベクトル.
                Vector3 refract = Vector3::UnitVector(
                    ray.dir * nnt - normalOrg * ( ( into ) ? 1.0 : -1.0 ) * ( ddn * nnt + sqrt(cos2t) ) );

                // SchlickによるFresnelの反射係数の近似を使う
                const f64 a = nt - nc;
                const f64 b = nt + nc;
                const f64 R0 = (a * a) / (b * b);

                const f64 c = 1.0 - ( ( into ) ? -ddn : Vector3::Dot( refract, normalOrg ) );
                const f64 Re = R0 + (1.0 - R0) * pow(c, 5.0); // 反射方向の光が反射してray.dirの方向に運ぶ割合。同時に屈折方向の光が反射する方向に運ぶ割合。
                const f64 Tr = ( 1.0 - Re );

                // 一定以上レイを追跡したら屈折と反射のどちらか一方を追跡する
                // ロシアンルーレットで決定する。
                const f64 P = 0.25 + 0.5 * Re;      // Memo : この式はどこから来たか謎... smallptにも特に説明なし.
                
                // 反射の場合.
                if ( rnd.GetAsF64() < P )
                {
                    // 重み更新.
                    Color weight = Vector3::Mul( pMaterial->GetColor(), pMaterial->GetTextureColor( record.texcoord ) ) * Re / ( P * prob );
                    W = Vector3::Mul( W, weight );

                    // レイを更新.
                    ray.pos = record.position;
                    ray.dir = reflect;
                }
                // 屈折の場合.
                else
                {
                    // 重み更新.
                    Color weight = Vector3::Mul( pMaterial->GetColor(), pMaterial->GetTextureColor( record.texcoord ) ) * Tr / ( ( 1.0 - P ) * prob );
                    W = Vector3::Mul( W, weight );

                    // レイを更新.
                    ray.pos = record.position;
                    ray.dir = refract;
                }

                #pragma endregion
            }
            break;
        }
    }

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
    // カメラ更新.
    Camera camera;
    camera.Update( 
        Vector3( 50.0, 52.0, 220.0 ),
        Vector3( 50.0, 50.0, 180.0 ),
        Vector3( 0.0, 1.0, 0.0 ),
        width,
        height,
        D_PIDIV4,
        1.0 );

    // レンダーターゲットのメモリを確保.
    Color* pRT = new Color[ width * height ];

    // レンダーターゲットをクリア.
    for( s32 i=0; i<width * height; ++i )
    { pRT[i] = Color( 0.0, 0.0, 0.0 ); }

    // 全サンプル数.
    const s32 numSamples = samples * supersamples * supersamples;

    // 1サブサンプルあたり.
    const f64 rate = (1.0 / supersamples);

#if _OPENMP
  #pragma omp parallel for schedule(dynamic, 1) num_threads(8)
#endif
    for (s32 y = 0; y < height; y ++) 
    {
        // 乱数
        s3d::Random rnd(y + 1);

        // 何%完了したか表示する.
        printf_s( "process %lf completed.\n", (100.0 * y / (height - 1)) );

        for (s32 x = 0; x < width; x ++) 
        {
            // ピクセルインデックス.
            const s32 idx = ( y * width ) + x;

            Color accRadiance = Color(0.0, 0.0, 0.0);

            // supersamples x supersamples のスーパーサンプリング
            for (s32 sy = 0; sy < supersamples; sy ++) 
            for (s32 sx = 0; sx < supersamples; sx ++) 
            {
                // 一つのサブピクセルあたりsamples回サンプリングする
                for (s32 s = 0; s < samples; s ++) 
                {
                    const f64 r1 = sx * rate + rate / 2.0;
                    const f64 r2 = sy * rate + rate / 2.0;

                    // ぶっ飛ばすレイを取得.
                    Ray ray = camera.GetRay(
                        ( r1 + x ) / width  - 0.5,
                        ( r2 + y ) / height - 0.5 );

                    // 加算.
                    accRadiance += ( Radiance( ray, rnd ) / numSamples );
                }
            }

            // ピクセルカラーを加算.
            pRT[ idx ] += accRadiance;
        }
    }

    // 最終結果を出力.
    s3d::SaveToBMP( "img/final_result.bmp", width, height, &pRT[0].x );

    // メモリ解放.
    if ( pRT )
    {
        delete[] pRT;
        pRT = nullptr;
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
//      アプリケーションを実行します.s
//--------------------------------------------------------------------------------
void App::Run( Config& config )
{
    // 起動画面.
    ILOG( "//=================================================================" );
    ILOG( "//  File   : s3d.exe" );
    ILOG( "//  Desc   : Path Tarcer \"Salty\"" );
    ILOG( "//  Author : Pocol" );
    ILOG( "//=================================================================" );
    ILOG( " Configuration : " );
    ILOG( "     width      = %d", config.width );
    ILOG( "     height     = %d", config.height );
    ILOG( "     sample     = %d", config.numSamples );
    ILOG( "     sub sumple = %d", config.numSubSamples );
    ILOG( "--------------------------------------------------------------------" );

    // 画像出力用ディレクトリ作成.
    _mkdir( "./img" );
    _mkdir( "./img/frame" );

    // レイトレ！
    PathTrace( config.width, config.height, config.numSamples, config.numSubSamples );
}


} // namespace s3d

