﻿//-------------------------------------------------------------------------------------------------
// File : s3d_leaf.cpp
// Desc : Leaf Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_leaf.h>


namespace s3d {

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
Leaf::Leaf( u32 count, IShape** ppShapes )
: m_Count(1)
{
    m_pShapes.resize( count );
    for(u32 i=0; i<count; ++i)
    {
        m_pShapes[i] = ppShapes[i];
        m_pShapes[i]->AddRef();
    }

    m_Box = ppShapes[ 0 ]->GetBox();
    for( u32 i=1; i<count; ++i )
    { m_Box = BoundingBox::Merge( m_Box, ppShapes[ i ]->GetBox() ); }
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
Leaf::~Leaf()
{
    for(size_t i=0; i<m_pShapes.size(); ++i)
    { SafeRelease(m_pShapes[i]); }

    m_pShapes.clear();
    m_pShapes.shrink_to_fit();
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void Leaf::AddRef()
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------------
void Leaf::Release()
{
    m_Count--;
    if (m_Count == 0)
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 Leaf::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------------------
bool Leaf::IsHit( const Ray& ray, HitRecord& record ) const
{
    auto hit = false;

    for( size_t i=0; i<m_pShapes.size(); ++i )
    { hit |= m_pShapes[ i ]->IsHit( ray, record ); }

    return hit;
}

//-------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------------
BoundingBox Leaf::GetBox() const
{ return m_Box; }

//-------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//-------------------------------------------------------------------------------------------------
Vector3 Leaf::GetCenter() const
{ return m_Box.center; }

//-------------------------------------------------------------------------------------------------
//      生成処理を行います.
//-------------------------------------------------------------------------------------------------
bool Leaf::Create(u32 count, IShape** ppShape, IShape** ppResult)
{
    auto instance = new(std::nothrow) Leaf(count, ppShape);
    if ( instance == nullptr )
    { return false; }

    *ppResult = instance;
    return true;
}

} // namespace s3d
