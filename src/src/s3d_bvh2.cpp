//-------------------------------------------------------------------------------------------------
// File : s3d_bvh2.cpp
// Desc : Binary BVH.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_bvh2.h>
#include <s3d_leaf.h>
#include <s3d_bucket.h>
#include <algorithm>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
// Constant Values.
//-------------------------------------------------------------------------------------------------
constexpr int BucketCount = 12;     //!< バケット数です.

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

} // namespace /* anonymous */


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// BVH2 class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
BVH2::BVH2( IShape* pShape0, IShape* pShape1, const BoundingBox& box )
: m_Count   ( 1 )
, m_Box     ( box )
{
    m_pNode[0] = pShape0;
    m_pNode[1] = pShape1;
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
BVH2::~BVH2()
{
    for(auto i=0; i<2; ++i)
    { SafeRelease( m_pNode[i] ); }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void BVH2::AddRef()
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理です.
//-------------------------------------------------------------------------------------------------
void BVH2::Release()
{
    m_Count--;
    if ( m_Count == 0 )
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 BVH2::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------------------
bool BVH2::IsHit( const RaySet& raySet, HitRecord& record ) const
{
    if ( !m_Box.IsHit( raySet.ray ) )
    { return false; }

    auto hit = false;
    for ( auto i=0; i<2; ++i )
    { hit |= m_pNode[i]->IsHit( raySet, record ); }

    return hit;
}

//-------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------------
BoundingBox BVH2::GetBox() const
{ return m_Box; }

//-------------------------------------------------------------------------------------------------
//      中心座標を取得します
//-------------------------------------------------------------------------------------------------
Vector3 BVH2::GetCenter() const
{
    auto center  = m_pNode[0]->GetCenter();
         center += m_pNode[1]->GetCenter();
    return center / 2.0f;
}

//-------------------------------------------------------------------------------------------------
//      生成処理を行います.
//-------------------------------------------------------------------------------------------------
IShape* BVH2::Create( size_t count, IShape** ppShapes )
{
    // 最小要素に満たないものは葉ノードとして生成.  2つのノードがそれぞれ子供をもつので 2 * 2 = 4 が最小.
    if ( count <= 4 )
    { return Leaf::Create(count, ppShapes); }

    auto bound = CreateMergedBox( count, ppShapes );

    // バウンディングボックスを生成.
    auto centroid = CreateCentroidBox( count, ppShapes );

    // 分割軸を決めるためバウンディングボックスの最長軸を取得.
    auto axis = GetLongestAxis( centroid );

    if (centroid.maxi.a[axis] == centroid.mini.a[axis])
    { return Leaf::Create(count, ppShapes); }

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

    auto mid = count / 2;

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
    {
        // 最小コスト満たすものがなかったら葉ノードとする.
        return Leaf::Create(count, ppShapes);
    }

    auto cnt0 = mid;
    auto cnt1 = count - mid;

    // 再帰呼び出し.
    return new BVH2(
        BVH2::Create(cnt0, &ppShapes[0]),
        BVH2::Create(cnt1, &ppShapes[mid]),
        bound);
}

} // namespace s3d
