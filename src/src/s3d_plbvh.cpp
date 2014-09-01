//----------------------------------------------------------------------------------------------------
// File : s3d_plbvh.cpp
// Desc : Partitioning LBVH Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------------------
#include <s3d_plbvh.h>
#include <algorithm>


namespace /* anonymous */ {

//----------------------------------------------------------------------------------------------------
// Constant Values
//----------------------------------------------------------------------------------------------------
static const u32 DIV_BIT = 10;
static const u32 DIV_NUM = 1024;


//----------------------------------------------------------------------------------------------------
//      モートンコードを取得します.s
//----------------------------------------------------------------------------------------------------
static inline u32 GetMortonCode(u32 x, u32 y, u32 z)
{
    return ( ( s3d::Part1By2( x ) << 2 ) |
             ( s3d::Part1By2( y ) << 1 ) |
             ( s3d::Part1By2( z ) << 0 ) );
}

//----------------------------------------------------------------------------------------------------
//      バウンディングボックスと位置座標からモートンコードを生成します.
//----------------------------------------------------------------------------------------------------
static inline u32 GetMortonCode(const s3d::Vector3& p, const s3d::BoundingBox& box )
{
    auto ix = (u32)( DIV_NUM * (( p.x - box.mini.x ) / ( box.maxi.x - box.mini.x )));
    auto iy = (u32)( DIV_NUM * (( p.y - box.mini.y ) / ( box.maxi.y - box.mini.y )));
    auto iz = (u32)( DIV_NUM * (( p.z - box.mini.z ) / ( box.maxi.z - box.mini.z )));
    return GetMortonCode( ix, iy, iz );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Separator structure
//////////////////////////////////////////////////////////////////////////////////////////////////////
struct Separator
{
    //------------------------------------------------------------------------------------------------
    //      コンストラクタです.
    //------------------------------------------------------------------------------------------------
    Separator(s32 _level, const u32* _codes)
    : pCodes(_codes)
    {
        s32 p = 3 * DIV_BIT - 1 - _level;
        mask = 0x1 << p;
    }

    //------------------------------------------------------------------------------------------------
    //      operator()
    //------------------------------------------------------------------------------------------------
    inline bool operator()(size_t i)const
    {
        return (mask & pCodes[i]) == 0;
    }

    u32        mask;        //!< ビットマスクです.
    const u32* pCodes;      //!< モートンコードです.
};


} // namespace /* anonymous */


namespace s3d {


//////////////////////////////////////////////////////////////////////////////////////////////////////
// BVH structure
//////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------------------------
PLBVH::PLBVH()
: box   ()
, pLeft ( nullptr )
, pRight( nullptr )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------------------------
PLBVH::PLBVH( IShape* pShape1, IShape* pShape2, const BoundingBox& bbox )
: box   ( bbox )
, pLeft ( pShape1 )
, pRight( pShape2 )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------------------------
PLBVH::PLBVH( IShape* pShape1, IShape* pShape2 )
: pLeft ( pShape1 )
, pRight( pShape2 )
{
    box = BoundingBox::Merge( pShape1->GetBox(), pShape2->GetBox() );
}

//----------------------------------------------------------------------------------------------------
//      解放処理を行います.
//----------------------------------------------------------------------------------------------------
void PLBVH::Dispose()
{
    if ( pLeft->IsPrimitive() )
    { pLeft = nullptr; }
    else
    {
        auto pBVH = dynamic_cast<IDispose*>( pLeft );
        if ( pBVH != nullptr )
        { pBVH->Dispose(); }
    }

    if ( pRight->IsPrimitive() )
    { pRight = nullptr; }
    else
    {
        auto pBVH = dynamic_cast<IDispose*>( pRight );
        if ( pBVH != nullptr )
        { pBVH->Dispose(); }
    }

    delete this;
}

//----------------------------------------------------------------------------------------------------
//      衝突判定を行います.
//----------------------------------------------------------------------------------------------------
bool PLBVH::IsHit( const Ray& ray, HitRecord& record ) const
{
    if ( !box.IsHit( ray ) )
    { return record.distance < F_MAX; }

    HitRecord rec;
    if ( pRight->IsHit( ray, rec ) )
    {
        if (rec.distance < record.distance)
        { record = rec; }
    }

    if ( pLeft->IsHit( ray, rec ) )
    {
        if (rec.distance < record.distance)
        { record = rec; }
    }

    return record.distance < F_MAX;
}

//----------------------------------------------------------------------------------------------------
//      マテリアルを取得します.
//----------------------------------------------------------------------------------------------------
IMaterial* PLBVH::GetMaterial() const
{ return nullptr; }

//----------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//----------------------------------------------------------------------------------------------------
BoundingBox PLBVH::GetBox() const
{ return box; }

//----------------------------------------------------------------------------------------------------
//      基本図形であるかどうか判定します.
//----------------------------------------------------------------------------------------------------
bool PLBVH::IsPrimitive() const
{ return false; }

//----------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//----------------------------------------------------------------------------------------------------
Vector3 PLBVH::GetCenter() const
{ return ( pLeft->GetCenter() + pRight->GetCenter() ) / 2.0f; }

//----------------------------------------------------------------------------------------------------
//      ブランチを構築します.
//----------------------------------------------------------------------------------------------------
IShape* PLBVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{

    auto box = CreateMergedBox( ppShapes, numShapes );
    auto morton_codes = new u32 [ numShapes ];
    auto indices = new u32 [ numShapes ];
    for( u32 i=0; i<numShapes; ++i )
    {
        morton_codes[i] = GetMortonCode( ppShapes[i]->GetCenter(), box );
        indices[i] = i;
    }

    auto result = InternalBuildBranch( morton_codes, indices, ppShapes, 0, numShapes, 0 );

    S3D_DELETE_ARRAY( indices );
    S3D_DELETE_ARRAY( morton_codes );

    return result;
}

//----------------------------------------------------------------------------------------------------
//          ブランチを構築する内部処理です.
//----------------------------------------------------------------------------------------------------
IShape* PLBVH::InternalBuildBranch
(
    const u32*  pMortonCodes,
    u32*        pIndices,
    IShape**    ppShapes,
    u32         a,
    u32         b,
    s32         level
)
{
    if ( ( b - a ) == 0 )
    { return new NullShape(); }

    if ( (b -  a) <= 2 || level >= DIV_BIT * 3 )
    { return new Leaf( ( b - a ), &ppShapes[a] ); }

    u32* pA = pIndices + a;
    u32* pB = pIndices + b;
    u32* pC = std::partition( pA, pB, Separator( level, pMortonCodes ) );

    u32 c = a + (u32)( pC - pA );

    u32 range[2][2] = {
        { a, c },
        { c, b } 
    };

    auto left  = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, range[0][0], range[0][1], level + 1 );
    auto right = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, range[1][0], range[1][1], level + 1 );

    return new PLBVH( left, right );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// PLQBVH structure
//////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------------------------
PLQBVH::PLQBVH()
: box()
{
    for( u32 i=0; i<4; ++i )
    { pShape[ i ] = nullptr; }
}

//------------------------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//------------------------------------------------------------------------------------------------------
PLQBVH::PLQBVH( IShape** ppShapes )
{
    for( auto i=0; i<4; ++i )
    { pShape[i] = ppShapes[i]; }

    box = BoundingBox4( 
        pShape[0]->GetBox(),
        pShape[1]->GetBox(),
        pShape[2]->GetBox(),
        pShape[3]->GetBox() );
}

//----------------------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------------------------
PLQBVH::PLQBVH( IShape** _ppShapes, const BoundingBox4& _box )
: box( _box )
{
    for( u32 i=0; i<4; ++i )
    {  pShape[ i ] = _ppShapes[ i ]; }
}

//----------------------------------------------------------------------------------------------------
//      破棄処理を行います.
//----------------------------------------------------------------------------------------------------
void PLQBVH::Dispose()
{
    for( u32 i=0; i<4; ++i )
    {
        if ( pShape[i] )
        {
            if ( pShape[i]->IsPrimitive() )
            { pShape[i] = nullptr; }
            else
            {
                auto* disposable = dynamic_cast<IDispose*>( pShape[i] );
                if ( disposable )
                { disposable->Dispose(); }
            }
        }
    }

    _aligned_free( this );
}

//----------------------------------------------------------------------------------------------------
//      衝突判定処理を行います.
//----------------------------------------------------------------------------------------------------
bool PLQBVH::IsHit( const Ray& ray, HitRecord& record ) const
{
    s32 mask;
    if ( !box.IsHit( Ray4(ray), mask ) )
    { return record.distance < F_MAX; }

    HitRecord rec;

    for( u32 i=0; i<4; ++i )
    {
        if ( mask & ( 0x1 << i ) )
        {
            if ( pShape[i]->IsHit( ray, rec ) )
            {
                if ( rec.distance < record.distance )
                { record = rec; }
            }
        }
    }

    return record.distance < F_MAX;
}

//----------------------------------------------------------------------------------------------------
//      マテリアルを取得します.
//----------------------------------------------------------------------------------------------------
IMaterial* PLQBVH::GetMaterial() const
{ return nullptr; }

//----------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//----------------------------------------------------------------------------------------------------
BoundingBox PLQBVH::GetBox() const
{ return box.GetBox(); }

//----------------------------------------------------------------------------------------------------
//      基本図形を取得します.
//----------------------------------------------------------------------------------------------------
bool PLQBVH::IsPrimitive() const
{ return false; }

//----------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//----------------------------------------------------------------------------------------------------
Vector3 PLQBVH::GetCenter() const
{
    auto result = pShape[0]->GetCenter();
    for( u32 i=1; i<4; ++i )
    { result += pShape[i]->GetCenter(); }
    result /= 4.0f;
    return result;
}

//---------------------------------------------------------------------------------------------------
//      ブランチを構築します.
//---------------------------------------------------------------------------------------------------
IShape* PLQBVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{
    auto box = CreateMergedBox( ppShapes, numShapes );
    auto morton_codes = new u32 [ numShapes ];
    auto indices      = new u32 [ numShapes ];

    for( u32 i=0; i<numShapes; ++i )
    {
        morton_codes[ i ] = GetMortonCode( ppShapes[ i ]->GetCenter(), box );
        indices[ i ] = i;
    }

    auto branch = InternalBuildBranch( morton_codes, indices, ppShapes, 0, numShapes, 0 );

    S3D_DELETE_ARRAY( indices );
    S3D_DELETE_ARRAY( morton_codes );

    return branch;
}

//----------------------------------------------------------------------------------------------------
//          ブランチを構築する内部処理です.
//----------------------------------------------------------------------------------------------------
IShape* PLQBVH::InternalBuildBranch
(
    const u32*  pMortonCodes,
    u32*        pIndices,
    IShape**    ppShapes,
    u32         a,
    u32         b,
    s32         level
)
{
    if ( ( b - a ) == 0 )
    { return new NullShape(); }

    if ( ( b - a ) <= 4 || level >= DIV_BIT * 3 )
    { return new Leaf( ( b - a ), &ppShapes[a] ); }

    u32* pA = pIndices + a;
    u32* pB = pIndices + b;
    u32* pC = std::partition( pA, pB, Separator( level, pMortonCodes ) );

    u32* pD = std::partition( pA, pC, Separator( level + 1, pMortonCodes ) );
    u32* pE = std::partition( pC, pB, Separator( level + 1, pMortonCodes ) );

    u32 c = a + ( pC - pA );
    u32 d = a + ( pD - pA );
    u32 e = c + ( pE - pC );

    IShape* pNode[4];
    pNode[0] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, a, d, level + 2 );
    pNode[1] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, d, c, level + 2 );
    pNode[2] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, c, e, level + 2 );
    pNode[3] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, e, b, level + 2 );

    auto buf = _aligned_malloc( sizeof(PLQBVH), 16 );
    assert( buf != nullptr );

    return new( buf ) PLQBVH( pNode );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// PLOBVH structure
//////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------------------------
PLOBVH::PLOBVH()
: box()
{
    for( u32 i=0; i<8; ++i )
    { pShape[ i ] = nullptr; }
}

//------------------------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//------------------------------------------------------------------------------------------------------
PLOBVH::PLOBVH( IShape** ppShapes )
{
    for( auto i=0; i<8; ++i )
    { pShape[i] = ppShapes[i]; }

    box = BoundingBox8( 
        pShape[0]->GetBox(),
        pShape[1]->GetBox(),
        pShape[2]->GetBox(),
        pShape[3]->GetBox(),
        pShape[4]->GetBox(),
        pShape[5]->GetBox(),
        pShape[6]->GetBox(),
        pShape[7]->GetBox());
}

//----------------------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------------------------
PLOBVH::PLOBVH( IShape** _ppShapes, const BoundingBox8& _box )
: box( _box )
{
    for( u32 i=0; i<8; ++i )
    {  pShape[ i ] = _ppShapes[ i ]; }
}

//----------------------------------------------------------------------------------------------------
//      破棄処理を行います.
//----------------------------------------------------------------------------------------------------
void PLOBVH::Dispose()
{
    for( u32 i=0; i<8; ++i )
    {
        if ( pShape[i] )
        {
            if ( pShape[i]->IsPrimitive() )
            { pShape[i] = nullptr; }
            else
            {
                auto* disposable = dynamic_cast<IDispose*>( pShape[i] );
                if ( disposable )
                { disposable->Dispose(); }
            }
        }
    }

    _aligned_free( this );
}

//----------------------------------------------------------------------------------------------------
//      衝突判定処理を行います.
//----------------------------------------------------------------------------------------------------
bool PLOBVH::IsHit( const Ray& ray, HitRecord& record ) const
{
    s32 mask;
    if ( !box.IsHit( Ray8(ray), mask ) )
    { return record.distance < F_MAX; }

    HitRecord rec;
    for( u32 i=0; i<8; ++i )
    {
        if ( mask & ( 0x1 << i ) )
        {
            if ( pShape[i]->IsHit( ray, rec ) )
            {
                if ( rec.distance < record.distance )
                { record = rec; }
            }
        }
    }

    return record.distance < F_MAX;
}

//----------------------------------------------------------------------------------------------------
//      マテリアルを取得します.
//----------------------------------------------------------------------------------------------------
IMaterial* PLOBVH::GetMaterial() const
{ return nullptr; }

//----------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//----------------------------------------------------------------------------------------------------
BoundingBox PLOBVH::GetBox() const
{ return box.GetBox(); }

//----------------------------------------------------------------------------------------------------
//      基本図形を取得します.
//----------------------------------------------------------------------------------------------------
bool PLOBVH::IsPrimitive() const
{ return false; }

//----------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//----------------------------------------------------------------------------------------------------
Vector3 PLOBVH::GetCenter() const
{
    auto result = pShape[0]->GetCenter();
    for( u32 i=1; i<8; ++i )
    { result += pShape[i]->GetCenter(); }
    result /= 8.0f;
    return result;
}

//---------------------------------------------------------------------------------------------------
//      ブランチを構築します.
//---------------------------------------------------------------------------------------------------
IShape* PLOBVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{
    auto box = CreateMergedBox( ppShapes, numShapes );
    auto morton_codes = new u32 [ numShapes ];
    auto indices      = new u32 [ numShapes ];

    for( u32 i=0; i<numShapes; ++i )
    {
        morton_codes[ i ] = GetMortonCode( ppShapes[ i ]->GetCenter(), box );
        indices[ i ] = i;
    }

    auto branch = InternalBuildBranch( morton_codes, indices, ppShapes, 0, numShapes, 0 );

    S3D_DELETE_ARRAY( indices );
    S3D_DELETE_ARRAY( morton_codes );

    return branch;
}

//----------------------------------------------------------------------------------------------------
//          ブランチを構築する内部処理です.
//----------------------------------------------------------------------------------------------------
IShape* PLOBVH::InternalBuildBranch
(
    const u32*  pMortonCodes,
    u32*        pIndices,
    IShape**    ppShapes,
    u32         a,
    u32         b,
    s32         level
)
{
    if ( ( b - a ) == 0 )
    { return new NullShape(); }

    if ( ( b - a ) <= 8 || level >= DIV_BIT * 3 )
    { return new Leaf( ( b - a ), &ppShapes[a] ); }

    u32* pA = pIndices + a;
    u32* pB = pIndices + b;
    u32* pC = std::partition( pA, pB, Separator( level, pMortonCodes ) );

    u32* pD = std::partition( pA, pC, Separator( level + 1, pMortonCodes ) );
    u32* pE = std::partition( pC, pB, Separator( level + 1, pMortonCodes ) );

    u32* pF = std::partition( pA, pD, Separator( level + 2, pMortonCodes ) );
    u32* pG = std::partition( pD, pC, Separator( level + 2, pMortonCodes ) );
    u32* pH = std::partition( pC, pE, Separator( level + 2, pMortonCodes ) );
    u32* pI = std::partition( pE, pB, Separator( level + 2, pMortonCodes ) );

    u32 f = a + ( pF - pA );
    u32 d = a + ( pD - pA );
    u32 g = a + ( pG - pA );
    u32 c = a + ( pC - pA );
    u32 h = c + ( pH - pC );
    u32 e = c + ( pE - pC );
    u32 i = c + ( pI - pC );

    IShape* pNode[8];
    pNode[0] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, a, f, level + 1 );
    pNode[1] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, f, d, level + 1 );
    pNode[2] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, d, g, level + 1 );
    pNode[3] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, g, c, level + 1 );
    pNode[4] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, c, h, level + 1 );
    pNode[5] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, h, e, level + 1 );
    pNode[6] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, e, i, level + 1 );
    pNode[7] = InternalBuildBranch( pMortonCodes, pIndices, ppShapes, i, b, level + 1 );

    auto buf = _aligned_malloc( sizeof(PLOBVH), 32 );
    assert( buf != nullptr );

    return new( buf ) PLOBVH( pNode );
}


} // namespace s3d
