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
    CHAR strings[ 32 ];     //!< ������ł�.

    //---------------------------------------------------------------------------------------
    //! @brief      �R���X�g���N�^�ł�.
    //---------------------------------------------------------------------------------------
    FONT_NAME()
    { strings[ 0 ] = '\0'; }

    //---------------------------------------------------------------------------------------
    //! @brief      �����t���R���X�g���N�^�ł�.
    //!
    //! @param [in]     _strings    �ݒ肷�镶����.
    //---------------------------------------------------------------------------------------
    FONT_NAME( const char* _strings )
    { strcpy_s( strings, _strings ); }
};


//-------------------------------------------------------------------------------------------
//! @brief      BMP�t�@�C�����쐬���܂�.
//!
//! @param [in]     filename        �o�̓t�@�C����.
//! @param [in]     width           �摜�̉���.
//! @param [in]     height          �摜�̏c��.
//! @param [in]     pPixelData      �s�N�Z���f�[�^(RGBA).
//-------------------------------------------------------------------------------------------
void CreateBMP( const char* filename, int width, int height, unsigned char* pPixelData )
{
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;

    // ���w�b�_�̒l��ݒ�.
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

    // �t�@�C���w�b�_�̒l��ݒ�.
    bmfh.bfType      = 0x4d42;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits   = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
    bmfh.bfSize      = bmfh.bfOffBits + bmih.biSizeImage;

    DWORD  tmp;
    HANDLE hFile;

    // �t�@�C������.
    hFile = CreateFileA( filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL );

    // �t�@�C���w�b�_��������.
    WriteFile( hFile, &bmfh, sizeof(bmfh), &tmp, NULL );

    // ���w�b�_��������.
    WriteFile( hFile, &bmih, sizeof(bmih), &tmp, NULL );

    // �t�@�C���|�C���^���ړ�.
    SetFilePointer( hFile, bmfh.bfOffBits, NULL, FILE_BEGIN );
    GdiFlush();

    // �s�N�Z���f�[�^��������.
    WriteFile( hFile, pPixelData, bmih.biSizeImage, &tmp, NULL );

    // �t�@�C�������.
    CloseHandle( hFile );
}

//----------------------------------------------------------------------------------------------
//! @brief      �t�H���g�o�C�i���𐶐����܂�.
//!
//! @param [in]     fontName        �t�H���g��.
//! @param [in]     fontSize        �t�H���g�T�C�Y.
//! @param [in]     outputFileName  �o�̓o�C�i���t�@�C����.
//! @retval true    �o�C�i���o�͂ɐ���.
//! @retval false   �o�C�i���o�͂Ɏ��s.
//----------------------------------------------------------------------------------------------
bool CreateFontBinary( FONT_NAME fontName, const int fontSize, const char* outputFileName )
{
    char strings[ 97 ];     // ( 0x80 - 0x20 + 1 )��. �Ō��+1�� NULL�I�[��.

    // �����i�[.
    for( int i=0x20, idx=0; i<0x80; ++i, ++idx )
    { strings[ idx ] = i; }

    // NULL�I�[��.
    strings[ 96 ] = '\0';

    // �s��
    const static MAT2 mat = {
        {0,1}, {0,0},
        {0,0}, {0,1}
    };

    unsigned int maxBB_W   = 0;     //!< �r�b�g�}�b�v�̍ő剡��.
    unsigned int maxBB_H   = 0;     //!< �r�b�g�}�b�v�̍ő�c��.
    LONG         maxWidth  = 0;     //!< �����艡��.
    LONG         maxHeight = 0;     //!< ������c��.

    GLYPHMETRICS gm;
    TEXTMETRICA  tm;

    ZeroMemory( &gm, sizeof(gm) );
    ZeroMemory( &tm, sizeof(tm) );

    // �f�o�C�X�R���e�L�X�g�n���h�����擾.
    HDC hdc = GetDC( HWND_DESKTOP );

    // �_���t�H���g�̐ݒ�.
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

    // �t�H���g���쐬.
    HFONT hFont    = CreateFontIndirectA( &fnt );

    // �쐬�����t�H���g��ݒ�.
    HFONT hFontOld = (HFONT)SelectObject( hdc, hFont );

    // �T�C�Y�`�F�b�N.
    for( char* p = strings; *p; p++ )
    {
        // �����R�[�h�擾.
        UINT uChar = (*p);

        // �����΍�.
        if ( 0x80 <= uChar )
        {
            uChar = ( ((UCHAR*)p)[0] << 8 ) + ((UCHAR*)p)[1];
            p++;
        }

        // �r�b�g�}�b�v�T�C�Y���擾.
        DWORD size = GetGlyphOutlineA( hdc, uChar, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat );

        // �t�H���g�����擾.
        GetTextMetricsA( hdc, &tm );

        // �u���b�N�{�b�N�X�T�C�Y.
        unsigned int bbX = ( gm.gmBlackBoxX + 3 ) & (~3);
        unsigned int bbY =   gm.gmBlackBoxY;

        // �����蕝.
        LONG        celX = ( gm.gmCellIncX  + 3 ) & (~3);

        // �ő�l����.
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

    unsigned int textureWidth  = ( maxBB_W * 96 );     // �ꕶ��������̕��@* 96������.
    unsigned int textureHeight = ( maxHeight * 1 );     // �ꕶ��������̕��@* 1�s��.
    unsigned int textureSize   = textureWidth * textureHeight * 4;  // �s�N�Z���T�C�Y.

    // �s�N�Z���̃��������m��.
    unsigned char* pPixels = new unsigned char [ textureSize ];
    assert( pPixels != nullptr );

    // memset�ł��܂������Ȃ������̂ŁCfor���Ń[���N���A.
    for( unsigned int i=0; i<textureSize; ++i )
    { pPixels[ i ] = 0; }

    int modPixelSize = maxBB_W * maxBB_H;
    unsigned char* pDataMod = new unsigned char[ modPixelSize ];
    assert( pDataMod != nullptr );

    // ���������[�v.
    for( size_t count=0; count<strlen(strings); count++ )
    {
        // �����R�[�h�擾.
        UINT    uChar = (UINT)strings[ count ];

        // �r�b�g�}�b�v�T�C�Y���擾.
        DWORD size = GetGlyphOutlineA( hdc, uChar, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat );

        // �t�H���g�����擾.
        GetTextMetricsA( hdc, &tm );

        // �u���b�N�{�b�N�X�T�C�Y.
        unsigned int bbX = ( gm.gmBlackBoxX + 3 ) & (~3);
        unsigned int bbY = gm.gmBlackBoxY;
        
        // �r�b�g�}�b�v�f�[�^.
        LPBYTE pData = NULL;

        // �T�C�Y������ꍇ.
        if ( size > 0 )
        {
            // �r�b�g�}�b�v�̃��������m��.
            pData = new BYTE[ size ];
            assert( pData != nullptr );

            // memset�����܂������Ȃ������̂ŁCfor���Ń[���N���A.
            for( DWORD i=0; i<size; ++i )
            { pData[ i ] = 0; }

            // �r�b�g�}�b�v���擾.
            GetGlyphOutlineA( hdc, uChar, GGO_GRAY8_BITMAP, &gm, size, pData, &mat );

            // �t�H���g�����擾.
            GetTextMetricsA( hdc, &tm );
        }
        else
        {
            // �[���N���A.
            pData          = 0;
            gm.gmBlackBoxX = 0;
            gm.gmBlackBoxY = 0;
            tm.tmAscent    = 0;

            // ����.
            continue;
        }

        // memset�����܂������Ȃ����̂�, for���Ń[���N���A.
        for( int i=0; i<modPixelSize; ++i )
        { pDataMod[ i ] = 0; }

        // ���T�C�Y�̃r�b�g�}�b�v�Ɏ擾�����t�H���g�̃r�b�g�}�b�v�𒣂�t����.
        unsigned int x = 0;
        unsigned int y = 0;

        // �c�����̃��[�v.
        for( unsigned int i=0; i<maxBB_H; i++ )
        {
            x = 0;

            // �������Ȃ������s�Ȃ̂ŁC�������X�L�b�v.
            if ( (LONG)i < ( tm.tmAscent - gm.gmptGlyphOrigin.y ) )
            { continue; }

            // �r�b�g�}�b�v�̏c���𒴂����̂ŁC�������X�L�b�v.
            if ( y >= bbY )
            { continue; }

            // �������̃��[�v
            for( unsigned int j=0; j<maxBB_W; j++ )
            {
                // �^�[�Q�b�g�ԍ�.
                unsigned int target = ( i * maxBB_W ) + j;

                // �������Ȃ������Ă�����Ȃ̂ŁC�������X�L�b�v.
                if ( (LONG)j < gm.gmptGlyphOrigin.x )
                { continue; }

                // �r�b�g�}�b�v�̉����𒴂����̂ŁC�������X�L�b�v.
                if ( x >= bbX )
                { continue; }

                // �傫���T�C�Y�^�ŃA���t�@�l���v�Z���Ă���.
                int  temp  = (int)pData[ y * bbX + x ] * 255 / 64;

                // �N�����v.
                temp = ( temp > 255 ) ? 255 : temp;
                temp = ( temp < 0   ) ? 0   : temp;

                // u8�^�ɃT�C�Y�𗎂Ƃ�.
                pDataMod[ target ] = (unsigned char)temp;

                // �r�b�g�}�b�v�̉�������i�߂�.
                x++;
            }

            // �r�b�g�}�b�v�̏c������i�߂�.
            y++;
        }

        // �s�v�ɂȂ����̂Ń����������.
        if ( pData )
        {
            delete [] pData;
            pData = nullptr;
        }

        // �ł������e�N�X�`���Ɉ��T�C�Y�̃r�b�g�}�b�v�𒣂�t����.
        for( unsigned int i=0; i<maxBB_H; i++ )
        {         
            for( unsigned int j=0; j<maxBB_W; ++j )
            {
                // ���͔ԍ�.
                int index = ( maxBB_H - 1 - i ) * maxBB_W + j;
                index = ( index < 0 ) ? 0 : index;
                
                // �A���t�@�l�擾.
                unsigned char alpha = pDataMod[ index ];

                // �J���[�ݒ�.
                unsigned char color = alpha;
                
                // �^�[�Q�b�g�ԍ�.
                int target = ( i * textureWidth * 4 ) + ( j * 4 ) + ( count * maxBB_W * 4 );

                // RGBA��ݒ�.
                pPixels[ target + 0 ] = color;
                pPixels[ target + 1 ] = color;
                pPixels[ target + 2 ] = color;
                pPixels[ target + 3 ] = alpha;
            }
        }
    }

    // �s�v�ɂȂ����̂Ń����������.
    if ( pDataMod )
    {
        delete [] pDataMod;
        pDataMod = nullptr;
    }

    // �f�o�b�O�pBMP�t�@�C������ݒ�.
    std::string debugBmpFileName( fontName.strings );
    debugBmpFileName += ".bmp";

    // �f�o�b�O�p��BMP�t�@�C�����o��.
    CreateBMP( debugBmpFileName.c_str(), textureWidth, textureHeight, pPixels );

    // �t�H���g��j��.
    SelectObject( hdc, hFontOld );
    DeleteObject( hFont );

    // �f�o�C�X�R���e�L�X�g�n���h�������.
    ReleaseDC( HWND_DESKTOP, hdc );

    //------------------------------------------
    //�@�o�C�i���o�͏���.
    //------------------------------------------
    FILE* pFile;

    // �t�@�C�����J��.
    errno_t err = fopen_s( &pFile, outputFileName, "wb" );
    if ( err != 0 )
    {
        // �s�N�Z���f�[�^�̃����������.
        delete [] pPixels;
        pPixels = nullptr;

        // �G���[���O�o��.
        printf_s( "Error : Ouput File Open Failed. filename = %s\n", outputFileName );

        // �ُ�I��.
        return false;
    }

    // �t�H���g�f�[�^�w�b�_��ݒ�
    FNT_DATA_HEADER dataHeader;
    strcpy_s( dataHeader.fontName, fontName.strings );
    dataHeader.fontWidth            = maxBB_W;
    dataHeader.fontHeight           = maxHeight;
    dataHeader.textureHeader.width  = textureWidth;
    dataHeader.textureHeader.height = textureHeight;
    dataHeader.textureHeader.format = FNT_FORMAT_R8G8B8A8;
    dataHeader.textureHeader.stride = textureWidth * 4;
    dataHeader.textureHeader.rows   = textureHeight;

    // �t�H���g�t�@�C���w�b�_����������.
    fwrite( FNT_MAGIC, sizeof( unsigned char ), 4, pFile );
    fwrite( &FNT_FILE_VERSION, sizeof( unsigned int ), 1, pFile );
    fwrite( &FNT_DATA_HEADER_SIZE, sizeof( unsigned int ), 1, pFile );
    fwrite( &dataHeader, FNT_DATA_HEADER_SIZE, 1, pFile );

    // �s�N�Z���f�[�^����������.
    fwrite( pPixels, sizeof( unsigned char ), textureSize, pFile );

    // �t�@�C�������.
    fclose( pFile );

    // �s�N�Z���f�[�^�̃����������.
    delete [] pPixels;
    pPixels = nullptr;

    // ����I��.
    return true;
}

//---------------------------------------------------------------------------------------------
//! @brief      �w���v��\�����܂�.
//---------------------------------------------------------------------------------------------
void ShowHelp()
{
    printf_s( "//------------------------------------------------------------------\n" );
    printf_s( "// File : FntConverter.exe\n" );
    printf_s( "// Copyright(c) Project Asura. All right reserved.\n" );
    printf_s( "//------------------------------------------------------------------\n" );
    printf_s( "[�g����] FntConverter.exe -f �t�H���g�� -s �t�H���g�T�C�Y -o �o�̓t�@�C����\n" );
    printf_s( "    ex) FntConverter.exe -f ���˃t�H���g�~�j-P -s 20 -o SetoFontMini-P.fnt\n" );
    printf_s( "\n" );
}

//---------------------------------------------------------------------------------------------
//! @brief      ���C���G���g���[�|�C���g�ł�.
//---------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    FONT_NAME       fontName;
    int             fontSize = 0;
    std::string     outfileName;

    // �R�}���h���C�������.
    for( int i=1; i<argc; ++i )
    {
        // �o�̓t�@�C����.
        if ( strcmp( argv[ i ], "-o" ) == 0 )
        {
            if ( i + 1 < argc )
            { i++; }

            outfileName = argv[ i ];
        }
        // �t�H���g�T�C�Y.
        else if ( strcmp( argv[ i ], "-s" ) == 0 )
        {
            if ( i + 1 < argc )
            { i++; }

            fontSize = atoi( argv[ i ] );
        }
        // �t�H���g��.
        else if ( strcmp( argv[ i ], "-f" ) == 0 )
        {
            if ( i + 1 < argc )
            { i++; }

            memset( fontName.strings, 0, sizeof( char ) * 32 );
            strcpy_s( fontName.strings, argv[ i ] );
        }
    }

    // �����`�F�b�N.
    if ( ( strlen( fontName.strings ) != 0 )
      && ( fontSize > 0 )
      && ( !outfileName.empty() ) )
    {
        // �o�C�i���ϊ����s.
        bool ret = CreateFontBinary( fontName, fontSize, outfileName.c_str() );

        // ����.
        if ( ret )
        {
            printf_s( "Info : Binary Convert Success!! outputFileName = %s\n", outfileName.c_str() );
        }
        // ���s.
        else
        {
            printf_s( "Error : Binary Convert Failed. \n" );
            return -1;
        }
    }
    else
    {
        // �w���v��\��.
        ShowHelp();
    }

    return 0;
}