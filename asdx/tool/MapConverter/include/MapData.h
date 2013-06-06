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
    MAP_FORMAT_A8 = 0,          //!< A8�t�H�[�}�b�g�ł�.
    MAP_FORMAT_L8,              //!< L8�t�H�[�}�b�g�ł�.
    MAP_FORMAT_R8G8B8A8,        //!< RGBA(8888)�t�H�[�}�b�g�ł�.
    MAP_FORMAT_BC1,             //!< BC1(DXT1)�t�H�[�}�b�g�ł�.
    MAP_FORMAT_BC2,             //!< BC2(DXT3)�t�H�[�}�b�g�ł�.
    MAP_FORMAT_BC3,             //!< BC3(DXT5)�t�H�[�}�b�g�ł�.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_DATA_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_DATA_HEADER
{
    unsigned int    width;          //!< �摜�̏c���ł�.
    unsigned int    height;         //!< �摜�̉����ł�.
    unsigned int    format;         //!< �t�H�[�}�b�g�ł�.
    unsigned int    mipMapCount;    //!< �~�b�v�}�b�v���ł�.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_RESOURCE_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_RESOURCE_HEADER
{
    unsigned int    width;          //!< �����ł�.
    unsigned int    height;         //!< �c���ł�.
    unsigned int    pitch;          //!< 1�s������̃o�C�g���ł�.
    unsigned int    slicePitch;     //!< �e�N�Z���f�[�^�̂̃o�C�g���ł�.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_FILE_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_FILE_HEADER
{
    unsigned char   magic[ 4 ];     //!< �}�W�b�N�ł�.
    unsigned int    version;        //!< �t�@�C���o�[�W�����ł�.
    unsigned int    dataHeaderSize; //!< �f�[�^�w�b�_�\���̂̃T�C�Y�ł�.
    MAP_DATA_HEADER dataHeader;     //!< �f�[�^�w�b�_�ł�.
};

static const unsigned char MAP_FILE_MAGIC[ 4 ]          = { 'M', 'A', 'P', '\0' };
static const unsigned int  MAP_FILE_VERSION             = 0x00000001;
static const unsigned int  MAP_DATA_HEADER_STRUCT_SIZE  = sizeof( MAP_DATA_HEADER );

/*
 MAP_FILE_HEADER
 MAP_DATA_HEADER
 MAP_RESOURCE_DATA_HEADER
 pixel data

 �̏��Ƀf�[�^���������݂܂�.
*/

#endif//__MAP_DATA_H__