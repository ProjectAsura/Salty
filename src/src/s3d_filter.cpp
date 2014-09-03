//------------------------------------------------------------------------------------------------
// File : s3d_filter.cpp
// Desc : Filter Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------------
#include <s3d_filter.h>
#include <s3d_math.h>


namespace /* anonymous */ {

//------------------------------------------------------------------------------------------------
// Constant Values
//------------------------------------------------------------------------------------------------
const s3d::Vector3 RGB2Y( 0.29900f, 0.58700f, 0.11400f );


//------------------------------------------------------------------------------------------------
//      輝度値を取得します.
//------------------------------------------------------------------------------------------------
S3D_INLINE
f32 RGBToY( const s3d::Vector3& value )
{ return s3d::Vector3::Dot( RGB2Y, value ); }

//------------------------------------------------------------------------------------------------
//      対数平均と最大輝度値を求めます.
//------------------------------------------------------------------------------------------------
void ComputeLogarithmicAverage
(
    const s32           width,
    const s32           height,
    const s3d::Vector3* pPixels,
    const f32           epsilon,
    f32&                aveLw,
    f32&                maxLw
)
{
    auto init = false;
    for( auto i=0; i<height; ++i )
    {
        for( auto j=0; j<width; ++j )
        {
            // ピクセル番号.
            auto idx = ( i * width ) + j;

            // 輝度値に変換.
            auto Lw = RGBToY( pPixels[idx] );

            // 最大輝度値を求める.
            if ( !init )
            {
                maxLw = Lw;
                init = true;
            }
            else
            { maxLw = s3d::Max( maxLw, Lw ); }

            // 輝度値の対数総和を求める.
            aveLw += logf( epsilon + Lw );
        }
    }

    // ピクセル数で除算.
    aveLw /= ( width * height );

    // 指数をとる.
    aveLw = expf( aveLw );
}

} // namespace /* anonymous */


namespace s3d {

//------------------------------------------------------------------------------------------------
//      トーンマッピングを行います.
//------------------------------------------------------------------------------------------------
std::vector<Vector3> ToneMapping( const s32 width, const s32 height, const Vector3* pPixels )
{
    assert( pPixels != nullptr );

    auto a     = 0.18f;
    auto aveLw = 0.0f;
    auto maxLw = 0.0f;

    // 対数平均と最大輝度値を求める.
    ComputeLogarithmicAverage( width, height, pPixels, 0.00001f, aveLw, maxLw );

    auto coeff = a / aveLw;
    auto maxLw2 = maxLw * coeff;
    maxLw2 *= maxLw2;

    // メモリを確保.
    std::vector<Vector3> result;
    result.resize( width * height );
    result.shrink_to_fit();

    for( auto i=0; i<height; ++i )
    {
        for( auto j=0; j<width; ++j )
        {
            // ピクセル番号.
            auto idx = ( i * width ) + j;

            // "Realistic Ray Tracing" p.180 式(12.9)より
            auto L   = ( pPixels[idx] * coeff );

            // "Realistic Ray Tracing" p.181 式(12.11)より
            auto Ld  = L * ( 1.0f + ( L / maxLw2 ) ) / ( 1.0f + L );

            // トーンマッピングした結果を格納.
            result[idx] = Ld;
        }
    }

    // トーンマッピングした結果を返却する.
    return result;
}

} // namespace s3d
