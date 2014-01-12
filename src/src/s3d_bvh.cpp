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

//---------------------------------------------------------------------------
//      分割します.
//---------------------------------------------------------------------------
s32 Split( s3d::IShape** ppShapes, s32 size, f64 pivotVal, s32 axis )
{
    s3d::BoundingBox bbox;
    f64 centroid;
    s32 result = 0;

    for( s32 i=0; i<size; ++i )
    {
        // AABBを取得.
        bbox = ppShapes[i]->GetBox();

        // 中心を取得.
        centroid = ( bbox.min.a[axis] + bbox.max.a[axis] ) / 2.0;

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
//      分割します(メッシュ用).
//---------------------------------------------------------------------------
s32 Split( s3d::Triangle* pShapes, s32 size, f64 pivotVal, s32 axis )
{
    s3d::BoundingBox bbox;
    f64 centroid;
    s32 result = 0;

    for( s32 i=0; i<size; ++i )
    {
        // AABBを取得.
        bbox = pShapes[i].GetBox();

        // 中心を取得.
        centroid = ( bbox.min.a[axis] + bbox.max.a[axis] ) / 2.0;

        // ピボットと比較.
        if ( centroid < pivotVal )
        {
            s3d::Triangle pTemp = pShapes[ i ];

            pShapes[ i ]      = pShapes[ result ];
            pShapes[ result ] = pTemp;
            result++;
        }
    }

    if ( result == 0 || result == size )
    { result = size / 2; }

    return result;
}

//---------------------------------------------------------------------------
//      マージしたバウンディングボックスを生成します.
//---------------------------------------------------------------------------
s3d::BoundingBox CreateMergedBox
(
    s3d::IShape** ppShapes,
    const u32     numShapes
)
{
    if ( numShapes == 0 || ppShapes == nullptr )
    { return s3d::BoundingBox(); }

    s3d::BoundingBox box = ppShapes[0]->GetBox();

    for( u32 i=1; i<numShapes; ++i )
    { box = s3d::BoundingBox::Merge( box, ppShapes[i]->GetBox() ); }

    return box;
}

//---------------------------------------------------------------------------
//      マージしたバウンディングボックスを生成します(メッシュ用).
//---------------------------------------------------------------------------
s3d::BoundingBox CreateMergedBox
(
    s3d::Triangle*  pTriangles,
    const u32       numTriangles
)
{
    if ( numTriangles == 0 || pTriangles == nullptr )
    { return s3d::BoundingBox(); }

    s3d::BoundingBox box = pTriangles[0].GetBox();

    for( u32 i=1; i<numTriangles; ++i )
    { box = s3d::BoundingBox::Merge( box, pTriangles[i].GetBox() ); }

    return box;
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

} // namespace /* anonymous */


namespace s3d {

////////////////////////////////////////////////////////////////////////////
// BVH structure
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------
BVH::BVH()
: box  ()
, pLeft ( nullptr )
, pRight( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
BVH::BVH( IShape* pShape1, IShape* pShape2, const BoundingBox& bbox )
: box  ( bbox )
, pLeft ( pShape1 )
, pRight( pShape2 )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
BVH::BVH( IShape* pShape1, IShape* pShape2 )
: pLeft ( pShape1 )
, pRight( pShape2 )
{
    box = BoundingBox::Merge( pShape1->GetBox(), pShape2->GetBox() );
}

//--------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------
BVH::~BVH()
{
    /* 解放処理はDestroyBranch()で行います */
    pLeft  = nullptr;
    pRight = nullptr;
}

//--------------------------------------------------------------------------
//      衝突判定を行います.
//--------------------------------------------------------------------------
bool BVH::IsHit( const Ray& ray, HitRecord& record ) const
{
    if ( !box.IsHit( ray ) )
    { return false; }

    bool isHit1 = pRight->IsHit( ray, record );
    bool isHit2 = pLeft->IsHit( ray, record );

    return ( isHit1 || isHit2 );
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
//      ブランチを構築します.
//--------------------------------------------------------------------------
IShape* BVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{
    // そのまま返却.
    if ( numShapes <= 1 ) 
    { return ppShapes[0]; }

    // 左と右を入れたインスタンスを生成.
    if ( numShapes == 2 )
    { return new BVH( ppShapes[0], ppShapes[1] ); }

    // AABBを求める.
    BoundingBox bbox = CreateMergedBox( ppShapes, numShapes );

    // ピボットを求める.
    Vector3 pivot = ( bbox.max + bbox.min ) * 0.5f;

    // AABBの各辺の長さを求める.
    Vector3 size  = bbox.max - bbox.min;
    s32 axis = 0;
    if ( size.x > size.y )
    { axis = ( size.x > size.z ) ? 0 : 2; }
    else
    { axis = ( size.y > size.z ) ? 1 : 2; }

    // 中間値.
    s32 midPoint = Split( ppShapes, numShapes, pivot.a[axis], axis );

    // ブランチ構築.
    IShape* left  = BuildBranch( ppShapes, midPoint );
    IShape* right = BuildBranch( &ppShapes[midPoint], numShapes - midPoint );

    // インスタンスを返却.
    return new BVH( left, right, bbox );
}


//--------------------------------------------------------------------------
//      ブランチを構築します.
//--------------------------------------------------------------------------
IShape* BVH::BuildBranch( Triangle* pShapes, const u32 numShapes )
{
    // そのまま返却.
    if ( numShapes == 1 ) 
    { return (IShape*)&pShapes[0]; }

    // 左と右を入れたインスタンスを生成.
    if ( numShapes == 2 )
    { return new BVH( &pShapes[0], &pShapes[1] ); }

    // AABBを求める.
    BoundingBox bbox = CreateMergedBox( pShapes, numShapes );

    // ピボットを求める.
    Vector3 pivot = ( bbox.max + bbox.min ) * 0.5f;

    // AABBの各辺の長さを求める.
    Vector3 size  = bbox.max - bbox.min;

    s32 axis = 0;
    if ( size.x > size.y )
    { axis = ( size.x > size.z ) ? 0 : 2; }
    else
    { axis = ( size.y > size.z ) ? 1 : 2; }

    // 中間値.
    s32 midPoint = Split( pShapes, numShapes, pivot.a[axis], axis );

    // ブランチ構築.
    IShape* left  = BuildBranch( pShapes, midPoint );
    IShape* right = BuildBranch( &pShapes[midPoint], numShapes - midPoint );

    // インスタンスを返却.
    return new BVH( left, right, bbox );
}

//--------------------------------------------------------------------------
//      ブランチを破棄します.
//--------------------------------------------------------------------------
void BVH::DestroyBranch( BVH* pShape )
{
    if ( pShape == nullptr )
    { return; }

    if ( pShape->IsPrimitive() )
    {
        pShape = nullptr;
        return;
    }

    DestroyBranch( (BVH*)pShape->pLeft );
    DestroyBranch( (BVH*)pShape->pRight );

    delete pShape;
    pShape = nullptr;
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
    pHitFunc  = nullptr;
}

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
QBVH::QBVH
(
    bool node,
    IShape* pShape1,
    IShape* pShape2,
    IShape* pShape3,
    IShape* pShape4
) 
{
    pShape[0] = pShape1;
    pShape[1] = pShape2;
    pShape[2] = pShape3;
    pShape[3] = pShape4;

    BoundingBox box0 = pShape1->GetBox();
    BoundingBox box1 = pShape2->GetBox();
    BoundingBox box2 = ( pShape3 == nullptr ) ? BoundingBox() : pShape3->GetBox();
    BoundingBox box3 = ( pShape4 == nullptr ) ? BoundingBox() : pShape4->GetBox();

    box = BoundingBox4( box0, box1, box2, box3 );
    pHitFunc = ( node ) ? HitFuncNode : HitFuncLeaf;
}

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
QBVH::QBVH
(
    bool node,
    IShape* pShape1, 
    IShape* pShape2,
    IShape* pShape3,
    IShape* pShape4,
    const BoundingBox4& quadBox
): box( quadBox )
{
    pShape[0] = pShape1;
    pShape[1] = pShape2;
    pShape[2] = pShape3;
    pShape[3] = pShape4;
    pHitFunc = ( node ) ? HitFuncNode : HitFuncLeaf;
}

//--------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------
QBVH::~QBVH()
{
    pShape[0] = nullptr;
    pShape[1] = nullptr;
    pShape[2] = nullptr;
    pShape[3] = nullptr;
}

//--------------------------------------------------------------------------
//      交差判定を行います.
//--------------------------------------------------------------------------
bool QBVH::IsHit( const Ray& ray, HitRecord& record ) const
{ return (*pHitFunc)( this, ray, record ); }

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
//      ブランチを構築します.
//--------------------------------------------------------------------------
IShape* QBVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{
    // そのまま返却.
    if ( numShapes <= 1 )
    { return ppShapes[0]; }

    // 16byteアライメントでメモリを確保.
    u8* pBuf = (u8*)_aligned_malloc( sizeof(QBVH), 16 );

    // 葉として生成.
    if ( numShapes == 2 )
    { return new(pBuf) QBVH( false, ppShapes[0], ppShapes[1], nullptr, nullptr ); }
    else if ( numShapes == 3 )
    { return new(pBuf) QBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], nullptr ); }
    else if ( numShapes == 4 )
    { return new(pBuf) QBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], ppShapes[3] ); }

    // AABBを求める.
    BoundingBox bbox = CreateMergedBox( ppShapes, numShapes );

    // ピボットを求める.
    Vector3 pivot = ( bbox.max + bbox.min ) / 2.0;

    // AABBの各辺の長さを求める.
    Vector3 size  = bbox.max - bbox.min;
    s32 axis = GetAxisIndex( size );

    // 中間値.
    s32 midPoint = Split( ppShapes, numShapes, pivot.a[axis], axis );

    // 更に分割するためにAABBを求める.
    BoundingBox bboxL = CreateMergedBox( ppShapes, midPoint );
    BoundingBox bboxR = CreateMergedBox( &ppShapes[midPoint], numShapes - midPoint );

    // AABBの各辺の長さを求める.
    Vector3 sizeL = bboxL.max - bboxL.min;
    Vector3 sizeR = bboxR.max - bboxR.min;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // 更に分割するピボットを求める.
    Vector3 pivotL = ( bboxL.max + bboxL.min ) / 2.0;
    Vector3 pivotR = ( bboxR.max + bboxR.min ) / 2.0;

    // 分割する.
    s32 midPointL = Split( ppShapes, midPoint, pivotL.a[axisL], axisL );
    s32 midPointR = Split( &ppShapes[midPoint], numShapes - midPoint, pivotR.a[axisR], axisR );

    IShape** ppChild[4] = {
        &ppShapes[0],
        &ppShapes[midPointL],
        &ppShapes[midPoint],
        &ppShapes[midPoint + midPointR]
    };
    u32 numChild[4] = {
        midPointL,
        midPoint - midPointL,
        midPointR,
        numShapes - (midPoint + midPointR)
    };

    // ブランチを生成.
    IShape* child0 = BuildBranch( ppChild[0], numChild[0] );
    IShape* child1 = BuildBranch( ppChild[1], numChild[1] );
    IShape* child2 = BuildBranch( ppChild[2], numChild[2] );
    IShape* child3 = BuildBranch( ppChild[3], numChild[3] );

    // チャイルドのAABBを求める.
    BoundingBox box0 = CreateMergedBox( ppChild[0], numChild[0] );
    BoundingBox box1 = CreateMergedBox( ppChild[1], numChild[1] );
    BoundingBox box2 = CreateMergedBox( ppChild[2], numChild[2] );
    BoundingBox box3 = CreateMergedBox( ppChild[3], numChild[3] );

    // ノードとして生成.
    return new(pBuf) QBVH( true, child0, child1, child2, child3, BoundingBox4( box0, box1, box2, box3 ) );
}

//--------------------------------------------------------------------------
//      ブランチを構築します(メッシュ用).
//--------------------------------------------------------------------------
IShape* QBVH::BuildBranch( Triangle* pShapes, const u32 numShapes )
{
    // そのまま返却.
    if ( numShapes <= 1 )
    { return (IShape*)&pShapes[0]; }

    // 16byteアライメントでメモリを確保.
    u8* pBuf = (u8*)_aligned_malloc( sizeof(QBVH), 16 );

    // 葉として生成.
    if ( numShapes == 2 )
    { return new(pBuf) QBVH( false, &pShapes[0], &pShapes[1], nullptr, nullptr ); }
    else if ( numShapes == 3 )
    { return new(pBuf) QBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], nullptr ); }
    else if ( numShapes == 4 )
    { return new(pBuf) QBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], &pShapes[3] ); }

    // AABBを求める.
    BoundingBox bbox = CreateMergedBox( pShapes, numShapes );

    // ピボットを求める.
    Vector3 pivot = ( bbox.max + bbox.min ) * 0.5f;

    // AABBの各辺の長さを求める.
    Vector3 size  = bbox.max - bbox.min;
    s32 axis = GetAxisIndex( size );

    // 中間値.
    s32 midPoint = Split( pShapes, numShapes, pivot.a[axis], axis );

    // 更に分割するためにAABBを求める.
    BoundingBox bboxL = CreateMergedBox( pShapes, midPoint );
    BoundingBox bboxR = CreateMergedBox( &pShapes[midPoint], numShapes - midPoint );

    // AABBの各辺の長さを求める.
    Vector3 sizeL = bboxL.max - bboxL.min;
    Vector3 sizeR = bboxR.max - bboxR.min;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // 更に分割するためのピボットを求める.
    Vector3 pivotL = ( bboxL.max + bboxL.min ) * 0.5f;
    Vector3 pivotR = ( bboxR.max + bboxR.min ) * 0.5f;

    // 分割する.
    s32 midPointL = Split( pShapes, midPoint, pivotL.a[axisL], axisL );
    s32 midPointR = Split( &pShapes[midPoint], numShapes - midPoint, pivotR.a[axisR], axisR );

    Triangle* ppChild[4] = {
        &pShapes[0],
        &pShapes[midPointL],
        &pShapes[midPoint],
        &pShapes[midPoint + midPointR]
    };
    u32 numChild[4] = {
        midPointL,
        midPoint - midPointL,
        midPointR,
        numShapes - (midPoint + midPointR)
    };

    // ブランチを生成.
    IShape* child0 = BuildBranch( ppChild[0], numChild[0] );
    IShape* child1 = BuildBranch( ppChild[1], numChild[1] );
    IShape* child2 = BuildBranch( ppChild[2], numChild[2] );
    IShape* child3 = BuildBranch( ppChild[3], numChild[3] );

    // チャイルドのAABBを求める.
    BoundingBox box0 = CreateMergedBox( ppChild[0], numChild[0] );
    BoundingBox box1 = CreateMergedBox( ppChild[1], numChild[1] );
    BoundingBox box2 = CreateMergedBox( ppChild[2], numChild[2] );
    BoundingBox box3 = CreateMergedBox( ppChild[3], numChild[3] );

    // ノードとして生成.
    return new(pBuf) QBVH( true, child0, child1, child2, child3, BoundingBox4( box0, box1, box2, box3 ) );
}

//--------------------------------------------------------------------------
//      ブランチを破棄します.
//--------------------------------------------------------------------------
void QBVH::DestroyBranch( QBVH* pShape )
{
    if ( pShape == nullptr )
    { return; }

    // 基本図形の場合は，作成元の方でメモリ解放を行うのでnullクリア.
    if ( pShape->IsPrimitive() )
    {
        pShape = nullptr;
        return;
    }

    // ブランチを破棄する.
    DestroyBranch( (QBVH*)pShape->pShape[0] );
    DestroyBranch( (QBVH*)pShape->pShape[1] );
    DestroyBranch( (QBVH*)pShape->pShape[2] );
    DestroyBranch( (QBVH*)pShape->pShape[3] );

    // placemenet newしたので明示的にデストラクタを呼び出し.
    pShape->~QBVH();

    // _aligned_malloc()でメモリを確保したので，_aligned_free()で解放.
    _aligned_free( pShape );

    // nullクリア
    pShape = nullptr;
}

//--------------------------------------------------------------------------
//      ノードの交差判定を行います.
//--------------------------------------------------------------------------
bool QBVH::HitFuncNode( const QBVH* pBVH, const Ray& ray, HitRecord& record )
{
    // まず子のバウンディングボックスと交差判定.
    s32 mask = 0;
    if ( !pBVH->box.IsHit( Ray4( ray ), mask ) )
    { return false; }

    // 次にバウンディングボックスとヒットした子のみたどっていく.
    bool isHit0 = false;
    bool isHit1 = false;
    bool isHit2 = false;
    bool isHit3 = false;

    if ( ( mask & 0x1 ) && pBVH->pShape[0] != nullptr )
    { isHit0 = pBVH->pShape[0]->IsHit( ray, record ); }

    if ( ( mask & 0x2 ) && pBVH->pShape[1] != nullptr )
    { isHit1 = pBVH->pShape[1]->IsHit( ray, record ); }

    if ( ( mask & 0x4 ) && pBVH->pShape[2] != nullptr )
    { isHit2 = pBVH->pShape[2]->IsHit( ray, record ); }

    if ( ( mask & 0x8 ) && pBVH->pShape[3] != nullptr )
    { isHit3 = pBVH->pShape[3]->IsHit( ray, record ); }

    return ( isHit0 || isHit1 || isHit2 || isHit3 );
}

//--------------------------------------------------------------------------
//      葉の交差判定を行います.
//--------------------------------------------------------------------------
bool QBVH::HitFuncLeaf( const QBVH* pBVH, const Ray& ray, HitRecord& record )
{
    bool isHit0 = false;
    bool isHit1 = false;
    bool isHit2 = false;
    bool isHit3 = false;

    if ( pBVH->pShape[0] != nullptr )
    { isHit0 = pBVH->pShape[0]->IsHit( ray, record ); }

    if ( pBVH->pShape[1] != nullptr )
    { isHit1 = pBVH->pShape[1]->IsHit( ray, record ); }

    if ( pBVH->pShape[2] != nullptr )
    { isHit2 = pBVH->pShape[2]->IsHit( ray, record ); }

    if ( pBVH->pShape[3] != nullptr )
    { isHit3 = pBVH->pShape[3]->IsHit( ray, record ); }

    return ( isHit0 || isHit1 || isHit2 || isHit3 );
}


} // namespace s3d
