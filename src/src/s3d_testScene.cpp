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


namespace /* anonymous */ {

using namespace s3d;

//Texture2D   g_TextureLeaf( "./res/texture/aglaonema_leaf.tga" );
Texture2D   g_TextureTile( "./res/texture/table.bmp" );
TextureSampler g_Sampler = TextureSampler();

// Lambert
Matte g_Matte[] = {
    Matte( Color4( 0.75f, 0.75f, 0.75f, 1.0f ), Color4( 0.0f, 0.0f, 0.0f, 1.0f ) ),
    Matte( Color4( 0.75f, 0.75f, 0.75f, 1.0f ), Color4( 0.0f, 0.0f, 0.0f, 1.0f ), &g_TextureTile, &g_Sampler ),
    Matte( Color4( 0.0f,  0.0f,  0.0f,  1.0f ), Color4( 100.0f, 100.0f, 100.0f, 1.0f ) ),
    Matte( Color4( 0.5f,  0.0f,  0.0f,  1.0f ), Color4( 36.0f, 1.0f, 1.0f, 1.0f ) ),
    Matte( Color4( 0.0f,  0.0f,  0.5f,  1.0f ), Color4( 1.0f, 1.0f, 36.0f, 1.0f ) ), 
    Matte( Color4( 0.75f, 0.75f, 0.75f, 1.0f ), Color4( 0.0f, 0.0f, 0.0f, 1.0f) ),

};

// Mirror
Mirror g_Mirror[] = {
    Mirror( Color4( 0.85f, 0.85f, 0.85f, 1.0f ) ),
    Mirror( Color4( 0.75f, 0.75f, 0.25f, 1.0f ) ),
};

// Refraction (Crystal)
Dielectric g_Crystal[] = {
    Dielectric( 1.54f, Color4( 0.85f, 0.65f, 0.65f, 1.0f ) ),
    Dielectric( 2.5f,  Color4( 0.9f,  0.85f, 0.85f, 1.0f ) ),
};

// Phong
Glossy g_Glossy[] = {
    Glossy( Color4( 0.95f, 0.95f, 0.95f, 1.0f ), 1000.0f ),
};

// Materials
IMaterial* g_pMaterials[] = {
    &g_Matte[0],        // 0 : 白.
    &g_Matte[1],        // 1 : タイル.
    &g_Matte[2],        // 2 : 照明.
    &g_Mirror[0],       // 3 : ミラー.
    &g_Mirror[1],       // 4 : 黄色ミラー.
    &g_Crystal[1],      // 5 : 水晶.
    &g_Glossy[0],       // 6 : メタル(緑).
    &g_Matte[3],
    &g_Matte[4]
};

// レンダリングするシーンデータ
Sphere g_Spheres[] = {
    Sphere( 16.5f,  Vector3( 25.0f,   26.5f, 57.0f ), g_pMaterials[5] ),    // 水晶
    Sphere( 7.5f,   Vector3( 50.0f,    6.5f, 78.0f ), g_pMaterials[4] ),    // 鏡.
    Sphere( 15.0f,  Vector3( 50.0f,  150.0f, 81.6f ), g_pMaterials[2] ),    // 照明
    Sphere( 5.0f,   Vector3( 10.0f,   90.0f, 30.0f ), g_pMaterials[7] ),
    Sphere( 2.5f,   Vector3( 10.0f,   20.0f, 120.0f ), g_pMaterials[8] ),
};


// 矩形.
Quad g_Quads[] = {

    // 左.
    Quad( 
        Face4(
            Vertex( Vector3( 0,  0.0,  250.0 ), Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3( 0, 100.0, 250.0 ), Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3( 0, 100.0, 0.0 ),   Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3( 0,  0.0,  0.0 ),   Vector2( 2.0, 0.0 ) ) ),
        &g_Matte[5]
    ),

    // 奥.
    Quad(
        Face4(
            Vertex( Vector3( 0,   0.0,  1.0 ),   Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3( 0, 100.0,  1.0 ),   Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3( 100, 100.0,  1.0 ), Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3( 100,   0.0,  1.0 ), Vector2( 2.0, 0.0 ) ) ),
        g_pMaterials[6]
    ),

    // 手前.
    Quad(
        Face4(
            Vertex( Vector3(  100,    0.0, 250.0 ), Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3(  100,  100.0, 250.0 ), Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3(  0,  100.0,  250.0 ),  Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3(  0,    0.0,  250.0 ),  Vector2( 2.0, 0.0 ) ) ),
        g_pMaterials[0]
    ),

    // 右.
    Quad(
        Face4(
            Vertex( Vector3( 100,   0.0,  0.0 ),   Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3( 100, 100.0,  0.0 ),   Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3( 100, 100.0, 250.0 ),  Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3( 100,  0.0, 250.0 ),   Vector2( 2.0, 0.0 ) ) ),
        g_pMaterials[6]
    ),

    // 上
    Quad(
        Face4(
            Vertex( Vector3( 100, 100.0, 250.0 ),  Vector2( 2.0, 0.0 ) ),
            Vertex( Vector3( 100, 100.0,  0.0 ),   Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3( 0, 100.0,  0.0 ),     Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3( 0, 100.0, 250.0 ),    Vector2( 0.0, 0.0 ) ) ),
        g_pMaterials[0]
   ),

    // 下.
    Quad(
        Face4(
            Vertex( Vector3( -50, 0.0, 250.0 ),      Vector2( 0.0, 0.0 ) ),
            Vertex( Vector3( -50, 0.0, -150.0 ),       Vector2( 0.0, 2.0 ) ),
            Vertex( Vector3( 150, 0.0, -150.0 ),     Vector2( 2.0, 2.0 ) ),
            Vertex( Vector3( 150, 0.0, 250.0 ),    Vector2( 2.0, 0.0 ) ) ),
        g_pMaterials[1]
   ),
};

Mesh g_Mesh;
Mesh g_Mesh2;
Mesh g_Mesh3;

}

namespace s3d {

TestScene::TestScene( const u32 width, const u32 height )
: Scene()
{
    if ( !g_Mesh.LoadFromFile( "res/mesh/can/coke_can.smd" ) )
    {
        assert( false );
    }

    if ( !g_Mesh2.LoadFromFile( "res/mesh/can/pepsi_can.smd" ) )
    {
        assert( false );
    }

    if ( !g_Mesh3.LoadFromFile( "res/mesh/paper_cup/paper_cup.smd") )
    {
        assert(false);
    }


    m_Instances.push_back( new Instance( &g_Mesh, Matrix::Translate( 100.0f, 20.0f, 100.0 )) );
    m_Instances.push_back( new Instance( &g_Mesh, Matrix::RotateY(ToRad(-30.0f)) *Matrix::RotateX(ToRad(90.0)) * Matrix::RotateY(ToRad(-70.0)) * Matrix::Translate( 75.0f, 10.0f, 85.0f)) );
    m_Instances.push_back( new Instance( &g_Mesh2, Matrix::RotateY(ToRad(59.5f)) * Matrix::Translate( 75.0f, 20.0f, 50.0f)) );
    m_Instances.push_back( new Instance( &g_Mesh2, Matrix::RotateY(ToRad(130.3f)) * Matrix::Translate( 60.0f, 20.0f, 35.0f)) );
    m_Instances.push_back( new Instance( &g_Mesh2, Matrix::RotateY(ToRad(260.0f)) * Matrix::Translate( 90.0f, 20.0f, 35.0f)) );
    m_Instances.push_back( new Instance( &g_Mesh3, Matrix::Translate( 50.0f, 20.0f, -55.0f )));
    m_Instances.push_back( new Instance( &g_Mesh3, Matrix::Translate( 100.0f, 20.0f, -70.0f )));


    //m_Shapes.push_back( &g_Quads[0] );
    //m_Shapes.push_back( &g_Quads[1] );
    //m_Shapes.push_back( &g_Quads[2] );
    //m_Shapes.push_back( &g_Quads[3] );
    //m_Shapes.push_back( &g_Quads[4] );
    m_Shapes.push_back( &g_Quads[5] );
    //m_Shapes.push_back( &g_Spheres[0] );
    //m_Shapes.push_back( &g_Spheres[1] );
    m_Shapes.push_back( m_Instances[0] );
    m_Shapes.push_back( m_Instances[1] );
    m_Shapes.push_back( m_Instances[2] );
    m_Shapes.push_back( m_Instances[3] );
    m_Shapes.push_back( m_Instances[4] );
    m_Shapes.push_back( m_Instances[5] );
    m_Shapes.push_back( m_Instances[6] );
    

    m_Shapes.push_back( &g_Spheres[2] );
    //m_Shapes.push_back( &g_Spheres[3] );
    //m_Shapes.push_back( &g_Spheres[4] );

    m_Shapes.shrink_to_fit();

#if 0
    m_pBVH = OBVH::Build( m_Shapes );
#else
    m_pBVH = OBVH::BuildBranch(&m_Shapes[0], u32(m_Shapes.size()));
#endif

    auto camera = new ThinLensCamera();
    camera->Update( 
        Vector3( 50.0f, 52.0f, 220.0f ),
        Vector3( 50.0f, 50.0f, 180.0f ),
        Vector3( 0.0f, 1.0f, 0.0f ),
        F_PIDIV4,
        (f32)width / (f32)height,
        1.0f, 
        120.0f, 
        1.5f );

    m_pCamera = camera;

    if ( !m_IBL.Init( "res/ibl/HDR_041_Path.hdr") )
    {
        assert(false);
    }
}

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

    g_Mesh.Release();
}

}