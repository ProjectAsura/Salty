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
#include <s3d_instance.h>
#include <vector>


namespace s3d {

class TestScene : public Scene
{
public:
    TestScene( const u32 width, const u32 height );
    virtual ~TestScene();
    void Update(float time) override;

private:
    //std::vector<IShape*>     m_Shapes;
    //std::vector<IMaterial*>  m_Material;
    Vector3 m_CanPos0;
    Vector3 m_CanPos1;
    Vector3 m_CupPos;
    float   m_CanRotateZ0;
    float   m_CanRotateZ1;
    float   m_CupRotate;
    Instance*   m_pCan0;
    Instance*   m_pCan1;
    Instance*   m_pCup;
    int m_FrameCount = 0;
};

} // namespace s3d
