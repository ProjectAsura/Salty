//-------------------------------------------------------------------------------------
// File : s3d_mesh.cpp
// Desc : Mesh Module. 
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------
#include <s3d_mesh.h>
#include <cstdio>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------
// Constant Values
//-------------------------------------------------------------------------------------

// 現在のSMDファイルのバージョン番号です.
static const unsigned int SMD_CURRENT_VERSION = 0x00000001;

/////////////////////////////////////////////////////////////////////////////////////////////
// SMD_DATA_HEADER structure
//! @brief  MSHファイルのデータヘッダーです.
/////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_DATA_HEADER
{
    unsigned int NumVertices;               //!< 頂点数です.
    unsigned int NumIndices;                //!< インデックス数です.
    unsigned int NumMaterials;              //!< マテリアル数です.
    unsigned int NumSubsets;                //!< サブセット数です.
    unsigned int VertexStructureSize;       //!< 頂点データ構造体のサイズです.
    unsigned int IndexStructureSize;        //!< 頂点インデックスデータの構造体です.
    unsigned int MaterialStructureSize;     //!< マテリアルデータ構造体のサイズです.
    unsigned int SubsetStructureSize;       //!< サブセットデータ構造体のサイズです.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// SMD_FILE_HEADER structure
//! @brief  MSHファイルのファイルヘッダーです.
/////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_FILE_HEADER
{
    unsigned char   Magic[ 4 ];         //!< マジックです( 'S, 'M', 'D', '\0' )
    unsigned int    Version;            //!< ファイルバージョンです.
    unsigned int    DataHeaderSize;     //!< データヘッダー構造体のサイズです.
    SMD_DATA_HEADER DataHeader;         //!< データヘッダーです.
};

} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////
// ResMesh class
///////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------
ResMesh::~ResMesh()
{
    Release();
}

//-------------------------------------------------------------------------------------
//      ファイルからロードします.
//-------------------------------------------------------------------------------------
bool ResMesh::LoadFromFile( const char* filename )
{
    FILE* pFile;

    // ファイルを開く.
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        printf_s( "Error : LoadFromFile() Failed. filename = %s\n", filename );
        return false;
    }

    // ファイルヘッダーを読み取り.
    SMD_FILE_HEADER header;
    fread( &header, sizeof( SMD_FILE_HEADER ), 1, pFile );

    // マジックをチェック.
    if ( ( header.Magic[ 0 ] != 'S' )
      || ( header.Magic[ 1 ] != 'M' )
      || ( header.Magic[ 2 ] != 'D' )
      || ( header.Magic[ 3 ] != '\0' ) )
    {
        printf_s( "Error : Invalid File.\n");
        fclose( pFile );
        return false;
    }

    // ファイルバージョンをチェック.
    if ( header.Version != SMD_CURRENT_VERSION )
    {
        printf_s( "Error : Invalid File Version.\n" );
        fclose( pFile );
        return false;
    }

    // データヘッダーサイズをチェック.
    if ( header.DataHeaderSize != sizeof( SMD_DATA_HEADER ) )
    {
        printf_s( "Error : Data Header Size Not Matched.\n" );
        fclose( pFile );
        return false;
    }

    // 頂点データサイズをチェック.
    if ( header.DataHeader.VertexStructureSize != sizeof( ResMesh::Vertex ) )
    {
        printf_s( "Error : Vertex Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( ResMesh::Vertex ), header.DataHeader.VertexStructureSize );
        fclose( pFile );
        return false;
    }

    // 頂点インデックスデータサイズをチェック.
    if ( header.DataHeader.IndexStructureSize != sizeof( u32 ) )
    {
        printf_s( "Error : Index Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( u32 ), header.DataHeader.IndexStructureSize );
        fclose( pFile );
        return false;
    }

    // マテリアルデータサイズをチェック.
    if ( header.DataHeader.MaterialStructureSize != sizeof( ResMesh::Material ) )
    {
        printf_s( "Error : Material Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( ResMesh::Material ), header.DataHeader.MaterialStructureSize );
        fclose( pFile );
        return false;
    }

    // サブセットデータサイズをチェック.
    if ( header.DataHeader.SubsetStructureSize != sizeof( ResMesh::Subset ) )
    {
        printf_s( "Error : Subset Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( ResMesh::Subset ), header.DataHeader.SubsetStructureSize );
        fclose( pFile );
        return false;
    }

    // カウント数を設定.
    m_VertexCount   = header.DataHeader.NumVertices;
    m_IndexCount    = header.DataHeader.NumIndices;
    m_MaterialCount = header.DataHeader.NumMaterials;
    m_SubsetCount   = header.DataHeader.NumSubsets;

#if 0
    //// デバッグログ.
    //printf_s( "VertexCount   : %d\n", m_VertexCount );
    //printf_s( "IndexCount    : %d\n", m_IndexCount );
    //printf_s( "MaterialCount : %d\n", m_MaterialCount );
    //printf_s( "SubsetCount   : %d\n", m_SubsetCount );
#endif

    // 頂点データのメモリを確保.
    m_pVertex = new Vertex[ m_VertexCount ];
    if ( m_pVertex == nullptr )
    {
        //　エラーログ出力.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // メモリ解放とカウント数をリセット.
        Release();

        // 異常終了.
        return false;
    }

    // 頂点インデックスデータのメモリを確保.
    m_pIndex = new u32 [ m_IndexCount ];
    if ( m_pIndex == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // メモリ解放とカウント数をリセット.
        Release();

        // 異常終了.
        return false;
    }

    // マテリアルデータのメモリを確保.
    m_pMaterial = new Material[ m_MaterialCount ];
    if ( m_pMaterial == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // メモリ解放とカウント数をリセット.
        Release();

        // 異常終了.
        return false;
    }

    // サブセットデータのメモリを確保.
    m_pSubset = new Subset[ m_SubsetCount ];
    if ( m_pSubset == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        // メモリ解放とカウント数をリセット.
        Release();

        // 異常終了.
        return false;
    }

    // データを一気に読み込み.
    fread( m_pVertex,   sizeof( Vertex ),   m_VertexCount,   pFile );
    fread( m_pIndex,    sizeof( u32 ),      m_IndexCount,    pFile );
    fread( m_pMaterial, sizeof( Material ), m_MaterialCount, pFile );
    fread( m_pSubset,   sizeof( Subset ),   m_SubsetCount,   pFile );

#if 0
    //for( u32 i=0; i<m_VertexCount; ++i )
    //{
    //    printf_s( "Vertex[ %d ] :\n", i );
    //    printf_s( "Position ( %f, %f, %f )\n", m_pVertex[ i ].Position.x, m_pVertex[ i ].Position.y, m_pVertex[ i ].Position.z );
    //    printf_s( "Normal   ( %f, %f, %f )\n", m_pVertex[ i ].Normal.x, m_pVertex[ i ].Normal.y, m_pVertex[ i ].Normal.z );
    //    printf_s( "TexCoord ( %f, %f )\n", m_pVertex[ i ].TexCoord.x, m_pVertex[ i ].TexCoord.y );
    //    printf_s( "\n" );
    //}
    //for( u32 i=0; i<m_IndexCount; ++i )
    //{
    //    printf_s( "Index[ %d ] = %u\n", i, m_pIndex[ i ] ); 
    //}
    //for( u32 i=0; i<m_MaterialCount; ++i )
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
    //for( u32 i=0; i<m_SubsetCount; ++i )
    //{
    //    printf_s( "Subset[ %d ] :\n", i );
    //    printf_s( "IndexOffset ( %d )\n", m_pSubset[ i ].IndexOffset );
    //    printf_s( "IndexCount  ( %d )\n", m_pSubset[ i ].IndexCount );
    //    printf_s( "MaterialID  ( %d )\n", m_pSubset[ i ].MaterialID );
    //    printf_s( "\n" );
    //}
#endif

    // ファイルを閉じる.
    fclose( pFile );

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------
//      メモリ解放処理を行います.
//-------------------------------------------------------------------------------------
void ResMesh::Release()
{
    if ( m_pVertex )
    {
        delete [] m_pVertex;
        m_pVertex = nullptr;
    }

    if ( m_pIndex )
    {
        delete [] m_pIndex;
        m_pIndex = nullptr;
    }

    if ( m_pMaterial )
    {
        delete [] m_pMaterial;
        m_pMaterial = nullptr;
    }

    if ( m_pSubset )
    {
        delete [] m_pSubset;
        m_pSubset = nullptr;
    }

    // カウンターリセット.
    m_VertexCount   = 0;
    m_IndexCount    = 0;
    m_MaterialCount = 0;
    m_SubsetCount   = 0;
}


} // namespace s3d

