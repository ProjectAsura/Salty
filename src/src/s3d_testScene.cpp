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


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
// Using Statements
//-------------------------------------------------------------------------------------------------
using namespace s3d;

//-------------------------------------------------------------------------------------------------
// Global Varaibles.
//-------------------------------------------------------------------------------------------------
Mesh            g_Can0;
Mesh            g_Can1;
Mesh            g_Cup;

Texture2D       g_TableTexture;
TextureSampler  g_Sampler = TextureSampler();

Matte           g_TableMaterial( Color4( 0.95f, 0.95f, 0.95f, 1.0f ), Color4( 0.0f, 0.0f, 0.0f, 1.0f ), &g_TableTexture, &g_Sampler );
Matte           g_LightMaterial( Color4( 0.0f,  0.0f,  0.0f,  1.0f ), Color4( 100.0f, 100.0f, 100.0f, 1.0f ) );

Quad            g_Table(
                    Face4(
                        Vertex( Vector3( -50, 0.0,  250.0 ),    Vector2( 0.0, 0.0 ) ),
                        Vertex( Vector3( -50, 0.0, -150.0 ),    Vector2( 0.0, 2.0 ) ),
                        Vertex( Vector3( 150, 0.0, -150.0 ),    Vector2( 2.0, 2.0 ) ),
                        Vertex( Vector3( 150, 0.0,  250.0 ),    Vector2( 2.0, 0.0 ) ) ),
                        &g_TableMaterial );

Sphere          g_Light( 15.0f,  Vector3( 50.0f,  150.0f, 90.0f ), &g_LightMaterial );

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
    if ( !g_Can0.LoadFromFile( "res/mesh/can/coke_can.smd" ) )
    {
        ELOG("Error : Can0 Load Failed." );
        assert( false );
    }

    if ( !g_Can1.LoadFromFile( "res/mesh/can/pepsi_can.smd" ) )
    {
        ELOG("Error : Can1 Load Failed." );
        assert( false );
    }

    if ( !g_Cup.LoadFromFile( "res/mesh/paper_cup/paper_cup.smd") )
    {
        ELOG("Error : Cup Load Failed." );
        assert(false);
    }

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

    m_Instances.push_back( new Instance( &g_Can0, Matrix::Translate( 100.0f, 20.0f, 100.0 )) );
    m_Instances.push_back( new Instance( &g_Can0, Matrix::RotateY(ToRad(-30.0f)) *Matrix::RotateX(ToRad(90.0)) * Matrix::RotateY(ToRad(-55.0)) * Matrix::Translate( 70.0f, 10.0f, 90.0f)) );
    m_Instances.push_back( new Instance( &g_Can1, Matrix::RotateY(ToRad(59.5f)) * Matrix::Translate( 75.0f, 20.0f, 50.0f)) );
    m_Instances.push_back( new Instance( &g_Can1, Matrix::RotateY(ToRad(130.3f)) * Matrix::Translate( 60.0f, 20.0f, 35.0f)) );
    m_Instances.push_back( new Instance( &g_Can1, Matrix::RotateY(ToRad(260.0f)) * Matrix::Translate( 90.0f, 20.0f, 35.0f)) );
    m_Instances.push_back( new Instance( &g_Can0, Matrix::RotateX(ToRad(90.0f)) * Matrix::RotateY(ToRad(70.0f)) * Matrix::Translate( 10.0f, 10.0f, 10.0f)) );
    m_Instances.push_back( new Instance( &g_Cup, Matrix::Translate( 30.0f, 20.0f, -55.0f )));
    m_Instances.push_back( new Instance( &g_Cup, Matrix::Translate( 70.0f, 20.0f, -70.0f )));
    m_Instances.shrink_to_fit();

    m_Shapes.push_back( &g_Table );
    m_Shapes.push_back( &g_Light );
    m_Shapes.push_back( m_Instances[0] );
    m_Shapes.push_back( m_Instances[1] );
    m_Shapes.push_back( m_Instances[2] );
    m_Shapes.push_back( m_Instances[3] );
    m_Shapes.push_back( m_Instances[4] );
    m_Shapes.push_back( m_Instances[5] );
    m_Shapes.push_back( m_Instances[6] );
    m_Shapes.push_back( m_Instances[7] );
    m_Shapes.shrink_to_fit();

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
    m_pBVH = OBVH::BuildBranch(&m_Shapes[0], u32(m_Shapes.size()));
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
TestScene::~TestScene()
{
    m_IBL.Term();

    if ( m_pBVH != nullptr )
    {
        auto disposable = dynamic_cast<IDisposable*>( m_pBVH );
        if ( disposable != nullptr )
        { disposable->Dispose(); }

        m_pBVH = nullptr;
    }

    SafeDelete( m_pCamera );

    m_Shapes.clear();
    for( auto i=0; i<m_Instances.size(); ++i)
    {
        SafeDelete(m_Instances[i]);
    }
    m_Instances.clear();

    g_Can0.Release();
    g_Can1.Release();
    g_Cup.Release();
    g_TableTexture.Release();
}

}
