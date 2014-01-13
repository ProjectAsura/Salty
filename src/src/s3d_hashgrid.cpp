//------------------------------------------------------------------------------------------
// File : s3d_hashgrid.cpp
// Desc : Hash Grid Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <s3d_hashgrid.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////////////////
// HashGrid class
////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------
//      コンストラクタです.
//------------------------------------------------------------------------------------------
HashGrid::HashGrid()
: m_Factor  ( 0.0f )
, m_NumHash ( 0 )
, m_Box     ()
, m_ppHead  ( nullptr )
{ /* DO_NOTHING */ }

//------------------------------------------------------------------------------------------
//      デストラクタです.
//------------------------------------------------------------------------------------------
HashGrid::~HashGrid()
{ Term(); }

//------------------------------------------------------------------------------------------
//      初期化処理を行います.
//------------------------------------------------------------------------------------------
bool HashGrid::Init( const s32 w, const s32 h, HitNode* pList )
{
    HitNode* pNode = pList;
    bool isInit = false;

    // 衝突点をすべて含むAABBを求める.
    while( pNode != nullptr )
    {
        HitRecordEx* pRecord = pNode->GetValue();
        if ( isInit )
        { m_Box = BoundingBox::Merge( m_Box, pRecord->position ); }
        else
        {
            m_Box = BoundingBox( pRecord->position );
            isInit = true;
        }

        pNode = pNode->GetNext();
    }

    Vector3 size = m_Box.max - m_Box.min;
    f32 aveSize  = ( size.x + size.y + size.z ) / 3.0f;
    f32 radius   = ( aveSize / ( ( w + h ) / 2.0f ) ) * 2.0f;
    Vector3 r( radius, radius, radius );


    s32 numPhoton = 0;
    pNode  = pList;
    isInit = false;

    while( pNode != nullptr )
    {
        HitRecordEx* pRecord = pNode->GetValue();
        pRecord->radiusSq    = radius * radius;
        pRecord->photonCount = 0;
        pRecord->flux        = Vector3( 0.0f, 0.0f, 0.0f );
        numPhoton++;

       // 半径を考慮してAABBを再設定する.
       if ( isInit )
       {
           m_Box = BoundingBox::Merge( m_Box, pRecord->position - r );
           m_Box = BoundingBox::Merge( m_Box, pRecord->position + r );
       }
       else
       {
           m_Box = BoundingBox( pRecord->position - r );
           m_Box = BoundingBox::Merge( m_Box, pRecord->position + r );
           isInit = true;
       }

       pNode = pNode->GetNext();
    }

    m_Factor  = 1.0f / ( radius * 2.0f );
    m_NumHash = numPhoton;
    m_ppHead  = new HitNode* [ m_NumHash ];

    for( u32 i=0; i<m_NumHash; ++i)
    { m_ppHead[i] = nullptr; }

    pNode = pList;
    while( pNode != nullptr )
    {
        HitRecordEx* pRecord = pNode->GetValue();
        Vector3 mini = ( ( pRecord->position - r ) - m_Box.min ) * m_Factor;
        Vector3 maxi = ( ( pRecord->position + r ) - m_Box.min ) * m_Factor;

        s32 minX = abs( s32( mini.x ) );
        s32 minY = abs( s32( mini.y ) );
        s32 minZ = abs( s32( mini.z ) );

        s32 maxX = abs( s32( maxi.x ) );
        s32 maxY = abs( s32( maxi.y ) );
        s32 maxZ = abs( s32( maxi.z ) );

        for( s32 z=minZ; z <= maxZ; ++z )
        {
            for( s32 y=minY; y <= maxY; ++y )
            {
                for( s32 x=minX; x <= maxX; ++x )
                {
                    u32 key = GetKey( x, y, z );
                    m_ppHead[ key ]->Add( pRecord );
                }
            }
        }

        pNode = pNode->GetNext();
    }

    return true;
}

//------------------------------------------------------------------------------------------
//      終了処理を行います.
//------------------------------------------------------------------------------------------
void HashGrid::Term()
{
    if ( m_ppHead )
    {
        HitNode* pNode = m_ppHead[ 0 ];
        HitNode* pTail = nullptr;

        // 末尾を見つける.
        while( pNode != nullptr )
        {
            if ( !pNode->HasNext() )
            {
                pTail = pNode;
                break;
            }

            pNode = pNode->GetNext();
        }

        // 末尾から前に向かえって進む.
        pNode = pTail;
        while( pNode != nullptr )
        {
            pNode = pNode->Remove();
        }

        delete[] m_ppHead;
        m_ppHead = nullptr;
    }

    m_Factor  = 0.0f;
    m_NumHash = 0;
    m_Box     = BoundingBox();
}

//------------------------------------------------------------------------------------------
//      ノードを取得します.
//------------------------------------------------------------------------------------------
HitNode* HashGrid::GetValue( const Vector3& point )
{
    Vector3 val = ( point - m_Box.min ) * m_Factor;
    s32 x = abs( s32( val.x ) );
    s32 y = abs( s32( val.y ) );
    s32 z = abs( s32( val.z ) );
    u32 key = GetKey( x, y, z );

    return m_ppHead[ key ];
}

//------------------------------------------------------------------------------------------
//      ハッシュキーを取得します.
//------------------------------------------------------------------------------------------
u32 HashGrid::GetKey( const s32 x, const s32 y, const s32 z )
{
    // M. Teschner, B. Heidelberger, M. Mueller, D. Pomeranets, M.Gross,
    // “Optimized Spatial Hashing for Collision Detection of Deformable Objects,” 
    // Proc. Vision, Modeling, Visualization VMV’03, Munich, Germany,
    // pp. 47-54, Nov. 19-21, 2003
    return (u32)( (x * 73856093)
               ^  (y * 19349663)
               ^  (z * 83492791) ) % m_NumHash;     /* 73856093, 19349663, 843492791 は素数.*/
}

} // namespace s3d
