//----------------------------------------------------------------------------------------
// File : s3d_mesh.cpp
// Desc : Mesh Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include <s3d_mesh.h>
#include <cstdio>
#include <cstring>


namespace /* anonymous */ {

//----------------------------------------------------------------------------------------
// Constant Values.
//----------------------------------------------------------------------------------------
static const u32 SMD_CURRENT_VERSION = 0x00000001;
static const u8  SMD_FILE_TAG[4]     = { 'S', 'M', 'D', '\0' };


//////////////////////////////////////////////////////////////////////////////////////////
// SMD_DATA_HEADER structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_DATA_HEADER
{
    u32     NumTriangles;           //!< 三角形数です.
    u32     NumMaterials;           //!< マテリアル数です.
    u32     NumTextures;            //!< テクスチャ数です.

    u32     TriangleStructSize;     //!< 三角形構造体のサイズです.
    u32     MaterialStructSize;     //!< マテリアル構造体のサイズです.
    u32     TextureStructSize;      //!< テクスチャ構造体のサイズです.
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_FILE_HEADER structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_FILE_HEADER
{
    u8              Magic[ 4 ];         //!< ファイルマジック "SMD0"です.
    u32             Version;            //!< ファイルバージョンです.
    u32             DataHeaderSize;     //!< データヘッダ構造体のサイズです.
    SMD_DATA_HEADER DataHeader;         //!< データヘッダです.
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_VERTEX structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_VERTEX
{
    s3d::Vector3    Position;       //!< 位置座標です.
    s3d::Vector3    Normal;         //!< 法線ベクトルです.
    s3d::Vector2    TexCoord;       //!< テクスチャ座標です.
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_TRIANGLE structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_TRIANGLE
{
    SMD_VERTEX      Vertex[3];      //!< 頂点データです.
    s32             MaterialId;     //!< マテリアルインデックスです.
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_MTERIAL structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_MATERIAL
{
    s3d::Vector3    Diffuse;            //!< 拡散反射成分です.
    s3d::Vector3    Emissive;           //!< 自己照明成分です.
    s32             DiffuseMap;         //!< ディフューズマップです.
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_TEXTURE structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_TEXTURE
{
    char            FileName[ 256 ];    //!< ファイル名です.
};

} // namespace /* anonymous */


namespace s3d {

//////////////////////////////////////////////////////////////////////////////////////////
// Mesh::Material
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------------
Mesh::Material::Material
(
    const Color&    diffuse,
    const Color&    emissive
)
: Diffuse       ( diffuse )
, Emissive      ( emissive )
, pDiffuseMap   ( nullptr )
, pDiffuseSmp   ( nullptr )
{
    Threshold = Max( diffuse.x, diffuse.y );
    Threshold = Max( diffuse.z, Threshold );
}

//---------------------------------------------------------------------------------------
//      自己照明成分を取得します.
//---------------------------------------------------------------------------------------
Color Mesh::Material::GetEmissive() const
{
    return Emissive; 
}

//---------------------------------------------------------------------------------------
//      ロシアンルーレットの閾値を取得します.
//---------------------------------------------------------------------------------------
f32 Mesh::Material::GetThreshold() const
{
    return Threshold;
}

//---------------------------------------------------------------------------------------
//      色を計算します.
//---------------------------------------------------------------------------------------
Color Mesh::Material::ComputeColor( ShadingArg& arg ) const
{
    // ========================
    // Lambert BRDF.
    // ========================

    // 補正済み法線データ (レイの入出を考慮済み).
    const Vector3 normalMod = ( Vector3::Dot( arg.normal, arg.input ) < 0.0 ) ? arg.normal : -arg.normal;

    // normalModの方向を基準とした正規直交基底(w, u, v)を作る。
    // この基底に対する半球内で次のレイを飛ばす。
    OrthonormalBasis onb;
    onb.InitFromW( normalMod );

    // インポータンスサンプリング.
    const f32 phi = F_2PI * arg.random.GetAsF32( );
    const f32 r = sqrtf( arg.random.GetAsF32( ) );
    const f32 x = r * cosf( phi );
    const f32 y = r * sinf( phi );
    const f32 z = sqrtf( 1 - x * x - y * y );

    // 出射方向.
    Vector3 dir = Vector3::UnitVector( onb.u * x + onb.v * y + onb.w * z );
    arg.output = dir;

    // 重み更新 (飛ぶ方向が不定なので確率で割る必要あり).
    if ( pDiffuseMap != nullptr )
    {
        return Color::Mul( Diffuse, pDiffuseMap->Sample( ( *pDiffuseSmp ), arg.texcoord ) ) / arg.prob;
    }

    return Diffuse / arg.prob;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Mesh class
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------------
Mesh::Mesh()
: m_NumTriangles( 0 )
, m_NumMaterials( 0 )
, m_NumTextures ( 0 )
, m_pTriangles  ( nullptr )
, m_pMaterials  ( nullptr )
, m_pTextures   ( nullptr )
, m_pBVH        ( nullptr )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------------
//      デストラクタです.
//----------------------------------------------------------------------------------------
Mesh::~Mesh()
{
    Release();
}

//----------------------------------------------------------------------------------------
//      解放処理を行います.
//----------------------------------------------------------------------------------------
void Mesh::Release()
{
    BVH::DestroyBranch( (BVH*)m_pBVH );

    if ( m_pTriangles )
    {
        delete [] m_pTriangles;
        m_pTriangles = nullptr;
    }

    if ( m_pMaterials )
    {
        delete [] m_pMaterials;
        m_pMaterials = nullptr;
    }

    if ( m_pTextures )
    {
        delete [] m_pTextures;
        m_pTextures = nullptr;
    }

    m_NumTriangles = 0;
    m_NumMaterials = 0;
    m_NumTextures  = 0;
}

//----------------------------------------------------------------------------------------
//      ファイルからロードを行います.
//----------------------------------------------------------------------------------------
bool Mesh::LoadFromFile( const char* filename )
{
    FILE* pFile;

    // ファイルを開きします.
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        return false;
    }

    // ファイルヘッダーを読み込みます.
    SMD_FILE_HEADER fileHeader;
    fread( &fileHeader, sizeof( SMD_FILE_HEADER ), 1, pFile );

    // ファイルマジックを取得します.
    if ( memcmp( fileHeader.Magic, SMD_FILE_TAG, sizeof(u8) * 4 ) != 0 )
    {
        fclose( pFile );
        return false;
    }

    // ファイルバージョンをチェックします.
    if ( fileHeader.Version != SMD_CURRENT_VERSION )
    {
        fclose( pFile );
        return false;
    }

    // データヘッダーのサイズをチェックをします.
    if ( fileHeader.DataHeaderSize != sizeof( SMD_DATA_HEADER ) )
    {
        fclose( pFile );
        return false;
    }

    // 三角形数を取得します.
    if ( fileHeader.DataHeader.NumTriangles == 0 )
    {
        fclose( pFile );
        return false;
    }

    // マテリアル数を取得します.
    if ( fileHeader.DataHeader.NumMaterials == 0 )
    {
        fclose( pFile );
        return false;
    }

    m_NumTriangles = fileHeader.DataHeader.NumTriangles;
    m_NumMaterials = fileHeader.DataHeader.NumMaterials;
    m_NumTextures  = fileHeader.DataHeader.NumTextures;

    // 三角形データのメモリを確保します.
    m_pTriangles = new Triangle[ m_NumTriangles ];
    if ( m_pTriangles == nullptr )
    {
        fclose( pFile );
        Release();
        return false;
    }

    // マテリアルデータのメモリを確保します.
    m_pMaterials = new Material[ m_NumMaterials ];
    if ( m_pMaterials == nullptr )
    {
        fclose( pFile );
        Release( );
        return false;
    }

    // テクスチャデータのメモリを確保します.
    if ( m_NumTextures )
    {
        m_pTextures = new Texture2D[ m_NumTextures ];
        if ( m_pTextures == nullptr )
        {
            fclose( pFile );
            Release( );
            return false;
        }
    }

    // 三角形データを読み込みます.
    for ( u32 i = 0; i < m_NumTriangles; ++i )
    {
        SMD_TRIANGLE triangle;
        fread( &triangle, sizeof( SMD_TRIANGLE ), 1, pFile );

        // 頂点0
        m_pTriangles[ i ].v0.pos      = triangle.Vertex[ 0 ].Position;
        m_pTriangles[ i ].v0.texcoord = triangle.Vertex[ 0 ].TexCoord;
        m_pTriangles[ i ].v0.normal   = triangle.Vertex[ 0 ].Normal;

        // 頂点1
        m_pTriangles[ i ].v1.pos      = triangle.Vertex[ 1 ].Position;
        m_pTriangles[ i ].v1.texcoord = triangle.Vertex[ 1 ].TexCoord;
        m_pTriangles[ i ].v1.normal   = triangle.Vertex[ 1 ].Normal;

        // 頂点2
        m_pTriangles[ i ].v2.pos      = triangle.Vertex[ 2 ].Position;
        m_pTriangles[ i ].v2.texcoord = triangle.Vertex[ 2 ].TexCoord;
        m_pTriangles[ i ].v2.normal   = triangle.Vertex[ 2 ].Normal;

        if ( triangle.MaterialId >= 0 )
        {
            m_pTriangles[ i ].pMaterial = &m_pMaterials[ triangle.MaterialId ];
        }
    }

    // マテリアルデータを読み込みます.
    for ( u32 i = 0; i < m_NumMaterials; ++i )
    {
        SMD_MATERIAL material;
        fread( &material, sizeof( SMD_MATERIAL ), 1, pFile );

        m_pMaterials[i] = Material( material.Diffuse, material.Emissive );
        if ( material.DiffuseMap >= 0 )
        {
            m_pMaterials[ i ].pDiffuseMap = &m_pTextures[ material.DiffuseMap ];
            m_pMaterials[ i ].pDiffuseSmp = &m_DiffuseSmp;
        }
    }

    // テクスチャデータを読み込みます.
    if ( m_NumTextures )
    {
        for ( u32 i = 0; i < m_NumTextures; ++i )
        {
            SMD_TEXTURE texture;
            fread( &texture, sizeof( SMD_TEXTURE ), 1, pFile );

            m_pTextures[ i ].LoadFromFile( texture.FileName );
        }
    }

    // BVHを構築します.
    m_pBVH = BVH::BuildBranch( (IShape**)&m_pTriangles, m_NumTriangles );

    return true;
}

//----------------------------------------------------------------------------------------
//      交差判定を行います.
//----------------------------------------------------------------------------------------
bool Mesh::IsHit( const Ray& ray, HitRecord& record ) const
{
    return m_pBVH->IsHit( ray, record );
}

//----------------------------------------------------------------------------------------
//      マテリアルを取得します.
//----------------------------------------------------------------------------------------
const IMaterial* Mesh::GetMaterial() const
{
    return nullptr;
}

//----------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//----------------------------------------------------------------------------------------
BoundingBox Mesh::GetBox() const
{
    return m_pBVH->GetBox();
}

//----------------------------------------------------------------------------------------
//      基本図形かどうかチェックします.
//----------------------------------------------------------------------------------------
bool Mesh::IsPrimitive() const
{
    return false; 
}

} // namespace s3d
