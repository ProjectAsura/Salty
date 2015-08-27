//------------------------------------------------------------------------------------------
// File : s3d_hdr.cpp
// Desc : HDR File Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <s3d_hdr.h>
#include <s3d_math.h>
#include <s3d_logger.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <vector>


namespace /* anonymous */ {

template<typename T>
T Mini(const T& a, const T& b)
{ return ( a < b ) ? a : b; }

////////////////////////////////////////////////////////////////////////////////////////////
// RGBE structure
////////////////////////////////////////////////////////////////////////////////////////////
struct RGBE
{
    union
    {
        struct
        {
            u8 r;
            u8 g;
            u8 b;
            u8 e;
        };
        u8 v[4];
    };
};

//------------------------------------------------------------------------------------------
//      RGBE形式からVector3形式に変換します.
//------------------------------------------------------------------------------------------
S3D_INLINE
s3d::Vector3 RGBEToVec3( const RGBE& val )
{
    s3d::Vector3 result;
    if ( val.e )
    {
        auto f = ldexp( 1.0, val.e - (int)(128+8) );
        result.x = static_cast<f32>( val.r * f );
        result.y = static_cast<f32>( val.g * f );
        result.z = static_cast<f32>( val.b * f );
    }
    else 
    {
        result.x = 0.0f;
        result.y = 0.0f;
        result.z = 0.0f;
    }
    return result;
}

//------------------------------------------------------------------------------------------
//      Vector3形式からRGBE形式に変換します.
//------------------------------------------------------------------------------------------
S3D_INLINE
RGBE Vec3ToRGBE( const s3d::Vector3& val )
{
    RGBE result;
    double d = s3d::Max( val.x, s3d::Max( val.y, val.z ) );

    if ( d <= 1e-32 )
    {
        result.r = 0;
        result.g = 0;
        result.b = 0;
        result.e = 0;
        return result;
    }

    int e;
    double m = frexp(d, &e); // d = m * 2^e
    d = m * 256.0 / d;

    result.r = (u32)(val.x * d);
    result.g = (u32)(val.y * d);
    result.b = (u32)(val.z * d);
    result.e = (u32)(e + 128);
    return result;
}

//------------------------------------------------------------------------------------------
//      HDRファイルのヘッダを書き込みます.
//------------------------------------------------------------------------------------------
void WriteHdrHeader( FILE* pFile, s32 width, s32 height, f32 gamma, f32 exposure )
{
    fprintf_s( pFile, "#?RADIANCE\n" );
    fprintf_s( pFile, "# Made with 0%% pure HDR Shop.\n" );
    fprintf_s( pFile, "FORMAT=32-bit_rle_rgbe\n" );
    fprintf_s( pFile, "GAMMA=%f\n", gamma );
    fprintf_s( pFile, "EXPOSURE=%lf\n", exposure );
    fprintf_s( pFile, "\n" );
    fprintf_s( pFile, "-Y %d +X %d\n", height, width ); // w行のデータがh列ある.
}

//------------------------------------------------------------------------------------------
//      HDRファイルのヘッダを読み込みします.
//------------------------------------------------------------------------------------------
bool ReadHdrHeader( FILE* pFile, s32& width, s32& height, f32& gamma, f32& exposure )
{
    char buf[ 256 ];
    fread( buf, sizeof(char), 2, pFile );

    if ( buf[0] != '#' || buf[1] != '?' )
    { return false; }

    auto valid = false;
    for( ;; )
    {
        if ( fgets( buf, 256, pFile ) == nullptr )
        { break; }

        if ( buf[0] == '\n' )
        { break; }
        else if ( buf[0] == '#' )
        { continue; }
        else
        {
            auto g = 1.0f;
            auto e = 1.0f;
            if ( sscanf_s( buf, "GAMMA=%f\n", &g ) != 0 ) 
            { gamma = g; }
            else if ( sscanf_s( buf, "EXPOSURE=%f\n", &e ) != 0 )
            { exposure = e; }
            else if ( strcmp( buf, "FORMAT=32-bit_rle_rgbe\n" ) == 0 )
            { valid = true; }
        }
    }

    if ( !valid )
    { return false; }

    if ( fgets( buf, 256, pFile ) != nullptr )
    {
        auto w = 0;
        auto h = 0;
        if ( sscanf_s( buf, "-Y %d +X %d\n", &h, &w ) != 0 )
        {
            width = w;
            height = h;
        }
        else if ( sscanf_s( buf, "+X %d -Y %d\n", &w, &h ) != 0 )
        {
            width = w;
            height = h;
        }
        else
        { return false; }
    }

    return true;
}

//------------------------------------------------------------------------------------------
//      旧形式のカラーを読み取ります.
//------------------------------------------------------------------------------------------
bool ReadOldColors( FILE* pFile, RGBE* pLine, s32 count )
{
    auto shift = 0;
    while( 0 < count )
    {
        pLine[0].r = getc( pFile );
        pLine[0].g = getc( pFile );
        pLine[0].b = getc( pFile );
        pLine[0].e = getc( pFile );

        if ( feof( pFile ) || ferror( pFile ) )
            return false;

        if ( pLine[0].r == 1
          && pLine[0].g == 1
          && pLine[0].b == 1 )
        {
            for( auto i=pLine[0].e << shift; i > 0; i-- )
            {
                pLine[0].r = pLine[-1].r;
                pLine[0].g = pLine[-1].g;
                pLine[0].b = pLine[-1].b;
                pLine[0].e = pLine[-1].e;
                pLine++;
                count--;
            }
            shift += 8;
        }
        else
        {
            pLine++;
            count--;
            shift = 0;
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------
//      カラーを読み取ります.
//------------------------------------------------------------------------------------------
bool ReadColor( FILE* pFile, RGBE* pLine, s32 count )
{
    if ( count < 8 || 0x7fff < count )
    { return ReadOldColors( pFile, pLine, count ); }

    auto i = getc( pFile );
    if ( i == EOF )
        return false;

    if ( i != 2 )
    {
        ungetc( i, pFile );
        return ReadOldColors( pFile, pLine, count );
    }

    pLine[0].g = getc( pFile );
    pLine[0].b = getc( pFile );

    if ( ( i = getc( pFile ) ) == EOF )
        return false;

    if ( pLine[0].g != 2 || pLine[0].b & 128 )
    {
        pLine[0].r = 2;
        pLine[0].e = i;
        return ReadOldColors( pFile, pLine + 1, count -1 );
    }

    if ( ( pLine[0].b << 8 | i ) != count )
        return false;

    for( i=0; i<4; ++i )
    {
        for( auto j=0; j<count; )
        {
            auto code = getc( pFile );
            if ( code == EOF )
                return false;

            if ( 128 < code )
            {
                code &= 127;
                auto val = getc( pFile );
                while( code-- )
                { pLine[j++].v[i] = val; }
            }
            else
            {
                while( code-- )
                { pLine[j++].v[i] = getc( pFile ); }
            }
        }
    }

    return ( feof( pFile ) ? false : true );
}

//------------------------------------------------------------------------------------------
//      HDRデータを読み取ります.
//------------------------------------------------------------------------------------------
bool ReadHdrData( FILE* pFile, const s32 width, const s32 height, f32** ppPixels )
{
    auto pLines = new(std::nothrow) RGBE [ width * height ];
    if ( pLines == nullptr )
    { return false; }

    auto pixels = new (std::nothrow) f32 [ width * height * 3 ];
    if ( pixels == nullptr )
    { return false; }

    for( auto y=0; y<height; ++y )
    {
        if ( !ReadColor( pFile, pLines, width ) )
        {
            SafeDeleteArray( pLines );
            SafeDeleteArray( pixels );
            return false;
        }

        for( auto x =0; x < width; x++ )
        {
            auto pix = RGBEToVec3( pLines[x] );
            auto idx = ( x * 3 ) + ( y * width *  3);
            pixels[idx + 0] = pix.x;
            pixels[idx + 1] = pix.y;
            pixels[idx + 2] = pix.z;
        }
    }

    SafeDeleteArray( pLines );
    (*ppPixels) = pixels;

    return true;
}


} // namespace /* anonymous */ 

namespace s3d {

//------------------------------------------------------------------------------------------
//      HDRファイルからデータをロードします.
//------------------------------------------------------------------------------------------
bool LoadFromHDR( const char* filename, s32& width, s32& height, f32& gamma, f32& exposure, f32** ppPixels )
{
    FILE* pFile;

    auto err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        ELOG( "Error : LoadFromHDR() Failed. File Open Failed. filename = %s", filename );
        return false;
    }

    gamma    = 1.0f;
    exposure = 1.0f;
    if ( !ReadHdrHeader( pFile, width, height, gamma, exposure ) )
    {
        ELOG( "Error : LoadFromHDR() Failed. Header Read Failed. filename = %s", filename );
        return false;
    }

    if ( !ReadHdrData( pFile, width, height, ppPixels ) )
    {
        ELOG( "Error : LoadFromHDR() Failed. Data Read Failed. filename = %s", filename );
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
//      HDRファイルにデータをセーブします.
//------------------------------------------------------------------------------------------
bool SaveToHDR( const char* filename, const s32 width, const s32 height, const s32 component, const f32 gamma, const f32 exposure, const f32* pPixels )
{
    FILE* pFile;
    auto err = fopen_s( &pFile, filename, "wb" );
    if ( err != 0 )
    {
        ELOG( "Error : SaveToHDR() Failed. File Open Failed. filename = %s", filename );
        return false;
    }

    WriteHdrHeader( pFile, width, height, gamma, exposure );

    for( auto i = height-1; i >=0; --i )
    {
        std::vector<RGBE> line;
        line.resize(width);
        for( auto j=0; j<width; ++j )
        {
            auto p = Vec3ToRGBE( Vector3(
                                    pPixels[ (j * component + 0) + (i * width * component) ],
                                    pPixels[ (j * component + 1) + (i * width * component) ],
                                    pPixels[ (j * component + 2) + (i * width * component) ] ) );
            line[j] = p;
        }

        fprintf_s( pFile, "%c%c", 0x02, 0x02 );
        fprintf_s( pFile, "%c%c", (width >>8) & 0xFF, width & 0xFF );

        for( auto k=0; k<4; ++k )
        {
            for( auto cursor = 0; cursor < width; )
            {
                const auto cursor_move = ::Mini( 127, width - cursor );
                fprintf_s( pFile, "%c", cursor_move );

                for( auto l = cursor; l < cursor + cursor_move; ++l )
                { fprintf_s( pFile, "%c", line[l].v[k] ); }
                cursor += cursor_move;
            }
        }

        line.clear();
    }

    fclose( pFile );

    return true;
}


} // namespace s3d
