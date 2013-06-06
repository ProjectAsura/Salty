//---------------------------------------------------------------------------------------------
// File : asdxFont.cpp
// Desc : Font Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------------
#include <asdxFont.h>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <asdxLog.h>
#include <asdxUtil.h>
#include <vector>


namespace /* anonymous */ {

////////////////////////////////////////////////////////////////////////////////////////////////
// FntFormat enum
////////////////////////////////////////////////////////////////////////////////////////////////
enum FntFormat
{
    FNT_FORMAT_R8G8B8A8 = 0,        //!< RGBA(8,8,8,8)�t�H�[�}�b�g�ł�.
    FNT_FORMAT_BC2                  //!< BC2_UNORM�t�H�[�}�b�g�ł�.
};

////////////////////////////////////////////////////////////////////////////////////////////////
// FntTextureHeader structure
////////////////////////////////////////////////////////////////////////////////////////////////
struct FntTextureHeader
{
    unsigned int    Width;          //!< �����ł�.
    unsigned int    Height;         //!< �c���ł�.
    unsigned int    Format;         //!< �t�H�[�}�b�g�ł�.
    unsigned int    Stride;         //!< �X�g���C�h�ł�(=1�s������̃f�[�^�̃o�C�g���ł�).
    unsigned int    Rows;           //!< �s���ł�.
};

////////////////////////////////////////////////////////////////////////////////////////////////
// FntDataHeader structure
////////////////////////////////////////////////////////////////////////////////////////////////
struct FntDataHeader
{
    char                FontName[ 32 ];     //!< �t�H���g���ł�.
    unsigned int        FontWidth;          //!< �t�H���g�̉����ł�.
    unsigned int        FontHeight;         //!< �t�H���g�̏c���ł�.
    FntTextureHeader    TextureHeader;      //!< �e�N�X�`���w�b�_�[�ł�.
};

////////////////////////////////////////////////////////////////////////////////////////////////
// FntFileHeader structure
////////////////////////////////////////////////////////////////////////////////////////////////
struct FntFileHeader
{
    unsigned char   Magic[ 4 ];         //!< �}�W�b�N�ł�.
    unsigned int    Version;            //!< �t�@�C���o�[�W�����ł�.
    unsigned int    DataHeaderSize;     //!< �f�[�^�w�b�_�\���̂̃T�C�Y�ł�.
    FntDataHeader   DataHeader;         //!< �f�[�^�w�b�_�ł�.
};

////////////////////////////////////////////////////////////////////////////////////////////////
// CbPerOnceVS structure
////////////////////////////////////////////////////////////////////////////////////////////////
struct CbPerOnceVS
{
    asdx::MATRIX    Transform;
};

//�@���݂̃t�H���g�t�@�C���o�[�W�����ł�.
static const unsigned int  FNT_CURRENT_FILE_VERSION         = 0x00000001;


} // namespace /* anonymous */ 


namespace asdx {

////////////////////////////////////////////////////////////////////////////////////////////////
// Font class
////////////////////////////////////////////////////////////////////////////////////////////////

// ���͗v�f�ł�.
const D3D11_INPUT_ELEMENT_DESC Font::InputElements[ InputElementCount ] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "VTXCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

//----------------------------------------------------------------------------------------------
//          �R���X�g���N�^�ł�.
//----------------------------------------------------------------------------------------------
Font::Font()
: m_pVS         ( nullptr )
, m_pPS         ( nullptr )
, m_pIL         ( nullptr )
, m_pVB         ( nullptr )
, m_pIB         ( nullptr )
, m_pCB         ( nullptr )
, m_pTexture    ( nullptr )
, m_pSRV        ( nullptr )
, m_pSmp        ( nullptr )
, m_pBS         ( nullptr )
, m_pDSS        ( nullptr )
, m_pRS         ( nullptr )
, m_SpriteCount ( 0 )
, m_FontWidth   ( 0 )
, m_FontHeight  ( 0 )
, m_OffsetU     ( 0.0f )
, m_OffsetV     ( 0.0f )
, m_ScreenSize  ( 0.0f, 0.0f )
, m_Color       ( 1.0f, 1.0f, 1.0f, 1.0f )
{
    m_FontName[0] = '\0';
    m_Transform.Identity();
}

//---------------------------------------------------------------------------------------------
//          �f�X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
Font::~Font()
{
    assert( m_pVS      == nullptr );
    assert( m_pPS      == nullptr );
    assert( m_pIL      == nullptr );
    assert( m_pVB      == nullptr );
    assert( m_pIB      == nullptr );
    assert( m_pCB      == nullptr );
    assert( m_pTexture == nullptr );
    assert( m_pSRV     == nullptr );
    assert( m_pSmp     == nullptr );
    assert( m_pBS      == nullptr );
    assert( m_pDSS     == nullptr );
}

//---------------------------------------------------------------------------------------------
//          �����������ł�.
//---------------------------------------------------------------------------------------------
bool Font::Init
(
    ID3D11Device*   pDevice,
    const char*     filename,
    float           screenWidth,
    float           screenHeight
)
{
    FILE* pFile;

    // �t�@�C�����J���܂�.
    errno_t err = fopen_s( &pFile, filename, "rb" );

    // �`�F�b�N.
    if ( err != 0 )
    {
        // �G���[���O�o��.
        ELOG( "Error : File Open Failed. filename = %s\n", filename );

        // �ُ�I��.
        return false;
    }

    // �t�@�C���w�b�_�ǂݍ���.
    FntFileHeader fileHeader;
    fread( &fileHeader, sizeof(FntFileHeader), 1, pFile );

    // �t�@�C���}�W�b�N���`�F�b�N.
    if ( ( fileHeader.Magic[0] != 'F' )
      || ( fileHeader.Magic[1] != 'N' )
      || ( fileHeader.Magic[2] != 'T' )
      || ( fileHeader.Magic[3] != '\0' ) )
    {
        // �G���[���O�o��.
        ELOG( "Error : Invalid File. filename = %s\n", filename );

        // �t�@�C�������.
        fclose( pFile );

        // �ُ�I��.
        return false;
    }

    // �t�@�C���o�[�W�����̃`�F�b�N.
    if ( fileHeader.Version != FNT_CURRENT_FILE_VERSION )
    {
        // �G���[���O�o��.
        ELOG( "Error : Invalid File Version. version = 0x%x, current_version = 0x%x\n", fileHeader.Version, FNT_CURRENT_FILE_VERSION );

        // �t�@�C�������.
        fclose( pFile );

        // �ُ�I��.
        return false;
    }

    // �f�[�^�w�b�_�\���̃T�C�Y���`�F�b�N.
    if ( fileHeader.DataHeaderSize != sizeof( FntDataHeader ) )
    {
        // �G���[���O�o��.
        ELOG( "Error : Invalid Data Header Size. " );

        // �t�@�C�������.
        fclose( pFile );

        // �ُ�I��.
        return false;
    }

    // �t�H���g�T�C�Y��ݒ�.
    m_FontWidth  = fileHeader.DataHeader.FontWidth;
    m_FontHeight = fileHeader.DataHeader.FontHeight;

    // �t�H���g����ݒ�.
    strcpy_s( m_FontName, fileHeader.DataHeader.FontName );

    // �e�N�X�`�����W�I�t�Z�b�g���Z�o.
    m_OffsetU = static_cast<float>( m_FontWidth )  / static_cast<float>( fileHeader.DataHeader.TextureHeader.Width );
    m_OffsetV = static_cast<float>( m_FontHeight ) / static_cast<float>( fileHeader.DataHeader.TextureHeader.Height );


    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

    // �t�H�[�}�b�g����.
    switch( fileHeader.DataHeader.TextureHeader.Format )
    {
    case FNT_FORMAT_R8G8B8A8:
        format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;

    case FNT_FORMAT_BC2:
        format = DXGI_FORMAT_BC2_UNORM;
        break;

    default:
        assert( false );
        break;
    }


    HRESULT hr = S_OK;
    {
        // �e�N�X�`���T�C�Y�Z�o.
        size_t textureSize = fileHeader.DataHeader.TextureHeader.Stride * fileHeader.DataHeader.TextureHeader.Rows;

        // �e�N�Z���̃��������m��.
        unsigned char* pPixels = new unsigned char [ textureSize ];

        // NULL�`�F�b�N.
        if ( pPixels == nullptr )
        {
            // �G���[���O�o��.
            ELOG( "Error : Memory Allocate Failed." );

            // �t�@�C�������.
            fclose( pFile );

            // �ُ�I��.
            return false;
        }

        // �e�N�Z���f�[�^����C�ɓǂݍ���.
        fread( pPixels, sizeof( unsigned char ), textureSize, pFile );

        // �T�u���\�[�X�f�[�^��ݒ�.
        D3D11_SUBRESOURCE_DATA subRes;
        subRes.pSysMem          = pPixels;
        subRes.SysMemPitch      = fileHeader.DataHeader.TextureHeader.Stride;
        subRes.SysMemSlicePitch = textureSize;

        // 2D�e�N�X�`���̐ݒ�.
        D3D11_TEXTURE2D_DESC desc;
        desc.Width              = fileHeader.DataHeader.TextureHeader.Width;
        desc.Height             = fileHeader.DataHeader.TextureHeader.Height;
        desc.MipLevels          = 1;
        desc.ArraySize          = 1;
        desc.Format             = format;
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage              = D3D11_USAGE_DEFAULT;
        desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags     = 0;
        desc.MiscFlags          = 0;

        // �e�N�X�`������.
        ID3D11Texture2D* pTexture = nullptr;
        hr = pDevice->CreateTexture2D( &desc, &subRes, &pTexture );

        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateTexture2D() Failed." );

            fclose( pFile );

            delete [] pPixels;
            pPixels = nullptr;

            return false;
        }

        // �e�N�X�`����ݒ�.
        m_pTexture = pTexture;

        // �f�o�b�O�I�u�W�F�N�g����ݒ�.
        SetDebugObjectName( m_pTexture, "asdx::Font" );

        {
            // �V�F�[�_���\�[�X�r���[�̐ݒ�.
            D3D11_SHADER_RESOURCE_VIEW_DESC sd;
            ZeroMemory( &sd, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );
            sd.Format              = format;
            sd.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
            sd.Texture2D.MipLevels = desc.MipLevels;

            // �V�F�[�_���\�[�X�r���[����.
            ID3D11ShaderResourceView* pSRV;
            hr = pDevice->CreateShaderResourceView( pTexture, &sd, &pSRV );

            // �G���[�`�F�b�N.
            if ( FAILED( hr ) )
            {
                // �e�N�X�`�����.
                pTexture->Release();

                delete [] pPixels;
                pPixels = nullptr;

                // �G���[���O�o��.
                ELOG( "Error : ID3D11Device::CreateShaderResourceView() Failed." );

                // �ُ�I��.
                return false;
            }

            // �V�F�[�_���\�[�X�r���[��ݒ�.
            m_pSRV = pSRV;

            // �f�o�b�O�I�u�W�F�N�g���ݒ�.
            SetDebugObjectName( m_pSRV, "asdx::Font" );
        }

        delete [] pPixels;
        pPixels = nullptr;

    }


    // �t�@�C�������.
    fclose( pFile );

    // ���_�o�b�t�@�̐���.
    {
        // ���_�o�b�t�@�̐ݒ�.
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        desc.Usage          = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth      = sizeof( Font::Vertex ) * NUM_SPRITES * NUM_VERTEX_PER_SPRITE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // ���_�o�b�t�@�̐���.
        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pVB );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
            return false;
        }
        SetDebugObjectName( m_pVB, "asdx::Font" );
    }

    // �C���f�b�N�X�o�b�t�@�̐���.
    {
        // �C���f�b�N�X�o�b�t�@�̐ݒ�.
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof( short ) * NUM_SPRITES * NUM_INDEX_PER_SPRITE;

        // �C���f�b�N�X�o�b�t�@�̃��������m��.
        std::vector< short > indices;
        indices.reserve( NUM_SPRITES * NUM_INDEX_PER_SPRITE );

        // �C���f�b�N�X�ݒ�.
        for( short i=0; i<NUM_SPRITES * NUM_INDEX_PER_SPRITE; i+=NUM_VERTEX_PER_SPRITE )
        {
            indices.push_back( i + 0 );
            indices.push_back( i + 1 );
            indices.push_back( i + 2 );

            indices.push_back( i + 1 );
            indices.push_back( i + 3 );
            indices.push_back( i + 2 );
        }

        // �T�u���\�[�X�f�[�^�̐ݒ�.
        D3D11_SUBRESOURCE_DATA res;
        ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
        res.pSysMem = &indices.front();

        // �C���f�b�N�X�o�b�t�@����.
        hr = pDevice->CreateBuffer( &desc, &res, &m_pIB );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // �C���f�b�N�X���N���A.
            indices.clear();

            // �ُ�I��.
            return false;
        }
        // �f�o�b�O�I�u�W�F�N�g��.
        SetDebugObjectName( m_pIB, "asdx::Font" );

        // �C���f�b�N�X���N���A.
        indices.clear();
    }

    // �V�F�[�_�̐ݒ�.
    {
        #include "../res/shader/Compiled/SpriteShader_VSFunc.inc"
        #include "../res/shader/Compiled/SpriteShader_PSFunc.inc"

        // ���_�V�F�[�_����.
        hr = pDevice->CreateVertexShader( SpriteShader_VSFunc, sizeof( SpriteShader_VSFunc ), nullptr, &m_pVS );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );

            // �ُ�I��.
            return false;
        }
        // �f�o�b�O�I�u�W�F�N�g����ݒ�.
        SetDebugObjectName( m_pVS, "asdx::Font" );

        // �s�N�Z���V�F�[�_����.
        hr = pDevice->CreatePixelShader( SpriteShader_PSFunc, sizeof( SpriteShader_PSFunc ), nullptr, &m_pPS );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreatePixelShader() Failed." );

            // �ُ�I��.
            return false;
        }
        // �f�o�b�O�I�u�W�F�N�g����ݒ�.
        SetDebugObjectName( m_pPS, "asdx::Font" );

        // ���̓��C�A�E�g�𐶐�.
        hr = pDevice->CreateInputLayout( Font::InputElements, Font::InputElementCount, SpriteShader_VSFunc, sizeof( SpriteShader_VSFunc ), &m_pIL );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateInputLayout() Failed. ");

            // �ُ�I��.
            return false;
        }
        // �f�o�b�O�I�u�W�F�N�g����ݒ�.
        SetDebugObjectName( m_pIL, "asdx::Font" );
    }

    // �萔�o�b�t�@�̐���.
    {
        // �萔�o�b�t�@�̐ݒ�.
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof( CbPerOnceVS );
        desc.Usage     = D3D11_USAGE_DEFAULT;

        // �萔�o�b�t�@����.
        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pCB );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // �ُ�I��.
            return false;
        }
        // �f�o�b�O�I�u�W�F�N�g����ݒ�.
        SetDebugObjectName( m_pCB, "asdx::Font" );
    }

    // �u�����h�X�e�[�g�̐ݒ�
    {
        // �A���t�@�u�����h : src = ONE, dest = INV_SRC_ALPHA
        D3D11_BLEND_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BLEND_DESC ) );
        desc.RenderTarget[0].BlendEnable    = TRUE;        
        // Color
        desc.RenderTarget[0].SrcBlend       = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
        // Alpha
        desc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
        // Mask        
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        // �u�����h�X�e�[�g�𐶐�.
        hr = pDevice->CreateBlendState( &desc, &m_pBS );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateBlendState() Failed." );

            // �ُ�I��.
            return false;
        }
        // �f�o�b�O�I�u�W�F�N�g����ݒ�.
        SetDebugObjectName( m_pBS, "asdx::Font" );

    }

    // �T���v���[�X�e�[�g�̐���.
    {
        // �T���v���[�X�e�[�g�̐ݒ�.
        D3D11_SAMPLER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_SAMPLER_DESC ) );
        desc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MaxAnisotropy  = ( pDevice->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ) ? 16 : 2;
        desc.MaxLOD         = FLT_MAX;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        // �T���v���[�X�e�[�g�𐶐�.
        hr = pDevice->CreateSamplerState( &desc, &m_pSmp );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateSamplerState() Failed." );

            // �ُ�I��.
            return false;
        }
        // �f�o�b�O�I�u�W�F�N�g����ݒ�.
        SetDebugObjectName( m_pSmp, "asdx::Font" );
    }

    // �[�x�X�e���V���X�e�[�g�̐���.
    {
        // �[�x�X�e���V���X�e�[�g�̐ݒ�.
        D3D11_DEPTH_STENCIL_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
        desc.DepthEnable    = FALSE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
        desc.StencilEnable  = FALSE;

        // �[�x�X�e���V���X�e�[�g�̐���.
        hr = pDevice->CreateDepthStencilState( &desc, &m_pDSS );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateDepthStencilState() Failed." );

            // �ُ�I��.
            return false;
        }
        // �f�o�b�O�I�u�W�F�N�g����ݒ�.
        SetDebugObjectName( m_pDSS, "asdx::Font" );
    }

    // ���X�^���C�U�[�X�e�[�g�̐���.
    {
        // ���X�^���C�U�[�X�e�[�g�̐ݒ�.
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_RASTERIZER_DESC ) );
        desc.CullMode          = D3D11_CULL_BACK;
        desc.FillMode          = D3D11_FILL_SOLID;
        desc.DepthClipEnable   = true;
        desc.MultisampleEnable = true;

        // ���X�^���C�U�[�X�e�[�g�̐���.
        hr = pDevice->CreateRasterizerState( &desc, &m_pRS );
        if ( FAILED( hr ) )
        {
            // �G���[���O�o��.
            ELOG( "Error : ID3D11Device::CreateRasterizerState() Failed." );

            // �ُ�I��.
            return false;
        }
        // �f�o�b�O�I�u�W�F�N�g����ݒ�.
        SetDebugObjectName( m_pRS, "asdx::Font" );
    }

    // �X�N���[���T�C�Y��ݒ�.
    SetScreenSize( screenWidth, screenHeight );

    // ����I��.
    return true;
};

//---------------------------------------------------------------------------------------------
//          �I�������ł�.
//---------------------------------------------------------------------------------------------
void Font::Term()
{
    // ���_�V�F�[�_�����.
    if ( m_pVS )
    {
        m_pVS->Release();
        m_pVS = nullptr;
    }

    // �s�N�Z���V�F�[�_�����.
    if ( m_pPS )
    {
        m_pPS->Release();
        m_pPS = nullptr;
    }

    // ���̓��C�A�E�g�����.
    if ( m_pIL )
    {
        m_pIL->Release();
        m_pIL = nullptr;
    }

    // ���_�o�b�t�@�����.
    if ( m_pVB )
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    // �C���f�b�N�X�o�b�t�@�����.
    if ( m_pIB )
    {
        m_pIB->Release();
        m_pIB = nullptr;
    }

    // �萔�o�b�t�@�����.
    if ( m_pCB )
    {
        m_pCB->Release();
        m_pCB = nullptr;
    }

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

    // �T���v���[�X�e�[�g�����.
    if ( m_pSmp )
    {
        m_pSmp->Release();
        m_pSmp = nullptr;
    }

    // �u�����h�X�e�[�g�����.
    if ( m_pBS )
    {
        m_pBS->Release();
        m_pBS = nullptr;
    }

    // �[�x�X�e���V���X�e�[�g�����.
    if ( m_pDSS )
    {
        m_pDSS->Release();
        m_pDSS = nullptr;
    }

    // ���X�^���C�U�[�X�e�[�g�����.
    if ( m_pRS )
    {
        m_pRS->Release();
        m_pRS = nullptr;
    }

    memset( &m_FontName, 0, sizeof( char ) * 32 );
    m_FontName[0]   = '\0';
    m_FontWidth     = 0;
    m_FontHeight    = 0;
    m_SpriteCount   = 0;
    m_OffsetU       = 0.0f;
    m_OffsetV       = 0.0f;
    m_ScreenSize.x  = 0.0f;
    m_ScreenSize.y  = 0.0f;
    m_Color         = asdx::FLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
}

//---------------------------------------------------------------------------------------------
//          �X�N���[���T�C�Y��ݒ�.
//---------------------------------------------------------------------------------------------
void Font::SetScreenSize( float width, float height )
{
    SetScreenSize( asdx::FLOAT2( width, height ) );
}

//---------------------------------------------------------------------------------------------
//          �X�N���[���T�C�Y��ݒ�.
//---------------------------------------------------------------------------------------------
void Font::SetScreenSize( const asdx::FLOAT2& size )
{
    m_ScreenSize = size;

    float xScale = ( m_ScreenSize.x > 0.0f ) ? 2.0f / m_ScreenSize.x : 0.0f;
    float yScale = ( m_ScreenSize.y > 0.0f ) ? 2.0f / m_ScreenSize.y : 0.0f;

    m_Transform._11 = xScale;
    m_Transform._12 = 0.0f;
    m_Transform._13 = 0.0f;
    m_Transform._14 = 0.0f;

    m_Transform._21 = 0.0f;
    m_Transform._22 = -yScale;
    m_Transform._23 = 0.0f;
    m_Transform._24 = 0.0f;

    m_Transform._31 = 0.0f;
    m_Transform._32 = 0.0f;
    m_Transform._33 = 1.0f;
    m_Transform._34 = 0.0f;

    m_Transform._41 = -1.0f;
    m_Transform._42 =  1.0f;
    m_Transform._43 =  0.0f;
    m_Transform._44 =  1.0f;
}

//---------------------------------------------------------------------------------------------
//          �e�L�X�g�J���[��ݒ�.
//---------------------------------------------------------------------------------------------
void Font::SetColor( float r, float g, float b, float a )
{
    m_Color.x = r;
    m_Color.y = g;
    m_Color.z = b;
    m_Color.w = a;
}

//---------------------------------------------------------------------------------------------
//          �e�L�X�g�J���[��ݒ�.
//---------------------------------------------------------------------------------------------
void Font::SetColor( const asdx::FLOAT4& color )
{
    m_Color = color;
}

//---------------------------------------------------------------------------------------------
//          �`��J�n�����ł�.
//---------------------------------------------------------------------------------------------
void Font::Begin( ID3D11DeviceContext* pDeviceContext )
{
    // �X�v���C�g�������Z�b�g.
    m_SpriteCount = 0;

    // �u�����h�X�e�[�g��ݒ肵�܂�.
    pDeviceContext->OMSetBlendState( m_pBS, nullptr, 0xFFFFFFFF );

    // �[�x�X�e���V���X�e�[�g��ݒ肵�܂�.
    pDeviceContext->OMSetDepthStencilState( m_pDSS, 0 );

    // ���X�^���C�U�[�X�e�[�g��ݒ肵�܂�.
    pDeviceContext->RSSetState( m_pRS );

    // �T���v���[�X�e�[�g��ݒ肵�܂�.
    pDeviceContext->PSSetSamplers( 0, 1, &m_pSmp );

    // �v���~�e�B�u�g�|���W�[��ݒ肵�܂�.
    pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // ���̓��C�A�E�g��ݒ肵�܂�.
    pDeviceContext->IASetInputLayout( m_pIL );

    // �V�F�[�_��ݒ肵�܂�.
    pDeviceContext->VSSetShader( m_pVS, nullptr, 0 );
    pDeviceContext->PSSetShader( m_pPS, nullptr, 0 );
    pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->DSSetShader( nullptr, nullptr, 0 );

    unsigned int stride = sizeof( Font::Vertex );
    unsigned int offset = 0;

    // ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@��ݒ肵�܂�.
    pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, &stride, &offset );
    pDeviceContext->IASetIndexBuffer( m_pIB, DXGI_FORMAT_R16_UINT, 0 );

    // �萔�o�b�t�@���X�V���C�ݒ肵�܂�.
    pDeviceContext->UpdateSubresource( m_pCB, 0, nullptr, &m_Transform, 0, 0 );
    pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pCB );

    // �V�F�[�_���\�[�X�r���[��ݒ肵�܂�.
    pDeviceContext->PSSetShaderResources( 0, 1, &m_pSRV );
}

//---------------------------------------------------------------------------------------------
//          �������`�悵�܂�.
//---------------------------------------------------------------------------------------------
void Font::DrawString( const int x, const int y, const char* text )
{
    DrawString( x, y, 0, text );
}

//---------------------------------------------------------------------------------------------
//          �������`�悵�܂�.
//---------------------------------------------------------------------------------------------
void Font::DrawString( const int x, const int y, const int layerDepth, const char* text )
{
    unsigned int stringsCount = (unsigned int)strlen( text );

    // �����񂪂Ȃ���Ή������Ȃ�.
    if ( stringsCount == 0 )
    { return; }

    // �ő�X�v���C�g���𒴂��Ȃ����`�F�b�N.
    if ( m_SpriteCount + stringsCount > NUM_SPRITES )
    { return; }

    // �J�n�ʒu��ݒ�.
    float posX = static_cast<float>( x );
    float posY = static_cast<float>( y );

    // 1����������̃T�C�Y�擾.
    float w = static_cast<float>( m_FontWidth );
    float h = static_cast<float>( m_FontHeight );
    float d = static_cast<float>( layerDepth );

    // ���_�f�[�^�̃|�C���^�擾.
    Font::Vertex* pVertices = &m_Vertices[ m_SpriteCount * NUM_VERTEX_PER_SPRITE ];

    // �����������[�v.
    for( unsigned int i=0; i<stringsCount; i++ )
    {
        if ( (unsigned int)text[i] == 0x0a )
        {
            posY += w;
            posX  = static_cast< float >( x );
        }
        // �C���f�b�N�X�Z�o.
        unsigned int idx = (unsigned int)text[i] - 0x20;

        if ( idx > 95 )
        { continue; }

        // �e�N�X�`�����W.
        float u0 = ( idx + 0 ) * m_OffsetU;
        float u1 = ( idx + 1 ) * m_OffsetU;
        float v0 = 0.0f;
        float v1 = m_OffsetV;
        
        // �ʒu���W.
        float x0 = posX;
        float x1 = posX + w;
        float y0 = posY;
        float y1 = posY + h;

        // ���_�C���f�b�N�X���Z�o.
        idx = ( i * NUM_VERTEX_PER_SPRITE );

        // Vertex : 0
        pVertices[ idx + 0 ].Position.x = x0;
        pVertices[ idx + 0 ].Position.y = y0;
        pVertices[ idx + 0 ].Position.z = d;
        pVertices[ idx + 0 ].Color      = m_Color;
        pVertices[ idx + 0 ].TexCoord.x = u0;
        pVertices[ idx + 0 ].TexCoord.y = v1;

        // Vertex : 1
        pVertices[ idx + 1 ].Position.x = x1;
        pVertices[ idx + 1 ].Position.y = y0;
        pVertices[ idx + 1 ].Position.z = d;
        pVertices[ idx + 1 ].Color      = m_Color;
        pVertices[ idx + 1 ].TexCoord.x = u1;
        pVertices[ idx + 1 ].TexCoord.y = v1;

        // Vertex : 2
        pVertices[ idx + 2 ].Position.x = x0;
        pVertices[ idx + 2 ].Position.y = y1;
        pVertices[ idx + 2 ].Position.z = d;
        pVertices[ idx + 2 ].Color      = m_Color;
        pVertices[ idx + 2 ].TexCoord.x = u0;
        pVertices[ idx + 2 ].TexCoord.y = v0;

        // Vertex : 3
        pVertices[ idx + 3 ].Position.x = x1;
        pVertices[ idx + 3 ].Position.y = y1;
        pVertices[ idx + 3 ].Position.z = d;
        pVertices[ idx + 3 ].Color      = m_Color;
        pVertices[ idx + 3 ].TexCoord.x = u1;
        pVertices[ idx + 3 ].TexCoord.y = v0;

        // �������Ɉړ�.
        posX += w;
    }

    // �X�v���C�g�����J�E���g�A�b�v���܂�.
    m_SpriteCount += stringsCount;
}

//---------------------------------------------------------------------------------------------
//      �����w�肵�ĕ������`�悵�܂�.
//---------------------------------------------------------------------------------------------
void Font::DrawStringArg( const int x, const int y, const char* format, ... )
{
    int result = 0;
    va_list arg;

    va_start( arg, format );
    result = vsprintf_s( m_Buffer, format, arg );
    va_end( arg );

    DrawString( x, y, 0, m_Buffer );
}

//---------------------------------------------------------------------------------------------
//      �����w�肵�ĕ������`�悵�܂�.
//---------------------------------------------------------------------------------------------
void Font::DrawStringArg( const int x, const int y, const int layerDepth, const char* format, ... )
{
    int result = 0;
    va_list arg;

    va_start( arg, format );
    result = vsprintf_s( m_Buffer, format, arg );
    va_end( arg );

    DrawString( x, y, layerDepth, m_Buffer );
}

//---------------------------------------------------------------------------------------------
//      �`��I���ł�.
//---------------------------------------------------------------------------------------------
void Font::End( ID3D11DeviceContext* pDeviceContext )
{
    D3D11_MAP mapType = ( m_SpriteCount == 0 ) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
    D3D11_MAPPED_SUBRESOURCE mappedBuffer;

    // �}�b�v���܂�.
    HRESULT hr = pDeviceContext->Map( m_pVB, 0, mapType, 0, &mappedBuffer );
    if ( FAILED( hr ) )
    { return; }

    // ���_�f�[�^�̃|�C���^�擾.
    Font::Vertex* pVertices = (Font::Vertex*)mappedBuffer.pData;

    // ���΂��ƃR�s��.
    memcpy( pVertices, m_Vertices, sizeof( Vertex ) * m_SpriteCount * NUM_VERTEX_PER_SPRITE );

    // �A���}�b�v���܂�.
    pDeviceContext->Unmap( m_pVB, 0 );

    // �C���f�b�N�X��.
    unsigned int indexCount = m_SpriteCount * NUM_INDEX_PER_SPRITE;

    // �X�v���C�g��`��.
    pDeviceContext->DrawIndexed( indexCount, 0, 0 );

    // �V�F�[�_���A���o�C�h.
    pDeviceContext->VSSetShader( nullptr, 0, 0 );
    pDeviceContext->PSSetShader( nullptr, 0, 0 );

    // �X�e�[�g��null�ɐݒ�.
    pDeviceContext->OMSetBlendState( nullptr, nullptr, 0xFFFFFFFF );
    pDeviceContext->OMSetDepthStencilState( nullptr, 0 );
    pDeviceContext->RSSetState( nullptr );
}


} // namespace asdx
