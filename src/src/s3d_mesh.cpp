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
#include <string>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------
// Constant Values
//-------------------------------------------------------------------------------------

// 現在のSMDファイルのバージョン番号です.
static const unsigned int SMD_CURRENT_VERSION = 0x00000001;


/////////////////////////////////////////////////////////////////////////////////////////////
// SMD_DATA_HEADER structure
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
/////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_FILE_HEADER
{
    unsigned char   Magic[ 4 ];         //!< マジックです( 'S, 'M', 'D', '\0' )
    unsigned int    Version;            //!< ファイルバージョンです.
    unsigned int    DataHeaderSize;     //!< データヘッダー構造体のサイズです.
    SMD_DATA_HEADER DataHeader;         //!< データヘッダーです.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// SMD_VERTEX structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_VERTEX
{
    s3d::Vector3    Position;       //!< 位置座標です.
    s3d::Vector3    Normal;         //!< 法線ベクトルです.
    s3d::Vector2    TexCoord;       //!< テクスチャ座標です.
};

/////////////////////////////////////////////////////////////////////////////////////////////
// SMD_MATERIAL structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_MATERIAL
{
    s3d::Color      Diffuse;            //!< 拡散反射色です.
    s3d::Color      Emissive;           //!< 自己照明色です.
    f64             Refractivity;       //!< 屈折率です.
    f64             Roughness;          //!< 面の粗さです.
    char            DiffuseMap[ 256 ];  //!< ディフューズマップです.
};

////////////////////////////////////////////////////////////////////////////////////////////
// SMD_SUBSET structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_SUBSET
{
    u32             IndexOffset;        //!< 開始面までのオフセットです.
    u32             IndexCount;         //!< インデックス数です.
    u32             MaterialID;         //!< マテリアル識別番号です.
};


} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////
// ResMesh class
///////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------
Mesh::Mesh()
: m_pTriangle()
, m_pMaterial()
, m_pBVH     ( nullptr )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------
Mesh::~Mesh()
{
    Release();
}

//-------------------------------------------------------------------------------------
//      ファイルからロードします.
//-------------------------------------------------------------------------------------
bool Mesh::LoadFromFile( const char* filename )
{
    FILE* pFile;

    // ファイルを開く.
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        printf_s( "Error : LoadFromFile() Failed. filename = %s\n", filename );
        return false;
    }

    std::string baseName;
    {
        std::string temp( filename );
        s32 n = temp.rfind( '/' );
        if ( n != std::string::npos )
        {
            baseName = temp.substr( 0, n );
        }
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
    if ( header.DataHeader.VertexStructureSize != sizeof( SMD_VERTEX ) )
    {
        printf_s( "Error : Vertex Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( SMD_VERTEX ), header.DataHeader.VertexStructureSize );
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
    if ( header.DataHeader.MaterialStructureSize != sizeof( SMD_MATERIAL ) )
    {
        printf_s( "Error : Material Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( SMD_MATERIAL ), header.DataHeader.MaterialStructureSize );
        fclose( pFile );
        return false;
    }

    // サブセットデータサイズをチェック.
    if ( header.DataHeader.SubsetStructureSize != sizeof( SMD_SUBSET ) )
    {
        printf_s( "Error : Subset Structure Size Not Matched. expect size = %d, value = %d\n", sizeof( SMD_SUBSET ), header.DataHeader.SubsetStructureSize );
        fclose( pFile );
        return false;
    }

    // カウント数を設定.
    u32 vertexCount   = header.DataHeader.NumVertices;
    u32 indexCount    = header.DataHeader.NumIndices;
    u32 materialCount = header.DataHeader.NumMaterials;
    u32 subsetCount   = header.DataHeader.NumSubsets;

#if 0
    //// デバッグログ.
    //printf_s( "VertexCount   : %d\n", m_VertexCount );
    //printf_s( "IndexCount    : %d\n", m_IndexCount );
    //printf_s( "MaterialCount : %d\n", m_MaterialCount );
    //printf_s( "SubsetCount   : %d\n", m_SubsetCount );
#endif

    SMD_VERTEX*   pVertex   = nullptr;
    SMD_MATERIAL* pMaterial = nullptr;
    SMD_SUBSET*   pSubset   = nullptr;
    u32*          pIndex    = nullptr;

    // 頂点データのメモリを確保.
    pVertex = new SMD_VERTEX[ vertexCount ];
    if ( pVertex == nullptr )
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
    pIndex = new u32 [ indexCount ];
    if ( pIndex == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        if ( pVertex )
        {
            delete [] pVertex;
            pVertex = nullptr;
        }

        // メモリ解放とカウント数をリセット.
        Release();

        // 異常終了.
        return false;
    }

    // マテリアルデータのメモリを確保.
    pMaterial = new SMD_MATERIAL[ materialCount ];
    if ( pMaterial == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        if ( pVertex )
        {
            delete [] pVertex;
            pVertex = nullptr;
        }

        if ( pIndex )
        {
            delete [] pIndex;
            pIndex = nullptr;
        }

        // メモリ解放とカウント数をリセット.
        Release();

        // 異常終了.
        return false;
    }

    // サブセットデータのメモリを確保.
    pSubset = new SMD_SUBSET[ subsetCount ];
    if ( pSubset == nullptr )
    {
        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed.\n" );

        // ファイルを閉じる.
        fclose( pFile );

        if ( pVertex )
        {
            delete [] pVertex;
            pVertex = nullptr;
        }

        if ( pIndex )
        {
            delete [] pIndex;
            pIndex = nullptr;
        }

        if ( pMaterial )
        {
            delete [] pMaterial;
            pMaterial = nullptr;
        }

        // メモリ解放とカウント数をリセット.
        Release();

        // 異常終了.
        return false;
    }

    // データを一気に読み込み.
    fread( pVertex,   sizeof( SMD_VERTEX ),   vertexCount,   pFile );
    fread( pIndex,    sizeof( u32 ),          indexCount,    pFile );
    fread( pMaterial, sizeof( SMD_MATERIAL ), materialCount, pFile );
    fread( pSubset,   sizeof( SMD_SUBSET ),   subsetCount,   pFile );

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


    //m_pMaterial.resize( materialCount );
    //for( u32 i=0; i<materialCount; ++i )
    //{
    //    if ( pMaterial[ i ].DiffuseMap[0] != 0 )
    //    {
    //        std::string textureName = baseName + "/" + pMaterial[ i].DiffuseMap;
    //        m_pMaterial[ i ] = MeshMaterial( 
    //            pMaterial[ i ].Diffuse,
    //            pMaterial[ i ].Emissive,
    //            pMaterial[ i ].Refractivity,
    //            pMaterial[ i ].Roughness,
    //            textureName.c_str() );
    //    }
    //    else
    //    {
    //        m_pMaterial[ i ] =  MeshMaterial( 
    //            pMaterial[ i ].Diffuse,
    //            pMaterial[ i ].Emissive,
    //            pMaterial[ i ].Refractivity,
    //            pMaterial[ i ].Roughness );
    //    }
    //}

    if ( pMaterial )
    {
        delete [] pMaterial;
        pMaterial = nullptr;
    }

    m_pTriangle.resize( indexCount );

    u32 count = 0;
    for( u32 i=0; i<subsetCount; ++i )
    {
        for( u32 j=0; j<pSubset->IndexCount; j+=3 )
        {
            u32 idx0 = pSubset->IndexOffset + ( j + 0 );
            u32 idx1 = pSubset->IndexOffset + ( j + 1 );
            u32 idx2 = pSubset->IndexOffset + ( j + 2 );

            m_pTriangle[ count ] = MeshTriangle(
                pVertex[ idx0 ].Position,
                pVertex[ idx1 ].Position,
                pVertex[ idx2 ].Position,
                pVertex[ idx0 ].Normal,
                pVertex[ idx1 ].Normal,
                pVertex[ idx2 ].Normal,
                pVertex[ idx0 ].TexCoord,
                pVertex[ idx1 ].TexCoord,
                pVertex[ idx2 ].TexCoord,
                &m_pMaterial[ pSubset->MaterialID ] );
            count++;
        }
    }

    if ( pVertex )
    {
        delete [] pVertex;
        pVertex = nullptr;
    }

    if ( pIndex )
    {
        delete [] pIndex;
        pIndex = nullptr;
    }

    if ( pSubset )
    {
        delete [] pSubset;
        pSubset = nullptr;
    }

    IShape*  pShape = &m_pTriangle[0];
    IShape** ppShapes = &pShape;

    m_pBVH = BVH::BuildBranch( ppShapes, m_pTriangle.size() );

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------
//      メモリ解放処理を行います.
//-------------------------------------------------------------------------------------
void Mesh::Release()
{
    if ( m_pBVH )
    {
        delete m_pBVH;
        m_pBVH = nullptr;
    }

    m_pTriangle.clear();
    m_pMaterial.clear();
}


} // namespace s3d

