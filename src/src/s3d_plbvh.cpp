//---------------------------------------------------------------------------
// File : s3d_plbvh.cpp
// Desc : Partitioning LBVH Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <s3d_plbvh.h>
#include <algorithm>


namespace /* anonymous */ {

#define DIV_BIT 10
#define DIV_NUM 1024

static inline u32 partby2(u32 n)
{
    n = (n ^ (n<<16)) & 0xff0000ff;
    n = (n ^ (n<< 8)) & 0x0300f00f;
    n = (n ^ (n<< 4)) & 0x030c30c3;
    n = (n ^ (n<< 2)) & 0x09249249;
    return n;
}

static inline u32 get_morton_code(u32 x, u32 y, u32 z)
{
    return ( partby2(x) << 2) |
            (partby2(y) << 1) |
            (partby2(z) );
}

static inline u32 get_morton_code(const s3d::Vector3& p, const s3d::BoundingBox& box )
{
    auto ix = (u32)( DIV_NUM * (( p.x - box.mini.x ) / ( box.maxi.x - box.mini.x )));
    auto iy = (u32)( DIV_NUM * (( p.y - box.mini.y ) / ( box.maxi.y - box.mini.y )));
    auto iz = (u32)( DIV_NUM * (( p.z - box.mini.z ) / ( box.maxi.z - box.mini.z )));
    return get_morton_code( ix, iy, iz );
}


struct separator
{
    separator(s32 _level, const u32* _codes)
    : pCodes(_codes)
    {
        s32 p = 3 * DIV_BIT - 1 - _level;
        mask = 1 << p;
    }

    inline bool operator()(size_t i)const
    {
        return (mask & pCodes[i]) == 0;
    }

    u32        mask;
    const u32* pCodes;
};

} // namespace /* anonymous */


namespace s3d {


////////////////////////////////////////////////////////////////////////////
// BVH structure
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------
PLBVH::PLBVH()
: box  ()
, pLeft ( nullptr )
, pRight( nullptr )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
PLBVH::PLBVH( IShape* pShape1, IShape* pShape2, const BoundingBox& bbox )
: box  ( bbox )
, pLeft ( pShape1 )
, pRight( pShape2 )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------
//      引数付きコンストラクタです.
//--------------------------------------------------------------------------
PLBVH::PLBVH( IShape* pShape1, IShape* pShape2 )
: pLeft ( pShape1 )
, pRight( pShape2 )
{
    box = BoundingBox::Merge( pShape1->GetBox(), pShape2->GetBox() );
}

//--------------------------------------------------------------------------
//      解放処理を行います.
//--------------------------------------------------------------------------
void PLBVH::Dispose()
{
    if ( pLeft->IsPrimitive() )
    { pLeft = nullptr; }
    else
    {
        PLBVH* pBVH = dynamic_cast<PLBVH*>( pLeft );
        if ( pBVH != nullptr )
        { pBVH->Dispose(); }
    }

    if ( pRight->IsPrimitive() )
    { pRight = nullptr; }
    else
    {
        PLBVH* pBVH = dynamic_cast<PLBVH*>( pRight );
        if ( pBVH != nullptr )
        { pBVH->Dispose(); }
    }

    delete this;
}

//--------------------------------------------------------------------------
//      衝突判定を行います.
//--------------------------------------------------------------------------
bool PLBVH::IsHit( const Ray& ray, HitRecord& record ) const
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
IMaterial* PLBVH::GetMaterial() const
{ return nullptr; }

//--------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//--------------------------------------------------------------------------
BoundingBox PLBVH::GetBox() const
{ return box; }

//--------------------------------------------------------------------------
//      基本図形であるかどうか判定します.
//--------------------------------------------------------------------------
bool PLBVH::IsPrimitive() const
{ return false; }

//--------------------------------------------------------------------------
//      中心座標を取得します.
//--------------------------------------------------------------------------
Vector3 PLBVH::GetCenter() const
{ return ( pLeft->GetCenter() + pRight->GetCenter() ) / 2.0f; }

//--------------------------------------------------------------------------
//      ブランチを構築します.
//--------------------------------------------------------------------------
IShape* PLBVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{

    auto box = CreateMergedBox( ppShapes, numShapes );
    auto morton_codes = new u32 [ numShapes ];
    auto indices = new u32 [ numShapes ];
    for( u32 i=0; i<numShapes; ++i )
    {
        morton_codes[i] = get_morton_code( ppShapes[i]->GetCenter(), box );
        indices[i] = i;
    }

    auto result = InternalBuildBranch( morton_codes, indices, ppShapes, 0, numShapes, 0 );

    delete [] indices;
    indices = nullptr;

    delete [] morton_codes;
    morton_codes = nullptr;

    return result;
}


IShape* PLBVH::InternalBuildBranch( const u32* pMortonCodes, u32* pIndices, IShape** ppShapes, u32 a, u32 b, s32 level )
{
    if ( ( b - a ) == 0 )
    { return new NullShape(); }

    if ( ( b - a ) == 1 )
    { return ppShapes[a]; }

    if ( level >= DIV_BIT * 3 )
    { return new Leaf( ( b - a ), &ppShapes[a] ); }

    u32* pA = pIndices + a;
    u32* pB = pIndices + b;
    u32* pC = std::partition( pA, pB, separator( level, pMortonCodes ) );

    u32 c = a + ( pC - pA );

    u32 range[2][2] = {
        { a, c },
        { c, b } 
    };

    auto left  = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, range[0][0], range[0][1], level + 1 );
    auto right = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, range[1][0], range[1][1], level + 1 );

    return new PLBVH( left, right );
}


} // namespace s3d