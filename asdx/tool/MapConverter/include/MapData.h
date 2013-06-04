//-------------------------------------------------------------------------------------------
// File : MapData.h
// Desc : Map Data Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

#ifndef __MAP_DATA_H__
#define __MAP_DATA_H__


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_FORMAT_TYPE enum
/////////////////////////////////////////////////////////////////////////////////////////////
enum MAP_FORMAT_TYPE
{
    MAP_FORMAT_INVALID = -1,
    MAP_FORMAT_A8 = 0,          //!< A8フォーマットです.
    MAP_FORMAT_L8,              //!< L8フォーマットです.
    MAP_FORMAT_R8G8B8A8,        //!< RGBA(8888)フォーマットです.
    MAP_FORMAT_BC1,             //!< BC1(DXT1)フォーマットです.
    MAP_FORMAT_BC2,             //!< BC2(DXT3)フォーマットです.
    MAP_FORMAT_BC3,             //!< BC3(DXT5)フォーマットです.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_DATA_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_DATA_HEADER
{
    unsigned int    width;          //!< 画像の縦幅です.
    unsigned int    height;         //!< 画像の横幅です.
    unsigned int    format;         //!< フォーマットです.
    unsigned int    mipMapCount;    //!< ミップマップ数です.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_RESOURCE_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_RESOURCE_HEADER
{
    unsigned int    width;          //!< 横幅です.
    unsigned int    height;         //!< 縦幅です.
    unsigned int    pitch;          //!< 1行当たりのバイト数です.
    unsigned int    slicePitch;     //!< テクセルデータののバイト数です.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_FILE_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_FILE_HEADER
{
    unsigned char   magic[ 4 ];     //!< マジックです.
    unsigned int    version;        //!< ファイルバージョンです.
    unsigned int    dataHeaderSize; //!< データヘッダ構造体のサイズです.
    MAP_DATA_HEADER dataHeader;     //!< データヘッダです.
};

static const unsigned char MAP_FILE_MAGIC[ 4 ]          = { 'M', 'A', 'P', '\0' };
static const unsigned int  MAP_FILE_VERSION             = 0x00000001;
static const unsigned int  MAP_DATA_HEADER_STRUCT_SIZE  = sizeof( MAP_DATA_HEADER );

/*
 MAP_FILE_HEADER
 MAP_DATA_HEADER
 MAP_RESOURCE_DATA_HEADER
 pixel data

 の順にデータを書き込みます.
*/

#endif//__MAP_DATA_H__