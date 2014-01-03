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
static const char       SSD_FILE_MAGIC[4]   = { 'S', 'S', 'D', '\0' };      //!< �t�@�C���}�W�b�N�ł�.
static const u32        SSD_CURRENT_VERSION = 0x00000001;                   //!< �t�@�C���o�[�W�����ł�.
static const u32        SSD_INVALID_ID      = 0xffffffff;                   //!< �����Ƃ݂Ȃ�ID�ł�.


///////////////////////////////////////////////////////////////////////////////////
// SSD_SHAPE_TYPE enum
///////////////////////////////////////////////////////////////////////////////////
enum SSD_SHAPE_TYPE
{
    SSD_SHAPE_SPHERE = 1,       //!< ���ł�.
    SSD_SHAPE_TRIANGLE,         //!< �O�p�`�ł�.
    SSD_SHAPE_QUAD,             //!< �l�p�`�ł�.
    SSD_SHAPE_MESH,             //!< ���b�V���ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_MATERIAL_TYPE enum
///////////////////////////////////////////////////////////////////////////////////
enum SSD_MATERIAL_TYPE
{
    SSD_MATERIAL_MATTE = 1,     //!< �}�b�g(���S�g�U����)�ł�.
    SSD_MATERIAL_MIRROR,        //!< �~���[(���S���ʔ���)�ł�.
    SSD_MATERIAL_GLASS,         //!< �K���X(����)�ł�.
    SSD_MATERIAL_GLOSSY,        //!< �O���b�V�[(���ʔ���)�ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_CAMERA_TYPE enum
///////////////////////////////////////////////////////////////////////////////////
enum SSD_CAMERA_TYPE
{
    SSD_CAMERA_PINHOLE = 1,     //!< �s���z�[���J�����ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_DATA_HEADER structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_DATA_HEADER
{
    u32     NumTextures;        //!< �e�N�X�`�����ł�.
    u32     NumSamplers;        //!< �e�N�X�`���T���v���[���ł�.
    u32     NumMaterials;       //!< �}�e���A�����ł�.
    u32     NumShapes;          //!< �V�F�C�v���ł�.
    u32     NumCameras;         //!< �J�������ł�.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_FILE_HEADER
////////////////////////////////////////////////////////////////////////////////////
struct SSD_FILE_HEADER
{
    char            Magic[ 4 ];     //!< �}�W�b�N( 'S', 'S', 'D', '\0' )�ł�.
    u32             Version;        //!< �t�@�C���o�[�W�����ł�.
    u32             DataHeaderSize; //!< �f�[�^�w�b�_�̃T�C�Y�ł�.
    SSD_DATA_HEADER DataHeader;     //!< �f�[�^�w�b�_�ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_SPHERE_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_SPHERE_DATA
{
    f32             Radius;     //!< ���a�ł�.
    s3d::Vector3    Position;   //!< �ʒu���W�ł�.
    u32             MaterialId; //!< �}�e���A��ID�ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_TRIANGLE_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_TRIANGLE_DATA
{
    s3d::Vertex     Vertex0;        //!< ���_0�ł�.
    s3d::Vertex     Vertex1;        //!< ���_1�ł�.
    s3d::Vertex     Vertex2;        //!< ���_2�ł�.
    u32             MaterialId;     //!< �}�e���A��ID�ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_QUAD_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_QUAD_DATA
{
    s3d::Vertex     Vertex0;        //!< ���_0�ł�.
    s3d::Vertex     Vertex1;        //!< ���_1�ł�.
    s3d::Vertex     Vertex2;        //!< ���_3�ł�.
    s3d::Vertex     Vertex3;        //!< ���_4�ł�.
    u32             MaterialId;     //!< �}�e���A��ID�ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_MESH_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_MESH_DATA
{
    char            FileName[ 256 ];    //!< �t�@�C�����ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_MATTE_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_MATTE_DATA
{
    s3d::Color      Color;          //!< �J���[�ł�.
    s3d::Color      Emissive;       //!< ���ȏƖ��F�ł�.
    u32             TextureId;      //!< �e�N�X�`��ID�ł�.
    u32             SamplerId;      //!< �T���v���[ID�ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_MIRROR_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_MIRROR_DATA
{
    s3d::Color      Color;          //!< �J���[�ł�.
    s3d::Color      Emissive;       //!< ���ȏƖ��F�ł�.
    u32             TextureId;      //!< �e�N�X�`��ID�ł�.
    u32             SamplerId;      //!< �T���v���[ID�ł�.
};

///////////////////////////////////////////////////////////////////////////////////
// SSD_GLASS_DATA structure
///////////////////////////////////////////////////////////////////////////////////
struct SSD_GLASS_DATA
{
    f32             Ior;            //!< ���ܗ��ł�.
    s3d::Color      Color;          //!< �J���[�ł�.
    s3d::Color      Emissive;       //!< ���ȏƖ��F�ł�.
    u32             TextureId;      //!< �e�N�X�`��ID�ł�.
    u32             SamplerId;      //!< �T���v���[ID�ł�.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_GLOSSY_DATA structure
////////////////////////////////////////////////////////////////////////////////////
struct SSD_GLOSSY_DATA
{
    s3d::Color      Specular;       //!< ���ʔ��ːF�ł�.
    f32             Power;          //!< ���ʔ��ˋ��x�ł�.
    s3d::Color      Emissive;       //!< ���ȏƖ��F�ł�.
    u32             TextureId;      //!< �e�N�X�`��ID�ł�.
    u32             SamplerId;      //!< �T���v���[ID�ł�.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_TEXTURE_DATA structure
////////////////////////////////////////////////////////////////////////////////////
struct SSD_TEXTURE_DATA
{
    char            FileName[ 256 ];    //!< �t�@�C�����ł�.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_SAMPLER_DATA structure
////////////////////////////////////////////////////////////////////////////////////
struct SSD_SAMPLER_DATA
{
    u32             AddressU;       //!< U�����̃e�N�X�`���A�h���b�V���O���[�h�ł�.
    u32             AddressV;       //!< V�����̃e�N�X�`���A�h���b�V���O���[�h�ł�.
    s3d::Color      BorderColor;    //!< �{�[�_�[�J���[�ł�.
};

////////////////////////////////////////////////////////////////////////////////////
// SSD_PINHOLE_CAMERA_DATA structure
////////////////////////////////////////////////////////////////////////////////////
struct SSD_PINHOLE_CAMERA_DATA
{
    s3d::Vector3    Position;       //!< �ʒu�ł�.
    s3d::Vector3    Target;         //!< �����_�ł�.
    s3d::Vector3    Upward;         //!< ����������x�N�g���ł�.
    f32             FieldOfView;    //!< ������p�ł�.
    f32             AspectRatio;    //!< �A�X�y�N�g��ł�.
    f32             NearClip;       //!< �j�A�N���b�v���ʂ܂ł̋����ł�.
    f32             FarClip;        //!< �t�@�[�N���b�v���ʂ܂ł̋����ł�.
};


/*
    �f�[�^�����͉��L�̒ʂ�

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
//      �R���X�g���N�^�ł�.
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
//      �f�X�g���N�^�ł�.
//----------------------------------------------------------------------------------
Scene::~Scene()
{
    Clear();
}

//----------------------------------------------------------------------------------
//      �N���A���܂�.
//----------------------------------------------------------------------------------
void Scene::Clear()
{
    // BVH���Ԃ���.
    if ( m_pBVH )
    {
        BVH::DestroyBranch( (BVH*)m_pBVH );
        m_pBVH = nullptr;
    }

    // �e�V�F�C�v�̃����������.
    if ( m_ppShapes )
    {
        for( u32 i=0; i<m_NumShapes; ++i )
        { S3D_DELETE( m_ppShapes[i] ); }

        delete[] m_ppShapes;
        m_ppShapes = nullptr;
    }

    // �e�}�e���A���̃����������.
    if ( m_ppMaterials )
    {
        for( u32 i=0; i<m_NumMaterials; ++i )
        { S3D_DELETE( m_ppMaterials[i] ); }

        delete[] m_ppMaterials;
        m_ppMaterials = nullptr;
    }

    // �e�e�N�X�`���̃����������.
    if ( m_pTextures )
    {
        for( u32 i=0; i<m_NumTextures; ++i )
        { m_pTextures[i].Release(); }

        delete[] m_pTextures;
        m_pTextures = nullptr;
    }

    // �e�T���v���[�̃����������.
    S3D_DELETE_ARRAY( m_pSamplers );

    // �J�����̃����������.
    S3D_DELETE( m_pCamera );

    // �T�C�Y���[���ɂ���.
    m_NumShapes     = 0;
    m_NumMaterials  = 0;
    m_NumTextures   = 0;
    m_NumSamplers   = 0;
}

//----------------------------------------------------------------------------------
//      �t�@�C������ǂݍ��݂��܂�.
//----------------------------------------------------------------------------------
bool Scene::LoadFromFile( const char* filename )
{
    FILE* pFile;

    // �t�@�C�����J��.
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        return false;
    }

    // �t�@�C���w�b�_�[�ǂݎ��.
    SSD_FILE_HEADER fileHeader;
    fread( &fileHeader, sizeof(fileHeader), 1, pFile );

    // �t�@�C���}�W�b�N���`�F�b�N.
    if ( strcmp( fileHeader.Magic, SSD_FILE_MAGIC ) != 0 )
    {
        fclose( pFile );
        return false;
    }

    // �t�@�C���o�[�W�������`�F�b�N.
    if ( fileHeader.Version != SSD_CURRENT_VERSION )
    {
        fclose( pFile );
        return false;
    }

    // �f�[�^�w�b�_�̃T�C�Y���`�F�b�N.
    if ( fileHeader.DataHeaderSize != sizeof(SSD_DATA_HEADER) )
    {
        fclose( pFile );
        return false;
    }

    // �T�C�Y��ݒ�.
    u32 numShape    = fileHeader.DataHeader.NumShapes;
    u32 numMaterial = fileHeader.DataHeader.NumMaterials;
    u32 numTexture  = fileHeader.DataHeader.NumTextures;
    u32 numSampler  = fileHeader.DataHeader.NumSamplers;
    u32 numCamera   = fileHeader.DataHeader.NumCameras;
    assert( numShape    > 0 );
    assert( numMaterial > 0 );
    assert( numCamera == 1 );

    // �������m��.
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

        // �t�@�C�������.
        fclose( pFile );

        // �ُ�I��.
        return false;
    }

    // �e�N�X�`���f�[�^��ǂݍ���.
    for( u32 i=0; i<numTexture; ++i )
    {
        SSD_TEXTURE_DATA data;
        fread( &data, sizeof(data), 1, pFile );

        m_pTextures[i].LoadFromFile( data.FileName );
    }

    // �T���v���[�f�[�^��ǂݍ���.
    for( u32 i=0; i<numSampler; ++i )
    {
        SSD_SAMPLER_DATA data;
        fread( &data, sizeof(data), 1, pFile );

        m_pSamplers[i] = TextureSampler( 
            (TEXTURE_ADDRESS_MODE)data.AddressU,
            (TEXTURE_ADDRESS_MODE)data.AddressV,
            data.BorderColor );
    }

    // �}�e���A���f�[�^��ǂݍ���.
    for( u32 i=0; i<numMaterial; ++i )
    {
        // �^�C�v���擾.
        u32 type = 0;
        fread( &type, sizeof(u32), 1, pFile );

        Texture2D*      pTexture = nullptr;
        TextureSampler* pSampler = nullptr;

        // �^�C�v����f�[�^�𔻕ʂ���.
        switch( type )
        {
            // �g�U����.
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

            // ���S���ʔ���.
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

            // ����.
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

            // ���ʔ���.
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

    // �V�F�C�v�f�[�^��ǂݍ���.
    for( u32 i=0; i<numShape; ++i )
    {
        // �^�C�v��ǂݍ���.
        u32 type = 0;
        fread( &type, sizeof(u32), 1, pFile );

        IMaterial* pMaterial = nullptr;

        // �^�C�v����f�[�^�𔻕ʂ���.
        switch( type )
        {
            // ��.
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

            // �O�p�`.
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

            // �l�p�`.
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

            // ���b�V��.
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

    // �J�����f�[�^��ǂݍ���.
    for( u32 i=0; i<numCamera; ++i )
    {
        // �^�C�v��ǂݍ���.
        u32 type = 0;
        fread( &type, sizeof(type), 1, pFile );

        // �^�C�v����f�[�^�𔻕ʂ���.
        switch( type )
        {
            // �s���z�[���J����.
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

    // �t�@�C�������.
    fclose( pFile );

    // �T�C�Y��ݒ�.
    m_NumShapes    = numShape;
    m_NumMaterials = numMaterial;
    m_NumTextures  = numTexture;
    m_NumSamplers  = numSampler;

    // BVH���\�z����.
    m_pBVH = BVH::BuildBranch( m_ppShapes, m_NumShapes );

    // ����I��.
    return true;
}

//----------------------------------------------------------------------------------
//      ����������s���܂�.
//----------------------------------------------------------------------------------
bool Scene::Intersect( const Ray& ray, HitRecord& record )
{
    assert( m_pBVH != nullptr );
    HitRecord temp;

    // ��������.
    if ( m_pBVH->IsHit( ray, temp ) )
    { record = temp; }

    return (record.pShape != nullptr);
}

//----------------------------------------------------------------------------------
//      �X�N���[���܂łւ̃��C���擾���܂�.
//----------------------------------------------------------------------------------
Ray Scene::GetRay( const f32 x, const f32 y )
{
    assert( m_pCamera != nullptr );
    return m_pCamera->GetRay( x, y );
}


} // namespace s3d