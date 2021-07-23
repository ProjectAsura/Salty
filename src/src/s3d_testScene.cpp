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
#include <s3d_bvh4.h>
#include <s3d_logger.h>
#include <s3d_mesh.h>
#include <s3d_instance.h>
#include <s3d_sphere.h>
#include <s3d_materialfactory.h>
#include <s3d_timer.h>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
// Using Statements
//-------------------------------------------------------------------------------------------------
using namespace s3d;

//-------------------------------------------------------------------------------------------------
// Global Varaibles.
//-------------------------------------------------------------------------------------------------
Texture2D       g_TableTexture;

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
#if 1
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

    if ( !m_IBL.Init( "res/ibl/lauter_waterfall_4k.hdr") )
    {
        ELOG("Error : IBL Load Failed." );
        assert(false);
    }


    m_Material.push_back( MaterialFactory::CreateLambert( Color4( 0.95f, 0.95f, 0.95f, 1.0f ), &g_TableTexture ) );
    m_Material.push_back( MaterialFactory::CreateLambert( Color4( 0.0f, 0.0f, 0.0f, 1.0f ), nullptr, Color4( 1000.0f, 1000.0f, 1000.0f, 1.0f ) ) );

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

    m_CanRotateZ0 = 0.0f;
    m_CanRotateZ1 = 0.0f;
    m_CupRotate = 0.0f;
    m_CanPos0 = Vector3(55.0f, 10.0f, 90.0f);
    m_CanPos1 = Vector3(10.0f, 10.0f, 10.0f);
    m_CupPos  = Vector3(30.0f, 20.0f, -55.0f);
    m_pCan0 = static_cast<Instance*>(Instance::Create( pCan1, Matrix::RotateY(ToRad(-30.0f)) * Matrix::RotateX(ToRad(90.0)) * Matrix::RotateY(ToRad(-65.0)) * Matrix::Translate(m_CanPos0.x, m_CanPos0.y, m_CanPos0.z) ));
    m_pCan1 = static_cast<Instance*>(Instance::Create( pCan0, Matrix::RotateY(m_CanRotateZ1) * Matrix::RotateX(ToRad(90.0f)) * Matrix::RotateY(ToRad(70.0f)) * Matrix::Translate(m_CanPos1.x, m_CanPos1.y, m_CanPos1.z) ));
    m_pCup  = static_cast<Instance*>(Instance::Create( pCup,  Matrix::Translate(m_CupPos.x, m_CupPos.y, m_CupPos.z)));
 
    m_Shapes.push_back( Sphere::Create( 15.0f, Vector3( 50.0f,  150.0f, 90.0f ), m_Material[1] ) );
    m_Shapes.push_back( Instance::Create( pCan0, Matrix::Translate( 100.0f, 20.0f, 100.0 ) ) );
    m_Shapes.push_back( m_pCan0 );
    m_Shapes.push_back( Instance::Create( pCan1, Matrix::RotateY(ToRad(59.5f)) * Matrix::Translate( 75.0f, 20.0f, 50.0f) ));
    //m_Shapes.push_back( Instance::Create( pCan1, Matrix::RotateY(ToRad(130.3f)) * Matrix::Translate( 60.0f, 20.0f, 35.0f) ));
//    m_Shapes.push_back( Instance::Create( pCan1, Matrix::RotateY(ToRad(260.0f)) * Matrix::Translate( 90.0f, 20.0f, 35.0f) ));
    m_Shapes.push_back( m_pCan1 );
    m_Shapes.push_back( m_pCup );
    //m_Shapes.push_back( Instance::Create( pCup,  Matrix::Translate( 70.0f, 20.0f, -70.0f ) ));
    m_Shapes.push_back( pQuad );

    m_pLightList.push_back( m_Shapes[0] );

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
#else
    if ( !m_IBL.Init("res/ibl/skukuza_golf_4k.hdr") )
    {
        ELOG( "Error : IBL Not Found." );
        assert(false);
    }

    auto pSceneMesh = Mesh::Create( "res/mesh/sv98/sv98.smd" );
//    auto pSceneMesh = Mesh::Create( "res/mesh/test/test.smd" );
    assert(pSceneMesh != nullptr);

    if ( !g_TableTexture.LoadFromFile("./res/texture/table.bmp") )
    {
        ELOG("Error : TableTexture Load Failed." );
        assert(false);
    }


    m_Material.push_back( MaterialFactory::CreateLambert( Color4( 0.0f, 0.0f, 0.0f, 1.0f ), nullptr, nullptr, Color4( 1000.0f, 1000.0f, 1000.0f, 1.0f ) ) );
    m_Material.push_back( MaterialFactory::CreateLambert( Color4( 0.95f, 0.95f, 0.95f, 1.0f ), &g_TableTexture, &g_Sampler ) );

    IShape* pQuad = nullptr;
    {
        Vertex vertices[6];
        vertices[0].Position = Vector3( -15.0f, -7.0f,  25.0f );
        vertices[0].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[0].TexCoord = Vector2( 0.0, 0.0 );

        vertices[1].Position = Vector3( -15, -7.0f, -25.0 );
        vertices[1].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[1].TexCoord = Vector2( 0.0, 5.0 );

        vertices[2].Position = Vector3( 15, -7.0f, -25.0 );
        vertices[2].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[2].TexCoord = Vector2( 5.0, 5.0 );

        vertices[3].Position = Vector3( 15, -7.0f, -25.0 );
        vertices[3].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[3].TexCoord = Vector2( 5.0, 5.0 );

        vertices[4].Position = Vector3( 15, -7.0f,  25.0 );
        vertices[4].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[4].TexCoord = Vector2( 5.0, 0.0 );

        vertices[5].Position = Vector3( -15.0f, -7.0f, 25.0f );
        vertices[5].Normal   = Vector3( 0.0f, 1.0f, 0.0f );
        vertices[5].TexCoord = Vector2( 0.0, 0.0 );

        pQuad = Mesh::Create(6, vertices, m_Material[1]);
        assert(pQuad != nullptr);
    }

    m_Shapes.reserve(3);
    m_Shapes.push_back( pSceneMesh );
    //m_Shapes.push_back( Sphere::Create( 10.0f, Vector3( 0.0f, 150.0f, 90.0f ), m_Material[0] ) );
    m_Shapes.push_back( pQuad );


    auto pos = Vector3( 8.0f, 0.0f, 25.0f );
    auto dst = Vector3( 0.0f, 1.0f, 0.0f );
    auto dir = dst - pos;
    dir.Normalize();

    auto target = pos + dir * 20.0f;

//    SafeRelease(pSceneMesh);
    auto camera = new ThinLensCamera();
    camera->Update( 
        pos,
        target,
        Vector3( 0.0f, 1.0f, 0.0f ),
        ToRad(39.6f),
        static_cast<f32>(width) / static_cast<f32>(height),
        1.0f, 
        0.25f );

#endif

    m_pCamera = camera;
    m_FrameCount = 0;
    //m_pBVH = BVH4::Create( m_Shapes.size(), m_Shapes.data() );
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
TestScene::~TestScene()
{
    m_IBL.Term();

    //SafeRelease( m_pBVH );
    SafeDelete( m_pCamera );

    m_pCan0 = nullptr;
    m_pCan1 = nullptr;
    m_pCup  = nullptr;

    for(size_t i=0; i<m_Shapes.size(); ++i)
    { SafeRelease(m_Shapes[i]); }

    for(size_t i=0; i<m_Material.size(); ++i)
    { SafeRelease(m_Material[i]); }

    m_Shapes  .clear();
    m_Material.clear();
    m_pLightList.clear();
    g_TableTexture.Release();
}

//-------------------------------------------------------------------------------------------------
//      シーンを更新します.
//-------------------------------------------------------------------------------------------------
void TestScene::Update(float deltaTime)
{
    m_CanRotateZ0 -= 2.5f;
    m_CanRotateZ1 += 4.0f;
//    auto dir0 = Vector3(cos(-65.0f) * sin(-65.0f), 0.0f, cos(-65.0f));
//    m_CanPos0 += dir0 * deltaTime * 0.5f;
    m_CanPos0.z += 0.5f;
    m_CanPos0.x += 0.1f;
    m_CanPos1.z += 1.5f;
    auto mat0 = Matrix::RotateY(ToRad(-30.0f + m_CanRotateZ0)) * Matrix::RotateX(ToRad(90.0)) * Matrix::RotateY(ToRad(-65.0)) * Matrix::Translate(m_CanPos0.x, m_CanPos0.y, m_CanPos0.z);
    auto mat1 = Matrix::RotateY(ToRad(m_CanRotateZ1)) * Matrix::RotateX(ToRad(90.0f)) * Matrix::RotateY(ToRad(70.0f)) * Matrix::Translate(m_CanPos1.x, m_CanPos1.y, m_CanPos1.z);

    m_pCan0->UpdateMatrix(mat0);
    m_pCan1->UpdateMatrix(mat1);

    if (m_FrameCount >= 43)
    {
        m_CupRotate += 3.75f;
        if (m_CupRotate >= 90.0f)
        { m_CupRotate = 90.0f; }

        auto mat3 = Matrix::RotateX(ToRad(m_CupRotate)) * Matrix::Translate(m_CupPos.x, m_CupPos.y, m_CupPos.z);
        m_pCup->UpdateMatrix(mat3);
    }

    m_FrameCount++;
}

} 
