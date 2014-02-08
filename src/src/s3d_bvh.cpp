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

    s32 idx[2] = {
        0,
        midPoint
    };

    u32 num[2] = {
        midPoint,
        numShapes - midPoint
    };

    // ブランチ構築.
    IShape* left  = BuildBranch( &ppShapes[idx[0]], num[0] );
    IShape* right = BuildBranch( &ppShapes[idx[1]], num[1] );

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

    s32 idx[2] = {
        0,
        midPoint
    };

    s32 num[2] = {
        midPoint,
        numShapes - midPoint
    };

    // ブランチ構築.
    IShape* left  = BuildBranch( &pShapes[idx[0]], num[0] );
    IShape* right = BuildBranch( &pShapes[idx[1]], num[1] );

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

    s32 idx1[2] = {
        0,
        midPoint
    };

    s32 num1[2] = {
        midPoint,
        numShapes - midPoint
    };

    // 更に分割するためにAABBを求める.
    BoundingBox bboxL = CreateMergedBox( &ppShapes[idx1[0]], num1[0] );
    BoundingBox bboxR = CreateMergedBox( &ppShapes[idx1[1]], num1[1] );

    // AABBの各辺の長さを求める.
    Vector3 sizeL = bboxL.max - bboxL.min;
    Vector3 sizeR = bboxR.max - bboxR.min;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // 更に分割するピボットを求める.
    Vector3 pivotL = ( bboxL.max + bboxL.min ) / 2.0;
    Vector3 pivotR = ( bboxR.max + bboxR.min ) / 2.0;

    // 分割する.
    s32 midPointL = Split( &ppShapes[idx1[0]], num1[0], pivotL.a[axisL], axisL );
    s32 midPointR = Split( &ppShapes[idx1[1]], num1[1], pivotR.a[axisR], axisR );

    u32 idx2[4] = {
        idx1[0],
        midPointL,
        idx1[1],
        idx1[1] + midPointR
    };

    u32 num2[4] = {
        midPointL,
        num1[0] - midPointL,
        midPointR,
        num1[1] - midPointR
    };

    // ブランチを生成.
    IShape* child0 = BuildBranch( &ppShapes[idx2[0]], num2[0] );
    IShape* child1 = BuildBranch( &ppShapes[idx2[1]], num2[1] );
    IShape* child2 = BuildBranch( &ppShapes[idx2[2]], num2[2] );
    IShape* child3 = BuildBranch( &ppShapes[idx2[3]], num2[3] );

    // チャイルドのAABBを求める.
    BoundingBox box0 = CreateMergedBox( &ppShapes[idx2[0]], num2[0] );
    BoundingBox box1 = CreateMergedBox( &ppShapes[idx2[1]], num2[1] );
    BoundingBox box2 = CreateMergedBox( &ppShapes[idx2[2]], num2[2] );
    BoundingBox box3 = CreateMergedBox( &ppShapes[idx2[3]], num2[3] );

    // ノードとして生成.
    return new(pBuf) QBVH(
        true,
        child0,
        child1,
        child2,
        child3,
        BoundingBox4( box0, box1, box2, box3 ) );
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

    s32 idx1[2] = {
        0,
        midPoint
    };

    s32 num1[2] = {
        midPoint,
        numShapes - midPoint
    };

    // 更に分割するためにAABBを求める.
    BoundingBox bboxL = CreateMergedBox( &pShapes[idx1[0]], num1[0] );
    BoundingBox bboxR = CreateMergedBox( &pShapes[idx1[1]], num1[1] );

    // AABBの各辺の長さを求める.
    Vector3 sizeL = bboxL.max - bboxL.min;
    Vector3 sizeR = bboxR.max - bboxR.min;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // 更に分割するためのピボットを求める.
    Vector3 pivotL = ( bboxL.max + bboxL.min ) * 0.5f;
    Vector3 pivotR = ( bboxR.max + bboxR.min ) * 0.5f;

    // 分割する.
    s32 midPointL = Split( &pShapes[idx1[0]], num1[0], pivotL.a[axisL], axisL );
    s32 midPointR = Split( &pShapes[idx1[1]], num1[1], pivotR.a[axisR], axisR );

    s32 idx2[4] = {
        idx1[0],
        midPointL,
        idx1[1],
        idx1[1] + midPointR
    };

    s32 num2[4] = {
        midPointL,
        num1[0] - midPointL,
        midPointR,
        num1[1] - midPointR
    };
    // ブランチを生成.
    IShape* child0 = BuildBranch( &pShapes[idx2[0]], num2[0] );
    IShape* child1 = BuildBranch( &pShapes[idx2[1]], num2[1] );
    IShape* child2 = BuildBranch( &pShapes[idx2[2]], num2[2] );
    IShape* child3 = BuildBranch( &pShapes[idx2[3]], num2[3] );

    // チャイルドのAABBを求める.
    BoundingBox box0 = CreateMergedBox( &pShapes[idx2[0]], num2[0] );
    BoundingBox box1 = CreateMergedBox( &pShapes[idx2[1]], num2[1] );
    BoundingBox box2 = CreateMergedBox( &pShapes[idx2[2]], num2[2] );
    BoundingBox box3 = CreateMergedBox( &pShapes[idx2[3]], num2[3] );

    // ノードとして生成.
    return new(pBuf) QBVH(
        true, 
        child0,
        child1,
        child2,
        child3,
        BoundingBox4( box0, box1, box2, box3 ) );
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


////////////////////////////////////////////////////////////////////////////
// OBVH structure
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------
OBVH::OBVH()
{
    pShape[0] = nullptr;
    pShape[1] = nullptr;
    pShape[2] = nullptr;
    pShape[3] = nullptr;
    pShape[4] = nullptr;
    pShape[5] = nullptr;
    pShape[6] = nullptr;
    pShape[7] = nullptr;
    pHitFunc  = nullptr;
}

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
OBVH::OBVH
(
    bool node,
    IShape* pShape1,
    IShape* pShape2,
    IShape* pShape3,
    IShape* pShape4,
    IShape* pShape5,
    IShape* pShape6,
    IShape* pShape7,
    IShape* pShape8
) 
{
    pShape[0] = pShape1;
    pShape[1] = pShape2;
    pShape[2] = pShape3;
    pShape[3] = pShape4;
    pShape[4] = pShape5;
    pShape[5] = pShape6;
    pShape[6] = pShape7;
    pShape[7] = pShape8;

    BoundingBox box0 = pShape1->GetBox();
    BoundingBox box1 = pShape2->GetBox();
    BoundingBox box2 = ( pShape3 == nullptr ) ? BoundingBox() : pShape3->GetBox();
    BoundingBox box3 = ( pShape4 == nullptr ) ? BoundingBox() : pShape4->GetBox();
    BoundingBox box4 = ( pShape5 == nullptr ) ? BoundingBox() : pShape5->GetBox();
    BoundingBox box5 = ( pShape6 == nullptr ) ? BoundingBox() : pShape6->GetBox();
    BoundingBox box6 = ( pShape7 == nullptr ) ? BoundingBox() : pShape7->GetBox();
    BoundingBox box7 = ( pShape8 == nullptr ) ? BoundingBox() : pShape8->GetBox();


    box = BoundingBox8( box0, box1, box2, box3, box4, box5, box6, box7 );
    pHitFunc = ( node ) ? HitFuncNode : HitFuncLeaf;
}

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
OBVH::OBVH
(
    bool node,
    IShape* pShape1, 
    IShape* pShape2,
    IShape* pShape3,
    IShape* pShape4,
    IShape* pShape5, 
    IShape* pShape6,
    IShape* pShape7,
    IShape* pShape8,
    const BoundingBox8& octBox
): box( octBox )
{
    pShape[0] = pShape1;
    pShape[1] = pShape2;
    pShape[2] = pShape3;
    pShape[3] = pShape4;
    pShape[4] = pShape5;
    pShape[5] = pShape6;
    pShape[6] = pShape7;
    pShape[7] = pShape8;
    pHitFunc = ( node ) ? HitFuncNode : HitFuncLeaf;
}

//--------------------------------------------------------------------------
//      デストラクタです.
//--------------------------------------------------------------------------
OBVH::~OBVH()
{
    pShape[0] = nullptr;
    pShape[1] = nullptr;
    pShape[2] = nullptr;
    pShape[3] = nullptr;
    pShape[4] = nullptr;
    pShape[5] = nullptr;
    pShape[6] = nullptr;
    pShape[7] = nullptr;
}

//--------------------------------------------------------------------------
//      交差判定を行います.
//--------------------------------------------------------------------------
bool OBVH::IsHit( const Ray& ray, HitRecord& record ) const
{ return (*pHitFunc)( this, ray, record ); }

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
//      ブランチを構築します.
//--------------------------------------------------------------------------
IShape* OBVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{
    // そのまま返却.
    if ( numShapes <= 1 )
    { return ppShapes[0]; }

    // 32byteアライメントでメモリを確保.
    u8* pBuf = (u8*)_aligned_malloc( sizeof(OBVH), 32 );

    // 葉として生成.
    if ( numShapes == 2 )
    { return new(pBuf) OBVH( false, ppShapes[0], ppShapes[1], nullptr, nullptr, nullptr, nullptr, nullptr, nullptr ); }
    else if ( numShapes == 3 )
    { return new(pBuf) OBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], nullptr, nullptr, nullptr, nullptr, nullptr ); }
    else if ( numShapes == 4 )
    { return new(pBuf) OBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], ppShapes[3], nullptr, nullptr, nullptr, nullptr ); }
    else if ( numShapes == 5 )
    { return new(pBuf) OBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], ppShapes[3], ppShapes[4], nullptr, nullptr, nullptr ); }
    else if ( numShapes == 6 )
    { return new(pBuf) OBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], ppShapes[3], ppShapes[4], ppShapes[5], nullptr, nullptr ); }
    else if ( numShapes == 7 )
    { return new(pBuf) OBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], ppShapes[3], ppShapes[4], ppShapes[5], ppShapes[6], nullptr ); }
    else if ( numShapes == 8 )
    { return new(pBuf) OBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], ppShapes[3], ppShapes[4], ppShapes[5], ppShapes[6], ppShapes[7] ); }

    // -------------------------
    //      1段階目.
    // -------------------------
    // AABBを求める.
    BoundingBox bbox = CreateMergedBox( ppShapes, numShapes );

    // ピボットを求める.
    Vector3 pivot = ( bbox.max + bbox.min ) / 2.0f;

    // AABBの各辺の長さを求める.
    Vector3 size  = bbox.max - bbox.min;
    s32 axis = GetAxisIndex( size );

    // 中間値.
    s32 midPoint = Split( ppShapes, numShapes, pivot.a[axis], axis );

    s32 idx1[2] = {
        0,
        midPoint
    };

    s32 num1[2] = {
        midPoint,
        numShapes - midPoint
    };

    // -------------------------
    //      2段階目.
    // -------------------------
    // 更に分割するためにAABBを求める.
    BoundingBox bboxL = CreateMergedBox( &ppShapes[idx1[0]], num1[0] );
    BoundingBox bboxR = CreateMergedBox( &ppShapes[idx1[1]], num1[1] );

    // AABBの各辺の長さを求める.
    Vector3 sizeL = bboxL.max - bboxL.min;
    Vector3 sizeR = bboxR.max - bboxR.min;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // 更に分割するピボットを求める.
    Vector3 pivotL = ( bboxL.max + bboxL.min ) / 2.0f;
    Vector3 pivotR = ( bboxR.max + bboxR.min ) / 2.0f;

    // 分割する.
    s32 midPointL = Split( &ppShapes[idx1[0]], num1[0], pivotL.a[axisL], axisL );
    s32 midPointR = Split( &ppShapes[idx1[1]], num1[1], pivotR.a[axisR], axisR );

    u32 idx2[4] = {
        idx1[0],
        midPointL,
        idx1[1],
        idx1[1] + midPointR
    };

    u32 num2[4] = {
        midPointL,
        num1[0] - midPointL,
        midPointR,
        num1[1] - midPointR
    };


    // -------------------------
    //      3段階目.
    // -------------------------

    // 更に更に分割するためにAABBを求める.
    BoundingBox bbox1 = CreateMergedBox( &ppShapes[idx2[0]], num2[0] );
    BoundingBox bbox2 = CreateMergedBox( &ppShapes[idx2[1]], num2[1] );
    BoundingBox bbox3 = CreateMergedBox( &ppShapes[idx2[2]], num2[2] );
    BoundingBox bbox4 = CreateMergedBox( &ppShapes[idx2[3]], num2[3] );

    // AABBの各辺の長さを求める.
    Vector3 size1 = bbox1.max - bbox1.min;
    Vector3 size2 = bbox2.max - bbox2.min;
    Vector3 size3 = bbox3.max - bbox3.min;
    Vector3 size4 = bbox4.max - bbox4.min;
    s32 axis1 = GetAxisIndex( size1 );
    s32 axis2 = GetAxisIndex( size2 );
    s32 axis3 = GetAxisIndex( size3 );
    s32 axis4 = GetAxisIndex( size4 );

    // 更に更に分割するピボットを求める.
    Vector3 pivot1 = ( bbox1.max + bbox1.min ) / 2.0f;
    Vector3 pivot2 = ( bbox2.max + bbox2.min ) / 2.0f;
    Vector3 pivot3 = ( bbox3.max + bbox3.min ) / 2.0f;
    Vector3 pivot4 = ( bbox4.max + bbox4.min ) / 2.0f;

    // 分割する.
    s32 midPoint1 = Split( &ppShapes[idx2[0]], num2[0], pivot1.a[axis1], axis1 );
    s32 midPoint2 = Split( &ppShapes[idx2[1]], num2[1], pivot2.a[axis2], axis2 );
    s32 midPoint3 = Split( &ppShapes[idx2[2]], num2[2], pivot3.a[axis3], axis3 );
    s32 midPoint4 = Split( &ppShapes[idx2[3]], num2[3], pivot4.a[axis4], axis4 );

    s32 idx3[8] = {
        idx2[0],
        midPoint1,
        idx2[1],
        idx2[1] + midPoint2,
        idx2[2],
        midPoint3,
        idx2[3],
        idx2[3] + midPoint4
    };

    s32 num3[8] = {
        midPoint1,
        num2[0] - midPoint1,
        midPoint2,
        num2[1] - midPoint2,
        midPoint3,
        num2[2] - midPoint3,
        midPoint4,
        num2[3] - midPoint4,
    };

    // ブランチを生成.
    IShape* child0 = BuildBranch( &ppShapes[idx3[0]], num3[0] );
    IShape* child1 = BuildBranch( &ppShapes[idx3[1]], num3[1] );
    IShape* child2 = BuildBranch( &ppShapes[idx3[2]], num3[2] );
    IShape* child3 = BuildBranch( &ppShapes[idx3[3]], num3[3] );
    IShape* child4 = BuildBranch( &ppShapes[idx3[4]], num3[4] );
    IShape* child5 = BuildBranch( &ppShapes[idx3[5]], num3[5] );
    IShape* child6 = BuildBranch( &ppShapes[idx3[6]], num3[6] );
    IShape* child7 = BuildBranch( &ppShapes[idx3[7]], num3[7] );

    // チャイルドのAABBを求める.
    BoundingBox box0 = CreateMergedBox( &ppShapes[idx3[0]], num3[0] );
    BoundingBox box1 = CreateMergedBox( &ppShapes[idx3[1]], num3[1] );
    BoundingBox box2 = CreateMergedBox( &ppShapes[idx3[2]], num3[2] );
    BoundingBox box3 = CreateMergedBox( &ppShapes[idx3[3]], num3[3] );
    BoundingBox box4 = CreateMergedBox( &ppShapes[idx3[4]], num3[4] );
    BoundingBox box5 = CreateMergedBox( &ppShapes[idx3[5]], num3[5] );
    BoundingBox box6 = CreateMergedBox( &ppShapes[idx3[6]], num3[6] );
    BoundingBox box7 = CreateMergedBox( &ppShapes[idx3[7]], num3[7] );

    // ノードとして生成.
    return new(pBuf) OBVH(
        true,
        child0,
        child1,
        child2,
        child3,
        child4,
        child5,
        child6,
        child7,
        BoundingBox8( box0, box1, box2, box3, box4, box5, box6, box7 ) );
}

//--------------------------------------------------------------------------
//      ブランチを構築します(メッシュ用).
//--------------------------------------------------------------------------
IShape* OBVH::BuildBranch( Triangle* pShapes, const u32 numShapes )
{
    // そのまま返却.
    if ( numShapes <= 1 )
    { return (IShape*)&pShapes[0]; }

    // 32byteアライメントでメモリを確保.
    u8* pBuf = (u8*)_aligned_malloc( sizeof(OBVH), 32 );

    // 葉として生成.
    if ( numShapes == 2 )
    { return new(pBuf) OBVH( false, &pShapes[0], &pShapes[1], nullptr, nullptr, nullptr, nullptr, nullptr, nullptr ); }
    else if ( numShapes == 3 )
    { return new(pBuf) OBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], nullptr, nullptr, nullptr, nullptr, nullptr ); }
    else if ( numShapes == 4 )
    { return new(pBuf) OBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], &pShapes[3], nullptr, nullptr, nullptr, nullptr ); }
    else if ( numShapes == 5 )
    { return new(pBuf) OBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], &pShapes[3], &pShapes[4], nullptr, nullptr, nullptr ); }
    else if ( numShapes == 6 )
    { return new(pBuf) OBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], &pShapes[3], &pShapes[4], &pShapes[5], nullptr, nullptr ); }
    else if ( numShapes == 7 )
    { return new(pBuf) OBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], &pShapes[3], &pShapes[4], &pShapes[5], &pShapes[6], nullptr ); }
    else if ( numShapes == 8 )
    { return new(pBuf) OBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], &pShapes[3], &pShapes[4], &pShapes[5], &pShapes[6], &pShapes[7] ); }

    // -------------------------
    //      1段階目.
    // -------------------------
    // AABBを求める.
    BoundingBox bbox = CreateMergedBox( pShapes, numShapes );

    // ピボットを求める.
    Vector3 pivot = ( bbox.max + bbox.min ) / 2.0f;

    // AABBの各辺の長さを求める.
    Vector3 size  = bbox.max - bbox.min;
    s32 axis = GetAxisIndex( size );

    // 中間値.
    s32 midPoint = Split( pShapes, numShapes, pivot.a[axis], axis );

    s32 idx1[2] = {
        0,
        midPoint
    };

    s32 num1[2] = {
        midPoint,
        numShapes - midPoint
    };

    // -------------------------
    //      2段階目.
    // -------------------------
    // 更に分割するためにAABBを求める.
    BoundingBox bboxL = CreateMergedBox( &pShapes[idx1[0]], num1[0] );
    BoundingBox bboxR = CreateMergedBox( &pShapes[idx1[1]], num1[1] );

    // AABBの各辺の長さを求める.
    Vector3 sizeL = bboxL.max - bboxL.min;
    Vector3 sizeR = bboxR.max - bboxR.min;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // 更に分割するピボットを求める.
    Vector3 pivotL = ( bboxL.max + bboxL.min ) / 2.0f;
    Vector3 pivotR = ( bboxR.max + bboxR.min ) / 2.0f;

    // 分割する.
    s32 midPointL = Split( &pShapes[idx1[0]], num1[0], pivotL.a[axisL], axisL );
    s32 midPointR = Split( &pShapes[idx1[1]], num1[1], pivotR.a[axisR], axisR );

    s32 idx2[4] = {
        idx1[0],
        midPointL,
        idx1[1],
        idx1[1] + midPointR
    };

    s32 num2[4] = {
        midPointL,
        num1[0] - midPointL,
        midPointR,
        num1[1] - midPointR
    };


    // -------------------------
    //      3段階目.
    // -------------------------

    // 更に更に分割するためにAABBを求める.
    BoundingBox bbox1 = CreateMergedBox( &pShapes[idx2[0]], num2[0] );
    BoundingBox bbox2 = CreateMergedBox( &pShapes[idx2[1]], num2[1] );
    BoundingBox bbox3 = CreateMergedBox( &pShapes[idx2[2]], num2[2] );
    BoundingBox bbox4 = CreateMergedBox( &pShapes[idx2[3]], num2[3] );

    // AABBの各辺の長さを求める.
    Vector3 size1 = bbox1.max - bbox1.min;
    Vector3 size2 = bbox2.max - bbox2.min;
    Vector3 size3 = bbox3.max - bbox3.min;
    Vector3 size4 = bbox4.max - bbox4.min;
    s32 axis1 = GetAxisIndex( size1 );
    s32 axis2 = GetAxisIndex( size2 );
    s32 axis3 = GetAxisIndex( size3 );
    s32 axis4 = GetAxisIndex( size4 );

    // 更に更に分割するピボットを求める.
    Vector3 pivot1 = ( bbox1.max + bbox1.min ) / 2.0f;
    Vector3 pivot2 = ( bbox2.max + bbox2.min ) / 2.0f;
    Vector3 pivot3 = ( bbox3.max + bbox3.min ) / 2.0f;
    Vector3 pivot4 = ( bbox4.max + bbox4.min ) / 2.0f;

    // 分割する.
    s32 midPoint1 = Split( &pShapes[idx2[0]], num2[0], pivot1.a[axis1], axis1 );
    s32 midPoint2 = Split( &pShapes[idx2[1]], num2[1], pivot2.a[axis2], axis2 );
    s32 midPoint3 = Split( &pShapes[idx2[2]], num2[2], pivot3.a[axis3], axis3 );
    s32 midPoint4 = Split( &pShapes[idx2[3]], num2[3], pivot4.a[axis4], axis4 );

    s32 idx3[8] = {
        idx2[0],
        midPoint1,
        idx2[1],
        idx2[1] + midPoint2,
        idx2[2],
        midPoint3,
        idx2[3],
        idx2[3] + midPoint4
    };

    s32 num3[8] = {
        midPoint1,
        num2[0] - midPoint1,
        midPoint2,
        num2[1] - midPoint2,
        midPoint3,
        num2[2] - midPoint3,
        midPoint4,
        num2[3] - midPoint4,
    };

    // ブランチを生成.
    IShape* child0 = BuildBranch( &pShapes[idx3[0]], num3[0] );
    IShape* child1 = BuildBranch( &pShapes[idx3[1]], num3[1] );
    IShape* child2 = BuildBranch( &pShapes[idx3[2]], num3[2] );
    IShape* child3 = BuildBranch( &pShapes[idx3[3]], num3[3] );
    IShape* child4 = BuildBranch( &pShapes[idx3[4]], num3[4] );
    IShape* child5 = BuildBranch( &pShapes[idx3[5]], num3[5] );
    IShape* child6 = BuildBranch( &pShapes[idx3[6]], num3[6] );
    IShape* child7 = BuildBranch( &pShapes[idx3[7]], num3[7] );

    // チャイルドのAABBを求める.
    BoundingBox box0 = CreateMergedBox( &pShapes[idx3[0]], num3[0] );
    BoundingBox box1 = CreateMergedBox( &pShapes[idx3[1]], num3[1] );
    BoundingBox box2 = CreateMergedBox( &pShapes[idx3[2]], num3[2] );
    BoundingBox box3 = CreateMergedBox( &pShapes[idx3[3]], num3[3] );
    BoundingBox box4 = CreateMergedBox( &pShapes[idx3[4]], num3[4] );
    BoundingBox box5 = CreateMergedBox( &pShapes[idx3[5]], num3[5] );
    BoundingBox box6 = CreateMergedBox( &pShapes[idx3[6]], num3[6] );
    BoundingBox box7 = CreateMergedBox( &pShapes[idx3[7]], num3[7] );

    // ノードとして生成.
    return new(pBuf) OBVH(
        true,
        child0,
        child1,
        child2,
        child3,
        child4,
        child5,
        child6,
        child7,
        BoundingBox8( box0, box1, box2, box3, box4, box5, box6, box7 ) );
}

//--------------------------------------------------------------------------
//      ブランチを破棄します.
//--------------------------------------------------------------------------
void OBVH::DestroyBranch( OBVH* pShape )
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
    DestroyBranch( (OBVH*)pShape->pShape[0] );
    DestroyBranch( (OBVH*)pShape->pShape[1] );
    DestroyBranch( (OBVH*)pShape->pShape[2] );
    DestroyBranch( (OBVH*)pShape->pShape[3] );
    DestroyBranch( (OBVH*)pShape->pShape[4] );
    DestroyBranch( (OBVH*)pShape->pShape[5] );
    DestroyBranch( (OBVH*)pShape->pShape[6] );
    DestroyBranch( (OBVH*)pShape->pShape[7] );

    // placemenet newしたので明示的にデストラクタを呼び出し.
    pShape->~OBVH();

    // _aligned_malloc()でメモリを確保したので，_aligned_free()で解放.
    _aligned_free( pShape );

    // nullクリア
    pShape = nullptr;
}

//--------------------------------------------------------------------------
//      ノードの交差判定を行います.
//--------------------------------------------------------------------------
bool OBVH::HitFuncNode( const OBVH* pBVH, const Ray& ray, HitRecord& record )
{
    // まず子のバウンディングボックスと交差判定.
    s32 mask = 0;
    if ( !pBVH->box.IsHit( Ray8( ray ), mask ) )
    { return false; }

    // 次にバウンディングボックスとヒットした子のみたどっていく.
    bool isHit0 = false;
    bool isHit1 = false;
    bool isHit2 = false;
    bool isHit3 = false;
    bool isHit4 = false;
    bool isHit5 = false;
    bool isHit6 = false;
    bool isHit7 = false;

    if ( ( mask & 0x1 ) && pBVH->pShape[0] != nullptr )
    { isHit0 = pBVH->pShape[0]->IsHit( ray, record ); }

    if ( ( mask & 0x2 ) && pBVH->pShape[1] != nullptr )
    { isHit1 = pBVH->pShape[1]->IsHit( ray, record ); }

    if ( ( mask & 0x4 ) && pBVH->pShape[2] != nullptr )
    { isHit2 = pBVH->pShape[2]->IsHit( ray, record ); }

    if ( ( mask & 0x8 ) && pBVH->pShape[3] != nullptr )
    { isHit3 = pBVH->pShape[3]->IsHit( ray, record ); }

    if ( ( mask & 0x10 ) && pBVH->pShape[4] != nullptr )
    { isHit4 = pBVH->pShape[4]->IsHit( ray, record ); }

    if ( ( mask & 0x20 ) && pBVH->pShape[5] != nullptr )
    { isHit5 = pBVH->pShape[5]->IsHit( ray, record ); }

    if ( ( mask & 0x40 ) && pBVH->pShape[6] != nullptr )
    { isHit6 = pBVH->pShape[6]->IsHit( ray, record ); }

    if ( ( mask & 0x80 ) && pBVH->pShape[7] != nullptr )
    { isHit7 = pBVH->pShape[7]->IsHit( ray, record ); }


    return ( isHit0 || isHit1 || isHit2 || isHit3 || isHit4 || isHit5 || isHit6 || isHit7 );
}

//--------------------------------------------------------------------------
//      葉の交差判定を行います.
//--------------------------------------------------------------------------
bool OBVH::HitFuncLeaf( const OBVH* pBVH, const Ray& ray, HitRecord& record )
{
    bool isHit0 = false;
    bool isHit1 = false;
    bool isHit2 = false;
    bool isHit3 = false;
    bool isHit4 = false;
    bool isHit5 = false;
    bool isHit6 = false;
    bool isHit7 = false;

    if ( pBVH->pShape[0] != nullptr )
    { isHit0 = pBVH->pShape[0]->IsHit( ray, record ); }

    if ( pBVH->pShape[1] != nullptr )
    { isHit1 = pBVH->pShape[1]->IsHit( ray, record ); }

    if ( pBVH->pShape[2] != nullptr )
    { isHit2 = pBVH->pShape[2]->IsHit( ray, record ); }

    if ( pBVH->pShape[3] != nullptr )
    { isHit3 = pBVH->pShape[3]->IsHit( ray, record ); }

    if ( pBVH->pShape[4] != nullptr )
    { isHit4 = pBVH->pShape[4]->IsHit( ray, record ); }

    if ( pBVH->pShape[5] != nullptr )
    { isHit5 = pBVH->pShape[5]->IsHit( ray, record ); }

    if ( pBVH->pShape[6] != nullptr )
    { isHit6 = pBVH->pShape[6]->IsHit( ray, record ); }

    if ( pBVH->pShape[7] != nullptr )
    { isHit7 = pBVH->pShape[7]->IsHit( ray, record ); }


    return ( isHit0 || isHit1 || isHit2 || isHit3 || isHit4 || isHit5 || isHit6 || isHit7 );
}


} // namespace s3d
