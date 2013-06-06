//------------------------------------------------------------------------------------------
// File : dfw_RenderTarget2D.cpp
// Desc : RenderTarget2D Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <cassert>
#include <asdxRenderTarget.h>
#include <asdxUtil.h>


namespace /* anonymous */ {

//--------------------------------------------------------------------------------------------
//! @brief      �K�؂ȃt�H�[�}�b�g�ɕϊ����܂�.
//!
//! @param [in]     format              ���̓t�H�[�}�b�g.
//! @param [out]    textureFormat       �e�N�X�`���t�H�[�}�b�g.
//--------------------------------------------------------------------------------------------
void ConvertRenderTargetFormat
(
    const DXGI_FORMAT&  format,
    DXGI_FORMAT&        textureFormat
)
{
    textureFormat  = format;

    switch( format )
    {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        {
            textureFormat = DXGI_FORMAT_R32G32B32A32_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        {
            textureFormat = DXGI_FORMAT_R32G32B32_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
        {
            textureFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
        {
            textureFormat = DXGI_FORMAT_R32G32_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        {
            textureFormat = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
        }
        break;

    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
        {
            textureFormat = DXGI_FORMAT_R10G10B10A2_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
        {
            textureFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
        {
            textureFormat = DXGI_FORMAT_R16G16_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
        {
            textureFormat = DXGI_FORMAT_R32_TYPELESS;
        }
        break;

    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        {
            textureFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
        {
            textureFormat = DXGI_FORMAT_R8G8_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
        {
            textureFormat = DXGI_FORMAT_R16_TYPELESS;
        }
        break;

    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
        {
            textureFormat = DXGI_FORMAT_R8_TYPELESS;
        }
        break;

    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        {
            textureFormat = DXGI_FORMAT_BC1_TYPELESS;
        }
        break;

    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        {
            textureFormat = DXGI_FORMAT_BC2_TYPELESS;
        }
        break;

    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        {
            textureFormat = DXGI_FORMAT_BC3_TYPELESS;
        }
        break;

    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        {
            textureFormat = DXGI_FORMAT_BC4_TYPELESS;
        }
        break;

    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        {
            textureFormat = DXGI_FORMAT_BC5_TYPELESS;
        }
        break;

    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        {
            textureFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
        }
        break;

    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        {
            textureFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
        }
        break;

    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
        {
            textureFormat = DXGI_FORMAT_BC6H_TYPELESS;
        }
        break;

    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        {
            textureFormat = DXGI_FORMAT_BC7_TYPELESS;
        }
        break;
    }
}

} // namespace /* anoymous */ 

namespace asdx {

///////////////////////////////////////////////////////////////////////////////////////////////
// RenderTarget2D class
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
RenderTarget2D::RenderTarget2D()
: m_pTexture( nullptr )
, m_pSRV    ( nullptr )
, m_pRTV    ( nullptr )
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
RenderTarget2D::~RenderTarget2D()
{ Release(); }

//---------------------------------------------------------------------------------------------
//      �o�b�N�o�b�t�@����`��^�[�Q�b�g�𐶐����܂�.
//---------------------------------------------------------------------------------------------
bool RenderTarget2D::CreateFromBackBuffer( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain )
{
    HRESULT hr = S_OK;

    // �o�b�N�o�b�t�@���擾.
    hr = pSwapChain->GetBuffer( 0, IID_ID3D11Texture2D, (LPVOID*)&m_pTexture );
    if( FAILED( hr ) )
    {
        // �G���[���O���o��.
        OutputDebugStringA( "Error : IDXGISwapChain->GetBuffer() Failed. call from RenderTarget2D::CreateFromBackBuffer()" );

        // ���s.
        return false;
    }
    SetDebugObjectName( m_pTexture, "asdxRenderTarget" );

    // �����_�[�^�[�Q�b�g�r���[�𐶐�.
    hr = pDevice->CreateRenderTargetView( m_pTexture, nullptr, &m_pRTV );
    if ( FAILED( hr ) )
    {
        // �o�b�N�o�b�t�@�����.
        m_pTexture->Release();
        m_pTexture = nullptr;

        // �G���[���O���o��.
        OutputDebugStringA( "Error : ID3D11Device::CreateRenderTargetView() Failed. call from RenderTarget2D::CreateFromBackBuffer()" );

        // ���s.
        return false;
    }
    SetDebugObjectName( m_pRTV, "asdxRenderTarget" );

    // �V�F�[�_���\�[�X�r���[�𐶐�.
    hr = pDevice->CreateShaderResourceView( m_pTexture, nullptr, &m_pSRV );
    if ( FAILED( hr ) )
    {
        // �e�N�X�`�������.
        m_pTexture->Release();
        m_pTexture = nullptr;

        // �����_�[�^�[�Q�b�g�r���[�����.
        m_pRTV->Release();
        m_pRTV = nullptr;

        // �G���[���O���o��.
        OutputDebugStringA( "Error : ID3D11Device::CreateShaderResourcdeView() Failed. call from RenderTarget2D::CreateFromBackBuffer()" );

        // ���s.
        return false;
    }
    SetDebugObjectName( m_pSRV, "asdxRenderTarget" );

    // ����.
    return true;
}

//---------------------------------------------------------------------------------------------
//      �`��^�[�Q�b�g�𐶐����܂�.
//---------------------------------------------------------------------------------------------
bool RenderTarget2D::Create( ID3D11Device* pDevice, const RenderTarget2D::Description& desc )
{
    assert( desc.ArraySize >= 1 );
    HRESULT hr = S_OK;

    DXGI_FORMAT textureFormat;

    // �K�؂ȃt�H�[�}�b�g�ɕϊ�.
    ConvertRenderTargetFormat( desc.Format, textureFormat );

    // �e�N�X�`���̐ݒ�.
    D3D11_TEXTURE2D_DESC td;
    td.Width                = desc.Width;
    td.Height               = desc.Height;
    td.MipLevels            = desc.MipLevels;
    td.ArraySize            = desc.ArraySize;
    td.Format               = textureFormat;
    td.SampleDesc.Count     = desc.SampleDesc.Count;
    td.SampleDesc.Quality   = desc.SampleDesc.Quality;
    td.Usage                = D3D11_USAGE_DEFAULT;
    td.BindFlags            = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags       = desc.CPUAccessFlags;
    td.MiscFlags            = desc.MiscFlags;

    // �e�N�X�`���𐶐����܂�.
    hr = pDevice->CreateTexture2D( &td, nullptr, &m_pTexture );
    if ( FAILED( hr ) )
    {
        // �G���[���O�o��.
        OutputDebugStringA( "Error : ID3D11Device::CreateTexture2D() Failed. call from RenderTarget2D::Create()" );

        // ���s.
        return false;
    }
    SetDebugObjectName( m_pTexture, "asdxRenderTarget" );

    // �����_�[�^�[�Q�b�g�r���[�̐ݒ�.
    D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    ZeroMemory( &rtvd, sizeof( D3D11_RENDER_TARGET_VIEW_DESC ) );
    rtvd.Format = desc.Format;

    // �z��T�C�Y��1�̏ꍇ.
    if ( desc.ArraySize == 1 )
    {
        // �}���`�T���v�����O�����̏ꍇ.
        if ( desc.SampleDesc.Count == 0 )
        {
            rtvd.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtvd.Texture2D.MipSlice = 0;
        }
        // �}���`�T���v�����O�L��̏ꍇ.
        else
        {
            rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        }
    }
    // �z��T�C�Y��1���傫���ꍇ.
    else
    {
        // �}���`�T���v�����O�����̏ꍇ.
        if ( desc.SampleDesc.Count == 0 )
        {
            rtvd.ViewDimension                  = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvd.Texture2DArray.ArraySize       = desc.ArraySize;
            rtvd.Texture2DArray.FirstArraySlice = 0;
            rtvd.Texture2DArray.MipSlice        = 0;
        }
        // �}���`�T���v�����O�L��̏ꍇ.
        else
        {
            rtvd.ViewDimension                    = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
            rtvd.Texture2DMSArray.ArraySize       = desc.ArraySize;
            rtvd.Texture2DMSArray.FirstArraySlice = 0;
        }
    }

    // �����_�[�^�[�Q�b�g�r���[�𐶐�.
    hr = pDevice->CreateRenderTargetView( m_pTexture, &rtvd, &m_pRTV );
    if ( FAILED( hr ) )
    {
        // �e�N�X�`�������.
        m_pTexture->Release();
        m_pTexture = nullptr;

        // �G���[���O�o��.
        OutputDebugStringA( "Error : ID3D11Device::CreateRenderTargetView() Failed. call from RenderTarget2D::Create()" );

        // ���s.
        return false;
    }
    SetDebugObjectName( m_pRTV, "asdxRenderTarget" );

    // �V�F�[�_���\�[�X�r���[�̐ݒ�.
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    ZeroMemory( &srvd, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );
    srvd.Format = desc.Format;

    // �z��T�C�Y��1�̏ꍇ.
    if ( desc.ArraySize == 1 )
    {
        // �}���`�T���v�����O�����̏ꍇ.
        if ( desc.SampleDesc.Count == 0 )
        {
            srvd.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvd.Texture2D.MipLevels       = desc.MipLevels;
            srvd.Texture2D.MostDetailedMip = 0;
        }
        // �}���`�T���v�����O�L��̏ꍇ.
        else
        {
            srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
        }
    }
    // �z��T�C�Y��1���傫���ꍇ.
    else
    {
        // �}���`�T���v�����O�����̏ꍇ.
        if ( desc.SampleDesc.Count == 0 )
        {
            srvd.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvd.Texture2DArray.ArraySize       = desc.ArraySize;
            srvd.Texture2DArray.FirstArraySlice = 0;
            srvd.Texture2DArray.MipLevels       = desc.MipLevels;
            srvd.Texture2DArray.MostDetailedMip = 0;
        }
        // �}���`�T���v�����O�L��̏ꍇ.
        else
        {
            srvd.ViewDimension                    = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
            srvd.Texture2DMSArray.ArraySize       = desc.ArraySize;
            srvd.Texture2DMSArray.FirstArraySlice = 0;
        }
    }

    // �V�F�[�_���\�[�X�r���[�𐶐�.
    hr = pDevice->CreateShaderResourceView( m_pTexture, &srvd, &m_pSRV );
    if ( FAILED( hr ) )
    {
        // �e�N�X�`�������.
        m_pTexture->Release();
        m_pTexture = nullptr;

        // �����_�[�^�[�Q�b�g�r���[�����.
        m_pRTV->Release();
        m_pRTV = nullptr;

        // �G���[���O���o��.
        OutputDebugStringA( "Error : ID3D11Device::CreateShaderResourceView() Failed. call from RenderTarget2D::Create()" );

        // ���s.
        return false;
    }
    SetDebugObjectName( m_pSRV, "asdxRenderTarget" );

    // ����.
    return true;
}

//---------------------------------------------------------------------------------------------
//      ��������ł�.
//---------------------------------------------------------------------------------------------
void RenderTarget2D::Release()
{
    // �V�F�[�_���\�[�X�r���[�����.
    if ( m_pSRV )
    {
        m_pSRV->Release();
        m_pSRV = nullptr;
    }

    // �����_�[�^�[�Q�b�g�r���[�����.
    if ( m_pRTV )
    {
        m_pRTV->Release();
        m_pRTV = nullptr;
    }

    // �e�N�X�`�������.
    if ( m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = nullptr;
    }
}

//---------------------------------------------------------------------------------------------
//      �e�N�X�`�����擾���܂�.
//---------------------------------------------------------------------------------------------
ID3D11Texture2D* RenderTarget2D::GetTexture() const
{ return m_pTexture; }

//---------------------------------------------------------------------------------------------
//      �V�F�[�_���\�[�X�r���[���擾���܂�.
//---------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* RenderTarget2D::GetSRV() const
{ return m_pSRV; }

//---------------------------------------------------------------------------------------------
//      �����_�[�^�[�Q�b�g�r���[���擾���܂�.
//---------------------------------------------------------------------------------------------
ID3D11RenderTargetView* RenderTarget2D::GetRTV()
{ return m_pRTV; }

//---------------------------------------------------------------------------------------------
//      �����_�[�^�[�Q�b�g�r���[���擾���܂�.
//---------------------------------------------------------------------------------------------
ID3D11RenderTargetView* const RenderTarget2D::GetRTV() const
{ return m_pRTV; }

} // namespace asdx

