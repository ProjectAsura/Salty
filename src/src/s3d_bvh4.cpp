﻿//-------------------------------------------------------------------------------------------------
// File : s3d_bvh4.cpp
// Desc : Quad BVH.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_bvh4.h>
#include <s3d_leaf.h>


namespace /* anonymous */ {

} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// BVH4 class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
BVH4::BVH4( IShape** ppShapes, const BoundingBox4& box )
: m_Count   ( 1 )
, m_Box     ( box )
{
    for(auto i=0; i<4; ++i)
    { m_pNode[i] = ppShapes[i]; }
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
BVH4::~BVH4()
{
    for(auto i=0; i<4; ++i)
    { SafeRelease( m_pNode[i] ); }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void BVH4::AddRef()
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理です.
//-------------------------------------------------------------------------------------------------
void BVH4::Release()
{
    m_Count--;
    if ( m_Count == 0 )
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 BVH4::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------------------
bool BVH4::IsHit( const Ray& ray, HitRecord& record ) const
{
    s32 mask = 0;
    if ( !m_Box.IsHit( Ray4(ray), mask ) )
    { return false; }

    auto hit = false;
    for ( auto i=0; i<4; ++i )
    {
        auto bit = 0x1 << i;
        if ( (mask & bit) == bit )
        { hit |= m_pNode[i]->IsHit( ray, record ); }
    }

    return hit;
}

//-------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------------
BoundingBox BVH4::GetBox() const
{ return m_Box.GetBox(); }

//-------------------------------------------------------------------------------------------------
//      中心座標を取得します
//-------------------------------------------------------------------------------------------------
Vector3 BVH4::GetCenter() const
{
    auto result = m_pNode[0]->GetCenter();
    for( auto i=1; i<4; ++i )
    { result += m_pNode[i]->GetCenter(); }
    result /= 4.0f;

    return result;
}

//-------------------------------------------------------------------------------------------------
//      new 演算子のオーバーロードです.
//-------------------------------------------------------------------------------------------------
void* BVH4::operator new (size_t size)
{ return _aligned_malloc(size, 16); }

//-------------------------------------------------------------------------------------------------
//      new[] 演算子のオーバーロードです.
//-------------------------------------------------------------------------------------------------
void* BVH4::operator new[] (size_t size)
{ return _aligned_malloc(size, 16); }

//-------------------------------------------------------------------------------------------------
//      delete 演算子のオーバーロードです.
//-------------------------------------------------------------------------------------------------
void BVH4::operator delete (void* ptr)
{ _aligned_free(ptr); }

//-------------------------------------------------------------------------------------------------
//      delete[] 演算子のオーバーロードです.
//-------------------------------------------------------------------------------------------------
void BVH4::operator delete[] (void* ptr)
{ _aligned_free(ptr); }


} // namespace s3d
