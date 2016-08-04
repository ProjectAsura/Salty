//----------------------------------------------------------------------------------------
// File : s3d_mesh.cpp
// Desc : Mesh Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <string>
#include <s3d_mesh.h>
#include <s3d_bvh.h>
#include <s3d_logger.h>


namespace /* anonymous */ {

//----------------------------------------------------------------------------------------
// Constant Values.
//----------------------------------------------------------------------------------------
static const u32 SMD_CURRENT_VERSION = 0x00000002;
static const u8  SMD_FILE_TAG[4]     = { 'S', 'M', 'D', '\0' };

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_MATERIAL_TYPE
//////////////////////////////////////////////////////////////////////////////////////////
enum SMD_MATERIAL_TYPE
{
    SMD_MATERIAL_TYPE_MATTE = 0,        //!< Lambert
    SMD_MATERIAL_TYPE_MIRROR,           //!< Perfect Specular
    SMD_MATERIAL_TYPE_DIELECTRIC,       //!< Dielectric
    SMD_MATERIAL_TYPE_GLOSSY,           //!< Phong
    SMD_MATERIAL_TYPE_PLASTIC,          //!< Lambert + Phong
};

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

struct SMD_VECTOR2
{
    float x;
    float y;
};

struct SMD_VECTOR3 
{
    float x;
    float y;
    float z;
};

struct SMD_VECTOR4
{
    float x;
    float y;
    float z;
    float w;
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_VERTEX structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_VERTEX
{
    SMD_VECTOR3    Position;       //!< 位置座標です.
    SMD_VECTOR3    Normal;         //!< 法線ベクトルです.
    SMD_VECTOR2    TexCoord;       //!< テクスチャ座標です.
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
// SMD_MATTE structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_MATTE
{
    SMD_VECTOR3    Color;
    SMD_VECTOR3    Emissive;
    s32            ColorMap;
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_MIRROR structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_MIRROR
{
    SMD_VECTOR3    Color;
    SMD_VECTOR3    Emissive;
    s32            ColorMap;
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_DIELECTRIC structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_DIELECTRIC
{
    SMD_VECTOR3     Color;
    f32             Ior;
    SMD_VECTOR3     Emissive;
    s32             ColorMap;
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_GLOSSY structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_GLOSSY
{
    SMD_VECTOR3     Color;
    f32             Power;
    SMD_VECTOR3     Emissive;
    s32             ColorMap;
};

//////////////////////////////////////////////////////////////////////////////////////////
// SMD_PLASTIC structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_PLASTIC
{
    SMD_VECTOR3     Diffuse;
    SMD_VECTOR3     Specular;
    f32             Power;
    SMD_VECTOR3     Emissive;
    s32             DiffuseMap;
    s32             SpcularMap;
};


//////////////////////////////////////////////////////////////////////////////////////////
// SMD_TEXTURE structure
//////////////////////////////////////////////////////////////////////////////////////////
struct SMD_TEXTURE
{
    char            FileName[ 256 ];    //!< ファイル名です.
};

s3d::Vector2 Convert(const SMD_VECTOR2& value)
{
    return s3d::Vector2(
        value.x,
        value.y);
}

s3d::Vector3 Convert(const SMD_VECTOR3& value)
{
    return s3d::Vector3(
        value.x,
        value.y,
        value.z);
}

s3d::Vector4 Convert(const SMD_VECTOR4& value)
{
    return s3d::Vector4(
        value.x,
        value.y,
        value.z,
        value.w);
}


} // namespace /* anonymous */


namespace s3d {

//////////////////////////////////////////////////////////////////////////////////////////
// Mesh class
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------------
Mesh::Mesh()
: m_pBVH        ( nullptr )
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
        auto pBVH = dynamic_cast<IDisposable*>( m_pBVH );
        if ( pBVH != nullptr )
        { pBVH->Dispose(); }
        m_pBVH = nullptr;
    }

    if ( !m_Triangles.empty() )
    {
        for( size_t i=0; i<m_Triangles.size(); ++i )
        {
            SafeDelete( m_Triangles[i] );
        }
        m_Triangles.clear();
    }

    if ( !m_Materials.empty() )
    {
        for( size_t i=0; i<m_Materials.size(); ++i )
        {
            SafeDelete( m_Materials[i] );
        }
        m_Materials.clear();
    }

    m_Textures.clear();
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

#if 0
    ILOG( "Num Triangle  : %d", m_NumTriangles );
    ILOG( "Num Materials : %d", m_NumMaterials );
    ILOG( "Num Textures  : %d", m_NumTextures );
#endif

    m_Center = Vector3( 0.0f, 0.0f, 0.0f );

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
        #if 0
            ILOG( "Texture[%d] : ", i );
            ILOG( "    File Name : %s", path.c_str() );
        #endif
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
        default:
            {
                SMD_MATTE value;
                fread( &value, sizeof(value), 1, pFile );

                auto material = new Matte( 
                    Color4(value.Color.x, value.Color.y, value.Color.z, 1.0f),
                    Color4(value.Emissive.x, value.Emissive.y, value.Emissive.z, 1.0f) );
                if ( value.ColorMap >= 0 )
                {
                    material->pTexture = &m_Textures[value.ColorMap];
                    material->pSampler = &m_DiffuseSmp;
                }

                m_Materials[i] = material;
            }
            break;

        case SMD_MATERIAL_TYPE_MIRROR:
            {
                SMD_MIRROR value;
                fread( &value, sizeof(value), 1, pFile );

                auto material = new Mirror(
                    Color4(value.Color.x, value.Color.y, value.Color.z, 1.0f),
                    Color4(value.Emissive.x, value.Emissive.y, value.Emissive.z, 1.0f) );
                if ( value.ColorMap >= 0 )
                {
                    material->pTexture = &m_Textures[value.ColorMap];
                    material->pSampler = &m_SpecularSmp;
                }

                m_Materials[i] = material;
            }
            break;

        case SMD_MATERIAL_TYPE_DIELECTRIC:
            {
                SMD_DIELECTRIC value;
                fread( &value, sizeof(value), 1, pFile );

                auto material = new Dielectric(
                    value.Ior,
                    Color4(value.Color.x, value.Color.y, value.Color.z, 1.0f),
                    Color4(value.Emissive.x, value.Emissive.y, value.Emissive.z, 1.0f) );
                if ( value.ColorMap >= 0 )
                {
                    material->pTexture = &m_Textures[value.ColorMap];
                    material->pSampler = &m_SpecularSmp;
                }

                m_Materials[i] = material;
            }
            break;

        case SMD_MATERIAL_TYPE_GLOSSY:
            {
                SMD_GLOSSY value;
                fread( &value, sizeof(value), 1, pFile );

                auto material = new Glossy(
                    Color4(value.Color.x, value.Color.y, value.Color.z, 1.0f),
                    value.Power,
                    Color4(value.Emissive.x, value.Emissive.y, value.Emissive.z, 1.0f) );
                if ( value.ColorMap >= 0 )
                {
                    material->pTexture = &m_Textures[value.ColorMap];
                    material->pSampler = &m_SpecularSmp;
                }

                m_Materials[i] = material;
            }
            break;

        case SMD_MATERIAL_TYPE_PLASTIC:
            {
                SMD_PLASTIC value;
                fread( &value, sizeof(value), 1, pFile );

                auto material = new Plastic( 
                    Color4(value.Diffuse.x, value.Diffuse.y, value.Diffuse.z, 1.0f), 
                    Color4(value.Specular.x, value.Specular.y, value.Specular.z, 1.0f),
                    value.Power,
                    Color4(value.Emissive.x, value.Emissive.y, value.Emissive.z, 1.0f) );
                if ( value.DiffuseMap >= 0 )
                {
                    material->pDiffuseMap = &m_Textures[value.DiffuseMap];
                    material->pDiffuseSmp = &m_DiffuseSmp;
                }

                m_Materials[i] = material;
            }
            break;
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

    // 三角形データを読み込みます.
    for ( size_t i = 0; i < m_Triangles.size(); ++i )
    {
        SMD_TRIANGLE triangle;
        fread( &triangle, sizeof( SMD_TRIANGLE ), 1, pFile );

        auto tri = new Triangle();
        // 頂点0
        tri->v0.pos      = Convert( triangle.Vertex[ 0 ].Position );
        tri->v0.texcoord = Convert( triangle.Vertex[ 0 ].TexCoord );
        tri->v0.normal   = Convert( triangle.Vertex[ 0 ].Normal );
        m_Center += Convert( triangle.Vertex[ 0 ].Position );

        // 頂点1
        tri->v1.pos      = Convert( triangle.Vertex[ 1 ].Position );
        tri->v1.texcoord = Convert( triangle.Vertex[ 1 ].TexCoord );
        tri->v1.normal   = Convert( triangle.Vertex[ 1 ].Normal );
        m_Center += Convert( triangle.Vertex[ 1 ].Position );

        // 頂点2
        tri->v2.pos      = Convert( triangle.Vertex[ 2 ].Position );
        tri->v2.texcoord = Convert( triangle.Vertex[ 2 ].TexCoord );
        tri->v2.normal   = Convert( triangle.Vertex[ 2 ].Normal );
        m_Center += Convert( triangle.Vertex[ 2 ].Position );

        if ( triangle.MaterialId >= 0 )
        {
            tri->pMaterial = m_Materials[ triangle.MaterialId ];
        }

        m_Triangles[ i ] = tri;

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

    m_Center /= ( m_Triangles.size() * 3.0f );

    // BVHを構築します.
#if S3D_IS_AVX
    m_pBVH = OBVH::BuildBranch( &m_Triangles[0], u32(m_Triangles.size()));
#else
    m_pBVH = QBVH::BuildBranch( &m_Triangles[0], u32(m_Triangles.size()));
#endif

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

//----------------------------------------------------------------------------------------
//      中心座標を取得します.
//----------------------------------------------------------------------------------------
Vector3 Mesh::GetCenter() const
{ return m_Center; }

} // namespace s3d
