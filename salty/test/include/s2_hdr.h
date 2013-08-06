//---------------------------------------------------------------------------------------
// File : s2_hdr.h
// Desc : HDR Utility
//---------------------------------------------------------------------------------------

#ifndef __S2_HDR_WRITER_H__
#define __S2_HDR_WRITER_H__

//---------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "s2_math.h"


//---------------------------------------------------------------------------------------
//! @brief      HDRのピクセル構造体です.
//---------------------------------------------------------------------------------------
struct HDRPixel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char e;

    HDRPixel(
        const unsigned char r_ = 0,
        const unsigned char g_ = 0,
        const unsigned char b_ = 0,
        const unsigned char e_ = 0
    )
    : r(r_)
    , g(g_)
    , b(b_)
    , e(e_)
    { /* DO_NOTHING */ }

    unsigned char Get(int idx)
    {
        switch (idx) 
        {
            case 0: { return r; }
            case 1: { return g; }
            case 2: { return b; }
            case 3: { return e; }
        }
        return 0;
    }

};

//---------------------------------------------------------------------------------------
//! @brief      doubleのRGB要素を*.hdrフォーマット用に変換します.
//!
//! @param [in]     color       変換する色.
//! @return     hdrフォーマットのピクセルを返却します.
//---------------------------------------------------------------------------------------
HDRPixel GetHdrPixel( const COLOR &color ) 
{
    // 逆ガンマ
    double r = pow( color.x, 1.0 / 2.2 );
    double g = pow( color.y, 1.0 / 2.2 );
    double b = pow( color.z, 1.0 / 2.2 );

    double d = Max( r, Max( g, b ) );

    if ( d <= 1e-32 )
    { return HDRPixel(0, 0, 0, 0); }

    int e;
    double m = frexp(d, &e); // d = m * 2^e
    d = m * 256.0 / d;

    return HDRPixel(
        (unsigned char)(r * d),
        (unsigned char)(g * d),
        (unsigned char)(b * d),
        (unsigned char)(e + 128));
}

//---------------------------------------------------------------------------------------
//! @brief      HDRファイルにピクセルを書き出します.
//!
//! @param [in]     ファイル名.
//! @param [in]     ピクセル.
//! @param [in]     画像の横幅.
//! @param [in]     画像の縦幅.
//! @retval true    書き出しに成功.
//! @retval false   書き出しに失敗.
//---------------------------------------------------------------------------------------
bool SaveToHDRFile
(
    const char*     filename,
    const COLOR*    image,
    const int       width,
    const int       height
)
{
    FILE* fp;
    errno_t err = fopen_s( &fp, filename, "wb" );

    if ( err != 0 )
    {
        fprintf_s( stderr, "Error : %s", filename );
        return false;
    }

    // .hdrフォーマットに従ってデータを書きだす
    // ヘッダ
    unsigned char ret = 0x0a;
    fprintf( fp, "#?RADIANCE%c", (unsigned char)ret );
    fprintf( fp, "# Made with 100%% pure HDR Shop%c", ret );
    fprintf( fp, "FORMAT=32-bit_rle_rgbe%c", ret );
    fprintf( fp, "EXPOSURE=1.0000000000000%c%c", ret, ret);

    // 輝度値書き出し
    fprintf( fp, "-Y %d +X %d%c", height, width, ret );

    for (int i = height - 1; i >= 0; i --) 
    {
        std::vector<HDRPixel> line;

        for (int j = 0; j < width; ++j )
        {
            HDRPixel p = GetHdrPixel(image[j + i * width]);
            line.push_back(p);
        }

        fprintf( fp, "%c%c", 0x02, 0x02 );
        fprintf( fp, "%c%c", (width >> 8) & 0xFF, width & 0xFF);

        for ( int k=0; k<4; ++k ) 
        {
            for ( int cursor=0; cursor<width; ) 
            {
                const int cursor_move = Min(127, width - cursor);
                fprintf(fp, "%c", cursor_move);

                for (int l = cursor;  l < cursor + cursor_move; ++l )
                {
                    fprintf(fp, "%c", line[l].Get(k));
                }

                cursor += cursor_move;
            }
        }
    }

    fclose( fp );
    fp = nullptr;

    return true;
}


#endif//__S2_HDR_WRITER_H__
