//-------------------------------------------------------------------------------------------
// File : asdxTexture.cpp
// Desc : Texture Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <asdxTexture.h>
#include <asdxUtil.h>
#include <cstdio>
#include <cassert>



namespace asdx {

//-------------------------------------------------------------------------------------------
// Constant Values
//-------------------------------------------------------------------------------------------
static const unsigned int  MAP_CURRENT_VERSION = 0x00000001;

/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_DATA_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_DATA_HEADER
{
    unsigned int    Width;          //!< �摜�̉����ł�.
    unsigned int    Height;         //!< �摜�̏c���ł�.
    unsigned int    Format;         //!< �t�H�[�}�b�g�ł�.
    unsigned int    MipMapCount;    //!< �~�b�v�}�b�v���ł�.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_RESOURCE_DATA structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_RESOURCE_HEADER
{
    unsigned int    Width;          //!< �����ł�.
    unsigned int    Height;         //!< �c���ł�.
    unsigned int    Pitch;          //!< 1�s������̃o�C�g���ł�.
    unsigned int    SlicePitch;     //!< �e�N�Z���f�[�^�̃o�C�g���ł�.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_RESOURCE_DATA structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_RESOURCE_DATA
{
    MAP_RESOURCE_HEADER header;     //!< ���\�[�X�w�b�_�ł�.
    unsigned char*      pPixels;    //!< �s�N�Z���f�[�^�ł�.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MAP_FILE_HEADER structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct MAP_FILE_HEADER
{
    unsigned char   Magic[ 4 ];     //!< �}�W�b�N�ł�( 'M', 'A', 'P', '\0' )
    unsigned int    Version;        //!< �t�@�C���o�[�W�����ł�.
    unsigned int    DataHeaderSize; //!< �f�[�^�w�b�_�\���̂̃T�C�Y�ł�.
    MAP_DATA_HEADER DataHeader;     //!< �f�[�^�w�b�_�ł�.
};


//-------------------------------------------------------------------------------------------
//! @brief      �e�N�X�`��2D�𐶐����܂�.
//!
//! @param [in]     pDevice         �f�o�C�X�ł�.
//! @param [in]     width           �e�N�X�`���̉����ł�.
//! @param [in]     height          �e�N�X�`���̏c���ł�.
//! @param [in]     mipCount        �~�b�v�}�b�v���ł�.
//! @param [in]     format          DXGI�t�H�[�}�b�g�ł�.
//! @param [in]     usage           
//! @param [in]     bindFlags       �o�C���h�t���O�ł�.
//! @param [in]     cpuAccessFlags  CPUA�A�N�Z�X�t���O�ł�.
//! @param [in]     miscFlags       
//! @param [in]     pInitData       �T�u���\�[�X�ł�.
//! @param [out]    ppTexture       �e�N�X�`���ł�.
//! @param [out]    ppSRV           �V�F�[�_���\�[�X�r���[�ł�.
//! @retval true    �����ɐ���.
//! @retval false   �����Ɏ��s.
//-------------------------------------------------------------------------------------------
bool CreateTexture2D
(
    ID3D11Device*               pDevice,
    UINT                        width,
    UINT                        height,
    UINT                        mipCount,
    DXGI_FORMAT                 format,
    D3D11_USAGE                 usage,
    UINT                        bindFlags,
    UINT                        cpuAccessFlags,
    UINT                        miscFlags,
    D3D11_SUBRESOURCE_DATA*     pInitData,
    ID3D11Texture2D**           ppTexture,
    ID3D11ShaderResourceView**  ppSRV
)
{
    // �e�N�X�`��2D�̐ݒ�
    D3D11_TEXTURE2D_DESC td;

    td.Width              = width;
    td.Height             = height;
    td.MipLevels          = mipCount;
    td.ArraySize          = 1;
    td.Format             = format;
    td.SampleDesc.Count   = 1;
    td.SampleDesc.Quality = 0;
    td.Usage              = usage;
    td.BindFlags          = bindFlags;
    td.CPUAccessFlags     = cpuAccessFlags;
    td.MiscFlags          = miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE;

    HRESULT hr = S_OK;

    // �e�N�X�`���𐶐�.
    ID3D11Texture2D* pTexture = nullptr;
    hr = pDevice->CreateTexture2D( &td, pInitData, &pTexture );

    // �`�F�b�N.
    if ( SUCCEEDED( hr ) && ( pTexture != nullptr ) )
    {
        // �V�F�[�_���\�[�X�r���[���w�肳��Ă���ꍇ.
        if ( ppSRV != nullptr )
        {
            // �V�F�[�_���\�[�X�r���[�̐ݒ�.
            D3D11_SHADER_RESOURCE_VIEW_DESC sd;
            memset( &sd, 0, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );

            sd.Format              = format;
            sd.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
            sd.Texture2D.MipLevels = td.MipLevels;

            // �V�F�[�_���\�[�X�r���[�𐶐�.
            hr = pDevice->CreateShaderResourceView( pTexture, &sd, ppSRV );

            // �`�F�b�N.
            if ( FAILED( hr ) )
            {
                // �e�N�X�`�������.
                pTexture->Release();

                // �ُ�I��.
                return false;
            }
        }

        // �e�N�X�`�����w�肳��Ă���ꍇ.
        if ( ppTexture != nullptr )
        {
            // �e�N�X�`����ݒ�.
            (*ppTexture) = pTexture;
        }
        else
        {
            // �f�o�b�O�I�u�W�F�N�g����ݒ�.
            SetDebugObjectName( pTexture, "asdxTexture" );

            // �e�N�X�`�������.
            pTexture->Release();
        }

        // ����I��.
        return true;
    }

    // �ُ�I��.
    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// Texture class
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//--------------------------------------------------------------------------------------------
Texture::Texture()
: m_Format  ( FORMAT_INVALID )
, m_pTexture( nullptr )
, m_pSRV    ( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//--------------------------------------------------------------------------------------------
Texture::~Texture()
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------------------
//      ��������������ł�.
//--------------------------------------------------------------------------------------------
void Texture::Release()
{
    // �e�N�X�`�������.
    if ( m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = nullptr;
    }

    // �V�F�[�_���\�[�X�r���[�����.
    if ( m_pSRV )
    {
        m_pSRV->Release();
        m_pSRV = nullptr;
    }

    m_Format = FORMAT_INVALID;
}

//--------------------------------------------------------------------------------------------
//      �t�@�C������e�N�X�`���𐶐����܂�.
//--------------------------------------------------------------------------------------------
bool Texture::CreateFromFile( ID3D11Device* pDevice, const char* filename )
{
    // NULL�`�F�b�N.
    if ( pDevice == nullptr )
    {
        // �G���[���O�o��.
        printf_s( "Error : Invalid Argument.\n" );

        // �ُ�I��.
        return false;
    }

    // NULL�`�F�b�N.
    if ( filename == nullptr )
    {
        // �G���[���O�o��.
        printf_s( "Error : Invalid Argument.\n" );

        // �ُ�I��.
        return false;
    }

    FILE* pFile;

    // �t�@�C�����J��.
    errno_t err = fopen_s( &pFile, filename, "rb" );

    // �t�@�C���I�[�v���`�F�b�N.
    if ( err != 0 )
    {
        // �G���[���O�o��.
        printf_s( "Error : CreateFromFile() Failed. filename = %s\n", filename );

        // �ُ�I��.
        return false;
    }

    // �t�@�C���w�b�_�ǂݍ���.
    MAP_FILE_HEADER header;
    fread( &header, sizeof( MAP_FILE_HEADER ), 1, pFile );

    // �}�W�b�N���`�F�b�N.
    if ( ( header.Magic[ 0 ] != 'M' )
      || ( header.Magic[ 1 ] != 'A' )
      || ( header.Magic[ 2 ] != 'P' )
      || ( header.Magic[ 3 ] != '\0' ) )
    {
        // �G���[���O�o��.
        printf_s( "Error : Invalid File.\n" );

        // �t�@�C�������.
        fclose( pFile );

        // �ُ�I��.
        return false;
    }

    // �o�[�W�����`�F�b�N.
    if ( header.Version != MAP_CURRENT_VERSION )
    {
        // �G���[���O�o��.
        printf_s( "Error : Invalid File Version.\n" );

        // �t�@�C�������.
        fclose( pFile );

        // �ُ�I��.
        return false;
    }

    // �f�[�^�w�b�_�\���̂̃T�C�Y���`�F�b�N.
    if ( header.DataHeaderSize != sizeof( MAP_DATA_HEADER ) )
    {
        // �G���[���O�o��.
        printf_s( "Error : Data Header Size Not Matched.\n" );

        // �t�@�C�������.
        fclose( pFile );

        // �ُ�I��.
        return false;
    }

    // ���\�[�X�f�[�^�̃��������m��.
    MAP_RESOURCE_DATA* pResources = new MAP_RESOURCE_DATA[ header.DataHeader.MipMapCount ];

    // NULL�`�F�b�N.
    if ( pResources == nullptr )
    {
        // �G���[���O�o��.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // �t�@�C�������.
        fclose( pFile );

        // �ُ�I��.
        return false;
    }

    // �~�b�v�}�b�v����ݒ�.
    const unsigned int mipCount = header.DataHeader.MipMapCount;

    // �e�~�b�v���x�����擾.
    for( unsigned int i=0; i<mipCount; ++i )
    {
        // ���\�[�X�f�[�^�w�b�_�̓ǂݍ���.
        fread( &pResources[ i ].header, sizeof( MAP_RESOURCE_HEADER ), 1, pFile );

        // �s�N�Z�����擾.
        size_t numPixels = static_cast< size_t >( pResources[ i ].header.SlicePitch );
        assert( numPixels != 0 );

        // �������m��.
        pResources[ i ].pPixels = new unsigned char [ numPixels ];

        // NULL�`�F�b�N.
        if ( pResources[ i ].pPixels == nullptr )
        {
            // �G���[���O�o��.
            printf_s( "Error : Memory Allocate Failed. \n" );

            // �t�@�C�������.
            fclose( pFile );

            // �s�N�Z���f�[�^�̃����������.
            for( unsigned int j=0; j<i; ++j )
            {
                delete [] pResources[ i ].pPixels;
                pResources[ i ].pPixels = nullptr;
            }

            // ���������.
            delete [] pResources;
            pResources = nullptr;

            // �ُ�I��.
            return false;
        }
        
        // �s�N�Z���f�[�^�ǂݎ��.
        fread( pResources[ i ].pPixels, sizeof( unsigned char ), numPixels, pFile );
    }

    // �t�@�C�������.
    fclose( pFile );

    // �t�H�[�}�b�g��ݒ�.
    m_Format = static_cast< int >( header.DataHeader.Format );

    // �t�H�[�}�b�g�𔻕�.
    DXGI_FORMAT format;
    switch( m_Format )
    {
    case FORMAT_A8:
        { format = DXGI_FORMAT_A8_UNORM; }
        break;

    case FORMAT_L8:
        { format = DXGI_FORMAT_R8_UNORM; }
        break;

    case FORMAT_R8G8B8A8:
        { format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; }
        break;

    case FORMAT_BC1:
        { format = DXGI_FORMAT_BC1_UNORM_SRGB; }
        break;

    case FORMAT_BC2:
        { format = DXGI_FORMAT_BC2_UNORM_SRGB; }
        break;

    case FORMAT_BC3:
        { format = DXGI_FORMAT_BC3_UNORM_SRGB; }
        break;
    }

    // ���������m��.
    D3D11_SUBRESOURCE_DATA* pSubRes = new D3D11_SUBRESOURCE_DATA[ mipCount ];

    // NULL�`�F�b�N.
    if ( pSubRes == nullptr )
    {
        // �G���[���O�o��.
        printf_s( "Error : Memory Allocate Failed. \n" );

        // �s�N�Z���f�[�^�̃��������.
        for( unsigned int i=0; i<mipCount; ++i )
        {
            delete [] pResources[ i ].pPixels;
            pResources[ i ].pPixels = nullptr;
        }

        // �����������.
        delete [] pResources;
        pResources = nullptr;

        // �ُ�I��.
        return false;
    }

    // �T�u���\�[�X��ݒ�.
    for( unsigned int i=0; i<mipCount; ++i )
    {
        pSubRes[ i ].pSysMem          = ( const void* )pResources[ i ].pPixels;
        pSubRes[ i ].SysMemPitch      = pResources[ i ].header.Pitch;
        pSubRes[ i ].SysMemSlicePitch = pResources[ i ].header.SlicePitch;
    }

    // �e�N�X�`���𐶐�.
    bool result = CreateTexture2D( 
                        pDevice, 
                        header.DataHeader.Width,
                        header.DataHeader.Height,
                        mipCount, 
                        format,
                        D3D11_USAGE_DEFAULT,
                        D3D11_BIND_SHADER_RESOURCE,
                        0,
                        0,
                        pSubRes,
                        &m_pTexture,
                        &m_pSRV );

    // �s�v�ɂȂ��������������
    {
        for( unsigned int i=0; i<mipCount; ++i )
        {
            delete [] pResources[ i ].pPixels;
            pResources[ i ].pPixels = nullptr;
        }

        delete [] pResources;
        pResources = nullptr;

        delete [] pSubRes;
        pSubRes = nullptr;
    }

    // �������ʂ�ԋp.
    return result;
}

//--------------------------------------------------------------------------------------------
//      �t�H�[�}�b�g���擾���܂�.
//--------------------------------------------------------------------------------------------
int Texture::GetFormat() const
{ return m_Format; }

//--------------------------------------------------------------------------------------------
//      �e�N�X�`�����擾���܂�.
//--------------------------------------------------------------------------------------------
ID3D11Texture2D* Texture::GetTexture()
{ return m_pTexture; }

//--------------------------------------------------------------------------------------------
//      �V�F�[�_���\�[�X�r���[���擾���܂�.
//--------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* Texture::GetSRV()
{ return m_pSRV; }


} // namespace asdx