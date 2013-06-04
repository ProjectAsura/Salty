//---------------------------------------------------------------------------------------------
// File : FntData.h
// Desc : Font Data Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------------

#ifndef __FNT_DATA_H__
#define __FNT_DATA_H__


enum FNT_FORMAT
{
    FNT_FORMAT_R8G8B8A8 = 0,        // DXGI_FORMAT_R8G8B8A8_UNORMに対応.
    FNT_FORMAT_BC2                  // DXGI_FORMAT_BC2_UNORMに対応.
};

struct FNT_TEXTURE_HEADER
{
    unsigned int    width;          //!< 横幅です.
    unsigned int    height;         //!< 縦幅です.
    unsigned int    format;         //!< フォーマットです.
    unsigned int    stride;         //!< ストライドです.
    unsigned int    rows;           //!< ローです.
};

struct FNT_DATA_HEADER
{
    char                fontName[ 32 ];         //!< フォント名です.
    unsigned int        fontWidth;              //!< フォント幅です.
    unsigned int        fontHeight;             //!< フォント高さです.
    FNT_TEXTURE_HEADER  textureHeader;          //!< テクスチャヘッダです.
};

struct FNT_FILE_HEADER
{
    unsigned char   magic[ 4 ];                 //!< マジックです "FNT\0"
    unsigned int    version;                    //!< ファイルバージョンです.
    unsigned int    dataHeaderSize;             //!< データヘッダ構造体のサイズです.
    FNT_DATA_HEADER dataHeader;                 //!< データヘッダです.
};

/*
    ファイル構造は.
     FNT_FILE_HEADER
     FNT_DATA_HEADER
     FNT_GLYPH_SECTION ( FNT_GLYPH * griphCount )
     TEXTURE_DATA_SECTION ( u8 * textureStride * textureRows )
    です.
*/

static const unsigned char FNT_MAGIC[4]             = { 'F', 'N', 'T', '\0' };
static const unsigned int  FNT_FILE_VERSION         = 0x00000001;
static const unsigned int  FNT_DATA_HEADER_SIZE     = sizeof( FNT_DATA_HEADER );

#endif//__FNT_DATA_H__