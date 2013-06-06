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
//      �R���X�g���N�^�ł�.
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
//      �f�X�g���N�^�ł�.
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
//      �����������ł�.
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
    // ���̓��C�A�E�g�𐶐�.
    if ( !OnCreateIL( pDevice, pShaderByteCode, byteCodeLength ) )
    {
        ELOG( "Error : Mesh::OnCreateIL() Failed." );
        return false;
    }

    // ���_�o�b�t�@�𐶐�.
    if ( !OnCreateVB( pDevice, mesh ) )
    {
        ELOG( "Error : Mesh::OnCreateVB() Failed." );
        return false;
    }

    // �C���f�b�N�X�o�b�t�@�𐶐�.
    if ( !OnCreateIB( pDevice, mesh ) )
    {
        ELOG( "Error : Mesh::OnCreateIB() Failed." );
        return false;
    }

    // �T�u�Z�b�g�𐶐�.
    if ( !OnCreateSubset( pDevice, mesh ) )
    {
        ELOG( "Error : Mesh::OnCreateSubset() Failed." );
        return false;
    }

    // �}�e���A���𐶐�.
    if ( !OnCreateMaterial( pDevice, mesh, resFolderPath, dummyFolderPath ) )
    {
        ELOG( "Error : Mesh::OnCreateMaterial() Failed." );
        return false;
    }

    // �}�e���A���o�b�t�@�𐶐�.
    if ( !OnCreateMB( pDevice, mesh ) )
    {
        ELOG( "Error : Mesh::OnCreateMB() Failed." );
        return false;
    }

    // �T���v���[�X�e�[�g�𐶐�.
    if ( !OnCreateSmp( pDevice ) )
    {
        ELOG( "Error : Mesh::OnCreateSmp() Failed." );
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------
//      �I�������ł�.
//-------------------------------------------------------------------------------------------
void Mesh::Term()
{
    // ���_�o�b�t�@��j��.
    if ( m_pVB )
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    // �C���f�b�N�X�o�b�t�@��j��.
    if ( m_pIB )
    {
        m_pIB->Release();
        m_pIB = nullptr;
    }

    // �}�e���A���o�b�t�@��j��.
    if ( m_pMB )
    {
        m_pMB->Release();
        m_pMB = nullptr;
    }

    // ���̓��C�A�E�g��j��.
    if ( m_pIL )
    {
        m_pIL->Release();
        m_pIL = nullptr;
    }

    // �f�B�t���[�Y�}�b�v�̃T���v���[�X�e�[�g��j��.
    if ( m_pDiffuseSmp )
    {
        m_pDiffuseSmp->Release();
        m_pDiffuseSmp = nullptr;
    }

    // �X�y�L�����[�}�b�v�̃T���v���[�X�e�[�g��j��.
    if ( m_pSpecularSmp )
    {
        m_pSpecularSmp->Release();
        m_pSpecularSmp = nullptr;
    }

    // �@���}�b�v�̃T���v���[�X�e�[�g��j��.
    if ( m_pNormalSmp )
    {
        m_pNormalSmp->Release();
        m_pNormalSmp = nullptr;
    }

    // �T�u�Z�b�g��j��.
    if ( m_pSubset )
    {
        delete [] m_pSubset;
        m_pSubset = nullptr;
    }

    // �}�e���A�������.
    OnTermMaterial();

    m_SubsetCount = 0;
    m_Stride      = 0;
    m_Offset      = 0;

}

//-------------------------------------------------------------------------------------------
//      �`�揈���ł�.
//-------------------------------------------------------------------------------------------
void Mesh::Draw( ID3D11DeviceContext* pDeviceContext )
{
    // �`��J�n�����ł�.
    OnDrawBegin( pDeviceContext );

    for( asdx::UINT i=0; i<m_SubsetCount; ++i )
    {
        // �T�u�Z�b�g�̕`�揈���ł�.
        OnDrawSubset( pDeviceContext, i );
    }
    
    // �`��I�������ł�.
    OnDrawEnd( pDeviceContext );
}

//-------------------------------------------------------------------------------------------
//      �f�t�H���g�̓��̓��C�A�E�g�쐬�����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateIL
(
    ID3D11Device*    pDevice,
    const void*      shaderByteCode,
    const asdx::UINT byteCodeLength
)
{
    // ���̓��C�A�E�g�𐶐�.
    HRESULT hr = pDevice->CreateInputLayout( 
        asdx::ResMesh::INPUT_ELEMENTS,
        asdx::ResMesh::NUM_INPUT_ELEMENT,
        shaderByteCode,
        byteCodeLength,
        &m_pIL );

    // �`�F�b�N.
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D11Device::CreateInputLayout() Failed." );
        return false;
    }
    SetDebugObjectName( m_pIL, "asdx::Mesh" );

    // ����I��.
    return true;
}

//-------------------------------------------------------------------------------------------
//      �f�t�H���g�̒��_�o�b�t�@�쐬�����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateVB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    // �o�b�t�@�̐ݒ�.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage          = D3D11_USAGE_DEFAULT;
    desc.ByteWidth      = sizeof( asdx::ResMesh::Vertex ) * mesh.GetVertexCount();
    desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;

    // �X�g���C�h�ƃI�t�Z�b�g��ݒ�.
    m_Stride = sizeof( asdx::ResMesh::Vertex );
    m_Offset = 0;

    // ���\�[�X�̐ݒ�.
    D3D11_SUBRESOURCE_DATA res;
    ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
    res.pSysMem = mesh.GetVertices();

    // ���_�o�b�t�@�̐���.
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
//      �f�t�H���g�̃C���f�b�N�X�o�b�t�@�쐬�����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateIB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    // �o�b�t�@�̐ݒ�.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage          = D3D11_USAGE_DEFAULT;
    desc.ByteWidth      = sizeof( asdx::UINT ) * mesh.GetIndexCount();
    desc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;

    // ���\�[�X�f�[�^�̐ݒ�.
    D3D11_SUBRESOURCE_DATA res;
    ZeroMemory( &res, sizeof( D3D11_SUBRESOURCE_DATA ) );
    res.pSysMem = mesh.GetIndices();

    // �C���f�b�N�X�o�b�t�@�𐶐�.
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
//      �f�t�H���g�̃}�e���A���o�b�t�@�쐬�����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateMB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    // �o�b�t�@�̐ݒ�.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage          = D3D11_USAGE_DEFAULT;
    desc.ByteWidth      = sizeof( Mesh::MaterialBuffer );
    desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;

    // �}�e���A���o�b�t�@�𐶐�.
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
//      �f�t�H���g�̃T���v���[�쐬�����ł�.
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
//      �f�t�H���g�̃T�u�Z�b�g�쐬�����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateSubset( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    // ���������m��.
    m_pSubset = new Mesh::Subset[ mesh.GetSubsetCount() ];
    if ( m_pSubset == nullptr )
    {
        ELOG( "Error : Memory Allocate Failed." );
        return false;
    }

    // �T�u�Z�b�g����ݒ�.
    m_SubsetCount = mesh.GetSubsetCount();

    for( asdx::UINT i=0; i<mesh.GetSubsetCount(); ++i )
    {
        asdx::ResMesh::Subset subset = mesh.GetSubset( i );

        // �T�u�Z�b�g��ݒ�.
        m_pSubset[ i ].IndexCount  = subset.IndexCount;
        m_pSubset[ i ].IndexOffset = subset.IndexOffset;
        m_pSubset[ i ].MaterialID  = subset.MaterialID;
    }

    return true;
}

//-------------------------------------------------------------------------------------------
//      �f�t�H���g�̃}�e���A���쐬�����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::DefOnCreateMaterial( ID3D11Device* pDevice, const asdx::ResMesh& mesh, const char* resPath, const char* dummyPath )
{
    // �}�e���A���̃��������m��.
    m_pMaterial = new Mesh::Material[ mesh.GetMaterialCount() ];
    if ( m_pMaterial == nullptr )
    {
        ELOG( "Error : Memory Allocate Failed." );
        return false;
    }

    // �}�e���A������ݒ�.
    m_MaterialCount = mesh.GetMaterialCount();

    for( asdx::UINT i=0; i<mesh.GetMaterialCount(); ++i )
    {
        ResMesh::Material material = mesh.GetMaterial( i );

        // �}�e���A���o�b�t�@�̐ݒ�.
        m_pMaterial[ i ].Buffer.Diffuse  = material.Diffuse;
        m_pMaterial[ i ].Buffer.Alpha    = material.Alpha;
        m_pMaterial[ i ].Buffer.Specular = material.Specular;
        m_pMaterial[ i ].Buffer.Power    = material.Power;

        // �f�B�t���[�Y�}�b�v.
        if( strlen( material.DiffuseMap ) != 0 )
        {
            std::string filepath( resPath );
            filepath += material.DiffuseMap;

            // �f�B�t���[�Y�}�b�v�𐶐�.
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

            // �_�~�[�e�N�X�`����ݒ�.
            if ( !m_pMaterial[ i ].TextureMap.Diffuse.CreateFromFile( pDevice, filepath.c_str() ) )
            {
                ELOG( "Error : Texture::CreateFromFile() Failed. filename = %s", filepath.c_str() );
                filepath.clear();
                return false;
            }
        }

        // �X�y�L�����[�}�b�v.
        if ( strlen( material.SpecularMap ) != 0 )
        {
            std::string filepath( resPath );
            filepath += material.SpecularMap;

            // �X�y�L�����[�}�b�v�𐶐�.
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

            // �_�~�[�e�N�X�`����ݒ�.
            if ( !m_pMaterial[ i ].TextureMap.Specular.CreateFromFile( pDevice, filepath.c_str() ) )
            {
                ELOG( "Error : Texture::CreateFromFile() Failed. filename = %s", filepath.c_str() );
                filepath.clear();
                return false;
            }
        }

        // �@���}�b�v.
        if ( strlen( material.BumpMap ) != 0 )
        {
            std::string filepath( resPath );
            filepath += material.BumpMap;

            // �@���}�b�v�𐶐�.
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

            // �_�~�[�e�N�X�`����ݒ�.
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
//      �f�t�H���g�̃}�e���A����������ł�.
//-------------------------------------------------------------------------------------------
void Mesh::DefOnTermMaterial()
{
    if ( m_pMaterial )
    {
        for( asdx::UINT i=0; i<m_MaterialCount; ++i )
        {
            // �e�N�X�`�������.
            m_pMaterial[ i ].TextureMap.Diffuse.Release();
            m_pMaterial[ i ].TextureMap.Specular.Release();
            m_pMaterial[ i ].TextureMap.Normal.Release();
        }

        //�@�����������.
        delete [] m_pMaterial;
        m_pMaterial = nullptr;
    }

    m_MaterialCount = 0;
}

//-------------------------------------------------------------------------------------------
//      �f�t�H���g�̕`��J�n�����ł�.
//-------------------------------------------------------------------------------------------
void Mesh::DefOnDrawBegin( ID3D11DeviceContext* pDeviceContext )
{
    // ���_�o�b�t�@��ݒ�.
    pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, &m_Stride, &m_Offset );

    // ���̓��C�A�E�g��ݒ�.
    pDeviceContext->IASetInputLayout( m_pIL );

    // �C���f�b�N�X�o�b�t�@��ݒ�.
    pDeviceContext->IASetIndexBuffer( m_pIB, DXGI_FORMAT_R32_UINT, 0 );

    // �v���~�e�B�u�g�|���W�[��ݒ�.
    pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

//-------------------------------------------------------------------------------------------
//      �f�t�H���g�̃T�u�Z�b�g�`�揈���ł�.
//-------------------------------------------------------------------------------------------
void Mesh::DefOnDrawSubset( ID3D11DeviceContext* pDeviceContext, const asdx::UINT index )
{
    // �}�e���A��ID�擾.
    asdx::UINT id = m_pSubset[ index ].MaterialID;

    // �}�e���A���o�b�t�@���X�V.
    pDeviceContext->UpdateSubresource( m_pMB, 0, nullptr, &m_pMaterial[ id ].Buffer, 0, 0 );

    // �s�N�Z���V�F�[�_�ɒ萔�o�b�t�@��ݒ�.
    pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pMB );

    ID3D11ShaderResourceView* pDiffuseSRV  = m_pMaterial[ id ].TextureMap.Diffuse.GetSRV();
    ID3D11ShaderResourceView* pSpecularSRV = m_pMaterial[ id ].TextureMap.Specular.GetSRV();
    ID3D11ShaderResourceView* pNormalSRV   = m_pMaterial[ id ].TextureMap.Normal.GetSRV();

    // �f�B�t���[�Y�}�b�v��ݒ�.
    pDeviceContext->PSSetShaderResources( TEXTURE_SLOT_DIFFUSE, 1, &pDiffuseSRV );
    pDeviceContext->PSSetSamplers( TEXTURE_SLOT_DIFFUSE, 1, &m_pDiffuseSmp );

    // �X�y�L�����[�}�b�v��ݒ�.
    pDeviceContext->PSSetShaderResources( TEXTURE_SLOT_SPECULAR, 1, &pSpecularSRV );
    pDeviceContext->PSSetSamplers( TEXTURE_SLOT_SPECULAR, 1, &m_pSpecularSmp );

    // �@���}�b�v��ݒ�.
    pDeviceContext->PSSetShaderResources( TEXTURE_SLOT_NORMAL, 1, &pNormalSRV );
    pDeviceContext->PSSetSamplers( TEXTURE_SLOT_NORMAL, 1, &m_pNormalSmp );

    // �C���f�b�N�X�t���ŕ`��.
    pDeviceContext->DrawIndexed( m_pSubset[ index ].IndexCount, m_pSubset[ index ].IndexOffset, 0 );

}

//-------------------------------------------------------------------------------------------
//      �f�t�H���g�̕`��I�������ł�.
//-------------------------------------------------------------------------------------------
void Mesh::DefOnDrawEnd( ID3D11DeviceContext* pDeviceContext )
{
    /* DO_NOTHING */
}

//-------------------------------------------------------------------------------------------
//      ���̓��C�A�E�g�������̏����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateIL( ID3D11Device* pDevice, const void* shaderByteCode, const asdx::UINT byteCodeLength )
{
    return DefOnCreateIL( pDevice, shaderByteCode, byteCodeLength );
}

//-------------------------------------------------------------------------------------------
//      ���_�o�b�t�@�������̏����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateVB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    return DefOnCreateVB( pDevice, mesh );
}

//-------------------------------------------------------------------------------------------
//      �C���f�b�N�X�o�b�t�@�������̏����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateIB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    return DefOnCreateIB( pDevice, mesh );
}

//-------------------------------------------------------------------------------------------
//      �}�e���A���o�b�t�@�������̏����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateMB( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    return DefOnCreateMB( pDevice, mesh );
}

//-------------------------------------------------------------------------------------------
//      �T���v���[�X�e�[�g�������̏����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateSmp( ID3D11Device* pDevice )
{
    return DefOnCreateSmp( pDevice );
}

//-------------------------------------------------------------------------------------------
//      �T�u�Z�b�g�������̏����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateSubset( ID3D11Device* pDevice, const asdx::ResMesh& mesh )
{
    return DefOnCreateSubset( pDevice, mesh );
}

//-------------------------------------------------------------------------------------------
//      �}�e���A���������̏����ł�.
//-------------------------------------------------------------------------------------------
bool Mesh::OnCreateMaterial( ID3D11Device* pDevice, const asdx::ResMesh& mesh, const char* resPath, const char* dummyPath )
{
    return DefOnCreateMaterial( pDevice, mesh, resPath, dummyPath );
}

//-------------------------------------------------------------------------------------------
//      �`��J�n���̏����ł�.
//-------------------------------------------------------------------------------------------
void Mesh::OnDrawBegin( ID3D11DeviceContext* pDeviceContext )
{
    DefOnDrawBegin( pDeviceContext );
}

//-------------------------------------------------------------------------------------------
//      �T�u�Z�b�g�`�掞�̏����ł�.
//-------------------------------------------------------------------------------------------
void Mesh::OnDrawSubset( ID3D11DeviceContext* pDeviceContext, const asdx::UINT index )
{
    DefOnDrawSubset( pDeviceContext, index );
}

//-------------------------------------------------------------------------------------------
//      �`��I�����̏����ł�.
//-------------------------------------------------------------------------------------------
void Mesh::OnDrawEnd( ID3D11DeviceContext* pDeviceContext )
{
    DefOnDrawEnd( pDeviceContext );
}

//-------------------------------------------------------------------------------------------
//      �}�e���A��������̏����ł�.
//-------------------------------------------------------------------------------------------
void Mesh::OnTermMaterial()
{
    DefOnTermMaterial();
}

} // namespace asdx
