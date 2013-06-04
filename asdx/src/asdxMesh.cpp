//--------------------------------------------------------------------------------------------
// File : asdxMesh.cpp
// Desc : Asura Mesh Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------
#include <asdxMesh.h>
#include <asdxLog.h>
#include <asdxUtil.h>
#include <string>
#include <cassert>


namespace asdx {

//////////////////////////////////////////////////////////////////////////////////////////////
// Mesh class
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------------------
Mesh::Mesh()
: m_pVB             ( nullptr )
, m_pIB             ( nullptr )
, m_pMB             ( nullptr )
, m_pIL             ( nullptr )
, m_pDiffuseSmp     ( nullptr )
, m_pSpecularSmp    ( nullptr )
, m_pNormalSmp      ( nullptr )
, m_Stride          ( 0 )
, m_Offset          ( 0 )
, m_SubsetCount     ( 0 )
, m_MaterialCount   ( 0 )
, m_pSubset         ( nullptr )
, m_pMaterial       ( nullptr )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
    assert( m_pVB == nullptr );
    assert( m_pIB == nullptr );
    assert( m_pMB == nullptr );
    assert( m_pIL == nullptr );
    assert( m_pDiffuseSmp  == nullptr );
    assert( m_pSpecularSmp == nullptr );
    assert( m_pNormalSmp   == nullptr );
    assert( m_pSubset      == nullptr );
    assert( m_pMaterial    == nullptr );
}

//-------------------------------------------------------------------------------------------
//      初期化処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::Init
(
    ID3D11Device*           pDevice,
    const asdx::ResMesh&    mesh,
    const void*             pShaderByteCode, 
    const asdx::UINT        byteCodeLength,
    const char*             resFolderPath,
    const char*             dummyFolderPath
)
{
    // 入力レイアウトを生成.
    if ( !OnCreateIL( pDevice, pShaderByteCode, byteCodeLength ) )
    {
        ELOG( "Error : Mesh::OnCreateIL() Failed." );
        return false;
    }

    // 頂点バッファを生成.
    if ( !OnCreateVB( pDevice, mesh ) )
    {
        ELOG( "Error : Mesh::OnCreateVB() Failed." );
        return false;
    }

    // インデックスバッファを生成.
    if ( !OnCreateIB( pDevice, mesh ) )
    {
        ELOG( "Error : Mesh::OnCreateIB() Failed." );
        return false;
    }

    // サブセットを生成.
    if ( !OnCreateSubset( pDevice, mesh ) )
    {
        ELOG( "Error : Mesh::OnCreateSubset() Failed." );
        return false;
    }

    // マテリアルを生成.
    if ( !OnCreateMaterial( pDevice, mesh, resFolderPath, dummyFolderPath ) )
    {
        ELOG( "Error : Mesh::OnCreateMaterial() Failed." );
        return false;
    }

    // マテリアルバッファを生成.
    if ( !OnCreateMB( pDevice, mesh ) )
    {
        ELOG( "Error : Mesh::OnCreateMB() Failed." );
        return false;
    }

    // サンプラーステートを生成.
    if ( !OnCreateSmp( pDevice ) )
    {
        ELOG( "Error : Mesh::OnCreateSmp() Failed." );
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------
//      終了処理です.
//-------------------------------------------------------------------------------------------
void Mesh::Term()
{
    // 頂点バッファを破棄.
    if ( m_pVB )
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    // インデックスバッファを破棄.
    if ( m_pIB )
    {
        m_pIB->Release();
        m_pIB = nullptr;
    }

    // マテリアルバッファを破棄.
    if ( m_pMB )
    {
        m_pMB->Release();
        m_pMB = nullptr;
    }

    // 入力レイアウトを破棄.
    if ( m_pIL )
    {
        m_pIL->Release();
        m_pIL = nullptr;
    }

    // ディフューズマップのサンプラーステートを破棄.
    if ( m_pDiffuseSmp )
    {
        m_pDiffuseSmp->Release();
        m_pDiffuseSmp = nullptr;
    }

    // スペキュラーマップのサンプラーステートを破棄.
    if ( m_pSpecularSmp )
    {
        m_pSpecularSmp->Release();
        m_pSpecularSmp = nullptr;
    }

    // 法線マップのサンプラーステートを破棄.
    if ( m_pNormalSmp )
    {
        m_pNormalSmp->Release();
        m_pNormalSmp = nullptr;
    }

    // サブセットを破棄.
    if ( m_pSubset )
    {
        delete [] m_pSubset;
        m_pSubset = nullptr;
    }

    // マテリアルを解放.
    OnTermMaterial();

    m_SubsetCount = 0;
    m_Stride      = 0;
    m_Offset      = 0;

}

//-------------------------------------------------------------------------------------------
//      描画処理です.
//-------------------------------------------------------------------------------------------
void Mesh::Draw( ID3D11DeviceContext* pDeviceContext )
{
    // 描画開始処理です.
    OnDrawBegin( pDeviceContext );

    for( asdx::UINT i=0; i<m_SubsetCount; ++i )
    {
        // サブセットの描画処理です.
        OnDrawSubset( pDeviceContext, i );
    }
    
    // 描画終了処理です.
    OnDrawEnd( pDeviceContext );
}

//-------------------------------------------------------------------------------------------
//      デフォルトの入力レイアウト作成処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateIL
(
    ID3D11Device*    pDevice,
    const void*      shaderByteCode,
    const asdx::UINT byteCodeLength
)
{
    // 入力レイアウトを生成.
    HRESULT hr = pDevice->CreateInputLayout( 
        asdx::ResMesh::INPUT_ELEMENTS,
        asdx::ResMesh::NUM_INPUT_ELEMENT,
        shaderByteCode,
        byteCodeLength,
        &m_pIL );

    // チェック.
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateInputLayout() Failed." );
        return false;
    }
    SetDebugObjectName( m_pIL, "asdx::Mesh" );

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------------
//      デフォルトの頂点バッファ作成処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateVB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    // バッファの設定.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage          = D3D11_USAGE_DEFAULT;
    desc.ByteWidth      = sizeof( asdx::ResMesh::Vertex ) * mesh.GetVertexCount();
    desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;

    // ストライドとオフセットを設定.
    m_Stride = sizeof( asdx::ResMesh::Vertex );
    m_Offset = 0;

    // リソースの設定.
    D3D11_SUBRESOURCE_DATA res;
    ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
    res.pSysMem = mesh.GetVertices();

    // 頂点バッファの生成.
    HRESULT hr = pDevice->CreateBuffer( &desc, &res, &m_pVB );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
        return false;
    }
    SetDebugObjectName( m_pVB, "asdx::Mesh" );

    return true;
}

//-------------------------------------------------------------------------------------------
//      デフォルトのインデックスバッファ作成処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateIB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    // バッファの設定.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage          = D3D11_USAGE_DEFAULT;
    desc.ByteWidth      = sizeof( asdx::UINT ) * mesh.GetIndexCount();
    desc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;

    // リソースデータの設定.
    D3D11_SUBRESOURCE_DATA res;
    ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
    res.pSysMem = mesh.GetIndices();

    // インデックスバッファを生成.
    HRESULT hr = pDevice->CreateBuffer( &desc, &res, &m_pIB );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
        return false;
    }
    SetDebugObjectName( m_pIB, "asdx::Mesh" );

    return true;
}

//-------------------------------------------------------------------------------------------
//      デフォルトのマテリアルバッファ作成処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateMB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    // バッファの設定.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage          = D3D11_USAGE_DEFAULT;
    desc.ByteWidth      = sizeof( Mesh::MaterialBuffer );
    desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;

    // マテリアルバッファを生成.
    HRESULT hr = pDevice->CreateBuffer( &desc, nullptr, &m_pMB );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );
        return false;
    }
    SetDebugObjectName( m_pMB, "asdx::Mesh" );

    return true;
}

//-------------------------------------------------------------------------------------------
//      デフォルトのサンプラー作成処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateSmp( ID3D11Device* pDevice )
{
    D3D11_SAMPLER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_SAMPLER_DESC ) );
    desc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    desc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.MinLOD         = 0;
    desc.MaxLOD         = D3D11_FLOAT32_MAX;

    HRESULT hr = S_OK;
    hr = pDevice->CreateSamplerState( &desc, &m_pDiffuseSmp );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateSamplerState() Failed." );
        return false;
    }
    SetDebugObjectName( m_pDiffuseSmp, "asdx::Mesh" );

    hr = pDevice->CreateSamplerState( &desc, &m_pSpecularSmp );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateSamplerState() Failed." );
        return false;
    }
    SetDebugObjectName( m_pSpecularSmp, "asdx::Mesh" );

    hr = pDevice->CreateSamplerState( &desc, &m_pNormalSmp );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateSamplerState() Failed." );
        return false;
    }
    SetDebugObjectName( m_pNormalSmp, "asdx::Mesh" );

    return true;
}

//-------------------------------------------------------------------------------------------
//      デフォルトのサブセット作成処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateSubset( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    // メモリを確保.
    m_pSubset = new Mesh::Subset[ mesh.GetSubsetCount() ];
    if ( m_pSubset == nullptr )
    {
        ELOG( "Error : Memory Allocate Failed." );
        return false;
    }

    // サブセット数を設定.
    m_SubsetCount = mesh.GetSubsetCount();

    for( asdx::UINT i=0; i<mesh.GetSubsetCount(); ++i )
    {
        asdx::ResMesh::Subset subset = mesh.GetSubset( i );

        // サブセットを設定.
        m_pSubset[ i ].IndexCount  = subset.IndexCount;
        m_pSubset[ i ].IndexOffset = subset.IndexOffset;
        m_pSubset[ i ].MaterialID  = subset.MaterialID;
    }

    return true;
}

//-------------------------------------------------------------------------------------------
//      デフォルトのマテリアル作成処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateMaterial( ID3D11Device* pDevice, const asdx::ResMesh& mesh, const char* resPath, const char* dummyPath )
{
    // マテリアルのメモリを確保.
    m_pMaterial = new Mesh::Material[ mesh.GetMaterialCount() ];
    if ( m_pMaterial == nullptr )
    {
        ELOG( "Error : Memory Allocate Failed." );
        return false;
    }

    // マテリアル数を設定.
    m_MaterialCount = mesh.GetMaterialCount();

    for( asdx::UINT i=0; i<mesh.GetMaterialCount(); ++i )
    {
        ResMesh::Material material = mesh.GetMaterial( i );

        // マテリアルバッファの設定.
        m_pMaterial[ i ].Buffer.Diffuse  = material.Diffuse;
        m_pMaterial[ i ].Buffer.Alpha    = material.Alpha;
        m_pMaterial[ i ].Buffer.Specular = material.Specular;
        m_pMaterial[ i ].Buffer.Power    = material.Power;

        // ディフューズマップ.
        if( strlen( material.DiffuseMap ) != 0 )
        {
            std::string filepath( resPath );
            filepath += material.DiffuseMap;

            // ディフューズマップを生成.
            if ( !m_pMaterial[ i ].TextureMap.Diffuse.CreateFromFile( pDevice, filepath.c_str() ) )
            {
                ELOG( "Error : Texture::CreateFromFile() Failed. filename = %s", filepath.c_str() );
                filepath.clear();
                return false;
            }
            filepath.clear();
        }
        else
        {
            std::string filepath( dummyPath );
            filepath += "dummyDiffuse.map";

            // ダミーテクスチャを設定.
            if ( !m_pMaterial[ i ].TextureMap.Diffuse.CreateFromFile( pDevice, filepath.c_str() ) )
            {
                ELOG( "Error : Texture::CreateFromFile() Failed. filename = %s", filepath.c_str() );
                filepath.clear();
                return false;
            }
        }

        // スペキュラーマップ.
        if ( strlen( material.SpecularMap ) != 0 )
        {
            std::string filepath( resPath );
            filepath += material.SpecularMap;

            // スペキュラーマップを生成.
            if ( !m_pMaterial[ i ].TextureMap.Specular.CreateFromFile( pDevice, filepath.c_str() ) )
            {
                ELOG( "Error : Texture::CreateFromFile() Failed. filename = %s", filepath.c_str() );
                filepath.clear();
                return false;
            }
        }
        else
        {
            std::string filepath( dummyPath );
            filepath += "dummySpecular.map";

            // ダミーテクスチャを設定.
            if ( !m_pMaterial[ i ].TextureMap.Specular.CreateFromFile( pDevice, filepath.c_str() ) )
            {
                ELOG( "Error : Texture::CreateFromFile() Failed. filename = %s", filepath.c_str() );
                filepath.clear();
                return false;
            }
        }

        // 法線マップ.
        if ( strlen( material.BumpMap ) != 0 )
        {
            std::string filepath( resPath );
            filepath += material.BumpMap;

            // 法線マップを生成.
            if ( !m_pMaterial[ i ].TextureMap.Normal.CreateFromFile( pDevice, filepath.c_str() ) )
            {
                ELOG( "Error : Texture::CreateFromFile() Failed. filename = %s", filepath.c_str() );
                filepath.clear();
                return false;
            }
        }
        else
        {
            std::string filepath( dummyPath );
            filepath += "dummyBump.map";

            // ダミーテクスチャを設定.
            if ( !m_pMaterial[ i ].TextureMap.Normal.CreateFromFile( pDevice, filepath.c_str() ) )
            {
                ELOG( "Error : Texture::CreateFromFile() Failed. filename = %s", filepath.c_str() );
                filepath.clear();
                return false;
            }
            filepath.clear();
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------
//      デフォルトのマテリアル解放処理です.
//-------------------------------------------------------------------------------------------
void Mesh::DefOnTermMaterial()
{
    if ( m_pMaterial )
    {
        for( asdx::UINT i=0; i<m_MaterialCount; ++i )
        {
            // テクスチャを解放.
            m_pMaterial[ i ].TextureMap.Diffuse.Release();
            m_pMaterial[ i ].TextureMap.Specular.Release();
            m_pMaterial[ i ].TextureMap.Normal.Release();
        }

        //　メモリを解放.
        delete [] m_pMaterial;
        m_pMaterial = nullptr;
    }

    m_MaterialCount = 0;
}

//-------------------------------------------------------------------------------------------
//      デフォルトの描画開始処理です.
//-------------------------------------------------------------------------------------------
void Mesh::DefOnDrawBegin( ID3D11DeviceContext* pDeviceContext )
{
    // 頂点バッファを設定.
    pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, &m_Stride, &m_Offset );

    // 入力レイアウトを設定.
    pDeviceContext->IASetInputLayout( m_pIL );

    // インデックスバッファを設定.
    pDeviceContext->IASetIndexBuffer( m_pIB, DXGI_FORMAT_R32_UINT, 0 );

    // プリミティブトポロジーを設定.
    pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

//-------------------------------------------------------------------------------------------
//      デフォルトのサブセット描画処理です.
//-------------------------------------------------------------------------------------------
void Mesh::DefOnDrawSubset( ID3D11DeviceContext* pDeviceContext, const asdx::UINT index )
{
    // マテリアルID取得.
    asdx::UINT id = m_pSubset[ index ].MaterialID;

    // マテリアルバッファを更新.
    pDeviceContext->UpdateSubresource( m_pMB, 0, nullptr, &m_pMaterial[ id ].Buffer, 0, 0 );

    // ピクセルシェーダに定数バッファを設定.
    pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pMB );

    ID3D11ShaderResourceView* pDiffuseSRV  = m_pMaterial[ id ].TextureMap.Diffuse.GetSRV();
    ID3D11ShaderResourceView* pSpecularSRV = m_pMaterial[ id ].TextureMap.Specular.GetSRV();
    ID3D11ShaderResourceView* pNormalSRV   = m_pMaterial[ id ].TextureMap.Normal.GetSRV();

    // ディフューズマップを設定.
    pDeviceContext->PSSetShaderResources( TEXTURE_SLOT_DIFFUSE, 1, &pDiffuseSRV );
    pDeviceContext->PSSetSamplers( TEXTURE_SLOT_DIFFUSE, 1, &m_pDiffuseSmp );

    // スペキュラーマップを設定.
    pDeviceContext->PSSetShaderResources( TEXTURE_SLOT_SPECULAR, 1, &pSpecularSRV );
    pDeviceContext->PSSetSamplers( TEXTURE_SLOT_SPECULAR, 1, &m_pSpecularSmp );

    // 法線マップを設定.
    pDeviceContext->PSSetShaderResources( TEXTURE_SLOT_NORMAL, 1, &pNormalSRV );
    pDeviceContext->PSSetSamplers( TEXTURE_SLOT_NORMAL, 1, &m_pNormalSmp );

    // インデックス付きで描画.
    pDeviceContext->DrawIndexed( m_pSubset[ index ].IndexCount, m_pSubset[ index ].IndexOffset, 0 );

}

//-------------------------------------------------------------------------------------------
//      デフォルトの描画終了処理です.
//-------------------------------------------------------------------------------------------
void Mesh::DefOnDrawEnd( ID3D11DeviceContext* pDeviceContext )
{
    /* DO_NOTHING */
}

//-------------------------------------------------------------------------------------------
//      入力レイアウト生成時の処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateIL( ID3D11Device* pDevice, const void* shaderByteCode, const asdx::UINT byteCodeLength )
{
    return DefOnCreateIL( pDevice, shaderByteCode, byteCodeLength );
}

//-------------------------------------------------------------------------------------------
//      頂点バッファ生成時の処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateVB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    return DefOnCreateVB( pDevice, mesh );
}

//-------------------------------------------------------------------------------------------
//      インデックスバッファ生成時の処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateIB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    return DefOnCreateIB( pDevice, mesh );
}

//-------------------------------------------------------------------------------------------
//      マテリアルバッファ生成時の処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateMB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    return DefOnCreateMB( pDevice, mesh );
}

//-------------------------------------------------------------------------------------------
//      サンプラーステート生成時の処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateSmp( ID3D11Device* pDevice )
{
    return DefOnCreateSmp( pDevice );
}

//-------------------------------------------------------------------------------------------
//      サブセット生成時の処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateSubset( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    return DefOnCreateSubset( pDevice, mesh );
}

//-------------------------------------------------------------------------------------------
//      マテリアル生成時の処理です.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateMaterial( ID3D11Device* pDevice, const asdx::ResMesh& mesh, const char* resPath, const char* dummyPath )
{
    return DefOnCreateMaterial( pDevice, mesh, resPath, dummyPath );
}

//-------------------------------------------------------------------------------------------
//      描画開始時の処理です.
//-------------------------------------------------------------------------------------------
void Mesh::OnDrawBegin( ID3D11DeviceContext* pDeviceContext )
{
    DefOnDrawBegin( pDeviceContext );
}

//-------------------------------------------------------------------------------------------
//      サブセット描画時の処理です.
//-------------------------------------------------------------------------------------------
void Mesh::OnDrawSubset( ID3D11DeviceContext* pDeviceContext, const asdx::UINT index )
{
    DefOnDrawSubset( pDeviceContext, index );
}

//-------------------------------------------------------------------------------------------
//      描画終了時の処理です.
//-------------------------------------------------------------------------------------------
void Mesh::OnDrawEnd( ID3D11DeviceContext* pDeviceContext )
{
    DefOnDrawEnd( pDeviceContext );
}

//-------------------------------------------------------------------------------------------
//      マテリアル解放時の処理です.
//-------------------------------------------------------------------------------------------
void Mesh::OnTermMaterial()
{
    DefOnTermMaterial();
}

} // namespace asdx
