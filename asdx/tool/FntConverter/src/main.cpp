//-------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : Font Converter Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <iostream>
#include <Windows.h>
#include <string>
#include <cassert>
#include "FntData.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// FONT_NAME structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct FONT_NAME
{
    CHAR strings[ 32 ];     //!< 文字列です.

    //---------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------
    FONT_NAME()
    { strings[ 0 ] = '\0'; }

    //---------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //!
    //! @param [in]     _strings    設定する文字列.
    //---------------------------------------------------------------------------------------
    FONT_NAME( const char* _strings )
    { strcpy_s( strings, _strings ); }
};


//-------------------------------------------------------------------------------------------
//! @brief      BMPファイルを作成します.
//!
//! @param [in]     filename        出力ファイル名.
//! @param [in]     width           画像の横幅.
//! @param [in]     height          画像の縦幅.
//! @param [in]     pPixelData      ピクセルデータ(RGBA).
//-------------------------------------------------------------------------------------------
void CreateBMP( const char* filename, int width, int height, unsigned char* pPixelData )
{
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;

    // 情報ヘッダの値を設定.
    bmih.biSize          = sizeof(BITMAPINFOHEADER);
    bmih.biWidth         = width;
    bmih.biHeight        = height;
    bmih.biPlanes        = 1;
    bmih.biBitCount      = 32;
    bmih.biCompression   = BI_RGB;
    bmih.biSizeImage     = bmih.biWidth * bmih.biHeight * ( bmih.biBitCount / 8 );
    bmih.biXPelsPerMeter = 0;
    bmih.biYPelsPerMeter = 0;
    bmih.biClrUsed       = 0;
    bmih.biClrImportant  = 0;

    // ファイルヘッダの値を設定.
    bmfh.bfType      = 0x4d42;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits   = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
    bmfh.bfSize      = bmfh.bfOffBits + bmih.biSizeImage;

    DWORD  tmp;
    HANDLE hFile;

    // ファイル生成.
    hFile = CreateFileA( filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL );

    // ファイルヘッダ書き込み.
    WriteFile( hFile, &bmfh, sizeof(bmfh), &tmp, NULL );

    // 情報ヘッダ書き込み.
    WriteFile( hFile, &bmih, sizeof(bmih), &tmp, NULL );

    // ファイルポインタを移動.
    SetFilePointer( hFile, bmfh.bfOffBits, NULL, FILE_BEGIN );
    GdiFlush();

    // ピクセルデータ書き込み.
    WriteFile( hFile, pPixelData, bmih.biSizeImage, &tmp, NULL );

    // ファイルを閉じる.
    CloseHandle( hFile );
}

//----------------------------------------------------------------------------------------------
//! @brief      フォントバイナリを生成します.
//!
//! @param [in]     fontName        フォント名.
//! @param [in]     fontSize        フォントサイズ.
//! @param [in]     outputFileName  出力バイナリファイル名.
//! @retval true    バイナリ出力に成功.
//! @retval false   バイナリ出力に失敗.
//----------------------------------------------------------------------------------------------
bool CreateFontBinary( FONT_NAME fontName, const int fontSize, const char* outputFileName )
{
    char strings[ 97 ];     // ( 0x80 - 0x20 + 1 )個. 最後の+1は NULL終端分.

    // 文字格納.
    for( int i=0x20, idx=0; i<0x80; ++i, ++idx )
    { strings[ idx ] = i; }

    // NULL終端化.
    strings[ 96 ] = '\0';

    // 行列
    const static MAT2 mat = {
        {0,1}, {0,0},
        {0,0}, {0,1}
    };

    unsigned int maxBB_W   = 0;     //!< ビットマップの最大横幅.
    unsigned int maxBB_H   = 0;     //!< ビットマップの最大縦幅.
    LONG         maxWidth  = 0;     //!< 字送り横幅.
    LONG         maxHeight = 0;     //!< 字送り縦幅.

    GLYPHMETRICS gm;
    TEXTMETRICA  tm;

    ZeroMemory( &gm, sizeof(gm) );
    ZeroMemory( &tm, sizeof(tm) );

    // デバイスコンテキストハンドルを取得.
    HDC hdc = GetDC( HWND_DESKTOP );

    // 論理フォントの設定.
    LOGFONTA fnt;
    fnt.lfHeight         = fontSize;
    fnt.lfWidth          = 0;
    fnt.lfEscapement     = 0;
    fnt.lfOrientation    = 0;
    fnt.lfWeight         = FW_DONTCARE;
    fnt.lfItalic         = 0;
    fnt.lfUnderline      = 0;
    fnt.lfStrikeOut      = 0;
    fnt.lfCharSet        = SHIFTJIS_CHARSET;
    fnt.lfOutPrecision   = OUT_DEFAULT_PRECIS;//OUT_TT_ONLY_PRECIS;
    fnt.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
    fnt.lfQuality        = CLEARTYPE_QUALITY;
    fnt.lfPitchAndFamily = DEFAULT_PITCH;
    memcpy( fnt.lfFaceName, fontName.strings, sizeof(CHAR) * 32 );

    // フォントを作成.
    HFONT hFont    = CreateFontIndirectA( &fnt );

    // 作成したフォントを設定.
    HFONT hFontOld = (HFONT)SelectObject( hdc, hFont );

    // サイズチェック.
    for( char* p = strings; *p; p++ )
    {
        // 文字コード取得.
        UINT uChar = (*p);

        // 漢字対策.
        if ( 0x80 <= uChar )
        {
            uChar = ( ((UCHAR*)p)[0] << 8 ) + ((UCHAR*)p)[1];
            p++;
        }

        // ビットマップサイズを取得.
        DWORD size = GetGlyphOutlineA( hdc, uChar, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat );

        // フォント情報を取得.
        GetTextMetricsA( hdc, &tm );

        // ブラックボックスサイズ.
        unsigned int bbX = ( gm.gmBlackBoxX + 3 ) & (~3);
        unsigned int bbY =   gm.gmBlackBoxY;

        // 字送り幅.
        LONG        celX = ( gm.gmCellIncX  + 3 ) & (~3);

        // 最大値判定.
        maxBB_W   = ( maxBB_W   < bbX ) ? bbX : maxBB_W;
        maxBB_H   = ( maxBB_H   < bbY ) ? bbY : maxBB_H;

        maxWidth  = ( maxWidth  < celX        ) ? celX        : maxWidth;
        maxHeight = ( maxHeight < tm.tmHeight ) ? tm.tmHeight : maxHeight;

#if 0
        //printf_s( "char = %c\n", uChar );
        //printf_s( "bbX = %d, bbY = %d\n", bbX, bbY );
        //printf_s( "X = %d, Y = %d\n", gm.gmptGlyphOrigin.x, tm.tmAscent - gm.gmptGlyphOrigin.y );
        //printf_s( "celX = %d, celY = %d\n", celX, gm.gmCellIncY );
        //printf_s( "Height = %d\n", tm.tmHeight );
        //printf_s( "\n" );
#endif
    }

#if 0
    //printf_s( "maxBB_W = %d, maxBB_H = %d\n", maxBB_W, maxBB_H );
    //printf_s( "maxWidth = %d, maxHeight = %d\n", maxWidth, maxHeight );
#endif

    unsigned int textureWidth  = ( maxBB_W * 96 );     // 一文字あたりの幅　* 96文字分.
    unsigned int textureHeight = ( maxHeight * 1 );     // 一文字あたりの幅　* 1行分.
    unsigned int textureSize   = textureWidth * textureHeight * 4;  // ピクセルサイズ.

    // ピクセルのメモリを確保.
    unsigned char* pPixels = new unsigned char [ textureSize ];
    assert( pPixels != nullptr );

    // memsetでうまくいかなかったので，for分でゼロクリア.
    for( unsigned int i=0; i<textureSize; ++i )
    { pPixels[ i ] = 0; }

    int modPixelSize = maxBB_W * maxBB_H;
    unsigned char* pDataMod = new unsigned char[ modPixelSize ];
    assert( pDataMod != nullptr );

    // 文字分ループ.
    for( size_t count=0; count<strlen(strings); count++ )
    {
        // 文字コード取得.
        UINT    uChar = (UINT)strings[ count ];

        // ビットマップサイズを取得.
        DWORD size = GetGlyphOutlineA( hdc, uChar, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat );

        // フォント情報を取得.
        GetTextMetricsA( hdc, &tm );

        // ブラックボックスサイズ.
        unsigned int bbX = ( gm.gmBlackBoxX + 3 ) & (~3);
        unsigned int bbY = gm.gmBlackBoxY;
        
        // ビットマップデータ.
        LPBYTE pData = NULL;

        // サイズがある場合.
        if ( size > 0 )
        {
            // ビットマップのメモリを確保.
            pData = new BYTE[ size ];
            assert( pData != nullptr );

            // memsetがうまくいかなかったので，for分でゼロクリア.
            for( DWORD i=0; i<size; ++i )
            { pData[ i ] = 0; }

            // ビットマップを取得.
            GetGlyphOutlineA( hdc, uChar, GGO_GRAY8_BITMAP, &gm, size, pData, &mat );

            // フォント情報を取得.
            GetTextMetricsA( hdc, &tm );
        }
        else
        {
            // ゼロクリア.
            pData          = 0;
            gm.gmBlackBoxX = 0;
            gm.gmBlackBoxY = 0;
            tm.tmAscent    = 0;

            // 次へ.
            continue;
        }

        // memsetがうまくいかなったので, for分でゼロクリア.
        for( int i=0; i<modPixelSize; ++i )
        { pDataMod[ i ] = 0; }

        // 一定サイズのビットマップに取得したフォントのビットマップを張り付ける.
        unsigned int x = 0;
        unsigned int y = 0;

        // 縦方向のループ.
        for( unsigned int i=0; i<maxBB_H; i++ )
        {
            x = 0;

            // 処理しなくいい行なので，処理をスキップ.
            if ( (LONG)i < ( tm.tmAscent - gm.gmptGlyphOrigin.y ) )
            { continue; }

            // ビットマップの縦幅を超えたので，処理をスキップ.
            if ( y >= bbY )
            { continue; }

            // 横方向のループ
            for( unsigned int j=0; j<maxBB_W; j++ )
            {
                // ターゲット番号.
                unsigned int target = ( i * maxBB_W ) + j;

                // 処理しなくいっていい列なので，処理をスキップ.
                if ( (LONG)j < gm.gmptGlyphOrigin.x )
                { continue; }

                // ビットマップの横幅を超えたので，処理をスキップ.
                if ( x >= bbX )
                { continue; }

                // 大きいサイズ型でアルファ値を計算しておく.
                int  temp  = (int)pData[ y * bbX + x ] * 255 / 64;

                // クランプ.
                temp = ( temp > 255 ) ? 255 : temp;
                temp = ( temp < 0   ) ? 0   : temp;

                // u8型にサイズを落とす.
                pDataMod[ target ] = (unsigned char)temp;

                // ビットマップの横方向を進める.
                x++;
            }

            // ビットマップの縦方向を進める.
            y++;
        }

        // 不要になったのでメモリを解放.
        if ( pData )
        {
            delete [] pData;
            pData = nullptr;
        }

        // でっかいテクスチャに一定サイズのビットマップを張り付ける.
        for( unsigned int i=0; i<maxBB_H; i++ )
        {         
            for( unsigned int j=0; j<maxBB_W; ++j )
            {
                // 入力番号.
                int index = ( maxBB_H - 1 - i ) * maxBB_W + j;
                index = ( index < 0 ) ? 0 : index;
                
                // アルファ値取得.
                unsigned char alpha = pDataMod[ index ];

                // カラー設定.
                unsigned char color = alpha;
                
                // ターゲット番号.
                int target = ( i * textureWidth * 4 ) + ( j * 4 ) + ( count * maxBB_W * 4 );

                // RGBAを設定.
                pPixels[ target + 0 ] = color;
                pPixels[ target + 1 ] = color;
                pPixels[ target + 2 ] = color;
                pPixels[ target + 3 ] = alpha;
            }
        }
    }

    // 不要になったのでメモリを解放.
    if ( pDataMod )
    {
        delete [] pDataMod;
        pDataMod = nullptr;
    }

    // デバッグ用BMPファイル名を設定.
    std::string debugBmpFileName( fontName.strings );
    debugBmpFileName += ".bmp";

    // デバッグ用にBMPファイルを出力.
    CreateBMP( debugBmpFileName.c_str(), textureWidth, textureHeight, pPixels );

    // フォントを破棄.
    SelectObject( hdc, hFontOld );
    DeleteObject( hFont );

    // デバイスコンテキストハンドルを解放.
    ReleaseDC( HWND_DESKTOP, hdc );

    //------------------------------------------
    //　バイナリ出力処理.
    //------------------------------------------
    FILE* pFile;

    // ファイルを開く.
    errno_t err = fopen_s( &pFile, outputFileName, "wb" );
    if ( err != 0 )
    {
        // ピクセルデータのメモリを解放.
        delete [] pPixels;
        pPixels = nullptr;

        // エラーログ出力.
        printf_s( "Error : Ouput File Open Failed. filename = %s\n", outputFileName );

        // 異常終了.
        return false;
    }

    // フォントデータヘッダを設定
    FNT_DATA_HEADER dataHeader;
    strcpy_s( dataHeader.fontName, fontName.strings );
    dataHeader.fontWidth            = maxBB_W;
    dataHeader.fontHeight           = maxHeight;
    dataHeader.textureHeader.width  = textureWidth;
    dataHeader.textureHeader.height = textureHeight;
    dataHeader.textureHeader.format = FNT_FORMAT_R8G8B8A8;
    dataHeader.textureHeader.stride = textureWidth * 4;
    dataHeader.textureHeader.rows   = textureHeight;

    // フォントファイルヘッダを書き込み.
    fwrite( FNT_MAGIC, sizeof( unsigned char ), 4, pFile );
    fwrite( &FNT_FILE_VERSION, sizeof( unsigned int ), 1, pFile );
    fwrite( &FNT_DATA_HEADER_SIZE, sizeof( unsigned int ), 1, pFile );
    fwrite( &dataHeader, FNT_DATA_HEADER_SIZE, 1, pFile );

    // ピクセルデータを書き込み.
    fwrite( pPixels, sizeof( unsigned char ), textureSize, pFile );

    // ファイルを閉じる.
    fclose( pFile );

    // ピクセルデータのメモリを解放.
    delete [] pPixels;
    pPixels = nullptr;

    // 正常終了.
    return true;
}

//---------------------------------------------------------------------------------------------
//! @brief      ヘルプを表示します.
//---------------------------------------------------------------------------------------------
void ShowHelp()
{
    printf_s( "//------------------------------------------------------------------\n" );
    printf_s( "// File : FntConverter.exe\n" );
    printf_s( "// Copyright(c) Project Asura. All right reserved.\n" );
    printf_s( "//------------------------------------------------------------------\n" );
    printf_s( "[使い方] FntConverter.exe -f フォント名 -s フォントサイズ -o 出力ファイル名\n" );
    printf_s( "    ex) FntConverter.exe -f 瀬戸フォントミニ-P -s 20 -o SetoFontMini-P.fnt\n" );
    printf_s( "\n" );
}

//---------------------------------------------------------------------------------------------
//! @brief      メインエントリーポイントです.
//---------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    FONT_NAME       fontName;
    int             fontSize = 0;
    std::string     outfileName;

    // コマンドラインを解析.
    for( int i=1; i<argc; ++i )
    {
        // 出力ファイル名.
        if ( strcmp( argv[ i ], "-o" ) == 0 )
        {
            if ( i + 1 < argc )
            { i++; }

            outfileName = argv[ i ];
        }
        // フォントサイズ.
        else if ( strcmp( argv[ i ], "-s" ) == 0 )
        {
            if ( i + 1 < argc )
            { i++; }

            fontSize = atoi( argv[ i ] );
        }
        // フォント名.
        else if ( strcmp( argv[ i ], "-f" ) == 0 )
        {
            if ( i + 1 < argc )
            { i++; }

            memset( fontName.strings, 0, sizeof( char ) * 32 );
            strcpy_s( fontName.strings, argv[ i ] );
        }
    }

    // 引数チェック.
    if ( ( strlen( fontName.strings ) != 0 )
      && ( fontSize > 0 )
      && ( !outfileName.empty() ) )
    {
        // バイナリ変換実行.
        bool ret = CreateFontBinary( fontName, fontSize, outfileName.c_str() );

        // 成功.
        if ( ret )
        {
            printf_s( "Info : Binary Convert Success!! outputFileName = %s\n", outfileName.c_str() );
        }
        // 失敗.
        else
        {
            printf_s( "Error : Binary Convert Failed. \n" );
            return -1;
        }
    }
    else
    {
        // ヘルプを表示.
        ShowHelp();
    }

    return 0;
}