﻿//------------------------------------------------------------------------------------------------
// File : s3d_tonemapper.cpp
// Desc : Tone Mapper Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------------
#include <s3d_tonemapper.h>
#include <s3d_math.h>


namespace /* anonymous */ {

//------------------------------------------------------------------------------------------------
// Constant Values
//------------------------------------------------------------------------------------------------
const s3d::Vector4 RGB2Y  (  0.29900f,  0.58700f,  0.11400f, 0.0f );
const s3d::Vector4 RGB2Cb ( -0.16874f, -0.33126f,  0.50000f, 0.0f );
const s3d::Vector4 RGB2Cr (  0.50000f, -0.41869f, -0.08131f, 0.0f );
const s3d::Vector4 YCbCr2R(  1.00000f,  0.00000f,  1.40200f, 0.0f );
const s3d::Vector4 YCbCr2G(  1.00000f, -0.34414f, -0.71414f, 0.0f );
const s3d::Vector4 YCbCr2B(  1.00000f,  1.77200f,  0.00000f, 0.0f );


//------------------------------------------------------------------------------------------------
//      輝度値を取得します.
//------------------------------------------------------------------------------------------------
S3D_INLINE
f32 RGBToY( const s3d::Vector4& value )
{ return s3d::Vector4::Dot( RGB2Y, value ); }

//------------------------------------------------------------------------------------------------
//      対数平均と最大輝度値を求めます.
//------------------------------------------------------------------------------------------------
void ComputeLogarithmicAverage
(
    const s32           width,
    const s32           height,
    const s3d::Color4*  pPixels,
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

//------------------------------------------------------------------------------------------------
//      Uncharted2のトーンマッピング式.
//------------------------------------------------------------------------------------------------
template<typename T>
T Uncharted2Tonemap( const T& color )
{
    const auto A = 0.15f; // Sholder Strength
    const auto B = 0.50f; // Linear Strength
    const auto C = 0.10f; // Linear Angle
    const auto D = 0.20f; // Toe Strength
    const auto E = 0.01f; // Toe Numerator
    const auto F = 0.30f; // Toe Denominator

    // John Hable, "Uncharted 2 : HDR Lighting" のスライドより.
    return ( (color * ( A * color + C * B) + D * E ) / ( color *( A* color + B ) + D * F )) - E / F;
}

} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// ToneMapper class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      トーンマッピングを行います.
//-------------------------------------------------------------------------------------------------
void ToneMapper::Map
(
    TONE_MAPPING_TYPE type,
    const s32         width, 
    const s32         height,
    const Color4*     pPixels,
    Color4*           pResult
)
{
    switch( type )
    {
        case TONE_MAPPING_REINHARD:
        default:
            ReinhardToneMapping( width, height, pPixels, pResult );
            break;

        case TONE_MAPPING_FILMIC:
            FilmicToneMapping( width, height, pPixels, pResult );
            break;
    }
}

//------------------------------------------------------------------------------------------------
//      Reinhardトーンマッピングを行います.
//------------------------------------------------------------------------------------------------
void ToneMapper::ReinhardToneMapping
(
    const s32     width, 
    const s32     height,
    const Color4* pPixels,
    Color4*        pResult
)
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
            pResult[idx].x = pow( Ld.x, 1.0f / 2.2f );
            pResult[idx].y = pow( Ld.y, 1.0f / 2.2f );
            pResult[idx].z = pow( Ld.z, 1.0f / 2.2f );
            pResult[idx].w = pPixels[idx].w;    // アルファはそのまま.
        }
    }
}

//------------------------------------------------------------------------------------------------
//      Filmic トーンマッピング.
//------------------------------------------------------------------------------------------------
void ToneMapper::FilmicToneMapping
(
    const s32     width,
    const s32     height,
    const Color4* pPixels,
    Color4*       pResult
)
{
    assert( pPixels != nullptr );
    auto a     = 0.18f;
    auto aveLw = 0.0f;
    auto maxLw = 0.0f;

    // 対数平均と最大輝度値を求める.
    ComputeLogarithmicAverage( width, height, pPixels, 0.00001f, aveLw, maxLw );

    auto coeff = a / aveLw;

    const auto LinearWhite = 11.2f;     // "Uncharted 2 : HDR Lighting" に記載の値を使用.

    for( auto i=0; i<height; ++i )
    {
        for( auto j=0; j<width; ++j )
        {
            // ピクセル番号.
            auto idx = ( i * width ) + j;

            auto texelColor = pPixels[idx] * coeff;

            auto color = Uncharted2Tonemap( 2.0f * texelColor ) / Uncharted2Tonemap( LinearWhite );

            pResult[idx].x = pow( color.x, 1.0f / 2.2f );
            pResult[idx].y = pow( color.y, 1.0f / 2.2f );
            pResult[idx].z = pow( color.z, 1.0f / 2.2f );
            pResult[idx].w = color.w;
        }
    }
}

} // namespace s3d