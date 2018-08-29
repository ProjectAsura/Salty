//-------------------------------------------------------------------------------------------------
// File : s3d_tga.cpp
// Desc : Targa Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <s3d_tga.h>


namespace /* anonymous */ {

////////////////////////////////////////////////////////////////////////////////////////////////////
// TGA_FORMA_TYPE enum
////////////////////////////////////////////////////////////////////////////////////////////////////
enum TGA_FORMAT_TYPE
{
    TGA_FORMAT_NONE             = 0,        //!< �C���[�W�Ȃ�.
    TGA_FORMAT_INDEXCOLOR       = 1,        //!< �C���f�b�N�X�J���[(256�F).
    TGA_FORMAT_FULLCOLOR        = 2,        //!< �t���J���[
    TGA_FORMAT_GRAYSCALE        = 3,        //!< ����.
    TGA_FORMAT_RLE_INDEXCOLOR   = 9,        //!< RLE���k�C���f�b�N�X�J���[.
    TGA_FORMAT_RLE_FULLCOLOR    = 10,       //!< RLE���k�t���J���[.
    TGA_FORMAT_RLE_GRAYSCALE    = 11,       //!< RLE���k����.
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// TGA_HEADER structure
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct TGA_HEADER
{
    u8  IdFieldLength;      // ID�t�B�[�h�̃T�C�Y(�͈͂�0�`255).
    u8  HasColorMap;        // �J���[�}�b�v�L��(0=�Ȃ�, 1=����)
    u8  Format;             // �摜�`��.
    u16 ColorMapEntry;      // �J���[�}�b�v�G���g���[.
    u16 ColorMapLength;     // �J���[�}�b�v�̃G���g���[�̑���.
    u8  ColorMapEntrySize;  // �J���[�}�b�v��1�G���g���[������̃r�b�g��.
    u16 OffsetX;            // �摜��X���W.
    u16 OffsetY;            // �摜��Y���W.
    u16 Width;              // �摜�̉���.
    u16 Height;             // �摜�̏c��.
    u8  BitPerPixel;        // �r�b�g�̐[��.
    u8  ImageDescriptor;    // (0~3bit) : ����, 4bit : �i�[����(0=������E,1=�E���獶), 5bit : �i�[����(0=�������, 1=�ォ�牺), 6~7bit : �C���^���[�u(�g�p�s��).
};
#pragma pack( pop )


////////////////////////////////////////////////////////////////////////////////////////////////////
// TGA_FOOTER structure
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct TGA_FOOTER
{
    u32  OffsetExt;      // �g���f�[�^�ւ̃I�t�Z�b�g(byte��) [�I�t�Z�b�g�̓t�@�C���̐擪����].
    u32  OffsetDev;      // �f�B�x���b�p�[�G���A�ւ̃I�t�Z�b�g(byte��)[�I�t�Z�b�g�̓t�@�C���̐擪����].
    char Tag[18];        // 'TRUEVISION-XFILE.\0'
};
#pragma pack( pop )


///////////////////////////////////////////////////////////////////////////////////////////////////
// TGA_EXTENSION structure
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct TGA_EXTENSION
{
    u16     Size;                       //!< �T�C�Y.
    char    AuthorName[ 41 ];           //!< ����Җ�.
    char    AuthorComment[ 324 ];       //!< ����҃R�����g.
    u16     StampMonth;                 //!< �^�C���X�^���v�@��(1-12).
    u16     StampDay;                   //!< �^�C���X�^���v�@��(1-31).
    u16     StampYear;                  //!< �^�C���X�^���v�@�N(4��, ��1989).
    u16     StampHour;                  //!< �^�C���X�^���v�@��(0-23).
    u16     StampMinute;                //!< �^�C���X�^���v�@��(0-59).
    u16     StampSecond;                //!< �^�C���X�^���v�@�b(0-59).
    char    JobName[ 41 ];              //!< �W���u�� (�Ō�̃o�C�g�̓[�����K�{).
    u16     JobHour;                    //!< �W���u����  ��(0-65535)
    u16     JobMinute;                  //!< �W���u���ԁ@��(0-59)
    u16     JobSecond;                  //!< �W���u���ԁ@�b(0-59)
    char    SoftwareId[ 41 ];           //!< �\�t�g�E�F�AID (�Ō�̃o�C�g�̓[�����K�{).
    u16     VersionNumber;              //!< �\�t�g�E�F�A�o�[�W����    VersionNumber * 100�ɂȂ�.
    u8      VersionLetter;              //!< �\�t�g�E�F�A�o�[�W����
    u32     KeyColor;                   //!< �L�[�J���[.
    u16     PixelNumerator;             //!< �s�N�Z���䕪�q�@�s�N�Z������.
    u16     PixelDenominator;           //!< �s�N�Z���䕪��@�s�N�Z���c��.
    u16     GammaNumerator;             //!< �K���}�l���q.
    u16     GammaDenominator;           //!< �K���}�l����
    u32     ColorCorrectionOffset;      //!< �F�␳�e�[�u���ւ̃I�t�Z�b�g.
    u32     StampOffset;                //!< �|�X�e�[�W�X�^���v�摜�ւ̃I�t�Z�b�g.
    u32     ScanLineOffset;             //!< �X�L�������C���I�t�Z�b�g.
    u8      AttributeType;              //!< �A���t�@�`�����l���f�[�^�̃^�C�v
};
#pragma pack( pop )



//-------------------------------------------------------------------------------------------------
//! @brief      8Bit�C���f�b�N�X�J���[�`������͂��܂�.
//!
//! @param[in]      pColorMap       �J���[�}�b�v�ł�.
//-------------------------------------------------------------------------------------------------
void Parse8Bits( FILE* pFile, u32 size, u8* pColorMap, u8* pPixels )
{
    u8 color = 0;
    for( u32 i=0; i<size; ++i )
    {
        color = (u8)fgetc( pFile );
        pPixels[ i * 3 + 2 ] = pColorMap[ color * 3 + 0 ];
        pPixels[ i * 3 + 1 ] = pColorMap[ color * 3 + 1 ];
        pPixels[ i * 3 + 0 ] = pColorMap[ color * 3 + 2 ];
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      16Bit�t���J���[�`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse16Bits( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        u16 color = fgetc( pFile ) + ( fgetc( pFile ) << 8 );
        pPixels[ i * 3 + 0 ] = (u8)(( ( color & 0x7C00 ) >> 10 ) << 3);
        pPixels[ i * 3 + 1 ] = (u8)(( ( color & 0x03E0 ) >>  5 ) << 3);
        pPixels[ i * 3 + 2 ] = (u8)(( ( color & 0x001F ) >>  0 ) << 3);
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      24Bit�t���J���[�`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse24Bits( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        pPixels[ i * 4 + 2 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 1 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 0 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 3 ] = 255;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      32Bit�t���J���[�`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse32Bits( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        pPixels[ i * 4 + 2 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 1 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 0 ] = (u8)fgetc( pFile );
        pPixels[ i * 4 + 3 ] = (u8)fgetc( pFile );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief     8Bit�O���[�X�P�[���`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse8BitsGrayScale( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        pPixels[ i ] = (u8)fgetc( pFile );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      16Bit�O���[�X�P�[���`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse16BitsGrayScale( FILE* pFile, u32 size, u8* pPixels )
{
    for( u32 i=0; i<size; ++i )
    {
        pPixels[ i * 2 + 0 ] = (u8)fgetc( pFile );
        pPixels[ i * 2 + 1 ] = (u8)fgetc( pFile );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      8BitRLE���k�C���f�b�N�X�J���[�`������͂��܂�.
//!
//! @param[in]  pColorMap       �J���[�}�b�v�ł�.
//-------------------------------------------------------------------------------------------------
void Parse8BitsRLE( FILE* pFile, u8* pColorMap, u32 size, u8* pPixels )
{
    u32 count  = 0;
    u8  color  = 0;
    u8  header = 0;
    u8* ptr    = pPixels;

    while( ptr < pPixels + size )   // size = width * height * 3.
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            color = (u8)fgetc( pFile );

            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                ptr[ 0 ] = pColorMap[ color * 3 + 2 ];
                ptr[ 1 ] = pColorMap[ color * 3 + 1 ];
                ptr[ 2 ] = pColorMap[ color * 3 + 0 ];
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                color = (u8)fgetc( pFile );

                ptr[ 0 ] = pColorMap[ color * 3 + 2 ];
                ptr[ 1 ] = pColorMap[ color * 3 + 1 ];
                ptr[ 2 ] = pColorMap[ color * 3 + 0 ];
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      16BitRLE���k�t���J���[�`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse16BitsRLE( FILE* pFile, u32 size, u8* pPixels )
{
    u32 count  = 0;
    u16 color  = 0;
    u8  header = 0;
    u8* ptr    = pPixels;

    while( ptr < pPixels + size )   // size = width * height * 3.
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            color = fgetc( pFile ) + ( fgetc( pFile ) << 8 ); 

            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                ptr[ 0 ] = (u8)(( ( color & 0x7C00 ) >> 10 ) << 3);
                ptr[ 1 ] = (u8)(( ( color & 0x03E0 ) >>  5 ) << 3);
                ptr[ 2 ] = (u8)(( ( color & 0x001F ) >>  0 ) << 3);
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                color = fgetc( pFile ) + ( fgetc( pFile ) << 8 );

                ptr[ 0 ] = (u8)(( ( color & 0x7C00 ) >> 10 ) << 3);
                ptr[ 1 ] = (u8)(( ( color & 0x03E0 ) >>  5 ) << 3);
                ptr[ 2 ] = (u8)(( ( color & 0x001F ) >>  0 ) << 3);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      24BitRLE���k�t���J���[�`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse24BitsRLE( FILE* pFile, u32 size, u8* pPixels )
{
    u32 count    = 0;
    u8  color[3] = { 0, 0, 0 };
    u8  header   = 0;
    u8* ptr      = pPixels;

    while( ptr < pPixels + size )   // size = width * height * 3.
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            fread( color, sizeof(u8), 3, pFile );

            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                ptr[ 0 ] = color[ 2 ];
                ptr[ 1 ] = color[ 1 ];
                ptr[ 2 ] = color[ 0 ];
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=3 )
            {
                ptr[ 2 ] = (u8)fgetc( pFile );
                ptr[ 1 ] = (u8)fgetc( pFile );
                ptr[ 0 ] = (u8)fgetc( pFile );
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      32BitRLE���k�t���J���[�`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse32BitsRLE( FILE* pFile, u32 size, u8* pPixels )
{
    u32 count    = 0;
    u8  color[4] = { 0, 0, 0, 0 };
    u8  header   = 0;
    u8* ptr      = pPixels;

    while( ptr < pPixels + size )   // size = width * height * 4.
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            fread( color, sizeof(u8), 4, pFile );

            for( u32 i=0; i<count; ++i, ptr+=4 )
            {
                ptr[ 0 ] = color[ 2 ];
                ptr[ 1 ] = color[ 1 ];
                ptr[ 2 ] = color[ 0 ];
                ptr[ 3 ] = color[ 3 ];
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=4 )
            {
                ptr[ 2 ] = (u8)fgetc( pFile );
                ptr[ 1 ] = (u8)fgetc( pFile );
                ptr[ 0 ] = (u8)fgetc( pFile );
                ptr[ 3 ] = (u8)fgetc( pFile );
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      8BitRLE���k�O���[�X�P�[���`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse8BitsGrayScaleRLE( FILE* pFile, u32 size, u8* pPixles )
{
    u32 count  = 0;
    u8  color  = 0;
    u8  header = 0;
    u8* ptr    = pPixles;

    while( ptr < pPixles + size ) // size = width * height
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            color = (u8)fgetc( pFile );

            for( u32 i=0; i<count; ++i, ptr++ )
            { (*ptr) = color; }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr++ )
            { (*ptr) = (u8)fgetc( pFile ); }
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      16BitRLE���k�O���[�X�P�[���`������͂��܂�.
//-------------------------------------------------------------------------------------------------
void Parse16BitsGrayScaleRLE( FILE* pFile, u32 size, u8* pPixles )
{
    u32 count  = 0;
    u8  color  = 0;
    u8  alpha  = 0;
    u8  header = 0;
    u8* ptr    = pPixles;

    while( ptr < pPixles + size ) // size = width * height * 2
    {
        header = (u8)fgetc( pFile );
        count = 1 + ( header & 0x7F );

        if ( header & 0x80 )
        {
            color = (u8)fgetc( pFile );
            alpha = (u8)fgetc( pFile );

            for( u32 i=0; i<count; ++i, ptr+=2 )
            {
                ptr[ 0 ] = color;
                ptr[ 1 ] = alpha;
            }
        }
        else
        {
            for( u32 i=0; i<count; ++i, ptr+=2 )
            {
                ptr[ 0 ] = (u8)fgetc( pFile );
                ptr[ 1 ] = (u8)fgetc( pFile );
            }
        }
    }
}

} // namespace /* anonymous */


namespace s3d {

//-------------------------------------------------------------------------------------------------
//      TGA�t�@�C����ǂݍ��݂܂�.
//-------------------------------------------------------------------------------------------------
bool LoadFromTGA
(
    const char* filename,
    s32&        width,
    s32&        height,
    s32&        component,
    f32**       ppPixel
)
{
    if ( filename == nullptr || ppPixel == nullptr )
    { return false; }

    FILE* pFile;
    auto err = fopen_s( &pFile, filename, "rb ");
    if ( err != 0 )
    { return false; }

    TGA_FOOTER footer;
    long offset = sizeof(footer);
    fseek( pFile, -offset, SEEK_END );
    fread( &footer, sizeof(footer), 1, pFile );

    if ( strcmp( footer.Tag, "TRUEVISION-XFILE." ) != 0 &&
         strcmp( footer.Tag, "TRUEVISION-TARGA." ) != 0 )
    {
        fclose( pFile );
        return false;
    }

    // �g���f�[�^������ꍇ�͓ǂݍ���.
    if ( footer.OffsetExt != 0 )
    {
        TGA_EXTENSION extension;

        fseek( pFile, footer.OffsetExt, SEEK_SET );
        fread( &extension, sizeof(extension), 1, pFile );
    }

    // �f�B�x���b�p�[�G���A������ꍇ.
    if ( footer.OffsetDev != 0 )
    {
        /* NOT IMPLEMENT */
    }

    // �t�@�C���擪�ɖ߂�.
    fseek( pFile, 0, SEEK_SET );

    // �w�b�_�f�[�^��ǂݍ���.
    TGA_HEADER header;
    fread( &header, sizeof(header), 1, pFile );

    // �t�H�[�}�b�g����.
    u32 bytePerPixel;
    switch( header.Format )
    {
    // �Y���Ȃ�.
    case TGA_FORMAT_NONE:
        {
            fclose( pFile );
            return false;
        }
        break;

    // �O���[�X�P�[��
    case TGA_FORMAT_GRAYSCALE:
    case TGA_FORMAT_RLE_GRAYSCALE:
        { 
            if ( header.BitPerPixel == 8 )
            { bytePerPixel = 1; }
            else
            { bytePerPixel = 2; }
        }
        break;

    // �J���[.
    case TGA_FORMAT_INDEXCOLOR:
    case TGA_FORMAT_FULLCOLOR:
    case TGA_FORMAT_RLE_INDEXCOLOR:
    case TGA_FORMAT_RLE_FULLCOLOR:
        {
            if ( header.BitPerPixel <= 24 )
            { bytePerPixel = 3; }
            else
            { bytePerPixel = 4; }
        }
        break;

    // ��L�ȊO.
    default:
        {
            fclose( pFile );
            return false;
        }
        break;
    }

    // ID�t�B�[���h�T�C�Y�������I�t�Z�b�g���ړ�������.
    fseek( pFile, header.IdFieldLength, SEEK_CUR );

    // �s�N�Z���T�C�Y�����肵�ă��������m��.
    auto size = header.Width * header.Height * bytePerPixel;
    auto pPixels = new u8 [ size ];
    if ( ppPixel == nullptr )
    {
        fclose( pFile );
        return false;
    }

    // �J���[�}�b�v�������`�F�b�N.
    u8* pColorMap = nullptr;
    if ( header.HasColorMap )
    {
        // �J���[�}�b�v�T�C�Y���Z�o.
        u32 colorMapSize = header.ColorMapEntry * ( header.ColorMapEntrySize >> 3 );

        // ���������m��.
        pColorMap = new u8 [ colorMapSize ];
        if ( pColorMap == nullptr )
        {
            SafeDeleteArray( pPixels );
            fclose( pFile );
            return false;
        }

        // ���΂��Ɠǂݍ���.
        fread( pColorMap, sizeof(u8), colorMapSize, pFile );
    }

    // ���E�����E�r�b�g�̐[���E�n�b�V���L�[��ݒ�.
    width       = header.Width;
    height      = header.Height;

    // �t�H�[�}�b�g�ɍ��킹�ăs�N�Z���f�[�^����͂���.
    switch( header.Format )
    {
    // �p���b�g.
    case TGA_FORMAT_INDEXCOLOR:
        {
            component = 3;
            Parse8Bits( pFile, width * height, pColorMap, pPixels );
        }
        break;

    // �t���J���[.
    case TGA_FORMAT_FULLCOLOR:
        {
            switch( header.BitPerPixel )
            {
            case 16:
                {
                    component = 3;
                    Parse16Bits( pFile, width * height, pPixels );
                }
                break;

            case 24:
                {
                    component = 3;
                    Parse24Bits( pFile, width * height, pPixels );
                }
                break;

            case 32:
                {
                    component = 4;
                    Parse32Bits( pFile, width * height, pPixels );
                }
                break;
            }
        }
        break;

    // �O���[�X�P�[��.
    case TGA_FORMAT_GRAYSCALE:
        {
            if ( header.BitPerPixel == 8 )
            {
                component = 1;
                Parse8BitsGrayScale( pFile, width * height, pPixels );
            }
            else
            {
                component = 2;
                Parse16BitsGrayScale( pFile, width * height, pPixels );
            }
        }
        break;

    // �p���b�gRLE���k.
    case TGA_FORMAT_RLE_INDEXCOLOR:
        {
            component = 3;
            Parse8BitsRLE( pFile, pColorMap, width * height * 3, pPixels );
        }
        break;

    // �t���J���[RLE���k.
    case TGA_FORMAT_RLE_FULLCOLOR:
        {
            switch( header.BitPerPixel )
            {
            case 16:
                {
                    component = 3;
                    Parse16BitsRLE( pFile, width * height * 3, pPixels );
                }
                break;

            case 24:
                {
                    component = 3;
                    Parse24BitsRLE( pFile, width * height * 3, pPixels );
                }
                break;

            case 32:
                {
                    component = 4;
                    Parse32BitsRLE( pFile, width * height * 4, pPixels );
                }
                break;
            }
        }
        break;

    // �O���[�X�P�[��RLE���k.
    case TGA_FORMAT_RLE_GRAYSCALE:
        {
            if ( header.BitPerPixel == 8 )
            {
                component = 1;
                Parse8BitsGrayScaleRLE( pFile, width * height, pPixels );
            }
            else
            {
                component = 2;
                Parse16BitsGrayScaleRLE( pFile, width * height * 2, pPixels );
            }
        }
        break;
    }

    // �s�v�ȃ����������.
    SafeDeleteArray( pColorMap );

    // �t�@�C�������.
    fclose( pFile );

    // Float �ɕϊ�.
    auto pixelCount = width * height * component;
    (*ppPixel) = new f32 [ width * height *4 ];

    switch( component )
    {
        case 1:
            {
                for( auto i=0, j=0; i<pixelCount; i++, j+=4 )
                {
                    (*ppPixel)[j + 0] = static_cast<f32>( pPixels[i] ) / 255.0f;
                    (*ppPixel)[j + 1] = static_cast<f32>( pPixels[i] ) / 255.0f;
                    (*ppPixel)[j + 2] = static_cast<f32>( pPixels[i] ) / 255.0f;
                    (*ppPixel)[j + 3] = 1.0f;
                }
            }
            break;

        case 2:
            {
                for( auto i=0, j=0; i<pixelCount-2; i+=2, j+=4 )
                {
                    (*ppPixel)[j + 0] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 1] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 2] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 3] = static_cast<f32>( pPixels[i + 1] ) / 255.0f;;
                }

            }
            break;

        case 3:
            {
                for( auto i=0, j=0; i<pixelCount-3; i+3, j+=4 )
                {
                    (*ppPixel)[j + 0] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 1] = static_cast<f32>( pPixels[i + 1] ) / 255.0f;
                    (*ppPixel)[j + 2] = static_cast<f32>( pPixels[i + 2] ) / 255.0f;
                    (*ppPixel)[j + 3] = 1.0f;
                }
            }
            break;

        case 4:
            {
                for( auto i=0, j=0; i<pixelCount-4; i+=4, j+=4 )
                {
                    (*ppPixel)[j + 0] = static_cast<f32>( pPixels[i + 0] ) / 255.0f;
                    (*ppPixel)[j + 1] = static_cast<f32>( pPixels[i + 1] ) / 255.0f;
                    (*ppPixel)[j + 2] = static_cast<f32>( pPixels[i + 2] ) / 255.0f;
                    (*ppPixel)[j + 3] = static_cast<f32>( pPixels[i + 3] ) / 255.0f;
                }
            }
            break;
    }

    // �s�v�ȃ����������.
    SafeDeleteArray( pPixels );

    // ����I��.
    return true;
}

} // namespace s3d
