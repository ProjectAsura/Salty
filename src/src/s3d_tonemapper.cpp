//------------------------------------------------------------------------------------------------
// File : s3d_tonemapper.cpp
// Desc : Tone Mapper Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------------
#include <s3d_tonemapper.h>
#include <s3d_math.h>
#include <s3d_logger.h>


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

            if (s3d::IsNan(Lw))
            { continue; }

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

//------------------------------------------------------------------------------------------------
//      ACES Film
//------------------------------------------------------------------------------------------------
s3d::Vector4 ACESFilm( const s3d::Vector4& color )
{
    const auto a = 2.51f;
    const auto b = 0.03f;
    const auto c = 2.43f;
    const auto d = 0.59f;
    const auto e = 0.14f;
    const auto f = 0.665406f;
    const auto g = 12.0f;
    return s3d::Vector4(
        s3d::Saturate((color.GetX() * (a * color.GetX() * f / g + b)) / (color.GetX() * f / g * (c * color.GetX() * f + d) + e)),
        s3d::Saturate((color.GetY() * (a * color.GetY() * f / g + b)) / (color.GetY() * f / g * (c * color.GetY() * f + d) + e)),
        s3d::Saturate((color.GetZ() * (a * color.GetZ() * f / g + b)) / (color.GetZ() * f / g * (c * color.GetZ() * f + d) + e)),
        color.GetW());
}
s3d::Color4 median_value(s3d::Color4 c[9])
{
    s3d::Color4 buf;
    
    for (auto j = 0; j < 8; j++) 
    {
        for (auto i = 0; i < 8; i++) 
        {
            if (RGBToY(c[i+1]) < RGBToY(c[i]))
            {
                buf = c[i+1];
                c[i+1] = c[i];
                c[i] = buf;
            }
        }
    }
    return c[4];
}

void MedianFilter( const s32 width, const s32 height, const s3d::Color4* pPixels, s3d::Color4* pResult )
{
    for(auto i=0; i<height; ++i)
    {
        for(auto j=0; j<width; ++j)
        {
            auto idx  = i * width + j;
            auto idx1 = (i-1) * width + j - 1;
            auto idx2 = (i-1) * width + j;
            auto idx3 = (i-1) * width + j + 1;

            auto idx4 = i * width + j - 1;
            auto idx5 = i * width + j + 1;

            auto idx6 = (i+1) * width + j - 1;
            auto idx7 = (i+1) * width + j;
            auto idx8 = (i+1) * width + j + 1;

            idx1 = s3d::Clamp(idx1, 0, width * height);
            idx2 = s3d::Clamp(idx2, 0, width * height);
            idx3 = s3d::Clamp(idx3, 0, width * height);
            idx4 = s3d::Clamp(idx4, 0, width * height);
            idx5 = s3d::Clamp(idx5, 0, width * height);
            idx6 = s3d::Clamp(idx6, 0, width * height);
            idx7 = s3d::Clamp(idx7, 0, width * height);
            idx8 = s3d::Clamp(idx8, 0, width * height);

            s3d::Color4 p[] = {
                pPixels[idx],
                pPixels[idx1],
                pPixels[idx2],
                pPixels[idx3],
                pPixels[idx4],
                pPixels[idx5],
                pPixels[idx6],
                pPixels[idx7],
                pPixels[idx8]
            };

            pResult[i * width + j] = median_value(p);
        }
    }
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
    //MedianFilter( width, height, pPixels, pResult );

    switch( type )
    {
        case TONE_MAPPING_REINHARD:
        default:
            ReinhardToneMapping( width, height, pPixels, pResult );
            break;

        case TONE_MAPPING_UNCHARTED2_FILMIC:
            Uncharted2FilmicToneMapping( width, height, pPixels, pResult );
            break;

        case TONE_MAPPING_ACES_FILMIC:
            ACESFilmicToneMapping( width, height, pPixels, pResult );
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
            pResult[idx] = Color4(
                pow( Ld.GetX(), 1.0f / 2.2f ),
                pow( Ld.GetY(), 1.0f / 2.2f ),
                pow( Ld.GetZ(), 1.0f / 2.2f ),
                pPixels[idx].GetW() );
        }
    }
}

//------------------------------------------------------------------------------------------------
//      Uncharted2 Filmic トーンマッピング.
//------------------------------------------------------------------------------------------------
void ToneMapper::Uncharted2FilmicToneMapping
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

            // トーンマッピングした結果を格納.
            pResult[idx] = Color4(
                pow( color.GetX(), 1.0f / 2.2f ),
                pow( color.GetY(), 1.0f / 2.2f ),
                pow( color.GetZ(), 1.0f / 2.2f ),
                pPixels[idx].GetW() );
        }
    }
}

//------------------------------------------------------------------------------------------------
//      ACES Filmic トーンマッピング.
//------------------------------------------------------------------------------------------------
void ToneMapper::ACESFilmicToneMapping
(
    const s32     width,
    const s32     height,
    const Color4* pPixels,
    Color4*       pResult
)
{
    assert(pPixels != nullptr);
    auto a = 0.27f;
    auto aveLw = 0.0f;
    auto maxLw = 0.0f;

    // 対数平均と最大輝度値を求める.
    ComputeLogarithmicAverage(width, height, pPixels, 0.00001f, aveLw, maxLw);

    auto coeff = a / aveLw;

    for (auto i = 0; i<height; ++i)
    {
        for (auto j = 0; j<width; ++j)
        {
            // ピクセル番号.
            auto idx = (i * width) + j;

            auto texelColor = pPixels[idx] * coeff;

            auto color = ACESFilm(texelColor);

            // トーンマッピングした結果を格納.
            pResult[idx] = Color4(
                pow(color.GetX(), 1.0f / 2.2f),
                pow(color.GetY(), 1.0f / 2.2f),
                pow(color.GetZ(), 1.0f / 2.2f),
                pPixels[idx].GetW());
        }
    }
}

} // namespace s3d
