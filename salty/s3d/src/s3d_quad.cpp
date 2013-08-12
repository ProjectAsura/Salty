//-----------------------------------------------------------------------------
// File : s3d_quad.cpp
// Desc : Quad Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <s3d_quad.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////
// Quad class
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//      コンストラクタです.
//------------------------------------------------------------------------------
Quad::Quad
(
    const Vector3& a,
    const Vector3& b,
    const Vector3& c,
    const Vector3& d,
    const Color3& col
)
: p0    ( a )
, p1    ( b )
, p2    ( c )
, p3    ( d )
, color ( col )
{
    ComputeQuadNormal();
}

//--------------------------------------------------------------------------------
//      コピーコンストラクタです.
//--------------------------------------------------------------------------------
Quad::Quad( const Quad& value )
: p0    ( value.p0 )
, p1    ( value.p1 )
, p2    ( value.p2 )
, p3    ( value.p3 )
, normal( value.normal )
, color ( value.color )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      三角形ポリゴンの法線ベクトルを求めます.
//--------------------------------------------------------------------------------
Vector3 Quad::ComputeNormal
(
    const Vector3& a,
    const Vector3& b,
    const Vector3& c
)
{
    Vector3 e1 = b - a;
    Vector3 e2 = c - a;
    Vector3 n = Vector3::Cross( e1, e2 );
    return Vector3::UnitVector( n );
}

//--------------------------------------------------------------------------------
//      四角形ポリゴンの法線ベクトルを求めます.
//--------------------------------------------------------------------------------
void Quad::ComputeQuadNormal()
{
    Vector3 n1a = ComputeNormal( p0, p1, p2 );
    Vector3 n1b = ComputeNormal( p0, p2, p3 );
    Vector3 n2a = ComputeNormal( p1, p2, p3 );
    Vector3 n2b = ComputeNormal( p1, p3, p2 );
    if ( Vector3::Dot( n1a, n1b ) > Vector3::Dot( n2a, n2b ) )
    {
        Vector3 normal = n1a + n1b;
        normal.Normalize();
    }
    else
    {
        Vector3 normal = n2a + n2b;
        normal.Normalize();
    }
}

//--------------------------------------------------------------------------------
//      三角形ポリゴンと衝突しているかチェックします.
//--------------------------------------------------------------------------------
bool Quad::IsHitTriangle
(
    const Ray& ray, 
    const Vector3& a,
    const Vector3& b,
    const Vector3& c,
    const f64 mini,
    const f64 maxi,
    ShadeRec& shadeRec
)
{
    Vector3 e1 = b - a;
    Vector3 e2 = c - a;
    Vector3 dir = ray.GetDir();
    Vector3 s1 = Vector3::Cross( dir, e2 );
    register f64 div = Vector3::Dot( s1, e1 );
    
    if ( div == 0.0 )
    { return false; }
        
    Vector3 d = ray.GetPos() - a;
    register f64 b1 = Vector3::Dot( d, s1 ) / div;
    if ( b1 < 0.0 || b1 > 1.0 )
    { return false; }
    
    Vector3 s2 = Vector3::Cross( d, e1 );
    register f64 b2 = Vector3::Dot( dir, s2 ) / div;
    if ( b2 < 0.0 || ( b1 + b2 ) > 1.0 )
    { return false; }
    
    register f64 dist = Vector3::Dot( e2, s2 ) / div;
    
    if ( dist < mini || dist > maxi )
    { return false; }
    
    shadeRec.dist   = dist;
    if ( Vector3::Dot( ray.GetDir(), normal ) > 0.0 )
    {
        // 裏面と衝突した場合は，法線ベクトルは逆向きにする.
        shadeRec.normal = -normal;
    }
    else
    {
        // 表面と衝突した場合は，法線ベクトルをそのまま使用.
        shadeRec.normal = normal;
    }
    shadeRec.color  = color;

    return true;
}

//--------------------------------------------------------------------------------
//      衝突判定を行います.
//--------------------------------------------------------------------------------
bool Quad::IsHit( const Ray& ray, const f64 mini, const f64 maxi, ShadeRec& shadeRec )
{
    if ( IsHitTriangle( ray, p0, p1, p2, mini, maxi, shadeRec ) )
    { return true; }

    if ( IsHitTriangle( ray, p0, p2, p3, mini, maxi, shadeRec ) )
    { return true; }

    return false;
}

//--------------------------------------------------------------------------------
//      軸平行境界箱を取得します.
//--------------------------------------------------------------------------------
BoundingBox Quad::GetAABB() const
{
    Vector3 min = Vector3::Min( p0, p1 );
    min = Vector3::Min( min, p2 );
    min = Vector3::Min( min, p3 );

    Vector3 max = Vector3::Max( p0, p1 );
    max = Vector3::Max( max, p2 );
    max = Vector3::Max( max, p3 );

    return BoundingBox( min, max );
}


} // namespace s3d
