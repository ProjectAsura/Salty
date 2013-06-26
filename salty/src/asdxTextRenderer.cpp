//--------------------------------------------------------------------------------------------
// File : asdxTextRenderer.cpp
// Desc : Text Renderer Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------
#include <asdxTextRenderer.h>
#include <asdxUtil.h>
#include <asdxLog.h>
#include <cassert>
#include <cstdio>
#include <algorithm>


namespace /* anonymous */ {

struct CbPerOnceVS
{
    asdx::MATRIX    Transform;
};

} // namespace /* anonymous */

namespace asdx {

//////////////////////////////////////////////////////////////////////////////////////////////
// TextRenderer class
//////////////////////////////////////////////////////////////////////////////////////////////

TextRenderer::TextRenderer()
: m_pVS             ( nullptr )
, m_pPS             ( nullptr )
, m_pIL             ( nullptr )
, m_pVB             ( nullptr )
, m_pIB             ( nullptr )
, m_pCB             ( nullptr )
, m_pTexture        ( nullptr )
, m_pSRV            ( nullptr )
, m_pSmp            ( nullptr )
, m_Glyphs          ()
, m_pDefaultGlyph   ( nullptr )
, m_LineSpace       ( 0.0f )
, m_SpriteCount     ( 0 )
{ /* DO_NOTHING */ }

TextRenderer::~TextRenderer()
{
    assert( m_pVS == nullptr );
    assert( m_pPS == nullptr );
    assert( m_pIL == nullptr );
    assert( m_pVB == nullptr );
    assert( m_pIB == nullptr );
    assert( m_pCB == nullptr );
    assert( m_pTexture == nullptr );
    assert( m_pSRV == nullptr );
    assert( m_pSmp == nullptr );
    m_Glyphs.clear();
}

bool TextRenderer::Init( ID3D11Device* pDevice, const wchar_t* filename )
{
    if ( pDevice == nullptr )
    {
        ELOG( "Error : Invalid Argument. pDevice is NULL." );
        return false;
    }

    if ( filename == nullptr )
    {
        ELOG( "Error : Invalid Argument. filename is NULL." );
        return false;
    }

    HRESULT hr = S_OK;

    // フォントファイル解析.
    {
        const char FontMagic[] = "FNT\0";

        FILE* pFile;
        
        errno_t err = _wfopen_s( &pFile, filename, L"rb" );
        if ( err != 0 )
        {
            ELOGW( "Error : File Open Failed. filename = %s", filename );
            return false;
        }

        unsigned char magic[4];
        fread( magic, sizeof( unsigned char ), 4 );

        for( int i=0; i<4; ++i )
        {
            if ( magic[i] != FontMagic[i] )
            {
                ELOGW( "Error : Invalide File. filename = %s", filename );
                fclose( pFile );
                return false;
            }
        }

        unsigned int glyphCount = 0;
        Glyph glyph;

        fread( &glyphCount, sizeof( unsigned int ), 1, pFile );

        m_Glyphs.resize( glyphCount );
        for( unsigned int i=0; i<glyphCount; ++i )
        {
            fread( &glyph, sizeof( Glyph ), 1, pFile );
            m_Glyphs[ i ] = glyph;
        }

        fread( &m_LineSpace, sizeof( float ), 1, pFile );
        
        wchar_t defaultCharacter;
        fread( &defaultCharacter, sizeof( wchar_t ), 1, pFile );
        SetDefaultCharacter( defaultCharacter );

        unsigned int mapWidth;
        unsigned int mapHeight;
        DXGI_FORMAT  mapFormat;
        unsigned int mapStride;
        unsigned int mapRows;
        fread( &mapWidth,  sizeof( unsigned int ), 1, pFile );
        fread( &mapHeight, sizeof( unsigned int ), 1, pFile );
        fread( &mapFormat, sizeof( DXGI_FORMAT ),  1, pFile );
        fread( &mapStride, sizeof( unsigned int ), 1, pFile );
        fread( &mapRows,   sizeof( unsigned int ), 1, pFile );

        unsigned char* pMapData = new unsigned char [ mapStride * mapRows ];
        if ( pMapData == nullptr )
        {
            ELOG( "Error : Memory allocate Failed. ");
            return false;
        }

        fread( pMapData, sizeof( unsigned char ), mapStride * mapRows, pFile );

        CD3D11_TEXTURE2D_DESC mapDesc( 
            mapFormat,
            mapWidth,
            mapHeight,
            1,
            1,
            D3D11_BIND_SHADER_RESOURCE,
            D3D11_USAGE_IMMUTABLE );
        CD3D11_SHADER_RESOURCE_VIEW_DESC viewDesc( D3D11_SRV_DIMENSION_TEXTURE2D, mapFormat );
        D3D11_SUBRESOURCE_DATA initData = { pMapData, mapStride };

        hr = pDevice->CreateTexture2D( &mapDesc, &initData, &m_pTexture );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateTexture2D() Failed. ");

            delete [] pMapData;
            pMapData = nullptr;

            return false;
        }

        hr = pDevice->CreateShaderResourceView( m_pTexture, &viewDesc, &m_pSRV );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateShaderResourceView() Failed. ");

            delete [] pMapData;
            pMapData = nullptr;

            return false;
        }

        delete [] pMapData;
        pMapData = nullptr;

        SetDebugObjectName( m_pTexture, "asdx::TextRenderer" );
        SetDebugObjectName( m_pSRV,     "asdx::TextRenderer" );
    }

    // 頂点バッファ生成.
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = sizeof( TextRenderer::Vertex ) * NUM_MAX_SPRITE * NUM_VERTEX_PER_SPRITE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pVB );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
            return false;
        }
        SetDebugObjectName( m_pVB, "asdx::TextRenderer" );
    }

    // インデックスバッファ生成.
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof( short ) * NUM_MAX_SPRITE * NUM_INDEX_PER_SRPITE;

        std::vector<short> indices;
        indices.reserve( NUM_MAX_SPRITE * NUM_INDEX_PER_SRPITE );

        for( short i=0; i<NUM_MAX_SPRITE * NUM_VERTEX_PER_SPRITE; i+= NUM_VERTEX_PER_SPRITE )
        {
            indices.push_back( i + 0 );
            indices.push_back( i + 1 );
            indices.push_back( i + 2 );

            indices.push_back( i + 1 );
            indices.push_back( i + 3 );
            indices.push_back( i + 2 );
        }

        D3D11_SUBRESOURCE_DATA res;
        ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
        res.pSysMem = &indices.front();

        hr = pDevice->CreateBuffer( &desc, &res, &m_pIB );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
            return false;
        }
        SetDebugObjectName( m_pIB, "asdx::TextRenderer" );

        indices.clear();
    }

    // シェーダ生成.
    {
        // ビルド済みシェーダコード.
        #include "../res/shader/Compiled/SpriteShader_VSFunc.inc"
        #include "../res/shader/Compiled/SpriteShader_PSFunc.inc"

        hr = pDevice->CreateVertexShader( SpriteShader_VSFunc, sizeof( SpriteShader_VSFunc ), nullptr, &m_pVS );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateVertexShader() Failed." );
            return false;
        }
        SetDebugObjectName( m_pVS, "asdx::TextRenderer" );

        hr = pDevice->CreatePixelShader( SpriteShader_PSFunc, sizeof( SpriteShader_PSFunc), nullptr, &m_pPS );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreatePixelShader() Failed." );
            return false;
        }
        SetDebugObjectName( m_pPS, "asdx::TextRenderer" );

        hr = pDevice->CreateInputLayout( Vertex::InputElements, Vertex::InputElementCount, SpriteShader_VSFunc, sizeof( SpriteShader_VSFunc ) );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateInputLayout() Failed." );
            return false;
        }
        SetDebugObjectName( m_pIL, "asdx::TextRenderer" );
    }

    // 定数バッファの生成.
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof( CbPerOnceVS );
        desc.Usage     = D3D11_USAGE_DEFAULT;

        hr = pDevice->CreateBuffer( &desc, nullptr, &m_pCB );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
            return false;
        }
        SetDebugObjectName( m_pCB, "asdx::TextRenderer" );
    }

    // サンプラーステートの生成.
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_SAMPLER_DESC ) );

        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MaxAnisotropy = ( pDevice->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ) ? 16 : 2;
        desc.MaxLOD = FLT_MAX;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        hr = pDevice->CreateSamplerState( &desc, &m_pSmp );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D11Device::CreateSamplerState() Failed." );
            return false;
        }
        SetDebugObjectName( m_pSmp, "asdx::TextRenderer" );
    }

    return true;
}

void TextRenderer::Term()
{
    if ( m_pVS )
    {
        m_pVS->Release();
        m_pVS = nullptr;
    }

    if ( m_pPS )
    {
        m_pPS->Release();
        m_pPS = nullptr;
    }

    if ( m_pIL )
    {
        m_pIL->Release();
        m_pIL = nullptr;
    }

    if ( m_pVB )
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    if ( m_pIB )
    {
        m_pIB->Release();
        m_pIB = nullptr;
    }

    if ( m_pCB )
    {
        m_pCB->Release();
        m_pCB = nullptr;
    }

    if ( m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = nullptr;
    }

    if ( m_pSRV )
    {
        m_pSRV->Release();
        m_pSRV = nullptr;
    }

    if ( m_pSmp )
    {
        m_pSmp->Release();
        m_pSmp = nullptr;
    }

    m_Glyphs.clear();

    m_LineSpace = 0.0f;
    m_SpriteCount = 0;
}

TextRenderer::Glyph const* TextRenderer::FindGlyph( wchar_t character ) const
{
    auto glyph = std::lower_bound( m_Glyphs.begin(), m_Glyphs.end(), character );

    if ( ( glyph != m_Glyphs.end() ) && ( glyph->Character == character ) )
    { return &(*glyph); }

    if ( m_pDefaultGlyph )
    { return m_pDefaultGlyph; }

    return nullptr;
}

FLOAT2 TextRenderer::MeasureString( wchar_t const* text ) const
{
    FLOAT2 result( 0.0f, 0.0f );

    float x = 0.0f;
    float y = 0.0f;

    for( ; *text; text++ )
    {
        wchar_t character = (*text);

        switch( character )
        {
        case '\r':
            continue;

        case '\n':
            {
                x = 0.0f;
                y += m_LineSpace;
            }
            break;

        default:
            {
                auto glyph = FindGlyph( character );
                x += glyph->OffsetX;

                if ( x < 0 )
                { x = 0.0f; }

                if ( !iswspace( character ) )
                {
                    float w = (float)(glyph->SubRect.right - glyph->SubRect.left);
                    float h = (float)(glyph->SubRect.bottom - glyph->SubRect.top ) + glyph->OffsetY;

                    h = Max( h, m_LineSpace );

                    result = Max( result, FLOAT2( x + w, y + h );
                }

                x += glyph->SubRect.right - glyph->SubRect.left + glyph->AdvanceX;
            }
            break;
        }
    }

    return result;
}

void TextRenderer::Begin()
{
    m_IsBeginDraw = true;
}

void TextRenderer::DrawString( wchar_t* text, FLOAT2 position, FLOAT4 color, float layerDepth )
{
    assert( text != nullptr );
    assert( m_IsBeginDraw == true );

    {
        float x = 0.0f;
        float y = 0.0f;

        for( ; *text; text++ )
        {
            wchar_t character = (*text);

            switch( character )
            {
            case '\r':
                continue;

            case '\n':
                {
                    x = 0.0f;
                    y += m_LineSpace;
                }
                break;

            default:
                {
                    auto glyph = FindGlyph( character );
                    x += glyph->OffsetX;

                    FLOAT2 offset( x, y + glyph->OffsetY );

                    if ( x < 0 )
                    { x = 0.0f; }

                    if ( !iswspace( character ) )
                    { DrawSprite( m_pSRV, position, &glyph->SubRect, color, offset, layerDepth ); }

                    x += glyph->SubRect.right - glyph->SubRect.left + glyph->AdvanceX;
                }
                break;
            }
        }

    }
}

void TextRenderer::End( ID3D11DeviceContext* pDeviceContext )
{
}

float TextRenderer::GetLineSpace() const
{ return m_LineSpace; }

void TextRenderer::SetLineSpace( float lineSpace )
{ m_LineSpace = lineSpace; }

wchar_t TextRenderer::GetDefaultCharacter() const
{
    return ( m_pDefaultGlyph ) ? (wchar_t)m_pDefaultGlyph->Character : 0;
}

void TextRenderer::SetDefaultCharacter( wchar_t character )
{
    m_pDefaultGlyph = nullptr;

    if ( character )
    { m_pDefaultGlyph = FindGlyph( character ); }
}


} // namespace asdx