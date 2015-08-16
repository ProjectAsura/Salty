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
    s3d::Vector3    Specular;           //!< 鏡面反射成分です.
    f32             Power;              //!< 鏡面反射強度です.
    s3d::Vector3    Emissive;           //!< 自己照明成分です.
    s32             DiffuseMap;         //!< ディフューズマップです.
    s32             SpecularMap;        //!< スペキュラーマップです.
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
    const Color4&    diffuse,
    const Color4&    specular,
    const f32        power,
    const Color4&    emissive
)
: Diffuse       ( diffuse )
, Specular      ( specular )
, Power         ( power )
, Emissive      ( emissive )
, pDiffuseMap   ( nullptr )
, pSpecularSmp  ( nullptr )
{
    Threshold = Max( diffuse.x, diffuse.y );
    Threshold = Max( diffuse.z, Threshold );
}

//---------------------------------------------------------------------------------------
//      自己照明成分を取得します.
//---------------------------------------------------------------------------------------
Color4 MeshMaterial::GetEmissive() const
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

bool MeshMaterial::AlphaTest( const Vector2& texcoord, const f32 value ) const
{
    if ( pDiffuseMap == nullptr || pDiffuseSmp == nullptr )
    { return true; }

    return pDiffuseMap->AlphaTest( (*pDiffuseSmp), texcoord, value );
}

#if 1
Color4 MeshMaterial::GetDebugColor() const
{ return Diffuse; }
#endif

//---------------------------------------------------------------------------------------
//      色を計算します.
//---------------------------------------------------------------------------------------
Color4 MeshMaterial::ComputeColor( ShadingArg& arg ) const
{
    // 補正済み法線データ (レイの入出を考慮済み).
    auto cosine = Vector3::Dot( arg.normal, arg.input );
    const Vector3 normalMod = ( cosine < 0.0 ) ? arg.normal : -arg.normal;
    if ( cosine < 0.0f ) cosine = -cosine;

    auto temp1 = 1.0f - cosine;
    const auto R0 = 0.5f;
    auto R = R0 + ( 1.0f - R0 ) * temp1 * temp1 * temp1 * temp1 * temp1;
    auto P = ( R + 0.5f ) / 2.0f;

    if ( arg.random.GetAsF32() <= P )
    {
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
        Color4 result;
        if ( pDiffuseMap != nullptr )
        { result = Color4::Mul( Diffuse, pDiffuseMap->Sample( ( *pDiffuseSmp ), arg.texcoord ) ) * R / P; }
        else
        { result = Diffuse  * R / P; }

        return result;
    }
    else
    {
        // インポータンスサンプリング.
        const f32 phi = F_2PI * arg.random.GetAsF32();
        const f32 cosTheta = powf( 1.0f - arg.random.GetAsF32(), 1.0f / ( Power + 1.0f ) );
        const f32 sinTheta = SafeSqrt( 1.0f - ( cosTheta * cosTheta ) );
        const f32 x = cosf( phi ) * sinTheta;
        const f32 y = sinf( phi ) * sinTheta;
        const f32 z = cosTheta;

        // 反射ベクトル.
        Vector3 w = Vector3::Reflect( arg.input, normalMod );
        w.Normalize();

        // 基底ベクトルを求める.
        OrthonormalBasis onb;
        onb.InitFromW( w );

        // 出射方向.
        auto dir = Vector3::UnitVector( onb.u * x + onb.v * y + onb.w * z );

        // 出射方向と法線ベクトルの内積を求める.
        auto dots = Vector3::Dot( dir, normalMod );

        arg.output = dir;

        // 重み更新.
        if ( pSpecularMap != nullptr && pSpecularSmp != nullptr )
        {
            return Color4::Mul( Specular, pSpecularMap->Sample( (*pSpecularSmp), arg.texcoord ) ) * dots * ( 1.0f - R ) / ( 1.0f - P );
        }

        return Specular * dots * ( 1.0f - R ) / ( 1.0f - P );
    }
}


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

    m_Materials.clear();
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

    // 三角形データを読み込みます.
    for ( size_t i = 0; i < m_Triangles.size(); ++i )
    {
        SMD_TRIANGLE triangle;
        fread( &triangle, sizeof( SMD_TRIANGLE ), 1, pFile );

        auto tri = new Triangle();
        // 頂点0
        tri->v0.pos      = triangle.Vertex[ 0 ].Position;
        tri->v0.texcoord = triangle.Vertex[ 0 ].TexCoord;
        tri->v0.normal   = triangle.Vertex[ 0 ].Normal;
        m_Center += triangle.Vertex[ 0 ].Position;

        // 頂点1
        tri->v1.pos      = triangle.Vertex[ 1 ].Position;
        tri->v1.texcoord = triangle.Vertex[ 1 ].TexCoord;
        tri->v1.normal   = triangle.Vertex[ 1 ].Normal;
        m_Center += triangle.Vertex[ 1 ].Position;

        // 頂点2
        tri->v2.pos      = triangle.Vertex[ 2 ].Position;
        tri->v2.texcoord = triangle.Vertex[ 2 ].TexCoord;
        tri->v2.normal   = triangle.Vertex[ 2 ].Normal;
        m_Center += triangle.Vertex[ 2 ].Position;

        if ( triangle.MaterialId >= 0 )
        {
            tri->pMaterial = &m_Materials[ triangle.MaterialId ];
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

    // マテリアルデータを読み込みます.
    for ( size_t i = 0; i < m_Materials.size(); ++i )
    {
        SMD_MATERIAL material;
        fread( &material, sizeof( SMD_MATERIAL ), 1, pFile );

        Color4 diffuse;
        diffuse.x = material.Diffuse.x;
        diffuse.y = material.Diffuse.y;
        diffuse.z = material.Diffuse.z;
        diffuse.w = 1.0f;

        Color4 emissive;
        emissive.x = material.Emissive.x;
        emissive.y = material.Emissive.y;
        emissive.z = material.Emissive.z;
        emissive.w = 1.0f;

        Color4 specular;
        specular.x = material.Specular.x;
        specular.y = material.Specular.y;
        specular.z = material.Specular.z;
        specular.w = 1.0f;

        m_Materials[i] = MeshMaterial( diffuse, specular, material.Power, emissive );
        if ( material.DiffuseMap >= 0 )
        {
            m_Materials[ i ].pDiffuseMap = &m_Textures[ material.DiffuseMap ];
            m_Materials[ i ].pDiffuseSmp = &m_DiffuseSmp;
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
                for( size_t j=0; j<m_Materials.size(); ++j )
                {
                    if ( m_Materials[j].pDiffuseMap == (&m_Textures[i]) )
                    {
                        m_Materials[j].pDiffuseMap = nullptr;
                        m_Materials[j].pDiffuseSmp = nullptr;
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
    m_pBVH = OBVH::BuildBranch( &m_Triangles[0], static_cast<u32>(m_Triangles.size()) );

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
