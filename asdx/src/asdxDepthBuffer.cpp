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
 //! @brief     適切なフォーマットに変換します.
 //!
 //! @param [in]        format              入力フォーマット.
 //! @param [out]       textureFormat       テクスチャフォーマット.
 //! @param [out]       resourceFormat      リソースフォーマット.
 //-----------------------------------------------------------------------------------------
void ConvertDepthFormat
(
    const DXGI_FORMAT& format,
    DXGI_FORMAT&       textureFormat,
    DXGI_FORMAT&       resourceFormat
)
{
    // 入力フォーマットで初期化.
    textureFormat  = format;
    resourceFormat = format;

    // 適切なフォーマットを取得.
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
//      コンストラクタです.
//---------------------------------------------------------------------------------------------
DepthStencilBuffer::DepthStencilBuffer()
: m_pTexture( nullptr )
, m_pSRV    ( nullptr )
, m_pDSV    ( nullptr )
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      デストラクタです.
//---------------------------------------------------------------------------------------------
DepthStencilBuffer::~DepthStencilBuffer()
{ Release(); }

//---------------------------------------------------------------------------------------------
//      深度ステンシルバッファを生成します.
//---------------------------------------------------------------------------------------------
bool DepthStencilBuffer::Create( ID3D11Device* pDevice, const DepthStencilBuffer::Description& desc )
{
    assert( desc.ArraySize >= 1 );
    HRESULT hr = S_OK;

    DXGI_FORMAT textureFormat;
    DXGI_FORMAT resourceFormat;

    // 適切なフォーマットに変換.
    ConvertDepthFormat( desc.Format, textureFormat, resourceFormat );

    // テクスチャの設定.
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

    // テクスチャを生成.
    hr = pDevice->CreateTexture2D( &td, nullptr, &m_pTexture );
    if ( FAILED( hr ) )
    {
        // エラーログを出力.
        OutputDebugStringA( "Error : ID3D11Device::CreateTexture2D() Failed. call from DepthStencilBuffer::Create()" );

        // 失敗.
        return false;
    }
    SetDebugObjectName( m_pTexture, "asdxDepthBuffer" );

    // 深度ステンシルビューの設定.
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    ZeroMemory( &dsvd, sizeof( D3D11_DEPTH_STENCIL_VIEW_DESC ) );
    dsvd.Format = desc.Format;

    // 配列サイズが1の場合.
    if ( desc.ArraySize == 1 )
    {
        // マルチサンプリング無しの場合.
        if ( desc.SampleDesc.Count == 0 )
        {
            dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            dsvd.Texture2D.MipSlice = 0;
        }
        // マルチサンプリング有りの場合.
        else
        {
            dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        }
    }
    // 配列サイズが1より大きい場合.
    else
    {
        // マルチサンプリング無しの場合.
        if ( desc.SampleDesc.Count == 0 )
        {
            dsvd.ViewDimension                  = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            dsvd.Texture2DArray.ArraySize       = desc.ArraySize;
            dsvd.Texture2DArray.FirstArraySlice = 0;
            dsvd.Texture2DArray.MipSlice        = 0;
        }
        // マルチサンプリング有りの場合.
        else
        {
            dsvd.ViewDimension                    = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
            dsvd.Texture2DMSArray.ArraySize       = desc.ArraySize;
            dsvd.Texture2DMSArray.FirstArraySlice = 0;
        }
    }

    // 深度ステンシルビューを生成.
    hr = pDevice->CreateDepthStencilView( m_pTexture, &dsvd, &m_pDSV );
    if ( FAILED( hr ) )
    {
        // テクスチャを解放.
        m_pTexture->Release();
        m_pTexture = nullptr;

        // エラーログを出力.
        OutputDebugStringA( "Error : ID3D11Device::CreateDepthStencilView() Failed. call from DepthStencilBuffer::Create()" );

        // 失敗.
        return false;
    }
    SetDebugObjectName( m_pDSV, "asdxDepthStencil" );

    // シェーダリソースビューの設定.
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    ZeroMemory( &srvd, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );
    srvd.Format = resourceFormat;

    // 配列サイズが1の場合.
    if ( desc.ArraySize == 1 )
    {
        // マルチサンプリング無しの場合.
        if ( desc.SampleDesc.Count == 0 )
        {
            srvd.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvd.Texture2D.MipLevels       = desc.MipLevels;
            srvd.Texture2D.MostDetailedMip = 0;
        }
        // マルチサンプリング有りの場合.
        else
        {
            srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
        }
    }
    // 配列サイズが1より大きい場合.
    else
    {
        // マルチサンプリング無しの場合.
        if ( desc.SampleDesc.Count == 0 )
        {
            srvd.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvd.Texture2DArray.ArraySize       = desc.ArraySize;
            srvd.Texture2DArray.FirstArraySlice = 0;
            srvd.Texture2DArray.MipLevels       = desc.MipLevels;
            srvd.Texture2DArray.MostDetailedMip = 0;
        }
        // マルチサンプリング有りの場合
        else
        {
            srvd.ViewDimension                    = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
            srvd.Texture2DMSArray.ArraySize       = desc.ArraySize;
            srvd.Texture2DMSArray.FirstArraySlice = 0;
        }
    }

    // シェーダリソースビューを生成.
    hr = pDevice->CreateShaderResourceView( m_pTexture, &srvd, &m_pSRV );
    if ( FAILED( hr ) )
    {
        // テクスチャを解放.
        m_pTexture->Release();
        m_pTexture = nullptr;

        // 深度ステンシルビューを解放.
        m_pDSV->Release();
        m_pDSV = nullptr;

        // エラーログを出力.
        OutputDebugStringA( "Error : ID3D11Device::CreateShaderResourceView() Failed. call from DepthStencilBuffer::Create()" );

        // 失敗.
        return false;
    }
    SetDebugObjectName( m_pSRV, "asdxDepthBuffer" );

    // 成功.
    return true;
}

//---------------------------------------------------------------------------------------------
//      解放処理を行います.
//---------------------------------------------------------------------------------------------
void DepthStencilBuffer::Release()
{
    // シェーダリソースビューを解放.
    if ( m_pSRV )
    {
        m_pSRV->Release();
        m_pSRV = nullptr;
    }

    // 深度ステンシルビューを解放.
    if ( m_pDSV )
    {
        m_pDSV->Release();
        m_pDSV = nullptr;
    }

    // テクスチャを解放.
    if ( m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = nullptr;
    }
}

//---------------------------------------------------------------------------------------------
//      テクスチャを取得します.
//---------------------------------------------------------------------------------------------
ID3D11Texture2D* DepthStencilBuffer::GetTexture() const
{ return m_pTexture; }

//---------------------------------------------------------------------------------------------
//      シェーダリソースビューを取得します.
//---------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* DepthStencilBuffer::GetSRV() const
{ return m_pSRV; }

//---------------------------------------------------------------------------------------------
//      深度ステンシルビューを取得します.
//---------------------------------------------------------------------------------------------
ID3D11DepthStencilView* DepthStencilBuffer::GetDSV() const
{ return m_pDSV; }

} // namespace asdx
