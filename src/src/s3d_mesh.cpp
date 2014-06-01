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
#include <string>
#include <s3d_bvh.h>


#ifndef DLOG
    #if defined(DEBUG) || defined(_DEBUG)
        #define DLOG( x, ... )   printf_s( "[File:%s, Line:%d] "x"\n", __FILE__, __LINE__ , ##__VA_ARGS__ )
    #else
        #define DLOG( x, ... )   void(0)
    #endif
#endif//DLOG

#ifndef ILOG
#define ILOG( x, ... )      printf_s( x"\n", ##__VA_ARGS__ )
#endif//ILOG


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
// MeshMaterial
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------------
MeshMaterial::MeshMaterial
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
Color MeshMaterial::GetEmissive() const
{
    return Emissive; 
}

//---------------------------------------------------------------------------------------
//      ロシアンルーレットの閾値を取得します.
//---------------------------------------------------------------------------------------
f32 MeshMaterial::GetThreshold() const
{
    return Threshold;
}

#if 1
Color MeshMaterial::GetDebugColor() const
{ return Diffuse; }
#endif

//---------------------------------------------------------------------------------------
//      色を計算します.
//---------------------------------------------------------------------------------------
Color MeshMaterial::ComputeColor( ShadingArg& arg ) const
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
    const f32 r = SafeSqrt( arg.random.GetAsF32( ) );
    const f32 x = r * cosf( phi );
    const f32 y = r * sinf( phi );
    const f32 z = SafeSqrt( 1.0f - ( x * x ) - ( y * y ) );

    // 出射方向.
    Vector3 dir = Vector3::UnitVector( onb.u * x + onb.v * y + onb.w * z );
    arg.output = dir;

    // 重み更新 (飛ぶ方向が不定なので確率で割る必要あり).
    Color result;
    if ( pDiffuseMap != nullptr )
    { result = Color::Mul( Diffuse, pDiffuseMap->Sample( ( *pDiffuseSmp ), arg.texcoord ) ) / arg.prob; }
    else
    { result = Diffuse / arg.prob; }

    return result;
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
    if ( m_pBVH )
    {
        IDispose* pBVH = dynamic_cast<IDispose*>( m_pBVH );
        if ( pBVH != nullptr )
        { pBVH->Dispose(); }
        m_pBVH = nullptr;
    }

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

    std::string dirPath;
    {
        std::string temp( filename );
        
        std::string::size_type idx = temp.rfind( "/" );
        if ( idx != std::string::npos )
        {
            dirPath = temp.substr( 0, idx );
        }
    }

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

#if 0
    ILOG( "Num Triangle  : %d", m_NumTriangles );
    ILOG( "Num Materials : %d", m_NumMaterials );
    ILOG( "Num Textures  : %d", m_NumTextures );
#endif

    // 三角形データのメモリを確保します.
    m_pTriangles = new Triangle[ m_NumTriangles ];
    if ( m_pTriangles == nullptr )
    {
        fclose( pFile );
        Release();
        return false;
    }

    // マテリアルデータのメモリを確保します.
    m_pMaterials = new MeshMaterial[ m_NumMaterials ];
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

    #if 0
        ILOG( "Triangle[%d] : ", i );
        ILOG( "    v0 pos      ( %f, %f, %f )", m_pTriangles[ i ].v0.pos.x,      m_pTriangles[ i ].v0.pos.y,    m_pTriangles[ i ].v0.pos.z );
        ILOG( "    v0 normal   ( %f, %f, %f )", m_pTriangles[ i ].v0.normal.x,   m_pTriangles[ i ].v0.normal.y, m_pTriangles[ i ].v0.normal.z );
        ILOG( "    v0 texcoord ( %f, %f )",     m_pTriangles[ i ].v0.texcoord.x, m_pTriangles[ i ].v0.texcoord.y );
        ILOG( "" );
        ILOG( "    v1 pos      ( %f, %f, %f )", m_pTriangles[ i ].v1.pos.x,      m_pTriangles[ i ].v1.pos.y,    m_pTriangles[ i ].v1.pos.z );
        ILOG( "    v1 normal   ( %f, %f, %f )", m_pTriangles[ i ].v1.normal.x,   m_pTriangles[ i ].v1.normal.y, m_pTriangles[ i ].v1.normal.z );
        ILOG( "    v1 texcoord ( %f, %f )",     m_pTriangles[ i ].v1.texcoord.x, m_pTriangles[ i ].v1.texcoord.y );
        ILOG( "" );
        ILOG( "    v2 pos      ( %f, %f, %f )", m_pTriangles[ i ].v2.pos.x,      m_pTriangles[ i ].v2.pos.y,    m_pTriangles[ i ].v2.pos.z );
        ILOG( "    v2 normal   ( %f, %f, %f )", m_pTriangles[ i ].v2.normal.x,   m_pTriangles[ i ].v2.normal.y, m_pTriangles[ i ].v2.normal.z );
        ILOG( "    v2 texcoord ( %f, %f )",     m_pTriangles[ i ].v2.texcoord.x, m_pTriangles[ i ].v2.texcoord.y );
        ILOG( "" );
        ILOG( "    Material : 0x%x", m_pTriangles[ i ].pMaterial );
    #endif
    }

    // マテリアルデータを読み込みます.
    for ( u32 i = 0; i < m_NumMaterials; ++i )
    {
        SMD_MATERIAL material;
        fread( &material, sizeof( SMD_MATERIAL ), 1, pFile );

        m_pMaterials[i] = MeshMaterial( material.Diffuse, material.Emissive );
        if ( material.DiffuseMap >= 0 )
        {
            m_pMaterials[ i ].pDiffuseMap = &m_pTextures[ material.DiffuseMap ];
            m_pMaterials[ i ].pDiffuseSmp = &m_DiffuseSmp;
        }

    #if 0
        ILOG( "Material[%d] : ", i );
        ILOG( "    Emissive ( %f, %f, %f )", m_pMaterials[ i ].Emissive.x, m_pMaterials[ i ].Emissive.y, m_pMaterials[ i ].Emissive.z );
        ILOG( "    Diffuse  ( %f, %f, %f )", m_pMaterials[ i ].Diffuse.x, m_pMaterials[ i ].Diffuse.y, m_pMaterials[ i ].Diffuse.z );
        ILOG( "    DiffuseMap  : 0x%x", m_pMaterials[ i ].pDiffuseMap );
        ILOG( "    DiffuseSmp  : 0x%x", m_pMaterials[ i ].pDiffuseSmp );
        ILOG( "    Threshold : %f",     m_pMaterials[ i ].Threshold );
    #endif
    }

    // テクスチャデータを読み込みます.
    if ( m_NumTextures )
    {
        for ( u32 i = 0; i < m_NumTextures; ++i )
        {
            SMD_TEXTURE texture;
            fread( &texture, sizeof( SMD_TEXTURE ), 1, pFile );

            std::string path = dirPath + "/" + texture.FileName;
            if ( !m_pTextures[ i ].LoadFromFile( path.c_str() ) )
            {
                ILOG( "Warning : Texture Load Failed. filename = %s", path.c_str() );
                for( u32 j=0; j<m_NumMaterials; ++j )
                {
                    if ( m_pMaterials[j].pDiffuseMap == (&m_pTextures[i]) )
                    {
                        m_pMaterials[j].pDiffuseMap = nullptr;
                        m_pMaterials[j].pDiffuseSmp = nullptr;
                    }
                }
            }
        #if 0
            ILOG( "Texture[%d] : ", i );
            ILOG( "    File Name : %s", path.c_str() );
        #endif
        }
    }

    // BVHを構築します.
    m_pBVH = OBVH::BuildBranch( m_pTriangles, m_NumTriangles );

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
    if ( m_pMaterials != nullptr )
    { return &m_pMaterials[0]; }

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
    return true; 
}

} // namespace s3d
