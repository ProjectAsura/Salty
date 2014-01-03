//---------------------------------------------------------------------------------
// File : s3d_scene.cpp
// Desc : Scene Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------
#include <s3d_scene.h>
#include <s3d_bvh.h>
#include <s3d_mesh.h>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <new>


namespace /* anonymous */ {

//---------------------------------------------------------------------------------
// Constant Values
//---------------------------------------------------------------------------------
static const char       SSD_FILE_MAGIC[4]   = { 'S', 'S', 'D', '\0' };      //!< ファイルマジックです.
static const u32        SSD_CURRENT_VERSION = 0x00000001;                   //!< ファイルバージョンです.
static const u32        SSD_INVALID_ID      = 0xffffffff;                   //!< 無効とみなすIDです.


///////////////////////////////////////////////////////////////////////////////////
// SSD_SHAPE_TYPE enum
///////////////////////////////////////////////////////////////////////////////////
enum SSD_SHAPE_TYPE
{
    SSD_SHAPE_SPHERE = 1,       //!< 球です.
    SSD_SHAPE_TRIANGLE,         //!< 三角形です.
    SSD_SHAPE_QUAD,             //!< 四角形です.
    SSD_SHAPE_MESH,             //!< メッシュです.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_MATERIAL_TYPE enum
///////////////////////////////////////////////////////////////////////////////////
enum SSD_MATERIAL_TYPE
{
    SSD_MATERIAL_MATTE = 1,     //!< マット(完全拡散反射)です.
    SSD_MATERIAL_MIRROR,        //!< ミラー(完全鏡面反射)です.
    SSD_MATERIAL_GLASS,         //!< ガラス(屈折)です.
    SSD_MATERIAL_GLOSSY,        //!< グロッシー(鏡面反射)です.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_CAMERA_TYPE enum
///////////////////////////////////////////////////////////////////////////////////
enum SSD_CAMERA_TYPE
{
    SSD_CAMERA_PINHOLE = 1,     //!< ピンホールカメラです.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_DATA_HEADER structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_DATA_HEADER
{
    u32     NumTextures;        //!< テクスチャ数です.
    u32     NumSamplers;        //!< テクスチャサンプラー数です.
    u32     NumMaterials;       //!< マテリアル数です.
    u32     NumShapes;          //!< シェイプ数です.
    u32     NumCameras;         //!< カメラ数です.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_FILE_HEADER
////////////////////////////////////////////////////////////////////////////////////
struct SSD_FILE_HEADER
{
    char            Magic[ 4 ];     //!< マジック( 'S', 'S', 'D', '\0' )です.
    u32             Version;        //!< ファイルバージョンです.
    u32             DataHeaderSize; //!< データヘッダのサイズです.
    SSD_DATA_HEADER DataHeader;     //!< データヘッダです.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_SPHERE_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_SPHERE_DATA
{
    f32             Radius;     //!< 半径です.
    s3d::Vector3    Position;   //!< 位置座標です.
    u32             MaterialId; //!< マテリアルIDです.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_TRIANGLE_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_TRIANGLE_DATA
{
    s3d::Vertex     Vertex0;        //!< 頂点0です.
    s3d::Vertex     Vertex1;        //!< 頂点1です.
    s3d::Vertex     Vertex2;        //!< 頂点2です.
    u32             MaterialId;     //!< マテリアルIDです.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_QUAD_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_QUAD_DATA
{
    s3d::Vertex     Vertex0;        //!< 頂点0です.
    s3d::Vertex     Vertex1;        //!< 頂点1です.
    s3d::Vertex     Vertex2;        //!< 頂点3です.
    s3d::Vertex     Vertex3;        //!< 頂点4です.
    u32             MaterialId;     //!< マテリアルIDです.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_MESH_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_MESH_DATA
{
    char            FileName[ 256 ];    //!< ファイル名です.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_MATTE_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_MATTE_DATA
{
    s3d::Color      Color;          //!< カラーです.
    s3d::Color      Emissive;       //!< 自己照明色です.
    u32             TextureId;      //!< テクスチャIDです.
    u32             SamplerId;      //!< サンプラーIDです.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_MIRROR_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_MIRROR_DATA
{
    s3d::Color      Color;          //!< カラーです.
    s3d::Color      Emissive;       //!< 自己照明色です.
    u32             TextureId;      //!< テクスチャIDです.
    u32             SamplerId;      //!< サンプラーIDです.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_GLASS_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_GLASS_DATA
{
    f32             Ior;            //!< 屈折率です.
    s3d::Color      Color;          //!< カラーです.
    s3d::Color      Emissive;       //!< 自己照明色です.
    u32             TextureId;      //!< テクスチャIDです.
    u32             SamplerId;      //!< サンプラーIDです.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_GLOSSY_DATA structure
////////////////////////////////////////////////////////////////////////////////////
struct SSD_GLOSSY_DATA
{
    s3d::Color      Specular;       //!< 鏡面反射色です.
    f32             Power;          //!< 鏡面反射強度です.
    s3d::Color      Emissive;       //!< 自己照明色です.
    u32             TextureId;      //!< テクスチャIDです.
    u32             SamplerId;      //!< サンプラーIDです.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_TEXTURE_DATA structure
////////////////////////////////////////////////////////////////////////////////////
struct SSD_TEXTURE_DATA
{
    char            FileName[ 256 ];    //!< ファイル名です.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_SAMPLER_DATA structure
////////////////////////////////////////////////////////////////////////////////////
struct SSD_SAMPLER_DATA
{
    u32             AddressU;       //!< U方向のテクスチャアドレッシングモードです.
    u32             AddressV;       //!< V方向のテクスチャアドレッシングモードです.
    s3d::Color      BorderColor;    //!< ボーダーカラーです.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_PINHOLE_CAMERA_DATA structure
////////////////////////////////////////////////////////////////////////////////////
struct SSD_PINHOLE_CAMERA_DATA
{
    s3d::Vector3    Position;       //!< 位置です.
    s3d::Vector3    Target;         //!< 注視点です.
    s3d::Vector3    Upward;         //!< 上向き方向ベクトルです.
    f32             FieldOfView;    //!< 垂直画角です.
    f32             AspectRatio;    //!< アスペクト比です.
    f32             NearClip;       //!< ニアクリップ平面までの距離です.
    f32             FarClip;        //!< ファークリップ平面までの距離です.
};


/*
    データ内部は下記の通り

    SSD_FILE_HEADER
    SSD_DATA_HEADER
    SSD_TEXTURE_DATA
    SSD_TEXTURE_DATA
    ...
    SSD_SAMPLER_DATA
    SSD_SAMPLER_DATA
    ...
    SSD_MATERIAL_TYPE,   Material0
    SSD_MATERIAL_TYPE,   Material1
    ...
    SSD_SHAPE_TYPE,      ShapeData0
    SSD_SHAPE_TYPE,      ShapeData1
    ...
    SSD_CAMERA_TYPE,     Camera0
*/

} // namespace /* anonymous */


namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// Scene class
////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------
Scene::Scene()
: m_NumShapes   ( 0 )
, m_NumMaterials( 0 )
, m_NumTextures ( 0 )
, m_NumSamplers ( 0 )
, m_ppShapes    ( nullptr )
, m_ppMaterials ( nullptr )
, m_pTextures   ( nullptr )
, m_pSamplers   ( nullptr )
, m_pCamera     ( nullptr )
, m_pBVH        ( nullptr )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      デストラクタです.
//----------------------------------------------------------------------------------
Scene::~Scene()
{
    Clear();
}

//----------------------------------------------------------------------------------
//      クリアします.
//----------------------------------------------------------------------------------
void Scene::Clear()
{
    // BVHをぶっ壊す.
    if ( m_pBVH )
    {
        BVH::DestroyBranch( (BVH*)m_pBVH );
        m_pBVH = nullptr;
    }

    // 各シェイプのメモリを解放.
    if ( m_ppShapes )
    {
        for( u32 i=0; i<m_NumShapes; ++i )
        { S3D_DELETE( m_ppShapes[i] ); }

        delete[] m_ppShapes;
        m_ppShapes = nullptr;
    }

    // 各マテリアルのメモリを解放.
    if ( m_ppMaterials )
    {
        for( u32 i=0; i<m_NumMaterials; ++i )
        { S3D_DELETE( m_ppMaterials[i] ); }

        delete[] m_ppMaterials;
        m_ppMaterials = nullptr;
    }

    // 各テクスチャのメモリを解放.
    if ( m_pTextures )
    {
        for( u32 i=0; i<m_NumTextures; ++i )
        { m_pTextures[i].Release(); }

        delete[] m_pTextures;
        m_pTextures = nullptr;
    }

    // 各サンプラーのメモリを解放.
    S3D_DELETE_ARRAY( m_pSamplers );

    // カメラのメモリを解放.
    S3D_DELETE( m_pCamera );

    // サイズをゼロにする.
    m_NumShapes     = 0;
    m_NumMaterials  = 0;
    m_NumTextures   = 0;
    m_NumSamplers   = 0;
}

//----------------------------------------------------------------------------------
//      ファイルから読み込みします.
//----------------------------------------------------------------------------------
bool Scene::LoadFromFile( const char* filename )
{
    FILE* pFile;

    // ファイルを開く.
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        return false;
    }

    // ファイルヘッダー読み取り.
    SSD_FILE_HEADER fileHeader;
    fread( &fileHeader, sizeof(fileHeader), 1, pFile );

    // ファイルマジックをチェック.
    if ( strcmp( fileHeader.Magic, SSD_FILE_MAGIC ) != 0 )
    {
        fclose( pFile );
        return false;
    }

    // ファイルバージョンをチェック.
    if ( fileHeader.Version != SSD_CURRENT_VERSION )
    {
        fclose( pFile );
        return false;
    }

    // データヘッダのサイズをチェック.
    if ( fileHeader.DataHeaderSize != sizeof(SSD_DATA_HEADER) )
    {
        fclose( pFile );
        return false;
    }

    // サイズを設定.
    u32 numShape    = fileHeader.DataHeader.NumShapes;
    u32 numMaterial = fileHeader.DataHeader.NumMaterials;
    u32 numTexture  = fileHeader.DataHeader.NumTextures;
    u32 numSampler  = fileHeader.DataHeader.NumSamplers;
    u32 numCamera   = fileHeader.DataHeader.NumCameras;
    assert( numShape    > 0 );
    assert( numMaterial > 0 );
    assert( numCamera == 1 );

    // メモリ確保.
    try
    {
        m_ppShapes    = new IShape*[ numShape ];
        m_ppMaterials = new IMaterial*[ numMaterial ];

        if ( numTexture > 0 )
        { m_pTextures   = new Texture2D[ numTexture ]; }
        if ( numSampler > 0 )
        { m_pSamplers   = new TextureSampler[ numSampler ]; }

        m_pCamera = new Camera;
    }
    catch ( std::bad_alloc ba )
    {
        S3D_DELETE_ARRAY( m_ppShapes );
        S3D_DELETE_ARRAY( m_ppMaterials );
        S3D_DELETE_ARRAY( m_pTextures );
        S3D_DELETE_ARRAY( m_pSamplers );
        S3D_DELETE( m_pCamera );

        // ファイルを閉じる.
        fclose( pFile );

        // 異常終了.
        return false;
    }

    // テクスチャデータを読み込む.
    for( u32 i=0; i<numTexture; ++i )
    {
        SSD_TEXTURE_DATA data;
        fread( &data, sizeof(data), 1, pFile );

        m_pTextures[i].LoadFromFile( data.FileName );
    }

    // サンプラーデータを読み込む.
    for( u32 i=0; i<numSampler; ++i )
    {
        SSD_SAMPLER_DATA data;
        fread( &data, sizeof(data), 1, pFile );

        m_pSamplers[i] = TextureSampler( 
            (TEXTURE_ADDRESS_MODE)data.AddressU,
            (TEXTURE_ADDRESS_MODE)data.AddressV,
            data.BorderColor );
    }

    // マテリアルデータを読み込む.
    for( u32 i=0; i<numMaterial; ++i )
    {
        // タイプを取得.
        u32 type = 0;
        fread( &type, sizeof(u32), 1, pFile );

        Texture2D*      pTexture = nullptr;
        TextureSampler* pSampler = nullptr;

        // タイプからデータを判別する.
        switch( type )
        {
            // 拡散反射.
            case SSD_MATERIAL_MATTE:
            {
                SSD_MATTE_DATA data;
                fread( &data, sizeof(data), 1, pFile );

                if ( data.TextureId != SSD_INVALID_ID )
                { pTexture = &m_pTextures[data.TextureId]; }

                if ( data.SamplerId != SSD_INVALID_ID )
                { pSampler = &m_pSamplers[data.SamplerId]; }
                
                m_ppMaterials[i] = new Matte(
                    data.Color,
                    data.Emissive,
                    pTexture,
                    pSampler );
            }
            break;

            // 完全鏡面反射.
            case SSD_MATERIAL_MIRROR:
            {
                SSD_MIRROR_DATA data;
                fread( &data, sizeof(data), 1, pFile );

                if ( data.TextureId != SSD_INVALID_ID )
                { pTexture = &m_pTextures[data.TextureId]; }

                if ( data.SamplerId != SSD_INVALID_ID )
                { pSampler = &m_pSamplers[data.SamplerId]; }

                m_ppMaterials[i] = new Mirror(
                    data.Color,
                    data.Emissive,
                    pTexture,
                    pSampler );
            }
            break;

            // 屈折.
            case SSD_MATERIAL_GLASS:
            {
                SSD_GLASS_DATA data;
                fread( &data, sizeof(data), 1, pFile );

                if ( data.TextureId != SSD_INVALID_ID )
                { pTexture = &m_pTextures[data.TextureId]; }

                if ( data.SamplerId != SSD_INVALID_ID )
                { pSampler = &m_pSamplers[data.SamplerId]; }

                m_ppMaterials[i] =  new Glass(
                    data.Ior,
                    data.Color,
                    data.Emissive,
                    pTexture,
                    pSampler );
            }
            break;

            // 鏡面反射.
            case SSD_MATERIAL_GLOSSY:
            {
                SSD_GLOSSY_DATA data;
                fread( &data, sizeof(data), 1, pFile  );

                if ( data.TextureId != SSD_INVALID_ID )
                { pTexture = &m_pTextures[data.TextureId]; }

                if ( data.SamplerId != SSD_INVALID_ID )
                { pSampler = &m_pSamplers[data.SamplerId]; }

                m_ppMaterials[i] = new Glossy(
                    data.Specular,
                    data.Power,
                    data.Emissive,
                    pTexture,
                    pSampler );
            }
            break;

            default:
            {
            }
            break;
        }
    }

    // シェイプデータを読み込む.
    for( u32 i=0; i<numShape; ++i )
    {
        // タイプを読み込む.
        u32 type = 0;
        fread( &type, sizeof(u32), 1, pFile );

        IMaterial* pMaterial = nullptr;

        // タイプからデータを判別する.
        switch( type )
        {
            // 球.
            case SSD_SHAPE_SPHERE:
            {
                SSD_SPHERE_DATA data;
                fread( &data, sizeof(data), 1, pFile );

                if ( data.MaterialId != SSD_INVALID_ID )
                { pMaterial = m_ppMaterials[ data.MaterialId ]; }

                m_ppShapes[i] = new Sphere( 
                    data.Radius,
                    data.Position, 
                    pMaterial );
            }
            break;

            // 三角形.
            case SSD_SHAPE_TRIANGLE:
            {
                SSD_TRIANGLE_DATA data;
                fread( &data, sizeof(data), 1, pFile );

                if ( data.MaterialId != SSD_INVALID_ID )
                { pMaterial = m_ppMaterials[ data.MaterialId ]; }

                m_ppShapes[i] = new Triangle(
                    data.Vertex0,
                    data.Vertex1,
                    data.Vertex2,
                    pMaterial );
            }
            break;

            // 四角形.
            case SSD_SHAPE_QUAD:
            {
                SSD_QUAD_DATA data;
                fread( &data, sizeof(data), 1, pFile );

                if ( data.MaterialId != SSD_INVALID_ID )
                { pMaterial = m_ppMaterials[ data.MaterialId ]; }

                m_ppShapes[i] = new Quad(
                    data.Vertex0,
                    data.Vertex1,
                    data.Vertex2,
                    data.Vertex3,
                    pMaterial );
            }
            break;

            // メッシュ.
            case SSD_SHAPE_MESH:
            {
                SSD_MESH_DATA data;
                fread( &data, sizeof(data), 1, pFile );

                Mesh* pMesh = new Mesh ();
                if ( pMesh->LoadFromFile( data.FileName ) )
                {
                    m_ppShapes[i] = pMesh;
                }
                else
                {
                    pMesh->Release();
                    delete pMesh;
                    pMesh = nullptr;
                }
            }
            break;

            default:
            {
            }
            break;
        }
    }

    // カメラデータを読み込む.
    for( u32 i=0; i<numCamera; ++i )
    {
        // タイプを読み込む.
        u32 type = 0;
        fread( &type, sizeof(type), 1, pFile );

        // タイプからデータを判別する.
        switch( type )
        {
            // ピンホールカメラ.
            case SSD_CAMERA_PINHOLE:
            {
                SSD_PINHOLE_CAMERA_DATA data;
                fread( &data, sizeof(data), 1, pFile );

                if ( !m_pCamera )
                {
                    m_pCamera = new Camera();
                }

                m_pCamera->Update( 
                    data.Position,
                    data.Target,
                    data.Upward,
                    data.FieldOfView,
                    data.AspectRatio,
                    data.NearClip );
            }
            break;

            default:
            {
            }
            break;
        }
    }

    // ファイルを閉じる.
    fclose( pFile );

    // サイズを設定.
    m_NumShapes    = numShape;
    m_NumMaterials = numMaterial;
    m_NumTextures  = numTexture;
    m_NumSamplers  = numSampler;

    // BVHを構築する.
    m_pBVH = BVH::BuildBranch( m_ppShapes, m_NumShapes );

    // 正常終了.
    return true;
}

//----------------------------------------------------------------------------------
//      交差判定を行います.
//----------------------------------------------------------------------------------
bool Scene::Intersect( const Ray& ray, HitRecord& record )
{
    assert( m_pBVH != nullptr );
    HitRecord temp;

    // 交差判定.
    if ( m_pBVH->IsHit( ray, temp ) )
    { record = temp; }

    return (record.pShape != nullptr);
}

//----------------------------------------------------------------------------------
//      スクリーンまでへのレイを取得します.
//----------------------------------------------------------------------------------
Ray Scene::GetRay( const f32 x, const f32 y )
{
    assert( m_pCamera != nullptr );
    return m_pCamera->GetRay( x, y );
}


} // namespace s3d