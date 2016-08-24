//-------------------------------------------------------------------------------------------------
// File : s3d_bvh8.cpp
// Desc : Oct BVH.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_bvh8.h>
#include <s3d_bvh4.h>
#include <s3d_bvh2.h>
#include <s3d_bucket.h>
#include <s3d_leaf.h>
#include <algorithm>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
// Constant Values.
//-------------------------------------------------------------------------------------------------
constexpr int BucketCount = 16;     //!< バケット数です.

//-------------------------------------------------------------------------------------------------
//      マージしたバウンディングボックスを生成します.
//-------------------------------------------------------------------------------------------------
s3d::BoundingBox CreateMergedBox( size_t count, s3d::IShape** ppShapes )
{
    if ( count == 0 || ppShapes == nullptr )
    { return s3d::BoundingBox(); }

    s3d::BoundingBox box = ppShapes[0]->GetBox();

    for( size_t i=1; i<count; ++i )
    { box = s3d::BoundingBox::Merge( box, ppShapes[i]->GetBox() ); }

    return box;
}

//-------------------------------------------------------------------------------------------------
//      中心座標をもとにバウンディングボックスを生成します.
//-------------------------------------------------------------------------------------------------
s3d::BoundingBox CreateCentroidBox( size_t count, s3d::IShape** ppShapes )
{
    if ( count == 0 || ppShapes == nullptr )
    { return s3d::BoundingBox(); }

    s3d::BoundingBox box( ppShapes[0]->GetCenter() );

    for( size_t i=1; i<count; ++i )
    { box = s3d::BoundingBox::Merge( box, ppShapes[i]->GetCenter() ); }

    return box;
}

//-------------------------------------------------------------------------------------------------
//      最長軸を取得します.
//-------------------------------------------------------------------------------------------------
int GetLongestAxis( const s3d::BoundingBox& box )
{
    auto vec = box.maxi - box.mini;

    if (vec.x > vec.y && vec.x > vec.z)
    { return 0; }
    else if (vec.y > vec.z)
    { return 1; }
    else
    { return 2; }
}

//-------------------------------------------------------------------------------------------------
//      オフセットを求めます.
//-------------------------------------------------------------------------------------------------
s3d::Vector3 CalcOffset( const s3d::BoundingBox& box, const s3d::Vector3& p )
{
    auto offset = p - box.mini;

    if (box.maxi.x > box.mini.x)
    { offset.x /= box.maxi.x - box.mini.x; }

    if (box.maxi.y > box.mini.y)
    { offset.y /= box.maxi.y - box.mini.y; }

    if (box.maxi.z > box.mini.z)
    { offset.z /= box.maxi.z - box.mini.z; }

    return offset;
}

//-------------------------------------------------------------------------------------------------
//      葉ノードを生成します.
//-------------------------------------------------------------------------------------------------
s3d::IShape* CreateNode( size_t count, s3d::IShape** ppShapes )
{
#if 0 // 細かすぎると遅くなるので BVH は作らない.
    if ( count <= 4 )
    { return s3d::Leaf::Create(count, ppShapes); }

    if ( count <= 16 )
    { return s3d::BVH2::Create(count, ppShapes); }

    return s3d::BVH4::Create(count, ppShapes);
#else
    if ( count <= 8 )
    { return s3d::Leaf::Create(count, ppShapes); }

    return s3d::BVH4::Create(count, ppShapes);
#endif
}

} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// BVH8 class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
BVH8::BVH8
(
    IShape* pShape0,
    IShape* pShape1,
    IShape* pShape2,
    IShape* pShape3,
    IShape* pShape4,
    IShape* pShape5,
    IShape* pShape6,
    IShape* pShape7
)
: m_Count(1)
{
    m_pNode[0] = pShape0;
    m_pNode[1] = pShape1;
    m_pNode[2] = pShape2;
    m_pNode[3] = pShape3;
    m_pNode[4] = pShape4;
    m_pNode[5] = pShape5;
    m_pNode[6] = pShape6;
    m_pNode[7] = pShape7;

    m_Box = BoundingBox8(
        pShape0->GetBox(),
        pShape1->GetBox(),
        pShape2->GetBox(),
        pShape3->GetBox(),
        pShape4->GetBox(),
        pShape5->GetBox(),
        pShape6->GetBox(),
        pShape7->GetBox());
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
BVH8::~BVH8()
{
    for(auto i=0; i<8; ++i)
    { SafeRelease( m_pNode[i] ); }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void BVH8::AddRef()
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------------
void BVH8::Release()
{
    m_Count--;
    if ( m_Count == 0 )
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 BVH8::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------------------
bool BVH8::IsHit( const RaySet& raySet, HitRecord& record ) const
{
    s32 mask = 0;
    if ( !m_Box.IsHit( raySet.ray8, mask ) )
    { return false; }

    auto hit = false;
    for (auto i=0; i<8; ++i)
    {
        auto bit = 0x1 << i;
        if ( (mask & bit) == bit )
        { hit |= m_pNode[i]->IsHit( raySet, record ); }
    }

    return hit;
}

//-------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------------
BoundingBox BVH8::GetBox() const
{ return m_Box.GetBox(); }

//-------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//-------------------------------------------------------------------------------------------------
Vector3 BVH8::GetCenter() const
{
    auto result = m_pNode[0]->GetCenter();
    for (auto i=0; i<8; ++i)
    { result += m_pNode[i]->GetCenter(); }
    result /= 8.0f;

    return result;
}

//-------------------------------------------------------------------------------------------------
//      new 演算子のオーバーロードです.
//-------------------------------------------------------------------------------------------------
void* BVH8::operator new (size_t size)
{ return _aligned_malloc(size, 32); }

//-------------------------------------------------------------------------------------------------
//      new[] 演算子のオーバーロードです.
//-------------------------------------------------------------------------------------------------
void* BVH8::operator new[] (size_t size)
{ return _aligned_malloc(size, 32); }

//-------------------------------------------------------------------------------------------------
//      delete 演算子のオーバーロードです.
//-------------------------------------------------------------------------------------------------
void BVH8::operator delete (void* ptr)
{ _aligned_free(ptr); }

//-------------------------------------------------------------------------------------------------
//      delete[] 演算子のオーバーロードです.
//-------------------------------------------------------------------------------------------------
void BVH8::operator delete[] (void* ptr)
{ _aligned_free(ptr); }

//-------------------------------------------------------------------------------------------------
//      SAH分割します.
//-------------------------------------------------------------------------------------------------
bool BVH8::SplitSAH( size_t count, IShape** ppShapes, size_t& mid )
{
    // 最小要素に満たないものは葉ノードとして生成
    if ( count <= 8 )
    { return false; }

    auto bound = CreateMergedBox( count, ppShapes );

    // バウンディングボックスを生成.
    auto centroid = CreateCentroidBox( count, ppShapes );

    // 分割軸を決めるためバウンディングボックスの最長軸を取得.
    auto axis = GetLongestAxis( centroid );

    if (centroid.maxi.a[axis] == centroid.mini.a[axis])
    { return false; }

    // SAH分割バケットの初期化処理.
    Bucket bucket[BucketCount];
    for (size_t i=0; i<count; ++i)
    {
        auto idx = static_cast<int>(BucketCount * CalcOffset( centroid, ppShapes[i]->GetCenter() ).a[axis]);

        if ( idx == BucketCount ) 
        { idx = BucketCount - 1; }
        assert( 0 <= idx && idx < BucketCount );

        bucket[idx].count++;
        bucket[idx].box = BoundingBox::Merge( bucket[idx].box, ppShapes[i]->GetBox() );
    }

    // 分割後の各バケットに対するコストを計算する.
    f32 cost[BucketCount - 1];
    for (auto i=0; i<BucketCount - 1; ++i)
    {
        BoundingBox b0, b1;
        int count0 = 0, count1 = 0;

        for(auto j=0; j<=i; ++j)
        {
            b0 = BoundingBox::Merge(b0, bucket[j].box);
            count0 += bucket[j].count;
        }

        for(auto j=i+1; j<BucketCount; ++j)
        {
            b1 = BoundingBox::Merge(b1, bucket[j].box);
            count1 += bucket[j].count;
        }

        cost[i] = 1 + (count0 * SurfaceArea(b0) + count1 * SurfaceArea(b1)) / SurfaceArea(bound);
    }

    // 最小SAHで分割のためのバケットを求める.
    f32 minCost = cost[0];
    int minCostSplitBucket = 0;
    for(auto i=1; i<BucketCount - 1; ++i)
    {
        if (cost[i] < minCost)
        {
            minCost = cost[i];
            minCostSplitBucket = i;
        }
    }

    // 選択されたSAHバケットにおいて葉ノードを生成するか，分割するかどうかを決定する.
    f32 leafCost = static_cast<f32>(count);
    if (minCost < leafCost)
    {
        auto pMid = std::partition(
            &ppShapes[0],
            &ppShapes[count - 1] + 1,
            [=](const IShape* pShape)
            {
                auto idx = BucketCount * CalcOffset(centroid, pShape->GetCenter()).a[axis];
                if ( idx == BucketCount )
                { idx = BucketCount - 1; }
                assert( 0 <= idx && idx < BucketCount );

                return idx <= minCostSplitBucket;
            });

        mid = pMid - &ppShapes[0];
    }
    else
    { return false; }

    return true;
}

//-------------------------------------------------------------------------------------------------
//      中間分割します.
//-------------------------------------------------------------------------------------------------
bool BVH8::SplitMid( size_t count, IShape** ppShapes, size_t& mid )
{
    // 最小要素に満たないものは葉ノードとして生成.  2つのノードがそれぞれ子供をもつので 2 * 2 = 4 が最小.
    if ( count <= 2 )
    { return false; }

    auto bound = CreateMergedBox( count, ppShapes );

    // バウンディングボックスを生成.
    auto centroid = CreateCentroidBox( count, ppShapes );

    // 分割軸を決めるためバウンディングボックスの最長軸を取得.
    auto axis = GetLongestAxis( centroid );

    if (centroid.maxi.a[axis] == centroid.mini.a[axis])
    { return false; }

    mid = 0;
    for (size_t i=0; i<count; ++i)
    {
        auto center = ppShapes[i]->GetBox().center;

        if (center.a[axis] < centroid.center.a[axis])
        {
            auto pTemp = ppShapes[i];
            ppShapes[i] = ppShapes[mid];
            ppShapes[mid] = pTemp;
            mid++;
        }
    }

    if ( mid == 0 || mid == count )
    { mid = count / 2; }

    return true;
}

//------------------------------------------------------------------------------------------------
//      分割します.
//------------------------------------------------------------------------------------------------
bool BVH8::Split( size_t count, IShape** ppShapes, size_t& mid )
{
#if 0
    // SAHによる分割.
    return SplitSAH( count, ppShapes, mid );
#else
    // 中間値による分割.
    return SplitMid( count, ppShapes, mid );
#endif
}

//-------------------------------------------------------------------------------------------------
//      生成処理を行います.
//-------------------------------------------------------------------------------------------------
IShape* BVH8::Create(size_t count, IShape** ppShapes)
{
    if ( count <= 8 )
    { return CreateNode( count, ppShapes ); }

    size_t mid = 0;
    if ( !Split( count, ppShapes, mid ) )
    { return CreateNode( count, ppShapes ); }

    auto idxL = 0;
    auto idxR = mid;

    auto countL = mid;
    auto countR = count - mid;

    size_t midL = 0;
    size_t midR = 0;

    if ( !Split( countL, &ppShapes[idxL], midL ) )
    { return CreateNode( count, ppShapes ); }

    if ( !Split( countR, &ppShapes[idxR], midR ) )
    { return CreateNode( count, ppShapes ); }

    auto idxA = 0;
    auto idxB = midL;
    auto idxC = mid;
    auto idxD = mid + midR;

    auto countA = idxB  - idxA;
    auto countB = idxC  - idxB;
    auto countC = idxD  - idxC;
    auto countD = count - idxD;

    size_t mid0 = 0;
    size_t mid1 = 0;
    size_t mid2 = 0;
    size_t mid3 = 0;

    if ( !Split( countA, &ppShapes[idxA], mid0 ) )
    { return CreateNode( count, ppShapes ); }

    if ( !Split( countB, &ppShapes[idxB], mid1 ) )
    { return CreateNode( count, ppShapes ); }

    if ( !Split( countC, &ppShapes[idxC], mid2 ) )
    { return CreateNode( count, ppShapes ); }

    if ( !Split( countD, &ppShapes[idxD], mid3 ) )
    { return CreateNode( count, ppShapes ); }

    auto idx0 = 0;
    auto idx1 = mid0;
    auto idx2 = midL;
    auto idx3 = midL + mid1;
    auto idx4 = mid;
    auto idx5 = mid + mid2;
    auto idx6 = mid + midR;
    auto idx7 = mid + midR + mid3;

    auto count0 = idx1  - idx0;
    auto count1 = idx2  - idx1;
    auto count2 = idx3  - idx2;
    auto count3 = idx4  - idx3;
    auto count4 = idx5  - idx4;
    auto count5 = idx6  - idx5;
    auto count6 = idx7  - idx6;
    auto count7 = count - idx7;

    return new BVH8(
        BVH8::Create(count0, &ppShapes[idx0]),
        BVH8::Create(count1, &ppShapes[idx1]),
        BVH8::Create(count2, &ppShapes[idx2]),
        BVH8::Create(count3, &ppShapes[idx3]),
        BVH8::Create(count4, &ppShapes[idx4]),
        BVH8::Create(count5, &ppShapes[idx5]),
        BVH8::Create(count6, &ppShapes[idx6]),
        BVH8::Create(count7, &ppShapes[idx7]));
}

} // namespace s3d
