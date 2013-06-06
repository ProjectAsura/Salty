//------------------------------------------------------------------------------------------
// File : asdxDepthBuffer.cpp
// Desc : Depth Stencil Buffer Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <cassert>
#include <asdxDepthBuffer.h>
#include <asdxUtil.h>

namespace /* anonymous */ {

 //-----------------------------------------------------------------------------------------
 //! @brief     �K�؂ȃt�H�[�}�b�g�ɕϊ����܂�.
 //!
 //! @param [in]        format              ���̓t�H�[�}�b�g.
 //! @param [out]       textureFormat       �e�N�X�`���t�H�[�}�b�g.
 //! @param [out]       resourceFormat      ���\�[�X�t�H�[�}�b�g.
 //-----------------------------------------------------------------------------------------
void ConvertDepthFormat
(
    const DXGI_FORMAT& format,
    DXGI_FORMAT&       textureFormat,
    DXGI_FORMAT&       resourceFormat
)
{
    // ���̓t�H�[�}�b�g�ŏ�����.
    textureFormat  = format;
    resourceFormat = format;

    // �K�؂ȃt�H�[�}�b�g���擾.
    switch( format )
    {
    case DXGI_FORMAT_D16_UNORM:
        {
            textureFormat  = DXGI_FORMAT_R16_TYPELESS;
            resourceFormat = DXGI_FORMAT_R16_UNORM;
        }
        break;

    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        {
            textureFormat  = DXGI_FORMAT_R24G8_TYPELESS;
            resourceFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        }
        break;

    case DXGI_FORMAT_D32_FLOAT:
        {
            textureFormat  = DXGI_FORMAT_R32_TYPELESS;
            resourceFormat = DXGI_FORMAT_R32_FLOAT;
        }
        break;

    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        {
            textureFormat  = DXGI_FORMAT_R32G8X24_TYPELESS;
            resourceFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        }
        break;
    }
}


} // namespace /* anonymous */

namespace asdx {

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencilBuffer class
////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
DepthStencilBuffer::DepthStencilBuffer()
: m_pTexture( nullptr )
, m_pSRV    ( nullptr )
, m_pDSV    ( nullptr )
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
DepthStencilBuffer::~DepthStencilBuffer()
{ Release(); }

//---------------------------------------------------------------------------------------------
//      �[�x�X�e���V���o�b�t�@�𐶐����܂�.
//---------------------------------------------------------------------------------------------
bool DepthStencilBuffer::Create( ID3D11Device* pDevice, const DepthStencilBuffer::Description& desc )
{
    assert( desc.ArraySize >= 1 );
    HRESULT hr = S_OK;

    DXGI_FORMAT textureFormat;
    DXGI_FORMAT resourceFormat;

    // �K�؂ȃt�H�[�}�b�g�ɕϊ�.
    ConvertDepthFormat( desc.Format, textureFormat, resourceFormat );

    // �e�N�X�`���̐ݒ�.
    D3D11_TEXTURE2D_DESC td;
    ZeroMemory( &td, sizeof( D3D11_TEXTURE2D_DESC ) );
    td.Width                = desc.Width;
    td.Height               = desc.Height;
    td.MipLevels            = desc.MipLevels;
    td.ArraySize            = desc.ArraySize;
    td.Format               = textureFormat;
    td.SampleDesc.Count     = desc.SampleDesc.Count;
    td.SampleDesc.Quality   = desc.SampleDesc.Quality;
    td.Usage                = D3D11_USAGE_DEFAULT;
    td.BindFlags            = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags       = desc.CPUAccessFlags;
    td.MiscFlags            = desc.MiscFlags;

    // �e�N�X�`���𐶐�.
    hr = pDevice->CreateTexture2D( &td, nullptr, &m_pTexture );
    if ( FAILED( hr ) )
    {
        // �G���[���O���o��.
        OutputDebugStringA( "Error : ID3D11Device::CreateTexture2D() Failed. call from DepthStencilBuffer::Create()" );

        // ���s.
        return false;
    }
    SetDebugObjectName( m_pTexture, "asdxDepthBuffer" );

    // �[�x�X�e���V���r���[�̐ݒ�.
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    ZeroMemory( &dsvd, sizeof( D3D11_DEPTH_STENCIL_VIEW_DESC ) );
    dsvd.Format = desc.Format;

    // �z��T�C�Y��1�̏ꍇ.
    if ( desc.ArraySize == 1 )
    {
        // �}���`�T���v�����O�����̏ꍇ.
        if ( desc.SampleDesc.Count == 0 )
        {
            dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            dsvd.Texture2D.MipSlice = 0;
        }
        // �}���`�T���v�����O�L��̏ꍇ.
        else
        {
            dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        }
    }
    // �z��T�C�Y��1���傫���ꍇ.
    else
    {
        // �}���`�T���v�����O�����̏ꍇ.
        if ( desc.SampleDesc.Count == 0 )
        {
            dsvd.ViewDimension                  = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            dsvd.Texture2DArray.ArraySize       = desc.ArraySize;
            dsvd.Texture2DArray.FirstArraySlice = 0;
            dsvd.Texture2DArray.MipSlice        = 0;
        }
        // �}���`�T���v�����O�L��̏ꍇ.
        else
        {
            dsvd.ViewDimension                    = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
            dsvd.Texture2DMSArray.ArraySize       = desc.ArraySize;
            dsvd.Texture2DMSArray.FirstArraySlice = 0;
        }
    }

    // �[�x�X�e���V���r���[�𐶐�.
    hr = pDevice->CreateDepthStencilView( m_pTexture, &dsvd, &m_pDSV );
    if ( FAILED( hr ) )
    {
        // �e�N�X�`�������.
        m_pTexture->Release();
        m_pTexture = nullptr;

        // �G���[���O���o��.
        OutputDebugStringA( "Error : ID3D11Device::CreateDepthStencilView() Failed. call from DepthStencilBuffer::Create()" );

        // ���s.
        return false;
    }
    SetDebugObjectName( m_pDSV, "asdxDepthStencil" );

    // �V�F�[�_���\�[�X�r���[�̐ݒ�.
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    ZeroMemory( &srvd, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );
    srvd.Format = resourceFormat;

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
        // �}���`�T���v�����O�L��̏ꍇ
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

        // �[�x�X�e���V���r���[�����.
        m_pDSV->Release();
        m_pDSV = nullptr;

        // �G���[���O���o��.
        OutputDebugStringA( "Error : ID3D11Device::CreateShaderResourceView() Failed. call from DepthStencilBuffer::Create()" );

        // ���s.
        return false;
    }
    SetDebugObjectName( m_pSRV, "asdxDepthBuffer" );

    // ����.
    return true;
}

//---------------------------------------------------------------------------------------------
//      ����������s���܂�.
//---------------------------------------------------------------------------------------------
void DepthStencilBuffer::Release()
{
    // �V�F�[�_���\�[�X�r���[�����.
    if ( m_pSRV )
    {
        m_pSRV->Release();
        m_pSRV = nullptr;
    }

    // �[�x�X�e���V���r���[�����.
    if ( m_pDSV )
    {
        m_pDSV->Release();
        m_pDSV = nullptr;
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
ID3D11Texture2D* DepthStencilBuffer::GetTexture() const
{ return m_pTexture; }

//---------------------------------------------------------------------------------------------
//      �V�F�[�_���\�[�X�r���[���擾���܂�.
//---------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* DepthStencilBuffer::GetSRV() const
{ return m_pSRV; }

//---------------------------------------------------------------------------------------------
//      �[�x�X�e���V���r���[���擾���܂�.
//---------------------------------------------------------------------------------------------
ID3D11DepthStencilView* DepthStencilBuffer::GetDSV() const
{ return m_pDSV; }

} // namespace asdx
