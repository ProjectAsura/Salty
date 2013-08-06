//-------------------------------------------------------------------------------------
// File : s2_bmp.h
// Desc : Bitmap Utility Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include <cstdio>
#include "s2_bmp.h"


//----------------------------------------------------------------------------------------
//! @brief      �r�b�g�}�b�v�t�@�C���w�b�_�[�ł�.
//----------------------------------------------------------------------------------------
#pragma pack( push, 1 )
struct BMP_FILE_HEADER
{
    unsigned short      Type;           // �t�@�C���^�C�v 'BM'
    unsigned int        Size;           // �t�@�C���T�C�Y.
    unsigned short      Reserved1;      // �\��̈� (0�Œ�).
    unsigned short      Reserved2;      // �\��̈� (0�Œ�).
    unsigned int        OffBits;        // �t�@�C���擪����摜�f�[�^�܂ł̃I�t�Z�b�g.
};
#pragma pack( pop )

//----------------------------------------------------------------------------------------
//! @brief      �r�b�g�}�b�v�̈��k�^�C�v�ł�.
//----------------------------------------------------------------------------------------
enum BMP_COMPRESSION_TYPE
{
    BMP_COMPRESSION_RGB       = 0,      // �����k.
    BMP_COMPRESSION_RLE8      = 1,      // RLE���k 8 bits/pixel.
    BMP_COMPRESSION_RLE4      = 2,      // RLE���k 4 bits/pixel.
    BMP_COMPRESSION_BITFIELDS = 3,      // �r�b�g�t�B�[���h.
};

//----------------------------------------------------------------------------------------
//! @brief      �r�b�g�}�b�v���w�b�_�[�ł�.
//----------------------------------------------------------------------------------------
#pragma pack( push, 1 )
struct BMP_INFO_HEADER
{
    unsigned int        Size;           // �w�b�_�T�C�Y (40�Œ�).
    long                Width;          // �摜�̉���.
    long                Height;         // �摜�̏c��.
    unsigned short      Planes;         // �v���[���� (1�Œ�).
    unsigned short      BitCount;       // 1�s�N�Z��������̃r�b�g��.
    unsigned int        Compression;    // ���k�`��.
    unsigned int        SizeImage;      // �摜�f�[�^���̃T�C�Y.
    long                XPelsPerMeter;  // �������̉𑜓x.
    long                YPelsPerMeter;  // �c�����̉𑜓x.
    unsigned int        ClrUsed;        // �i�[����Ă���p���b�g��.
    unsigned int        ClrImportant;   // �d�v�ȃp���b�g�̃C���f�b�N�X.
};
#pragma pack( pop )



//-------------------------------------------------------------------------------------------
//      BMP�t�@�C���w�b�_���������݂܂�.
//-------------------------------------------------------------------------------------------
void WriteBmpFileHeader( BMP_FILE_HEADER& header, FILE* pFile )
{
    fwrite( &header.Type,       sizeof(unsigned short), 1, pFile );
    fwrite( &header.Size,       sizeof(unsigned int),   1, pFile );
    fwrite( &header.Reserved1,  sizeof(unsigned short), 1, pFile );
    fwrite( &header.Reserved2,  sizeof(unsigned short), 1, pFile );
    fwrite( &header.OffBits,    sizeof(unsigned int),   1, pFile );
}

//-------------------------------------------------------------------------------------------
//      BMP���w�b�_���������݂܂�.
//-------------------------------------------------------------------------------------------
void WriteBmpInfoHeader( BMP_INFO_HEADER& header, FILE* pFile )
{
    fwrite( &header.Size,           sizeof(unsigned int),   1, pFile );
    fwrite( &header.Width,          sizeof(long),           1, pFile );
    fwrite( &header.Height,         sizeof(long),           1, pFile );
    fwrite( &header.Planes,         sizeof(unsigned short), 1, pFile );
    fwrite( &header.BitCount,       sizeof(unsigned short), 1, pFile );
    fwrite( &header.Compression,    sizeof(unsigned int),   1, pFile );
    fwrite( &header.SizeImage,      sizeof(unsigned int),   1, pFile );
    fwrite( &header.XPelsPerMeter,  sizeof(long),           1, pFile );
    fwrite( &header.YPelsPerMeter,  sizeof(long),           1, pFile );
    fwrite( &header.ClrUsed,        sizeof(unsigned int),   1, pFile );
    fwrite( &header.ClrImportant,   sizeof(unsigned int),   1, pFile );
}

//-------------------------------------------------------------------------------------------
//      BMP�t�@�C���ɏ����o���܂�.
//-------------------------------------------------------------------------------------------
void WriteBmp( FILE* pFile, const int width, const int height, const COLOR* pPixel )
{
    BMP_FILE_HEADER fileHeader;
    BMP_INFO_HEADER infoHeader;

    fileHeader.Type      = 'MB';
    fileHeader.Size      = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER) + ( width * height * 3 );
    fileHeader.Reserved1 = 0;
    fileHeader.Reserved2 = 0;
    fileHeader.OffBits   = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER);

    infoHeader.Size          = 40;
    infoHeader.Width         = width;
    infoHeader.Height        = height;
    infoHeader.Planes        = 1;
    infoHeader.BitCount      = 24;
    infoHeader.Compression   = BMP_COMPRESSION_RGB;
    infoHeader.SizeImage     = 0;
    infoHeader.XPelsPerMeter = 0;
    infoHeader.YPelsPerMeter = 0;
    infoHeader.ClrUsed       = 0;
    infoHeader.ClrImportant  = 0;

    WriteBmpFileHeader( fileHeader, pFile );
    WriteBmpInfoHeader( infoHeader, pFile );

    for ( int i=0; i<height; ++i )
    {
        for( int j=0; j<width; ++j )
        {
            int index = ( i * width ) + j;
#if 1
            // �t�K���}.
            unsigned char r = (unsigned char)( Clamp( pow( pPixel[index].x, 1.0 / 2.2 ), 0.0, 1.0 ) * 255.0 );
            unsigned char g = (unsigned char)( Clamp( pow( pPixel[index].y, 1.0 / 2.2 ), 0.0, 1.0 ) * 255.0 );
            unsigned char b = (unsigned char)( Clamp( pow( pPixel[index].z, 1.0 / 2.2 ), 0.0, 1.0 ) * 255.0 );
#else
            unsigned char r = (unsigned char)( Clamp( pPixel[index].x, 0.0, 1.0 ) * 255.0 );
            unsigned char g = (unsigned char)( Clamp( pPixel[index].y, 0.0, 1.0 ) * 255.0 );
            unsigned char b = (unsigned char)( Clamp( pPixel[index].z, 0.0, 1.0 ) * 255.0 );
#endif

            {
                fwrite( &b, sizeof(unsigned char), 1, pFile );
                fwrite( &g, sizeof(unsigned char), 1, pFile );
                fwrite( &r, sizeof(unsigned char), 1, pFile );
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
//      BMP�t�@�C���ɕۑ����܂�.
//-------------------------------------------------------------------------------------------
bool SaveToBMP( const char* filename, const int width, const int height, const COLOR* pPixel )
{
    FILE* pFile;
    errno_t err = fopen_s( &pFile, filename, "wb" );
    if ( err != 0 )
    {
        return false;
    }

    WriteBmp( pFile, width, height, pPixel );

    fclose( pFile );
    return true;
}

