//--------------------------------------------------------------------------------------------
// File : asdxResMesh.cpp
// Desc : Resource Mesh Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------
#include <asdxResMesh.h>
#include <cassert>
#include <cstdio>


namespace asdx {

//--------------------------------------------------------------------------------------------
// Constant Values
//--------------------------------------------------------------------------------------------

// ���͗v�f�ł�.
const D3D11_INPUT_ELEMENT_DESC ResMesh::INPUT_ELEMENTS[ ResMesh::NUM_INPUT_ELEMENT ] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// ���݂�MSH�t�@�C���̃o�[�W�����ԍ��ł�.
static const unsigned int MSH_CURRENT_VERSION = 0x00000001;


/////////////////////////////////////////////////////////////////////////////////////////////
// MSH_DATA_HEADER structure
//! @brief  MSH�t�@�C���̃f�[�^�w�b�_�[�ł�.
/////////////////////////////////////////////////////////////////////////////////////////////
struct MSH_DATA_HEADER
{
    unsigned int NumVertices;               //!< ���_���ł�.
    unsigned int NumIndices;                //!< �C���f�b�N�X���ł�.
    unsigned int NumMaterials;              //!< �}�e���A�����ł�.
    unsigned int NumSubsets;                //!< �T�u�Z�b�g���ł�.
    unsigned int VertexStructureSize;       //!< ���_�f�[�^�\���̂̃T�C�Y�ł�.
    unsigned int IndexStructureSize;        //!< ���_�C���f�b�N�X�f�[�^�̍\���̂ł�.
    unsigned int MaterialStructureSize;     //!< �}�e���A���f�[�^�\���̂̃T�C�Y�ł�.
    unsigned int SubsetStructureSize;       //!< �T�u�Z�b�g�f�[�^�\���̂̃T�C�Y�ł�.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MSH_FILE_HEADER structure
//! @brief  MSH�t�@�C���̃t�@�C���w�b�_�[�ł�.
/////////////////////////////////////////////////////////////////////////////////////////////
struct MSH_FILE_HEADER
{
    unsigned char   Magic[ 4 ];         //!< �}�W�b�N�ł�( 'M, 'S', 'H', '\0' )
    unsigned int    Version;            //!< �t�@�C���o�[�W�����ł�.
    unsigned int    DataHeaderSize;     //!< �f�[�^�w�b�_�[�\���̂̃T�C�Y�ł�.
    MSH_DATA_HEADER DataHeader;         //!< �f�[�^�w�b�_�[�ł�.
};


/////////////////////////////////////////////////////////////////////////////////////////////
// ResMesh class 
/////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
ResMesh::ResMesh()
: m_VertexCount     ( 0 )
, m_IndexCount      ( 0 )
, m_MaterialCount   ( 0 )
, m_SubsetCount     ( 0 )
, m_pVertex         ( nullptr )
, m_pIndex          ( nullptr )
, m_pMaterial       ( nullptr )
, m_pSubset         ( nullptr )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------
//      �R�s�[�R���X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
ResMesh::ResMesh( const ResMesh& ResMesh )
: m_VertexCount     ( ResMesh.m_VertexCount )
, m_IndexCount      ( ResMesh.m_IndexCount )
, m_MaterialCount   ( ResMesh.m_MaterialCount )
, m_SubsetCount     ( ResMesh.m_SubsetCount )
{
    // ���_�̃��������m��.
    m_pVertex = new ResMesh::Vertex[ m_VertexCount ];
    assert( m_pVertex != nullptr );

    // ���_�C���f�b�N�X�̃��������m��.
    m_pIndex = new ResMesh::Index[ m_IndexCount ];
    assert( m_pIndex != nullptr );

    // �}�e���A���̃��������m��.
    m_pMaterial = new ResMesh::Material[ m_MaterialCount ];
    assert( m_pMaterial != nullptr );

    // �T�u�Z�b�g�̃��������m��.
    m_pSubset = new ResMesh::Subset[ m_SubsetCount ];
    assert( m_pSubset != nullptr );

    // ���_�f�[�^���R�s�[.
    memcpy( m_pVertex, ResMesh.m_pVertex, sizeof( ResMesh::Vertex ) * ResMesh.m_VertexCount );

    // ���_�C���f�b�N�X�f�[�^���R�s�[.
    memcpy( m_pIndex, ResMesh.m_pIndex, sizeof( ResMesh::Index ) * ResMesh.m_IndexCount );

    // �}�e���A���f�[�^���R�s�[.
    memcpy( m_pMaterial, ResMesh.m_pMaterial, sizeof( ResMesh::Material ) * ResMesh.m_MaterialCount );

    // �T�u�Z�b�g�f�[�^���R�s�[.
    memcpy( m_pSubset, ResMesh.m_pSubset, sizeof( ResMesh::Subset ) * ResMesh.m_SubsetCount );
}

//-------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
ResMesh::~ResMesh()
{
    // �����������.
    Release(); 
}

//-------------------------------------------------------------------------------------------
//      ��������������܂�.
//-------------------------------------------------------------------------------------------
void ResMesh::Release()
{
    // ���_�f�[�^�̃����������.
    if ( m_pVertex )
    {
        delete [] m_pVertex;
        m_pVertex = nullptr;
    }

    // ���_�C���f�b�N�X�f�[�^�̃����������.
    if ( m_pIndex )
    {
        delete [] m_pIndex;
        m_pIndex = nullptr;
    }

    // �}�e���A���f�[�^�̃����������.
    if ( m_pMaterial )
    {
        delete [] m_pMaterial;
        m_pMaterial = nullptr;
    }

    // �T�u�Z�b�g�f�[�^�̃����������.
    if ( m_pSubset )
    {
        delete [] m_pSubset;
        m_pSubset = nullptr;
    }

    // �J�E���g�������Z�b�g.
    m_VertexCount   = 0;
    m_IndexCount    = 0;
    m_MaterialCount = 0;
    m_SubsetCount   = 0;
}

//--------------------------------------------------------------------------------------------
//      ������Z�q�ł�.
//--------------------------------------------------------------------------------------------
ResMesh& ResMesh::operator = ( const ResMesh& ResMesh )
{
    // ��������������Ă���.
    Release();

    // �J�E���g����ݒ�.
    m_VertexCount   = ResMesh.m_VertexCount;
    m_IndexCount    = ResMesh.m_IndexCount;
    m_MaterialCount = ResMesh.m_MaterialCount;
    m_SubsetCount   = ResMesh.m_SubsetCount;

    // ���_�f�[�^�̃��������m��.
    m_pVertex = new ResMesh::Vertex[ m_VertexCount ];
    assert( m_pVertex != nullptr );

    // ���_�C���f�b�N�X�f�[�^�̃��������m��.
    m_pIndex = new ResMesh::Index[ m_IndexCount ];
    assert( m_pIndex != nullptr );

    // �}�e���A���f�[�^�̃��������m��.
    m_pMaterial = new ResMesh::Material[ m_MaterialCount ];
    assert( m_pMaterial != nullptr );

    // �T�u�Z�b�g�f�[�^�̃��������m��.
    m_pSubset = new ResMesh::Subset[ m_SubsetCount ];
    assert( m_pSubset != nullptr );

    // ���_�f�[�^���R�s�[.
    memcpy( m_pVertex, ResMesh.m_pVertex, sizeof( ResMesh::Vertex ) * ResMesh.m_VertexCount );

    // ���_�C���f�b�N�X�f�[�^���R�s�[.
    memcpy( m_pIndex, ResMesh.m_pIndex, sizeof( ResMesh::Index ) * ResMesh.m_IndexCount );

    // �}�e���A���f�[�^���R�s�[.
    memcpy( m_pMaterial, ResMesh.m_pMaterial, sizeof( ResMesh::Material ) * ResMesh.m_MaterialCount );

    // �T�u�Z�b�g�f�[�^���R�s�[.
    memcpy( m_pSubset, ResMesh.m_pSubset, sizeof( ResMesh::Subset ) * ResMesh.m_SubsetCount );

    return (*this);
}

//-------------------------------------------------------------------------------------------
//      ���_�����擾���܂�.
//-------------------------------------------------------------------------------------------
asdx::UINT ResMesh::GetVertexCount() const
{ return m_VertexCount; }

//-------------------------------------------------------------------------------------------
//      ���_�C���f�b�N�X�����擾���܂�.
//-------------------------------------------------------------------------------------------
asdx::UINT ResMesh::GetIndexCount() const
{ return m_IndexCount; }

//-------------------------------------------------------------------------------------------
//      �}�e���A�������擾���܂�.
//-------------------------------------------------------------------------------------------
asdx::UINT ResMesh::GetMaterialCount() const
{ return m_MaterialCount; }

//-------------------------------------------------------------------------------------------
//      �T�u�Z�b�g�����擾���܂�.
//-------------------------------------------------------------------------------------------
asdx::UINT ResMesh::GetSubsetCount() const
{ return m_SubsetCount; }

//-------------------------------------------------------------------------------------------
//      �w�肳�ꂽ���_�f�[�^���擾���܂�.
//-------------------------------------------------------------------------------------------
asdx::ResMesh::Vertex ResMesh::GetVertex( const asdx::UINT idx ) const
{
    assert( idx < m_VertexCount );
    return m_pVertex[ idx ];
}

//-------------------------------------------------------------------------------------------
//      �w�肳�ꂽ���_�C���f�b�N�X���擾���܂�.
//-------------------------------------------------------------------------------------------
asdx::ResMesh::Index  ResMesh::GetIndex( const asdx::UINT idx ) const
{
    assert( idx < m_IndexCount );
    return m_pIndex[ idx ];
}

//-------------------------------------------------------------------------------------------
//      �w�肳�ꂽ�}�e���A�����擾���܂�.
//-------------------------------------------------------------------------------------------
asdx::ResMesh::Material ResMesh::GetMaterial( const asdx::UINT idx ) const
{
    assert( idx < m_MaterialCount );
    return m_pMaterial[ idx ];
}

//-------------------------------------------------------------------------------------------
//      �w�肳�ꂽ�T�u�Z�b�g���擾���܂�.
//-------------------------------------------------------------------------------------------
asdx::ResMesh::Subset ResMesh::GetSubset( const asdx::UINT idx ) const
{
    assert( idx < m_SubsetCount );
    return m_pSubset[ idx ];
}

//-------------------------------------------------------------------------------------------
//      ���_�f�[�^���擾���܂�.
//-------------------------------------------------------------------------------------------
const asdx::ResMesh::Vertex* ResMesh::GetVertices() const
{ return m_pVertex; }

//-------------------------------------------------------------------------------------------
//      ���_�C���f�b�N�X���擾���܂�.
//-------------------------------------------------------------------------------------------
const asdx::ResMesh::Index* ResMesh::GetIndices() const
{ return m_pIndex; }

//-------------------------------------------------------------------------------------------
//      �}�e���A�����擾���܂�.
//-------------------------------------------------------------------------------------------
const asdx::ResMesh::Material* ResMesh::GetMaterials() const
{ return m_pMaterial; }

//-------------------------------------------------------------------------------------------
//      �T�u�Z�b�g���擾���܂�.
//-------------------------------------------------------------------------------------------
const asdx::ResMesh::Subset* ResMesh::GetSubsets() const
{ return m_pSubset; }

//-------------------------------------------------------------------------------------------
//      �t�@�C������f�[�^�����[�h���܂�.
//-------------------------------------------------------------------------------------------
bool ResMesh::LoadFromFile( const char* filename )
{
    FILE* pFile;

    // �t�@�C�����J��.
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        printf_s( "Error : LoadFromFile() Failed. filename = %s\n", filename );
        return false;
    }

    // �t�@�C���w�b�_�[��ǂݎ��.
    MSH_FILE_HEADER header;
    fread( &header, sizeof( MSH_FILE_HEADER ), 1, pFile );

    // �}�W�b�N���`�F�b�N.
    if ( ( header.Magic[ 0 ] != 'M' )
      || ( header.Magic[ 1 ] != 'S' )
      || ( header.Magic[ 2 ] != 'H' )
      || ( header.Magic[ 3 ] != '\0' ) )
    {
        printf_s( "Error : Invalid File.\n");
        fclose( pFile );
        return false;
    }

    // �t�@�C���o�[�W�������`�F�b�N.
    if ( header.Version != MSH_CURRENT_VERSION )
    {
        printf_s( "Error : Invalid File Version.\n" );
        fclose( pFile );
        return false;
    }

    // �f�[�^�w�b�_�[�T�C�Y���`�F�b�N.
    if ( header.DataHeaderSize != sizeof( MSH_DATA_HEADER ) )
    {
        printf_s( "Error : Data Header Size Not Matched.\n" );
        fclose( pFile );
        return false;
    }

    // ���_�f�[�^�T�C�Y���`�F�b�N.
    if ( header.DataHeader.VertexStructureSize != sizeof( ResMesh::Vertex ) )
    {
        printf_s( "Error : Vertex Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( ResMesh::Vertex ), header.DataHeader.VertexStructureSize );
        fclose( pFile );
        return false;
    }

    // ���_�C���f�b�N�X�f�[�^�T�C�Y���`�F�b�N.
    if ( header.DataHeader.IndexStructureSize != sizeof( asdx::UINT ) )
    {
        printf_s( "Error : Index Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( asdx::UINT ), header.DataHeader.IndexStructureSize );
        fclose( pFile );
        return false;
    }

    // �}�e���A���f�[�^�T�C�Y���`�F�b�N.
    if ( header.DataHeader.MaterialStructureSize != sizeof( ResMesh::Material ) )
    {
        printf_s( "Error : Material Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( ResMesh::Material ), header.DataHeader.MaterialStructureSize );
        fclose( pFile );
        return false;
    }

    // �T�u�Z�b�g�f�[�^�T�C�Y���`�F�b�N.
    if ( header.DataHeader.SubsetStructureSize != sizeof( ResMesh::Subset ) )
    {
        printf_s( "Error : Subset Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( ResMesh::Subset ), header.DataHeader.SubsetStructureSize );
        fclose( pFile );
        return false;
    }

    // �J�E���g����ݒ�.
    m_VertexCount   = header.DataHeader.NumVertices;
    m_IndexCount    = header.DataHeader.NumIndices;
    m_MaterialCount = header.DataHeader.NumMaterials;
    m_SubsetCount   = header.DataHeader.NumSubsets;

#if 0
    //// �f�o�b�O���O.
    //printf_s( "VertexCount   : %d\n", m_VertexCount );
    //printf_s( "IndexCount    : %d\n", m_IndexCount );
    //printf_s( "MaterialCount : %d\n", m_MaterialCount );
    //printf_s( "SubsetCount   : %d\n", m_SubsetCount );
#endif

    // ���_�f�[�^�̃��������m��.
    m_pVertex = new Vertex[ m_VertexCount ];
    if ( m_pVertex == nullptr )
    {
        //�@�G���[���O�o��.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // �t�@�C�������.
        fclose( pFile );

        // ����������ƃJ�E���g�������Z�b�g.
        Release();

        // �ُ�I��.
        return false;
    }

    // ���_�C���f�b�N�X�f�[�^�̃��������m��.
    m_pIndex = new Index[ m_IndexCount ];
    if ( m_pIndex == nullptr )
    {
        // �G���[���O�o��.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // �t�@�C�������.
        fclose( pFile );

        // ����������ƃJ�E���g�������Z�b�g.
        Release();

        // �ُ�I��.
        return false;
    }

    // �}�e���A���f�[�^�̃��������m��.
    m_pMaterial = new Material[ m_MaterialCount ];
    if ( m_pMaterial == nullptr )
    {
        // �G���[���O�o��.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // �t�@�C�������.
        fclose( pFile );

        // ����������ƃJ�E���g�������Z�b�g.
        Release();

        // �ُ�I��.
        return false;
    }

    // �T�u�Z�b�g�f�[�^�̃��������m��.
    m_pSubset = new Subset[ m_SubsetCount ];
    if ( m_pSubset == nullptr )
    {
        // �G���[���O�o��.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // �t�@�C�������.
        fclose( pFile );

        // ����������ƃJ�E���g�������Z�b�g.
        Release();

        // �ُ�I��.
        return false;
    }

    // �f�[�^����C�ɓǂݍ���.
    fread( m_pVertex,   sizeof( Vertex ),   m_VertexCount,   pFile );
    fread( m_pIndex,    sizeof( Index ),    m_IndexCount,    pFile );
    fread( m_pMaterial, sizeof( Material ), m_MaterialCount, pFile );
    fread( m_pSubset,   sizeof( Subset ),   m_SubsetCount,   pFile );

#if 0
    //for( asdx::UINT i=0; i<m_VertexCount; ++i )
    //{
    //    printf_s( "Vertex[ %d ] :\n", i );
    //    printf_s( "Position ( %f, %f, %f )\n", m_pVertex[ i ].Position.x, m_pVertex[ i ].Position.y, m_pVertex[ i ].Position.z );
    //    printf_s( "Normal   ( %f, %f, %f )\n", m_pVertex[ i ].Normal.x, m_pVertex[ i ].Normal.y, m_pVertex[ i ].Normal.z );
    //    printf_s( "TexCoord ( %f, %f )\n", m_pVertex[ i ].TexCoord.x, m_pVertex[ i ].TexCoord.y );
    //    printf_s( "\n" );
    //}
    //for( asdx::UINT i=0; i<m_IndexCount; ++i )
    //{
    //    printf_s( "Index[ %d ] = %u\n", i, m_pIndex[ i ] ); 
    //}
    //for( asdx::UINT i=0; i<m_MaterialCount; ++i )
    //{
    //    printf_s( "Material[ %d ] :\n", i );
    //    printf_s( "Ambient ( %f, %f, %f )\n", m_pMaterial[ i ].Ambient.x, m_pMaterial[ i ].Ambient.y, m_pMaterial[ i ].Ambient.z );
    //    printf_s( "Diffuse ( %f, %f, %f )\n", m_pMaterial[ i ].Diffuse.x, m_pMaterial[ i ].Diffuse.y, m_pMaterial[ i ].Diffuse.z );
    //    printf_s( "Specular( %f, %f, %f )\n", m_pMaterial[ i ].Specular.x, m_pMaterial[ i ].Specular.y, m_pMaterial[ i ].Specular.z );
    //    printf_s( "Emissive( %f, %f, %f )\n", m_pMaterial[ i ].Emissive.x, m_pMaterial[ i ].Emissive.y, m_pMaterial[ i ].Emissive.z );
    //    printf_s( "Power   ( %f )\n", m_pMaterial[ i ].Power );
    //    printf_s( "Alpha   ( %f )\n", m_pMaterial[ i ].Alpha );
    //    printf_s( "AmbientMap      [%s]\n", m_pMaterial[ i ].AmbientMap );
    //    printf_s( "DiffuseMap      [%s]\n", m_pMaterial[ i ].DiffuseMap );
    //    printf_s( "SpecularMap     [%s]\n", m_pMaterial[ i ].SpecularMap );
    //    printf_s( "BumpMap         [%s]\n", m_pMaterial[ i ].BumpMap );
    //    printf_s( "DisplacementMap [%s]\n", m_pMaterial[ i ].DisplacementMap );
    //    printf_s( "\n" );
    //}
    //for( asdx::UINT i=0; i<m_SubsetCount; ++i )
    //{
    //    printf_s( "Subset[ %d ] :\n", i );
    //    printf_s( "IndexOffset ( %d )\n", m_pSubset[ i ].IndexOffset );
    //    printf_s( "IndexCount  ( %d )\n", m_pSubset[ i ].IndexCount );
    //    printf_s( "MaterialID  ( %d )\n", m_pSubset[ i ].MaterialID );
    //    printf_s( "\n" );
    //}
#endif

    // �t�@�C�������.
    fclose( pFile );

    // ����I��.
    return true;
}

} // namespace asdx

