//--------------------------------------------------------------------------------------------------
// File : s3d_mesh.cpp
// Desc : Mesh Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <string>
#include <s3d_mesh.h>
#include <s3d_bvh2.h>
#include <s3d_bvh4.h>
#include <s3d_bvh8.h>
#include <s3d_logger.h>
#include <s3d_triangle.h>
#include <s3d_materialfactory.h>


namespace /* anonymous */ {

//--------------------------------------------------------------------------------------------------
// Constant Values.
//--------------------------------------------------------------------------------------------------
static const u32 SMD_CURRENT_VERSION = 0x00000002;
static const u8  SMD_FILE_TAG[4]     = { 'S', 'M', 'D', '\0' };

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_MATERIAL_TYPE
///////////////////////////////////////////////////////////////////////////////////////////////////
enum SMD_MATERIAL_TYPE
{
    SMD_MATERIAL_TYPE_MATTE = 0,        //!< Lambert
    SMD_MATERIAL_TYPE_MIRROR,           //!< Perfect Specular
    SMD_MATERIAL_TYPE_DIELECTRIC,       //!< Dielectric
    SMD_MATERIAL_TYPE_GLOSSY,           //!< Phong
    SMD_MATERIAL_TYPE_PLASTIC,          //!< Lambert + Phong
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_DATA_HEADER structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_DATA_HEADER
{
    u32     NumTriangles;           //!< 三角形数です.
    u32     NumMaterials;           //!< マテリアル数です.
    u32     NumTextures;            //!< テクスチャ数です.

    u32     TriangleStructSize;     //!< 三角形構造体のサイズです.
    u32     MaterialStructSize;     //!< マテリアル構造体のサイズです.
    u32     TextureStructSize;      //!< テクスチャ構造体のサイズです.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_FILE_HEADER structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_FILE_HEADER
{
    u8              Magic[ 4 ];         //!< ファイルマジック "SMD0"です.
    u32             Version;            //!< ファイルバージョンです.
    u32             DataHeaderSize;     //!< データヘッダ構造体のサイズです.
    SMD_DATA_HEADER DataHeader;         //!< データヘッダです.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_VECTOR2 structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_VECTOR2
{
    f32 x;  //!< X成分です.
    f32 y;  //!< Y成分です.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_VECTOR3 structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_VECTOR3
{
    f32 x;  //!< X成分です.
    f32 y;  //!< Y成分です.
    f32 z;  //!< Z成分です.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_VECTOR4 structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_VECTOR4
{
    f32 x;  //!< X成分です.
    f32 y;  //!< Y成分です.
    f32 z;  //!< Z成分です.
    f32 w;  //!< W成分です.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_VERTEX structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_VERTEX
{
    SMD_VECTOR3    Position;       //!< 位置座標です.
    SMD_VECTOR3    Normal;         //!< 法線ベクトルです.
    SMD_VECTOR2    TexCoord;       //!< テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_TRIANGLE structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_TRIANGLE
{
    SMD_VERTEX      Vertex[3];      //!< 頂点データです.
    s32             MaterialId;     //!< マテリアルインデックスです.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_MATTE structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_MATTE
{
    SMD_VECTOR3    Color;
    SMD_VECTOR3    Emissive;
    s32            ColorMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_MIRROR structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_MIRROR
{
    SMD_VECTOR3    Color;
    SMD_VECTOR3    Emissive;
    s32            ColorMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_DIELECTRIC structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_DIELECTRIC
{
    SMD_VECTOR3     Color;
    f32             Ior;
    SMD_VECTOR3     Emissive;
    s32             ColorMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_GLOSSY structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_GLOSSY
{
    SMD_VECTOR3     Color;
    f32             Power;
    SMD_VECTOR3     Emissive;
    s32             ColorMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// SMD_TEXTURE structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SMD_TEXTURE
{
    char            FileName[ 256 ];    //!< ファイル名です.
};

//-------------------------------------------------------------------------------------------------
//      s3d::Vector2 に変換します.
//-------------------------------------------------------------------------------------------------
s3d::Vector2 Convert( SMD_VECTOR2& value )
{ return s3d::Vector2( value.x, value.y ); }

//-------------------------------------------------------------------------------------------------
//      s3d::Vector3 に変換します.
//-------------------------------------------------------------------------------------------------
s3d::Vector3 Convert( SMD_VECTOR3& value )
{ return s3d::Vector3( value.x, value.y, value.z ); }

} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
Mesh::Mesh()
: m_Count( 1 )
, m_pBVH ( nullptr )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
    SafeRelease( m_pBVH );

    for(size_t i=0; i<m_Triangles.size(); ++i)
    { SafeRelease(m_Triangles[i]); }
    
    for(size_t i=0; i<m_Materials.size(); ++i)
    { SafeRelease(m_Materials[i]); }

    m_Triangles.clear();
    m_Materials.clear();
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void Mesh::AddRef()
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------------
void Mesh::Release()
{
    m_Count--;
    if ( m_Count == 0 )
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 Mesh::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      ファイルから読み込みします.
//-------------------------------------------------------------------------------------------------
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
        ELOG( "Error : Load Faile Failed. filename = %s", filename );
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

    m_Triangles.resize( fileHeader.DataHeader.NumTriangles );
    m_Materials.resize( fileHeader.DataHeader.NumMaterials );
    m_Textures .resize( fileHeader.DataHeader.NumTextures );

    // テクスチャデータを読み込みます.
    if ( !m_Textures.empty() )
    {
        for ( size_t i = 0; i < m_Textures.size(); ++i )
        {
            SMD_TEXTURE texture;
            fread( &texture, sizeof( SMD_TEXTURE ), 1, pFile );

            std::string path = dirPath + "/" + texture.FileName;
            if ( !m_Textures[ i ].LoadFromFile( path.c_str() ) )
            {
                ILOG( "Warning : Texture Load Failed. filename = %s", path.c_str() );
            }
        }
    }

    // マテリアルデータを読み込みます.
    for ( size_t i = 0; i < m_Materials.size(); ++i )
    {
        // マテリアルタイプ読み込み.
        s32 materialType = -1;
        fread( &materialType, sizeof(s32), 1, pFile );

        switch( materialType )
        {
        case SMD_MATERIAL_TYPE_MATTE:
            {
                SMD_MATTE value;
                fread( &value, sizeof(value), 1, pFile );

                auto diffuse  = Color4(value.Color.x, value.Color.y, value.Color.z, 1.0f);
                auto emissive = Color4(value.Emissive.x, value.Emissive.y, value.Emissive.z, 1.0f);
                Texture2D*      pTexture = nullptr;
                TextureSampler* pSampler = nullptr;

                if ( value.ColorMap >= 0 )
                {
                    pTexture = &m_Textures[value.ColorMap];
                    pSampler = &m_DiffuseSmp;
                }

                m_Materials[i] = MaterialFactory::CreateLambert( diffuse, pTexture, pSampler, emissive );
            }
            break;

        case SMD_MATERIAL_TYPE_MIRROR:
            {
                SMD_MIRROR value;
                fread( &value, sizeof(value), 1, pFile );

                auto specular = Color4(value.Color.x, value.Color.y, value.Color.z, 1.0f);
                auto emissive = Color4(value.Emissive.x, value.Emissive.y, value.Emissive.z, 1.0f);
                Texture2D*      pTexture = nullptr;
                TextureSampler* pSampler = nullptr;

                if ( value.ColorMap >= 0 )
                {
                    pTexture = &m_Textures[value.ColorMap];
                    pSampler = &m_SpecularSmp;
                }

                m_Materials[i] = MaterialFactory::CreateMirror( specular, pTexture, pSampler, emissive );
            }
            break;

        case SMD_MATERIAL_TYPE_DIELECTRIC:
            {
                SMD_DIELECTRIC value;
                fread( &value, sizeof(value), 1, pFile );

                auto specular = Color4(value.Color.x, value.Color.y, value.Color.z, 1.0f);
                auto emissive = Color4(value.Emissive.x, value.Emissive.y, value.Emissive.z, 1.0f);
                auto ior      = value.Ior;
                Texture2D*      pTexture = nullptr;
                TextureSampler* pSampler = nullptr;

                if ( value.ColorMap >= 0 )
                {
                    pTexture = &m_Textures[value.ColorMap];
                    pSampler = &m_SpecularSmp;
                }

                m_Materials[i] = MaterialFactory::CreateGlass( specular, ior, pTexture, pSampler, emissive );
            }
            break;

        case SMD_MATERIAL_TYPE_GLOSSY:
            {
                SMD_GLOSSY value;
                fread( &value, sizeof(value), 1, pFile );

                auto specular = Color4(value.Color.x, value.Color.y, value.Color.z, 1.0f);
                auto emissive = Color4(value.Emissive.x, value.Emissive.y, value.Emissive.z, 1.0f);
                auto power    = value.Power;
                Texture2D*      pTexture = nullptr;
                TextureSampler* pSampler = nullptr;

                if ( value.ColorMap >= 0 )
                {
                    pTexture = &m_Textures[value.ColorMap];
                    pSampler = &m_SpecularSmp;
                }

                m_Materials[i] = MaterialFactory::CreatePhong( specular, power, pTexture, pSampler, emissive );
            }
            break;

        default:
            assert(false);
            break;
        }
    }

    // 三角形データを読み込みます.
    for ( size_t i = 0; i < m_Triangles.size(); ++i )
    {
        SMD_TRIANGLE triangle;
        fread( &triangle, sizeof( SMD_TRIANGLE ), 1, pFile );

        Vertex vertex[3];
        for(auto idx=0; idx<3; ++idx)
        {
            vertex[idx].Position = Convert(triangle.Vertex[idx].Position);
            vertex[idx].Normal   = Convert(triangle.Vertex[idx].Normal);
            vertex[idx].TexCoord = Convert(triangle.Vertex[idx].TexCoord);
        }

        auto material = (triangle.MaterialId >= 0 ) ? m_Materials[triangle.MaterialId] : nullptr;

        m_Triangles[i] = Triangle::Create(vertex, material);
    }

    // BVHを構築します.
    m_pBVH = BVH8::Create( m_Triangles.size(), m_Triangles.data() );

    return true;
}

//-------------------------------------------------------------------------------------------------
//      初期化処理を行います.
//-------------------------------------------------------------------------------------------------
bool Mesh::Init(u32 vertexCount, Vertex* pVertices, IMaterial* pMaterial)
{
    if (vertexCount % 3 != 0)
    { return false; }

    m_Materials.resize(1);
    m_Materials[0] = pMaterial;
    m_Materials[0]->AddRef();

    auto triangleCount = vertexCount / 3;

    bool failed = false;
    m_Triangles.resize(triangleCount);

    for(u32 i=0; i<triangleCount; ++i)
    {
       m_Triangles[i] = Triangle::Create(&pVertices[i * 3], pMaterial);
       if (m_Triangles[i] == nullptr)
       { failed = true; }
    }

    if (failed)
    { return false; }

    // BVHを構築します.
    m_pBVH = BVH4::Create( m_Triangles.size(), m_Triangles.data() );

    return true;
}

//-------------------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------------------
bool Mesh::IsHit( const RaySet& raySet, HitRecord& record ) const
{ return m_pBVH->IsHit( raySet, record ); }

//-------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------------
BoundingBox Mesh::GetBox() const
{ return m_pBVH->GetBox(); }

//-------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//-------------------------------------------------------------------------------------------------
Vector3 Mesh::GetCenter() const
{ return m_pBVH->GetCenter(); }

//-------------------------------------------------------------------------------------------------
//      生成処理を行います.
//-------------------------------------------------------------------------------------------------
IShape* Mesh::Create(const char* filename)
{
    auto instance = new (std::nothrow) Mesh();
    if ( instance == nullptr )
    { return nullptr; }

    if ( !instance->LoadFromFile(filename) )
    {
        SafeRelease(instance);
        return nullptr;
    }

    return instance;
}

//-------------------------------------------------------------------------------------------------
//      生成処理を行います.
//-------------------------------------------------------------------------------------------------
IShape* Mesh::Create(u32 vertexCount, Vertex* pVertices, IMaterial* pMaterial)
{
    auto instance = new (std::nothrow) Mesh();
    if ( instance == nullptr )
    { return nullptr; }

    if ( !instance->Init(vertexCount, pVertices, pMaterial) )
    {
        SafeRelease(instance);
        return nullptr;
    }

    return instance;
}

} // namespace s3d
