//---------------------------------------------------------------------------
// File : s3d_bvh.cpp
// Desc : Bounding Volume Hierarchy Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <s3d_bvh.h>


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
    s3d::BoundingBox box = pTriangles[0].GetBox();

    for( u32 i=1; i<numTriangles; ++i )
    { box = s3d::BoundingBox::Merge( box, pTriangles[i].GetBox() ); }

    return box;
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
    if ( numShapes == 1 ) 
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
    {
        return;
    }

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

} // namespace s3d
