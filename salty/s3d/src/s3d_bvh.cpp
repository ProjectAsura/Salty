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
        bbox = ppShapes[i]->GetAABB();

        // 中心を取得.
        centroid = ( bbox.min.v[axis] + bbox.max.v[axis] ) / 2.0;

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

} // namespace /* anonymous */


namespace s3d {

////////////////////////////////////////////////////////////////////////////
// BVH structure
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------
BVH::BVH()
: aabb  ()
, pLeft ( nullptr )
, pRight( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
BVH::BVH( IShape* pShape1, IShape* pShape2, const BoundingBox& bbox )
: aabb  ( bbox )
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
    aabb = BoundingBox::Merge( pShape1->GetAABB(), pShape2->GetAABB() );
}

//--------------------------------------------------------------------------
//      初期化処理を行います.
//--------------------------------------------------------------------------
void BVH::Init( IShape** ppShapes, const s32 numShapes )
{
    if ( numShapes == 1 )
    { (*this) = BVH( ppShapes[0], ppShapes[0] ); }
    if ( numShapes == 2 )
    { (*this) = BVH( ppShapes[0], ppShapes[1] ); }

    aabb = ppShapes[0]->GetAABB();
    for( s32 i=1; i<numShapes; ++i )
    { aabb = BoundingBox::Merge( aabb, ppShapes[i]->GetAABB() ); }

    Vector3 pivot = ( aabb.max + aabb.min ) / 2.0;

    s32 midPoint = Split( ppShapes, numShapes, pivot.x, 0 );

    pLeft  = BuildBranch( ppShapes, midPoint, 1 );
    pRight = BuildBranch( &ppShapes[ midPoint ], numShapes - midPoint, 1 );
}

//--------------------------------------------------------------------------
//      衝突判定を行います.
//--------------------------------------------------------------------------
bool BVH::IsHit( const Ray& ray, const f64 mini, const f64 maxi, ShadeRec& shadeRec ) const
{
    if ( !aabb.IsHit( ray, mini, maxi ) )
    { return false; }

    bool isHit1 = false;
    bool isHit2 = false;
    shadeRec.dist = maxi;

    isHit1 = pRight->IsHit( ray, mini, maxi, shadeRec );
    isHit2 = pLeft->IsHit( ray, mini, maxi, shadeRec );

    return ( isHit1 || isHit2 );
}

//--------------------------------------------------------------------------
//      軸平行境界箱を取得します.
//--------------------------------------------------------------------------
BoundingBox BVH::GetAABB() const
{ return aabb; }

//--------------------------------------------------------------------------
//      ブランチを構築します.
//--------------------------------------------------------------------------
IShape* BVH::BuildBranch( IShape** ppShapes, const s32 numShapes, s32 axis )
{
    if ( numShapes == 1 ) 
    { return ppShapes[0]; }
    if ( numShapes == 2 )
    { return new BVH( ppShapes[0], ppShapes[1] ); }

    BoundingBox bbox = ppShapes[0]->GetAABB();
    for( s32 i=1; i<numShapes; ++i )
    { bbox = BoundingBox::Merge( bbox, ppShapes[i]->GetAABB() ); }

    Vector3 pivot = ( bbox.max + bbox.min ) / 2.0;

    s32 midPoint = Split( ppShapes, numShapes, pivot.v[axis], axis );

    IShape* left  = BuildBranch( ppShapes, midPoint, (axis + 1) % 3 );
    IShape* right = BuildBranch( &ppShapes[midPoint], numShapes - midPoint, (axis + 1 ) % 3 );

    return new BVH( left, right, bbox );
}


} // namespace s3d