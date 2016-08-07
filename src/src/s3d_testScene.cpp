//-------------------------------------------------------------------------------------------------
// File : s3d_testScene.cpp
// Desc : Test Scene
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_testScene.h>
#include <s3d_material.h>
#include <s3d_shape.h>
#include <s3d_mesh.h>
#include <s3d_bvh.h>
#include <s3d_logger.h>
#include <s3d_mesh.h>
#include <s3d_instance.h>
#include <s3d_materialfactory.h>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
// Using Statements
//-------------------------------------------------------------------------------------------------
using namespace s3d;

//-------------------------------------------------------------------------------------------------
// Global Varaibles.
//-------------------------------------------------------------------------------------------------
Texture2D       g_TableTexture;
TextureSampler  g_Sampler = TextureSampler();

} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// TestScene class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
TestScene::TestScene( const u32 width, const u32 height )
: Scene()
{
    IShape* pQuad;

    auto pCan0 = Mesh::Create( "res/mesh/can/coke_can.smd" );
    assert(pCan0 != nullptr);

    auto pCan1 = Mesh::Create( "res/mesh/can/pepsi_can.smd" );
    assert(pCan1 != nullptr);

    auto pCup = Mesh::Create( "res/mesh/paper_cup/paper_cup.smd" );
    assert(pCup != nullptr);

    if ( !g_TableTexture.LoadFromFile("./res/texture/table.bmp") )
    {
        ELOG("Error : TableTexture Load Failed." );
        assert(false);
    }

    if ( !m_IBL.Init( "res/ibl/HDR_041_Path.hdr") )
    {
        ELOG("Error : IBL Load Failed." );
        assert(false);
    }

    m_Material.push_back( MaterialFactory::CreateLambert( Color4( 0.95f, 0.95f, 0.95f, 1.0f ), &g_TableTexture, &g_Sampler ) );
    m_Material.push_back( MaterialFactory::CreateLambert( Color4( 0.0f, 0.0f, 0.0f, 1.0f ), nullptr, nullptr, Color4( 100.0f, 100.0f, 100.0f, 1.0f ) ) );

    {
        Vertex vertices[6];
        vertices[0].Position = Vector3( -50.0f, 0.0f,  250.0f );
        vertices[0].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[0].TexCoord = Vector2( 0.0, 0.0 );

        vertices[1].Position = Vector3( -50, 0.0, -150.0 );
        vertices[1].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[1].TexCoord = Vector2( 0.0, 2.0 );

        vertices[2].Position = Vector3( 150, 0.0, -150.0 );
        vertices[2].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[2].TexCoord = Vector2( 2.0, 2.0 );

        vertices[3].Position = Vector3( 150, 0.0, -150.0 );
        vertices[3].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[3].TexCoord = Vector2( 2.0, 2.0 );

        vertices[4].Position = Vector3( 150, 0.0,  250.0 );
        vertices[4].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[4].TexCoord = Vector2( 2.0, 0.0 );

        vertices[5].Position = Vector3( -50.0f, 0.0f,  250.0f );
        vertices[5].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[5].TexCoord = Vector2( 0.0, 0.0 );

        pQuad = Mesh::Create(6, vertices, m_Material[0]);
        assert(pQuad != nullptr);
    }

    m_Shapes.reserve(9);

    m_Shapes.push_back( Instance::Create( pCan0, Matrix::Translate( 100.0f, 20.0f, 100.0 ) ) );
    m_Shapes.push_back( Instance::Create( pCan0, Matrix::RotateY(ToRad(-30.0f)) *Matrix::RotateX(ToRad(90.0)) * Matrix::RotateY(ToRad(-55.0)) * Matrix::Translate( 70.0f, 10.0f, 90.0f) ) );
    m_Shapes.push_back( Instance::Create( pCan1, Matrix::RotateY(ToRad(59.5f)) * Matrix::Translate( 75.0f, 20.0f, 50.0f) ));
    m_Shapes.push_back( Instance::Create( pCan1, Matrix::RotateY(ToRad(130.3f)) * Matrix::Translate( 60.0f, 20.0f, 35.0f) ));
    m_Shapes.push_back( Instance::Create( pCan1, Matrix::RotateY(ToRad(260.0f)) * Matrix::Translate( 90.0f, 20.0f, 35.0f) ));
    m_Shapes.push_back( Instance::Create( pCan0, Matrix::RotateX(ToRad(90.0f)) * Matrix::RotateY(ToRad(70.0f)) * Matrix::Translate( 10.0f, 10.0f, 10.0f) ));
    m_Shapes.push_back( Instance::Create( pCup,  Matrix::Translate( 30.0f, 20.0f, -55.0f ) ));
    m_Shapes.push_back( Instance::Create( pCup,  Matrix::Translate( 70.0f, 20.0f, -70.0f ) ));
    m_Shapes.push_back( pQuad );

    SafeRelease( pCan0 );
    SafeRelease( pCan1 );
    SafeRelease( pCup );

    auto camera = new ThinLensCamera();
    camera->Update( 
        Vector3( 80.0f, 50.0f, 250.0f ),
        Vector3( 50.0f, 40.0f, 100.0f ),
        Vector3( 0.0f, 1.0f, 0.0f ),
        ToRad(39.6f),
        static_cast<f32>(width) / static_cast<f32>(height),
        1.0f, 
        1.5f );

    m_pCamera = camera;
    m_pBVH = BVH::BuildBranch(m_Shapes.data(), m_Shapes.size());
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
TestScene::~TestScene()
{
    m_IBL.Term();

    SafeRelease( m_pBVH );
    SafeDelete( m_pCamera );

    for(size_t i=0; i<m_Shapes.size(); ++i)
    { SafeRelease(m_Shapes[i]); }

    for(size_t i=0; i<m_Material.size(); ++i)
    { SafeRelease(m_Material[i]); }

    m_Shapes  .clear();
    m_Material.clear();
    g_TableTexture.Release();
}

}
