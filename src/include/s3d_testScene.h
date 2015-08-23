//-------------------------------------------------------------------------------------------------
// File : s3d_testScene.h
// Desc : Test Scene.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_scene.h>
#include <vector>


namespace s3d {

class TestScene : public Scene
{
public:
    TestScene( const u32 width, const u32 height );
    virtual ~TestScene();

private:
    std::vector<IShape*>    m_Shapes;
    std::vector<Instance*>   m_Instances;
};

} // namespace s3d
