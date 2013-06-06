//---------------------------------------------------------------------------------------------
// File : FntData.h
// Desc : Font Data Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------------

#ifndef __FNT_DATA_H__
#define __FNT_DATA_H__


enum FNT_FORMAT
{
    FNT_FORMAT_R8G8B8A8 = 0,        // DXGI_FORMAT_R8G8B8A8_UNORM�ɑΉ�.
    FNT_FORMAT_BC2                  // DXGI_FORMAT_BC2_UNORM�ɑΉ�.
};

struct FNT_TEXTURE_HEADER
{
    unsigned int    width;          //!< �����ł�.
    unsigned int    height;         //!< �c���ł�.
    unsigned int    format;         //!< �t�H�[�}�b�g�ł�.
    unsigned int    stride;         //!< �X�g���C�h�ł�.
    unsigned int    rows;           //!< ���[�ł�.
};

struct FNT_DATA_HEADER
{
    char                fontName[ 32 ];         //!< �t�H���g���ł�.
    unsigned int        fontWidth;              //!< �t�H���g���ł�.
    unsigned int        fontHeight;             //!< �t�H���g�����ł�.
    FNT_TEXTURE_HEADER  textureHeader;          //!< �e�N�X�`���w�b�_�ł�.
};

struct FNT_FILE_HEADER
{
    unsigned char   magic[ 4 ];                 //!< �}�W�b�N�ł� "FNT\0"
    unsigned int    version;                    //!< �t�@�C���o�[�W�����ł�.
    unsigned int    dataHeaderSize;             //!< �f�[�^�w�b�_�\���̂̃T�C�Y�ł�.
    FNT_DATA_HEADER dataHeader;                 //!< �f�[�^�w�b�_�ł�.
};

/*
    �t�@�C���\����.
     FNT_FILE_HEADER
     FNT_DATA_HEADER
     FNT_GLYPH_SECTION ( FNT_GLYPH * griphCount )
     TEXTURE_DATA_SECTION ( u8 * textureStride * textureRows )
    �ł�.
*/

static const unsigned char FNT_MAGIC[4]             = { 'F', 'N', 'T', '\0' };
static const unsigned int  FNT_FILE_VERSION         = 0x00000001;
static const unsigned int  FNT_DATA_HEADER_SIZE     = sizeof( FNT_DATA_HEADER );

#endif//__FNT_DATA_H__