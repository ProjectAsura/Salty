﻿//-----------------------------------------------------------------------------
// File : s3d_triangle.cpp
// Desc : Triangle Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <s3d_triangle.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////
// Triangle strcture
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------
Triangle::Triangle
(
    const Vector3& pt0,
    const Vector3& pt1,
    const Vector3& pt2,
    IMaterial* mat
)
: p0( pt0 )
, p1( pt1 )
, p2( pt2 )
, pMaterial( mat )
{ 
    ComputeNormal();
}

//-----------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------
Triangle::Triangle( const Triangle& value )
: p0( value.p0 )
, p1( value.p1 )
, p2( value.p2 )
, pMaterial( value.pMaterial )
{
    ComputeNormal();
}

//-----------------------------------------------------------------------------
//      法線ベクトルを計算します
//-----------------------------------------------------------------------------
void Triangle::ComputeNormal()
{
    Vector3 e1 = p1 - p0;
    Vector3 e2 = p2 - p0;
    normal = Vector3::Cross( e1, e2 );
    normal.Normalize(); 
}

//-----------------------------------------------------------------------------
//      衝突判定を行います.
//-----------------------------------------------------------------------------
bool Triangle::IsHit( const Ray& ray, const f64 mini, const f64 maxi, ShadeRec& shadeRec )
{
    // 背面カリングをするため，裏面にヒットした場合は衝突していないとみなす.
    if ( Vector3::Dot( ray.GetDir(), normal ) > 0.0 )
    { return false; }

    Vector3 e1 = p1 - p0;
    Vector3 e2 = p2 - p0;
    Vector3 dir = ray.GetDir();
    Vector3 s1 = Vector3::Cross( dir, e2 );
    register f64 div = Vector3::Dot( s1, e1 );
    
    if ( div == 0.0 )
    { return false; }
        
    Vector3 d = ray.GetPos() - p0;
    register f64 b1 = Vector3::Dot( d, s1 ) / div;
    if ( b1 <= 0.0 || b1 >= 1.0 )
    { return false; }
    
    Vector3 s2 = Vector3::Cross( d, e1 );
    register f64 b2 = Vector3::Dot( dir, s2 ) / div;
    if ( b2 <= 0.0 || ( b1 + b2 ) >= 1.0 )
    { return false; }
    
    register f64 dist = Vector3::Dot( e2, s2 ) / div;
    
    if ( dist < mini || dist > maxi )
    { return false; }

    shadeRec.normal     = normal;
    shadeRec.dist       = dist;
    shadeRec.pMaterial  = pMaterial;

    return true;
}

//-----------------------------------------------------------------------------
//      軸平行境界箱を取得します.
//-----------------------------------------------------------------------------
BoundingBox Triangle::GetAABB() const
{
    Vector3 min = Vector3::Min( p0, p1 );
    min = Vector3::Min( min, p2 );

    Vector3 max = Vector3::Max( p0, p1 );
    max = Vector3::Max( max, p2 );

    return BoundingBox( min, max );
}


} // namespace s3d
