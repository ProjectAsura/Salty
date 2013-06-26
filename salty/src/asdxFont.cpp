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
    FNT_FORMAT_R8G8B8A8 = 0,        //!< RGBA(8,8,8,8)フォーマットです.
    FNT_FORMAT_BC2                  //!< BC2_UNORMフォーマットです.
};

////////////////////////////////////////////////////////////////////////////////////////////////
// FntTextureHeader structure
////////////////////////////////////////////////////////////////////////////////////////////////
struct FntTextureHeader
{
    u32    Width;          //!< 横幅です.
    u32    Height;         //!< 縦幅です.
    u32    Format;         //!< フォーマットです.
    u32    Stride;         //!< ストライドです(=1行あたりのデータのバイト数です).
    u32    Rows;           //!< 行数です.
};

////////////////////////////////////////////////////////////////////////////////////////////////
// FntDataHeader structure
////////////////////////////////////////////////////////////////////////////////////////////////
struct FntDataHeader
{
    char8               FontName[ 32 ];     //!< フォント名です.
    u32                 FontWidth;          //!< フォントの横幅です.
    u32                 FontHeight;         //!< フォントの縦幅です.
    FntTextureHeader    TextureHeader;      //!< テクスチャヘッダーです.
};

////////////////////////////////////////////////////////////////////////////////////////////////
// FntFileHeader structure
////////////////////////////////////////////////////////////////////////////////////////////////
struct FntFileHeader
{
    u8              Magic[ 4 ];         //!< マジックです.
    u32             Version;            //!< ファイルバージョンです.
    u32             DataHeaderSize;     //!< データヘッダ構造体のサイズです.
    FntDataHeader   DataHeader;         //!< データヘッダです.
};

////////////////////////////////////////////////////////////////////////////////////////////////
// CbPerOnceVS structure
////////////////////////////////////////////////////////////////////////////////////////////////
struct CbPerOnceVS
{
    asdx::Matrix    Transform;
};

//　現在のフォントファイルバージョンです.
static const u32  FNT_CURRENT_FILE_VERSION         = 0x00000001;


} // namespace /* anonymous */ 


namespace asdx {

////////////////////////////////////////////////////////////////////////////////////////////////
// Font class
////////////////////////////////////////////////////////////////////////////////////////////////

// 入力要素です.
const D3D11_INPUT_ELEMENT_DESC Font::InputElements[ InputElementCount ] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "VTXCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

//----------------------------------------------------------------------------------------------
//          コンストラクタです.
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
//          デストラクタです.
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
//          初期化処理です.
//---------------------------------------------------------------------------------------------
bool Font::Init
(
    ID3D11Device*   pDevice,
    const char8*     filename,
    f32           screenWidth,
    f32           screenHeight
)
{
    FILE* pFile;

    // ファイルを開きます.
    errno_t err = fopen_s( &pFile, filename, "rb" );

    // チェック.
    if ( err != 0 )
    {
        // エラーログ出力.
        ELOG( "Error : File Open Failed. filename = %s\n", filename );

        // 異常終了.
        return false;
    }

    // ファイルヘッダ読み込み.
    FntFileHeader fileHeader;
    fread( &fileHeader, sizeof(FntFileHeader), 1, pFile );

    // ファイルマジックをチェック.
    if ( ( fileHeader.Magic[0] != 'F' )
      || ( fileHeader.Magic[1] != 'N' )
      || ( fileHeader.Magic[2] != 'T' )
      || ( fileHeader.Magic[3] != '\0' ) )
    {
        // エラーログ出力.
        ELOG( "Error : Invalid File. filename = %s\n", filename );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // ファイルバージョンのチェック.
    if ( fileHeader.Version != FNT_CURRENT_FILE_VERSION )
    {
        // エラーログ出力.
        ELOG( "Error : Invalid File Version. version = 0x%x, current_version = 0x%x\n", fileHeader.Version, FNT_CURRENT_FILE_VERSION );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // データヘッダ構造体サイズをチェック.
    if ( fileHeader.DataHeaderSize != sizeof( FntDataHeader ) )
    {
        // エラーログ出力.
        ELOG( "Error : Invalid Data Header Size. " );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // フォントサイズを設定.
    m_FontWidth  = fileHeader.DataHeader.FontWidth;
    m_FontHeight = fileHeader.DataHeader.FontHeight;

    // フォント名を設定.
    strcpy_s( m_FontName, fileHeader.DataHeader.FontName );

    // テクスチャ座標オフセットを算出.
    m_OffsetU = static_cast<f32>( m_FontWidth )  / static_cast<f32>( fileHeader.DataHeader.TextureHeader.Width );
    m_OffsetV = static_cast<f32>( m_FontHeight ) / static_cast<f32>( fileHeader.DataHeader.TextureHeader.Height );


    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

    // フォーマット判定.
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
        // テクスチャサイズ算出.
        size_t textureSize = fileHeader.DataHeader.TextureHeader.Stride * fileHeader.DataHeader.TextureHeader.Rows;

        // テクセルのメモリを確保.
        u8* pPixels = new u8 [ textureSize ];

        // NULLチェック.
        if ( pPixels == nullptr )
        {
            // エラーログ出力.
            ELOG( "Error : Memory Allocate Failed." );

            // ファイルを閉じる.
            fclose( pFile );

            // 異常終了.
            return false;
        }

        // テクセルデータを一気に読み込み.
        fread( pPixels, sizeof( u8 ), textureSize, pFile );

        // サブリソースデータを設定.
        D3D11_SUBRESOURCE_DATA subRes;
        subRes.pSysMem          = pPixels;
        subRes.SysMemPitch      = fileHeader.DataHeader.TextureHeader.Stride;
        subRes.SysMemSlicePitch = textureSize;

        // 2Dテクスチャの設定.
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

        // テクスチャ生成.
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

        // テクスチャを設定.
        m_pTexture = pTexture;

        // デバッグオブジェクト名を設定.
        SetDebugObjectName( m_pTexture, "asdx::Font" );

        {
            // シェーダリソースビューの設定.
            D3D11_SHADER_RESOURCE_VIEW_DESC sd;
            ZeroMemory( &sd, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );
            sd.Format              = format;
            sd.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
            sd.Texture2D.MipLevels = desc.MipLevels;

            // シェーダリソースビュー生成.
            ID3D11ShaderResourceView* pSRV;
            hr = pDevice->CreateShaderResourceView( pTexture, &sd, &pSRV );

            // エラーチェック.
            if ( FAILED( hr ) )
            {
                // テクスチャ解放.
                pTexture->Release();

                delete [] pPixels;
                pPixels = nullptr;

                // エラーログ出力.
                ELOG( "Error : ID3D11Device::CreateShaderResourceView() Failed." );

                // 異常終了.
                return false;
            }

            // シェーダリソースビューを設定.
            m_pSRV = pSRV;

            // デバッグオブジェクト名設定.
            SetDebugObjectName( m_pSRV, "asdx::Font" );
        }

        delete [] pPixels;
        pPixels = nullptr;

    }


    // ファイルを閉じる.
    fclose( pFile );

    // 頂点バッファの生成.
    {
        // 頂点バッファの設定.
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        desc.Usage          = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth      = sizeof( Font::Vertex ) * NUM_SPRITES * NUM_VERTEX_PER_SPRITE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // 頂点バッファの生成.
        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pVB );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
            return false;
        }
        SetDebugObjectName( m_pVB, "asdx::Font" );
    }

    // インデックスバッファの生成.
    {
        // インデックスバッファの設定.
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof( short ) * NUM_SPRITES * NUM_INDEX_PER_SPRITE;

        // インデックスバッファのメモリを確保.
        std::vector< short > indices;
        indices.reserve( NUM_SPRITES * NUM_INDEX_PER_SPRITE );

        // インデックス設定.
        for( short i=0; i<NUM_SPRITES * NUM_INDEX_PER_SPRITE; i+=NUM_VERTEX_PER_SPRITE )
        {
            indices.push_back( i + 0 );
            indices.push_back( i + 1 );
            indices.push_back( i + 2 );

            indices.push_back( i + 1 );
            indices.push_back( i + 3 );
            indices.push_back( i + 2 );
        }

        // サブリソースデータの設定.
        D3D11_SUBRESOURCE_DATA res;
        ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
        res.pSysMem = &indices.front();

        // インデックスバッファ生成.
        hr = pDevice->CreateBuffer( &desc, &res, &m_pIB );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // インデックスをクリア.
            indices.clear();

            // 異常終了.
            return false;
        }
        // デバッグオブジェクト名.
        SetDebugObjectName( m_pIB, "asdx::Font" );

        // インデックスをクリア.
        indices.clear();
    }

    // シェーダの設定.
    {
        #include "../res/shader/Compiled/SpriteShader_VSFunc.inc"
        #include "../res/shader/Compiled/SpriteShader_PSFunc.inc"

        // 頂点シェーダ生成.
        hr = pDevice->CreateVertexShader( SpriteShader_VSFunc, sizeof( SpriteShader_VSFunc ), nullptr, &m_pVS );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );

            // 異常終了.
            return false;
        }
        // デバッグオブジェクト名を設定.
        SetDebugObjectName( m_pVS, "asdx::Font" );

        // ピクセルシェーダ生成.
        hr = pDevice->CreatePixelShader( SpriteShader_PSFunc, sizeof( SpriteShader_PSFunc ), nullptr, &m_pPS );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreatePixelShader() Failed." );

            // 異常終了.
            return false;
        }
        // デバッグオブジェクト名を設定.
        SetDebugObjectName( m_pPS, "asdx::Font" );

        // 入力レイアウトを生成.
        hr = pDevice->CreateInputLayout( Font::InputElements, Font::InputElementCount, SpriteShader_VSFunc, sizeof( SpriteShader_VSFunc ), &m_pIL );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateInputLayout() Failed. ");

            // 異常終了.
            return false;
        }
        // デバッグオブジェクト名を設定.
        SetDebugObjectName( m_pIL, "asdx::Font" );
    }

    // 定数バッファの生成.
    {
        // 定数バッファの設定.
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof( CbPerOnceVS );
        desc.Usage     = D3D11_USAGE_DEFAULT;

        // 定数バッファ生成.
        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pCB );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

            // 異常終了.
            return false;
        }
        // デバッグオブジェクト名を設定.
        SetDebugObjectName( m_pCB, "asdx::Font" );
    }

    // ブレンドステートの設定
    {
        // アルファブレンド : src = ONE, dest = INV_SRC_ALPHA
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

        // ブレンドステートを生成.
        hr = pDevice->CreateBlendState( &desc, &m_pBS );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateBlendState() Failed." );

            // 異常終了.
            return false;
        }
        // デバッグオブジェクト名を設定.
        SetDebugObjectName( m_pBS, "asdx::Font" );

    }

    // サンプラーステートの生成.
    {
        // サンプラーステートの設定.
        D3D11_SAMPLER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_SAMPLER_DESC ) );
        desc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MaxAnisotropy  = ( pDevice->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ) ? 16 : 2;
        desc.MaxLOD         = FLT_MAX;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        // サンプラーステートを生成.
        hr = pDevice->CreateSamplerState( &desc, &m_pSmp );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateSamplerState() Failed." );

            // 異常終了.
            return false;
        }
        // デバッグオブジェクト名を設定.
        SetDebugObjectName( m_pSmp, "asdx::Font" );
    }

    // 深度ステンシルステートの生成.
    {
        // 深度ステンシルステートの設定.
        D3D11_DEPTH_STENCIL_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
        desc.DepthEnable    = FALSE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
        desc.StencilEnable  = FALSE;

        // 深度ステンシルステートの生成.
        hr = pDevice->CreateDepthStencilState( &desc, &m_pDSS );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateDepthStencilState() Failed." );

            // 異常終了.
            return false;
        }
        // デバッグオブジェクト名を設定.
        SetDebugObjectName( m_pDSS, "asdx::Font" );
    }

    // ラスタライザーステートの生成.
    {
        // ラスタライザーステートの設定.
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_RASTERIZER_DESC ) );
        desc.CullMode          = D3D11_CULL_BACK;
        desc.FillMode          = D3D11_FILL_SOLID;
        desc.DepthClipEnable   = true;
        desc.MultisampleEnable = true;

        // ラスタライザーステートの生成.
        hr = pDevice->CreateRasterizerState( &desc, &m_pRS );
        if ( FAILED( hr ) )
        {
            // エラーログ出力.
            ELOG( "Error : ID3D11Device::CreateRasterizerState() Failed." );

            // 異常終了.
            return false;
        }
        // デバッグオブジェクト名を設定.
        SetDebugObjectName( m_pRS, "asdx::Font" );
    }

    // スクリーンサイズを設定.
    SetScreenSize( screenWidth, screenHeight );

    // 正常終了.
    return true;
};

//---------------------------------------------------------------------------------------------
//          終了処理です.
//---------------------------------------------------------------------------------------------
void Font::Term()
{
    // 頂点シェーダを解放.
    if ( m_pVS )
    {
        m_pVS->Release();
        m_pVS = nullptr;
    }

    // ピクセルシェーダを解放.
    if ( m_pPS )
    {
        m_pPS->Release();
        m_pPS = nullptr;
    }

    // 入力レイアウトを解放.
    if ( m_pIL )
    {
        m_pIL->Release();
        m_pIL = nullptr;
    }

    // 頂点バッファを解放.
    if ( m_pVB )
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    // インデックスバッファを解放.
    if ( m_pIB )
    {
        m_pIB->Release();
        m_pIB = nullptr;
    }

    // 定数バッファを解放.
    if ( m_pCB )
    {
        m_pCB->Release();
        m_pCB = nullptr;
    }

    // テクスチャを解放.
    if ( m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = nullptr;
    }

    // シェーダリソースビューを解放.
    if ( m_pSRV )
    {
        m_pSRV->Release();
        m_pSRV = nullptr;
    }

    // サンプラーステートを解放.
    if ( m_pSmp )
    {
        m_pSmp->Release();
        m_pSmp = nullptr;
    }

    // ブレンドステートを解放.
    if ( m_pBS )
    {
        m_pBS->Release();
        m_pBS = nullptr;
    }

    // 深度ステンシルステートを解放.
    if ( m_pDSS )
    {
        m_pDSS->Release();
        m_pDSS = nullptr;
    }

    // ラスタライザーステートを解放.
    if ( m_pRS )
    {
        m_pRS->Release();
        m_pRS = nullptr;
    }

    memset( &m_FontName, 0, sizeof( char8 ) * 32 );
    m_FontName[0]   = '\0';
    m_FontWidth     = 0;
    m_FontHeight    = 0;
    m_SpriteCount   = 0;
    m_OffsetU       = 0.0f;
    m_OffsetV       = 0.0f;
    m_ScreenSize.x  = 0.0f;
    m_ScreenSize.y  = 0.0f;
    m_Color         = asdx::Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
}

//---------------------------------------------------------------------------------------------
//          スクリーンサイズを設定.
//---------------------------------------------------------------------------------------------
void Font::SetScreenSize( f32 width, f32 height )
{
    SetScreenSize( asdx::Vector2( width, height ) );
}

//---------------------------------------------------------------------------------------------
//          スクリーンサイズを設定.
//---------------------------------------------------------------------------------------------
void Font::SetScreenSize( const asdx::Vector2& size )
{
    m_ScreenSize = size;

    f32 xScale = ( m_ScreenSize.x > 0.0f ) ? 2.0f / m_ScreenSize.x : 0.0f;
    f32 yScale = ( m_ScreenSize.y > 0.0f ) ? 2.0f / m_ScreenSize.y : 0.0f;

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
//          テキストカラーを設定.
//---------------------------------------------------------------------------------------------
void Font::SetColor( f32 r, f32 g, f32 b, f32 a )
{
    m_Color.x = r;
    m_Color.y = g;
    m_Color.z = b;
    m_Color.w = a;
}

//---------------------------------------------------------------------------------------------
//          テキストカラーを設定.
//---------------------------------------------------------------------------------------------
void Font::SetColor( const asdx::Vector4& color )
{
    m_Color = color;
}

//---------------------------------------------------------------------------------------------
//          描画開始処理です.
//---------------------------------------------------------------------------------------------
void Font::Begin( ID3D11DeviceContext* pDeviceContext )
{
    // スプライト数をリセット.
    m_SpriteCount = 0;

    // ブレンドステートを設定します.
    pDeviceContext->OMSetBlendState( m_pBS, nullptr, 0xFFFFFFFF );

    // 深度ステンシルステートを設定します.
    pDeviceContext->OMSetDepthStencilState( m_pDSS, 0 );

    // ラスタライザーステートを設定します.
    pDeviceContext->RSSetState( m_pRS );

    // サンプラーステートを設定します.
    pDeviceContext->PSSetSamplers( 0, 1, &m_pSmp );

    // プリミティブトポロジーを設定します.
    pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // 入力レイアウトを設定します.
    pDeviceContext->IASetInputLayout( m_pIL );

    // シェーダを設定します.
    pDeviceContext->VSSetShader( m_pVS, nullptr, 0 );
    pDeviceContext->PSSetShader( m_pPS, nullptr, 0 );
    pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
    pDeviceContext->DSSetShader( nullptr, nullptr, 0 );

    u32 stride = sizeof( Font::Vertex );
    u32 offset = 0;

    // 頂点バッファとインデックスバッファを設定します.
    pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, &stride, &offset );
    pDeviceContext->IASetIndexBuffer( m_pIB, DXGI_FORMAT_R16_UINT, 0 );

    // 定数バッファを更新し，設定します.
    pDeviceContext->UpdateSubresource( m_pCB, 0, nullptr, &m_Transform, 0, 0 );
    pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pCB );

    // シェーダリソースビューを設定します.
    pDeviceContext->PSSetShaderResources( 0, 1, &m_pSRV );
}

//---------------------------------------------------------------------------------------------
//          文字列を描画します.
//---------------------------------------------------------------------------------------------
void Font::DrawString( const int x, const int y, const char8* text )
{
    DrawString( x, y, 0, text );
}

//---------------------------------------------------------------------------------------------
//          文字列を描画します.
//---------------------------------------------------------------------------------------------
void Font::DrawString( const int x, const int y, const int layerDepth, const char8* text )
{
    u32 stringsCount = (u32)strlen( text );

    // 文字列がなければ何もしない.
    if ( stringsCount == 0 )
    { return; }

    // 最大スプライト数を超えないかチェック.
    if ( m_SpriteCount + stringsCount > NUM_SPRITES )
    { return; }

    // 開始位置を設定.
    f32 posX = static_cast<f32>( x );
    f32 posY = static_cast<f32>( y );

    // 1文字あたりのサイズ取得.
    f32 w = static_cast<f32>( m_FontWidth );
    f32 h = static_cast<f32>( m_FontHeight );
    f32 d = static_cast<f32>( layerDepth );

    // 頂点データのポインタ取得.
    Font::Vertex* pVertices = &m_Vertices[ m_SpriteCount * NUM_VERTEX_PER_SPRITE ];

    // 文字数分ループ.
    for( u32 i=0; i<stringsCount; i++ )
    {
        if ( (u32)text[i] == 0x0a )
        {
            posY += w;
            posX  = static_cast< f32 >( x );
        }
        // インデックス算出.
        u32 idx = (u32)text[i] - 0x20;

        if ( idx > 95 )
        { continue; }

        // テクスチャ座標.
        f32 u0 = ( idx + 0 ) * m_OffsetU;
        f32 u1 = ( idx + 1 ) * m_OffsetU;
        f32 v0 = 0.0f;
        f32 v1 = m_OffsetV;
        
        // 位置座標.
        f32 x0 = posX;
        f32 x1 = posX + w;
        f32 y0 = posY;
        f32 y1 = posY + h;

        // 頂点インデックスを算出.
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

        // 横方向に移動.
        posX += w;
    }

    // スプライト数をカウントアップします.
    m_SpriteCount += stringsCount;
}

//---------------------------------------------------------------------------------------------
//      書式指定して文字列を描画します.
//---------------------------------------------------------------------------------------------
void Font::DrawStringArg( const int x, const int y, const char8* format, ... )
{
    int result = 0;
    va_list arg;

    va_start( arg, format );
    result = vsprintf_s( m_Buffer, format, arg );
    va_end( arg );

    DrawString( x, y, 0, m_Buffer );
}

//---------------------------------------------------------------------------------------------
//      書式指定して文字列を描画します.
//---------------------------------------------------------------------------------------------
void Font::DrawStringArg( const int x, const int y, const int layerDepth, const char8* format, ... )
{
    int result = 0;
    va_list arg;

    va_start( arg, format );
    result = vsprintf_s( m_Buffer, format, arg );
    va_end( arg );

    DrawString( x, y, layerDepth, m_Buffer );
}

//---------------------------------------------------------------------------------------------
//      描画終了です.
//---------------------------------------------------------------------------------------------
void Font::End( ID3D11DeviceContext* pDeviceContext )
{
    D3D11_MAP mapType = ( m_SpriteCount == 0 ) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
    D3D11_MAPPED_SUBRESOURCE mappedBuffer;

    // マップします.
    HRESULT hr = pDeviceContext->Map( m_pVB, 0, mapType, 0, &mappedBuffer );
    if ( FAILED( hr ) )
    { return; }

    // 頂点データのポインタ取得.
    Font::Vertex* pVertices = (Font::Vertex*)mappedBuffer.pData;

    // がばっとコピる.
    memcpy( pVertices, m_Vertices, sizeof( Vertex ) * m_SpriteCount * NUM_VERTEX_PER_SPRITE );

    // アンマップします.
    pDeviceContext->Unmap( m_pVB, 0 );

    // インデックス数.
    u32 indexCount = m_SpriteCount * NUM_INDEX_PER_SPRITE;

    // スプライトを描画.
    pDeviceContext->DrawIndexed( indexCount, 0, 0 );

    // シェーダをアンバイド.
    pDeviceContext->VSSetShader( nullptr, 0, 0 );
    pDeviceContext->PSSetShader( nullptr, 0, 0 );

    // ステートをnullに設定.
    pDeviceContext->OMSetBlendState( nullptr, nullptr, 0xFFFFFFFF );
    pDeviceContext->OMSetDepthStencilState( nullptr, 0 );
    pDeviceContext->RSSetState( nullptr );
}


} // namespace asdx
