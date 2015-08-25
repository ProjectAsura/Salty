//---------------------------------------------------------------------------
// File : s3d_bvh.cpp
// Desc : Bounding Volume Hierarchy Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <s3d_bvh.h>
#include <new>


namespace /* anonymous */ {

// QBVH 巡回テーブル(早期打ち切り用).
const u32 QbvhTable[][4] =  {
    { 0, 1, 2, 3 },
    { 0, 1, 3, 2 },
    { 1, 0, 2, 3 },
    { 1, 0, 3, 2 },
    { 2, 3, 0, 1 },
    { 3, 2, 0, 1 },
    { 2, 3, 1, 0 },
    { 3, 2, 1, 0 },
};

//---------------------------------------------------------------------------
//      分割します.
//---------------------------------------------------------------------------
s32 MedianSplit( s3d::IShape** ppShapes, s32 size, f64 pivotVal, s32 axis )
{
    s3d::BoundingBox bbox;
    f64 centroid;
    s32 result = 0;

    for( s32 i=0; i<size; ++i )
    {
        // AABBを取得.
        bbox = ppShapes[i]->GetBox();

        // 中心を取得.
        centroid = ( bbox.mini.a[axis] + bbox.maxi.a[axis] ) / 2.0;

        // ピボットと比較.
        if ( centroid < pivotVal )
        {
            s3d::IShape* pTemp = ppShapes[ i ];

            ppShapes[ i ]      = ppShapes[ result ];
            ppShapes[ result ] = pTemp;
            result++;
        }
    }

    if ( result == 0 || result == size )
    { result = size / 2; }

    return result;
}


//---------------------------------------------------------------------------
//      最も長い軸を示すインデックスを取得します.
//---------------------------------------------------------------------------
s32 GetAxisIndex( s3d::Vector3 size )
{
    s32 axis = 0;
    if ( size.x > size.y )
    { axis = ( size.x > size.z ) ? 0 : 2; }
    else
    { axis = ( size.y > size.z ) ? 1 : 2; }

    return axis;
}

//-------------------------------------------------------------------------------------------------
//      SAHによる分割を行います.
//-------------------------------------------------------------------------------------------------
bool SahSplit( std::vector<s3d::IShape*> shapes, s32& bestIndex, s32& bestAxis )
{
    if ( shapes.size() <= 2 )
    { return false; }

    auto bestCost = F32_MAX;
    bestAxis  = -1;
    bestIndex = -1;

    for( auto axis = 0; axis < 3; ++axis )
    {
        std::vector<s3d::IShape*> axisSortedL(shapes);
        std::vector<s3d::IShape*> axisSortedR;

        // 現在の軸においてボックスの重心座標を用いてソートを行う.
        std::sort(
            axisSortedL.begin(), 
            axisSortedL.end(),
            [axis](const s3d::IShape* l, const s3d::IShape* r)
            {
                auto boxL = l->GetBox();
                auto boxR = r->GetBox();
                assert( !boxL.isEmpty );
                assert( !boxR.isEmpty );
                auto cl = ( boxL.maxi.a[axis] + boxL.mini.a[axis] ) * 0.5f;
                auto cr = ( boxR.maxi.a[axis] + boxR.mini.a[axis] ) * 0.5f;
                return cl < cr;
            }
        );

        std::vector<f32> leftArea (axisSortedL.size() + 1, F32_MAX);
        std::vector<f32> rightArea(axisSortedL.size() + 1, F32_MAX);

        axisSortedR.push_back( axisSortedL.back() );
        axisSortedL.pop_back();

        s3d::BoundingBox box = CreateMergedBox( &axisSortedR[0], static_cast<u32>(axisSortedR.size()) );

        for( auto i=static_cast<int>(axisSortedL.size()); i>=0; i-- )
        {
            rightArea[i] = SurfaceArea( box );
            auto pShape = axisSortedL.back();
            box = s3d::BoundingBox::Merge( box, pShape->GetBox() );

            axisSortedR.push_back( pShape );
            axisSortedL.pop_back();
        }

        axisSortedL.push_back( axisSortedR.back() );
        axisSortedR.pop_back();

        box = CreateMergedBox( &axisSortedL[0], static_cast<u32>(axisSortedL.size()) );

        s32 idx = 0;
        for( auto i=static_cast<int>(axisSortedR.size()); i>=0; i--, idx++ )
        {
            leftArea[idx] = SurfaceArea( box );
            auto pShape = axisSortedR.back();
            box = s3d::BoundingBox::Merge( box, pShape->GetBox() );

            axisSortedL.push_back( pShape );
            axisSortedR.pop_back();

            // "Ray Tracing Deformable Scenes Using Dynamic Bounding Volume Hierarchies"
            //  ACM Transactions on Graphics 26(1), 2007
            // Equation(2) により評価する.
            auto cost = leftArea[idx] * axisSortedL.size() + rightArea[idx] * axisSortedR.size(); // 大小さえわかればいいので，定数を省いた.

            if ( cost < bestCost )
            {
                bestCost  = cost;
                bestIndex = idx;
                bestAxis  = axis;
            }
        }
    }

    // 葉ノードよりも良いパーティションが見つからなかった.
    if ( bestAxis == -1 )
    { return false; }

    // "最良の軸" で S をソートする.
    std::sort( 
        shapes.begin(),
        shapes.end(),
        [bestAxis]( const s3d::IShape* l, const s3d::IShape* r)
        {
            auto boxL = l->GetBox();
            auto boxR = r->GetBox();
            assert( !boxL.isEmpty );
            assert( !boxR.isEmpty );
            auto cl = ( boxL.maxi.a[bestAxis] + boxL.mini.a[bestAxis] ) * 0.5f;
            auto cr = ( boxR.maxi.a[bestAxis] + boxR.mini.a[bestAxis] ) * 0.5f;
            return cl < cr;
        }
    );

    return true;
}

} // namespace /* anonymous */


namespace s3d {

////////////////////////////////////////////////////////////////////////////
// BVH structure
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------
BVH::BVH()
: box()
{
    pShape[0] = nullptr;
    pShape[1] = nullptr;
}

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
BVH::BVH( IShape* pShape1, IShape* pShape2, const BoundingBox& bbox )
: box  ( bbox )
{
    pShape[0] = pShape1;
    pShape[1] = pShape2;
}

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
BVH::BVH( IShape* pShape1, IShape* pShape2 )
{
    pShape[0] = pShape1;
    pShape[1] = pShape2;
    box = BoundingBox::Merge( pShape1->GetBox(), pShape2->GetBox() );
}

//--------------------------------------------------------------------------
//      解放処理を行います.
//--------------------------------------------------------------------------
void BVH::Dispose()
{
    for( u32 i=0; i<2; ++i )
    {
        if ( pShape[i]->IsPrimitive() )
        { pShape[i] = nullptr; }
        else
        {
            auto bvh = dynamic_cast<BVH*>( pShape[i] );
            if ( bvh != nullptr )
            { bvh->Dispose(); }
        }
    }

    delete this;
}

//--------------------------------------------------------------------------
//      衝突判定を行います.
//--------------------------------------------------------------------------
bool BVH::IsHit( const Ray& ray, HitRecord& record ) const
{
    if ( !box.IsHit( ray ) )
    { return false; }

    auto hit = false;
    for( u32 i=0; i<2; ++i )
    { hit |= pShape[i]->IsHit( ray, record ); }

    return hit;
}

//--------------------------------------------------------------------------
//      マテリアルを取得します.
//--------------------------------------------------------------------------
IMaterial* BVH::GetMaterial() const
{ return nullptr; }

//--------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//--------------------------------------------------------------------------
BoundingBox BVH::GetBox() const
{ return box; }

//--------------------------------------------------------------------------
//      基本図形であるかどうか判定します.
//--------------------------------------------------------------------------
bool BVH::IsPrimitive() const
{ return false; }

//--------------------------------------------------------------------------
//      中心座標を取得します.
//--------------------------------------------------------------------------
Vector3 BVH::GetCenter() const
{ return box.center; }

//--------------------------------------------------------------------------
//      ブランチを構築します.
//--------------------------------------------------------------------------
IShape* BVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{
    // 左と右を入れたインスタンスを生成.
    if ( numShapes <= 2 )
    { return new Leaf( numShapes, ppShapes ); }

    // AABBを求める.
    BoundingBox bbox = CreateMergedBox( ppShapes, numShapes );

    // ピボットを求める.
    Vector3 pivot = ( bbox.maxi + bbox.mini ) * 0.5f;

    // AABBの各辺の長さを求める.
    Vector3 size  = bbox.maxi - bbox.mini;
    s32 axis = GetAxisIndex( size );

    // 中間値.
    s32 midPoint = MedianSplit( ppShapes, numShapes, pivot.a[axis], axis );

    s32 idx[2] = {
        0,
        midPoint
    };

    u32 num[2] = {
        u32(midPoint),
        numShapes - u32(midPoint)
    };

    IShape* left;
    IShape* right;

    left  = BuildBranch( &ppShapes[idx[0]], num[0] );
    right = BuildBranch( &ppShapes[idx[1]], num[1] );

    // インスタンスを返却.
    return new BVH( left, right, bbox );
}

IShape* BVH::Build(std::vector<IShape*>& shapes)
{
    if ( shapes.size() <= 2 )
    { return new Leaf(shapes.size(), &shapes[0]); }

    s32 bestIndex = -1;
    s32 bestAxis  = -1;
    if ( !SahSplit( shapes, bestIndex, bestAxis ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    std::vector<IShape*> left(shapes.begin(), shapes.begin() + bestIndex);
    std::vector<IShape*> right(shapes.begin() + bestIndex, shapes.end());

    return new BVH(
        Build(left),
        Build(right));
}


////////////////////////////////////////////////////////////////////////////
// QBVH structure
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------
QBVH::QBVH()
{
    pShape[0] = nullptr;
    pShape[1] = nullptr;
    pShape[2] = nullptr;
    pShape[3] = nullptr;
}

QBVH::QBVH( IShape* shape0, IShape* shape1, IShape* shape2, IShape* shape3, s32 top, s32 left, s32 right )
: axisTop  ( top )
, axisLeft ( left )
, axisRight( right )
{
    pShape[0] = shape0;
    pShape[1] = shape1;
    pShape[2] = shape2;
    pShape[3] = shape3;

    BoundingBox box0 = shape0->GetBox();
    BoundingBox box1 = shape1->GetBox();
    BoundingBox box2 = shape2->GetBox();
    BoundingBox box3 = shape3->GetBox();

    box = BoundingBox4( box0, box1, box2, box3 );
}


//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
QBVH::QBVH( IShape** ppShapes, const BoundingBox4& quadBox, s32 top, s32 left, s32 right )
: box( quadBox )
, axisTop( top )
, axisLeft( left )
, axisRight( right )
{
    pShape[0] = ppShapes[ 0 ];
    pShape[1] = ppShapes[ 1 ];
    pShape[2] = ppShapes[ 2 ];
    pShape[3] = ppShapes[ 3 ];
}

//--------------------------------------------------------------------------
//      交差判定を行います.
//--------------------------------------------------------------------------
bool QBVH::IsHit( const Ray& ray, HitRecord& record ) const
{
    // まず子のバウンディングボックスと交差判定.
    s32 mask = 0;
    if ( !box.IsHit( Ray4( ray ), mask ) )
    { return false; }

    // 巡回テーブルのインデックスを算出.
    u32 idx = ( ray.sign[axisTop]  << 2 )
            | ( ray.sign[axisLeft] << 1 )
            | ( ray.sign[axisRight] );

    for( auto i=0; i<4; ++i )
    {
        auto id = QbvhTable[idx][i];
        if ( mask & ( 0x1 << id ) )
        {
            if ( pShape[id]->IsHit( ray, record ) )
            { return true; }
        }
    }

    return false;
}

//--------------------------------------------------------------------------
//      マテリアルを取得します.
//--------------------------------------------------------------------------
IMaterial* QBVH::GetMaterial() const
{ return nullptr; }

//--------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//--------------------------------------------------------------------------
BoundingBox QBVH::GetBox() const
{ return box.GetBox(); }

//--------------------------------------------------------------------------
//      基本図形かどうか判定します.
//--------------------------------------------------------------------------
bool QBVH::IsPrimitive() const
{ return false; }

//--------------------------------------------------------------------------
//      中心座標を取得します.
//--------------------------------------------------------------------------
Vector3 QBVH::GetCenter() const
{
    Vector3 result = pShape[0]->GetCenter();
    for( u32 i=1; i<4; ++i )
    { result += pShape[i]->GetCenter(); }
    result /= 4;

    return result;
}

//--------------------------------------------------------------------------
//      ブランチを構築します.
//--------------------------------------------------------------------------
IShape* QBVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{
    if ( numShapes <= 4 )
    { return new Leaf( numShapes, ppShapes ); }

    // AABBを求める.
    BoundingBox bbox = CreateMergedBox( ppShapes, numShapes );

    // ピボットを求める.
    Vector3 pivot = ( bbox.maxi + bbox.mini ) / 2.0;

    // AABBの各辺の長さを求める.
    Vector3 size  = bbox.maxi - bbox.mini;
    s32 axis = GetAxisIndex( size );

    // 中間値.
    s32 midPoint = MedianSplit( ppShapes, numShapes, pivot.a[axis], axis );

    s32 idx1[2] = {
        0,
        midPoint
    };

    u32 num1[2] = {
        u32(midPoint),
        numShapes - u32(midPoint),
    };

    if ( num1[0] == 0 || num1[1] == 0 )
    { return new Leaf( numShapes, ppShapes ); }

    // 更に分割するためにAABBを求める.
    BoundingBox bboxL = CreateMergedBox( &ppShapes[idx1[0]], num1[0] );
    BoundingBox bboxR = CreateMergedBox( &ppShapes[idx1[1]], num1[1] );

    // AABBの各辺の長さを求める.
    Vector3 sizeL = bboxL.maxi - bboxL.mini;
    Vector3 sizeR = bboxR.maxi - bboxR.mini;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // 更に分割するピボットを求める.
    Vector3 pivotL = ( bboxL.maxi + bboxL.mini ) / 2.0;
    Vector3 pivotR = ( bboxR.maxi + bboxR.mini ) / 2.0;

    // 分割する.
    s32 midPointL = MedianSplit( &ppShapes[idx1[0]], num1[0], pivotL.a[axisL], axisL );
    s32 midPointR = MedianSplit( &ppShapes[idx1[1]], num1[1], pivotR.a[axisR], axisR );

    s32 idx2[4] = {
        idx1[0],
        idx1[0] + midPointL,
        idx1[1],
        idx1[1] + midPointR
    };

    u32 num2[4] = {
        u32(midPointL),
        num1[0] - u32(midPointL),
        u32(midPointR),
        num1[1] - u32(midPointR),
    };

    if ( num2[0] == 0 || num2[1] == 0 || num2[2] == 0 || num2[3] == 0 )
    { return new Leaf( numShapes, ppShapes ); }

    IShape* pShapes[ 4 ];
    BoundingBox box[ 4 ];

    for( u32 i=0; i<4; ++i )
    {
        box[ i ] = CreateMergedBox( &ppShapes[ idx2[ i ] ], num2[ i ] );
        pShapes[ i ] = BuildBranch( &ppShapes[ idx2[ i ] ], num2[ i ] );
    }

    // 16byteアライメントでメモリを確保.
    u8* pBuf = (u8*)_aligned_malloc( sizeof(QBVH), 16 );
    return new (pBuf) QBVH( pShapes, BoundingBox4( box ), axis, axisL, axisR );
}

IShape* QBVH::Build(std::vector<IShape*>& shapes)
{
    if ( shapes.size() <= 4 )
    { return new Leaf(shapes.size(), &shapes[0]); }

    s32 bestIndex = -1;
    s32 bestAxis  = -1;
    if ( !SahSplit( shapes, bestIndex, bestAxis ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    std::vector<IShape*> left(shapes.begin(), shapes.begin() + bestIndex);
    std::vector<IShape*> right(shapes.begin() + bestIndex, shapes.end());

    s32 bestIndexL = -1;
    s32 bestIndexR = -1;
    s32 bestAxisL  = -1;
    s32 bestAxisR  = -1;
    if ( !SahSplit( left, bestIndexL, bestAxisL ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    if ( !SahSplit( right, bestIndexR, bestAxisR ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    std::vector<IShape*> a(left.begin(), left.begin() + bestIndexL);
    std::vector<IShape*> b(left.begin() + bestIndexL, left.end());
    std::vector<IShape*> c(right.begin(), right.begin() + bestIndexR);
    std::vector<IShape*> d(right.begin() + bestIndexR, right.end());

    auto buf = (u8*)_aligned_malloc( sizeof(QBVH), 16 );
    return new(buf) QBVH(
        Build(a),
        Build(b),
        Build(c),
        Build(d),
        bestIndex,
        bestIndexL,
        bestIndexR);
}

//--------------------------------------------------------------------------
//      破棄処理を行います.
//--------------------------------------------------------------------------
void QBVH::Dispose()
{
    for( u32 i=0; i<4; ++i )
    {
        if ( pShape[ i ] == nullptr )
        { continue; }

        if ( pShape[ i ]->IsPrimitive() )
        { pShape[ i ] = nullptr; }
        else
        {
            auto pObj = dynamic_cast<IDisposable*>( pShape[ i ] );
            if ( pObj != nullptr )
            { pObj->Dispose(); }
        }
    }

    _aligned_free( this );
}


////////////////////////////////////////////////////////////////////////////
// OBVH structure
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------
OBVH::OBVH()
{
    for( u32 i=0; i<8; ++i )
    { pShape[ i ] = nullptr; }
}

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
OBVH::OBVH( IShape** ppShapes )
{
    BoundingBox boxes[8];
    for( u32 i=0; i<8; ++i )
    {
        pShape[ i ] = ppShapes[ i ];
        boxes [ i ] = ppShapes[ i ]->GetBox();
    }

    box = BoundingBox8( boxes );
}

OBVH::OBVH
(
    IShape* shape0,
    IShape* shape1,
    IShape* shape2,
    IShape* shape3,
    IShape* shape4,
    IShape* shape5,
    IShape* shape6,
    IShape* shape7,
    s32 top,
    s32 left,
    s32 right,
    s32 a,
    s32 b,
    s32 c,
    s32 d
)
: axisTop( top )
, axisL( left )
, axisR( right )
, axisA( a )
, axisB( b )
, axisC( c )
, axisD( d )
{
    pShape[0] = shape0;
    pShape[1] = shape1;
    pShape[2] = shape2;
    pShape[3] = shape3;
    pShape[4] = shape4;
    pShape[5] = shape5;
    pShape[6] = shape6;
    pShape[7] = shape7;

    box = BoundingBox8(
        shape0->GetBox(),
        shape1->GetBox(),
        shape2->GetBox(),
        shape3->GetBox(),
        shape4->GetBox(),
        shape5->GetBox(),
        shape6->GetBox(),
        shape7->GetBox());
}

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
OBVH::OBVH( IShape** ppShapes, const BoundingBox8& octBox )
: box( octBox )
{
    for( u32 i=0; i<8; ++i )
    { pShape[ i ] = ppShapes[ i ]; }
}

//--------------------------------------------------------------------------
//      交差判定を行います.
//--------------------------------------------------------------------------
bool OBVH::IsHit( const Ray& ray, HitRecord& record ) const
{
    // まず子のバウンディングボックスと交差判定.
    s32 mask = 0;
    if ( !box.IsHit( Ray8( ray ), mask ) )
    { return record.distance < F_MAX; }

    // 次にバウンディングボックスとヒットした子のみたどっていく.
    auto hit = false;
    for( auto i=0; i<8; ++i )
    {
        if ( mask & ( 0x1 << i ) )
        { hit |= pShape[i]->IsHit( ray, record ); }
    }

    return hit;
}

//--------------------------------------------------------------------------
//      マテリアルを取得します.
//--------------------------------------------------------------------------
IMaterial* OBVH::GetMaterial() const
{ return nullptr; }

//--------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//--------------------------------------------------------------------------
BoundingBox OBVH::GetBox() const
{ return box.GetBox(); }

//--------------------------------------------------------------------------
//      基本図形かどうか判定します.
//--------------------------------------------------------------------------
bool OBVH::IsPrimitive() const
{ return false; }

//--------------------------------------------------------------------------
//      中心座標を取得します.
//--------------------------------------------------------------------------
Vector3 OBVH::GetCenter() const
{
    Vector3 result = pShape[0]->GetCenter();
    for( u32 i=1; i<8; ++i )
    { result += pShape[i]->GetCenter(); }
    result /= 8;
    return result;
}

//--------------------------------------------------------------------------
//      ブランチを構築します.
//--------------------------------------------------------------------------
IShape* OBVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{
    if ( numShapes <= 8 )
    { return new Leaf( numShapes, ppShapes ); }

    // -------------------------
    //      1段階目.
    // -------------------------
    // AABBを求める.
    BoundingBox bbox = CreateMergedBox( ppShapes, numShapes );

    // ピボットを求める.
    Vector3 pivot = ( bbox.maxi + bbox.mini ) / 2.0f;

    // AABBの各辺の長さを求める.
    Vector3 size  = bbox.maxi - bbox.mini;
    s32 axis = GetAxisIndex( size );

    // 中間値.
    s32 midPoint = MedianSplit( ppShapes, numShapes, pivot.a[axis], axis );

    s32 idx1[2] = {
        0,
        midPoint
    };

    u32 num1[2] = {
        u32(midPoint),
        numShapes - u32(midPoint)
    };

    if ( num1[0] == 0 || num1[1] == 0 )
    { return new Leaf( numShapes, ppShapes ); }

    // -------------------------
    //      2段階目.
    // -------------------------
    // 更に分割するためにAABBを求める.
    BoundingBox bboxL = CreateMergedBox( &ppShapes[idx1[0]], num1[0] );
    BoundingBox bboxR = CreateMergedBox( &ppShapes[idx1[1]], num1[1] );

    // AABBの各辺の長さを求める.
    Vector3 sizeL = bboxL.maxi - bboxL.mini;
    Vector3 sizeR = bboxR.maxi - bboxR.mini;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // 更に分割するピボットを求める.
    Vector3 pivotL = ( bboxL.maxi + bboxL.mini ) / 2.0f;
    Vector3 pivotR = ( bboxR.maxi + bboxR.mini ) / 2.0f;

    // 分割する.
    s32 midPointL = MedianSplit( &ppShapes[idx1[0]], num1[0], pivotL.a[axisL], axisL );
    s32 midPointR = MedianSplit( &ppShapes[idx1[1]], num1[1], pivotR.a[axisR], axisR );

   s32 idx2[4] = {
        idx1[0],
        midPointL,
        idx1[1],
        idx1[1] + midPointR
    };

    u32 num2[4] = {
        u32(midPointL),
        num1[0] - u32(midPointL),
        u32(midPointR),
        num1[1] - u32(midPointR)
    };

    if ( num2[0] == 0 || num2[1] == 0 || num2[2] == 0 || num2[3] == 0 )
    { return new Leaf( numShapes, ppShapes ); }

    // -------------------------
    //      3段階目.
    // -------------------------

    // 更に更に分割するためにAABBを求める.
    BoundingBox bbox1 = CreateMergedBox( &ppShapes[idx2[0]], num2[0] );
    BoundingBox bbox2 = CreateMergedBox( &ppShapes[idx2[1]], num2[1] );
    BoundingBox bbox3 = CreateMergedBox( &ppShapes[idx2[2]], num2[2] );
    BoundingBox bbox4 = CreateMergedBox( &ppShapes[idx2[3]], num2[3] );

    // AABBの各辺の長さを求める.
    Vector3 size1 = bbox1.maxi - bbox1.mini;
    Vector3 size2 = bbox2.maxi - bbox2.mini;
    Vector3 size3 = bbox3.maxi - bbox3.mini;
    Vector3 size4 = bbox4.maxi - bbox4.mini;
    s32 axis1 = GetAxisIndex( size1 );
    s32 axis2 = GetAxisIndex( size2 );
    s32 axis3 = GetAxisIndex( size3 );
    s32 axis4 = GetAxisIndex( size4 );

    // 更に更に分割するピボットを求める.
    Vector3 pivot1 = ( bbox1.maxi + bbox1.mini ) / 2.0f;
    Vector3 pivot2 = ( bbox2.maxi + bbox2.mini ) / 2.0f;
    Vector3 pivot3 = ( bbox3.maxi + bbox3.mini ) / 2.0f;
    Vector3 pivot4 = ( bbox4.maxi + bbox4.mini ) / 2.0f;

    // 分割する.
    s32 midPoint1 = MedianSplit( &ppShapes[idx2[0]], num2[0], pivot1.a[axis1], axis1 );
    s32 midPoint2 = MedianSplit( &ppShapes[idx2[1]], num2[1], pivot2.a[axis2], axis2 );
    s32 midPoint3 = MedianSplit( &ppShapes[idx2[2]], num2[2], pivot3.a[axis3], axis3 );
    s32 midPoint4 = MedianSplit( &ppShapes[idx2[3]], num2[3], pivot4.a[axis4], axis4 );

    s32 idx3[8] = {
        idx2[0],
        idx2[0] + midPoint1,
        idx2[1],
        idx2[1] + midPoint2,
        idx2[2],
        idx2[2] + midPoint3,
        idx2[3],
        idx2[3] + midPoint4
    };

    u32 num3[8] = {
        u32(midPoint1),
        num2[0] - u32(midPoint1),
        u32(midPoint2),
        num2[1] - u32(midPoint2),
        u32(midPoint3),
        num2[2] - u32(midPoint3),
        u32(midPoint4),
        num2[3] - u32(midPoint4),
    };

    if ( num3[0] == 0 || num3[1] == 0 || num3[2] == 0 || num3[3] == 0
      || num3[4] == 0 || num3[5] == 0 || num3[6] == 0 || num3[7] == 0 )
    { return new Leaf( numShapes, ppShapes ); }

    IShape* pShapes[ 8 ] = { nullptr };
    BoundingBox box[ 8 ];

    for( u32 i=0; i<8; ++i )
    {
        box[ i ] = CreateMergedBox( &ppShapes[ idx3[ i ] ], num3[ i ] );
        pShapes[ i ] = BuildBranch( &ppShapes[ idx3[ i ] ], num3[ i ] );
    }

    // 32byteアライメントでメモリを確保.
    u8* pBuf = (u8*)_aligned_malloc( sizeof(OBVH), 32 );
    return new (pBuf) OBVH( pShapes, box );
}

IShape* OBVH::Build(std::vector<IShape*>& shapes)
{
    if ( shapes.size() <= 8 )
    { return new Leaf(shapes.size(), &shapes[0]); }

    s32 bestIndex = -1;
    s32 bestAxis  = -1;
    if ( !SahSplit( shapes, bestIndex, bestAxis ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    std::vector<IShape*> left(shapes.begin(), shapes.begin() + bestIndex);
    std::vector<IShape*> right(shapes.begin() + bestIndex, shapes.end());

    s32 bestIndexL = -1, bestIndexR = -1;
    s32 bestAxisL  = -1, bestAxisR  = -1;
    if ( !SahSplit( left, bestIndexL, bestAxisL ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    if ( !SahSplit( right, bestIndexR, bestAxisR ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    std::vector<IShape*> a(left.begin(), left.begin() + bestIndexL);
    std::vector<IShape*> b(left.begin() + bestIndexL, left.end());
    std::vector<IShape*> c(right.begin(), right.begin() + bestIndexR);
    std::vector<IShape*> d(right.begin() + bestIndexR, right.end());

    s32 bestIndexA = -1, bestIndexB = -1, bestIndexC = -1, bestIndexD = -1;
    s32 bestAxisA  = -1, bestAxisB  = -1, bestAxisC  = -1, bestAxisD  = -1;

    if ( !SahSplit( a, bestIndexA, bestAxisA ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    if ( !SahSplit( b, bestIndexB, bestAxisB ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    if ( !SahSplit( c, bestIndexC, bestAxisC ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    if ( !SahSplit( d, bestIndexD, bestAxisD ) )
    { return new Leaf(shapes.size(), &shapes[0]); }

    std::vector<IShape*> e( a.begin(), a.begin() + bestIndexA );
    std::vector<IShape*> f( a.begin() + bestIndexA, a.end() );
    std::vector<IShape*> g( b.begin(), b.begin() + bestIndexB );
    std::vector<IShape*> h( b.begin() + bestIndexB, b.end() );
    std::vector<IShape*> i( c.begin(), c.begin() + bestIndexC );
    std::vector<IShape*> j( c.begin() + bestIndexC, c.end() );
    std::vector<IShape*> k( d.begin(), d.begin() + bestIndexD );
    std::vector<IShape*> l( d.begin() + bestIndexD, d.end() );

    auto buf = (u8*)_aligned_malloc( sizeof(OBVH), 32 );
    return new(buf) OBVH(
        Build(e),
        Build(f),
        Build(g),
        Build(h),
        Build(i),
        Build(j),
        Build(k),
        Build(l),
        bestAxis,
        bestAxisL,
        bestAxisR,
        bestAxisA,
        bestAxisB,
        bestAxisC,
        bestAxisD);
}

//--------------------------------------------------------------------------
//      破棄処理を行います.
//--------------------------------------------------------------------------
void OBVH::Dispose()
{
    for( u32 i=0; i<8; ++i )
    {
        if ( pShape[ i ] == nullptr )
        { continue; }

        if ( pShape[ i ]->IsPrimitive() )
        { pShape[ i ] = nullptr; }
        else
        {
            auto pObj = dynamic_cast<IDisposable*>( pShape[ i ] );
            if ( pObj != nullptr )
            { pObj->Dispose(); }
        }
    }

    _aligned_free( this );
}


} // namespace s3d
