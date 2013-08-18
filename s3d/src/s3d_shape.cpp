//--------------------------------------------------------------------------------------
// File : s3d_shape.cpp
// Desc : Shape Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------
#include <s3d_shape.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////////////
// Sphere structure
////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------------
Sphere::Sphere
(
    const f64           _radius,
    const Vector3&      _position,
    const Color&        _emissive,
    const Color&        _color,
    const MATERIAL_TYPE _material
)
: radius    ( _radius )
, position  ( _position )
, emissive  ( _emissive )
, color     ( _color )
, material  ( _material )
{ /* DO_NOTHING */ }


//--------------------------------------------------------------------------------------
//      交差判定を行います.
//--------------------------------------------------------------------------------------
bool Sphere::IsHit(const Ray &ray, HitRecord& record )
{
    const Vector3 po = position - ray.pos;
    const f64 b  = Vector3::Dot(po, ray.dir);
    const f64 D4 = b * b - Vector3::Dot(po, po) + radius * radius;

    if ( D4 < 0.0 )
    { return false; }   // 交差しなかった.

    const f64 sqrt_D4 = sqrt(D4);
    const f64 t1 = b - sqrt_D4;
    const f64 t2 = b + sqrt_D4;

    if (t1 < D_EPS && t2 < D_EPS)
    { return false; }   // 交差しなかった.

    if (t1 > D_EPS) 
    { record.distance = t1; }
    else 
    { record.distance = t2; }

    record.position  = ray.pos + record.distance * ray.dir;
    record.normal    = Vector3::UnitVector(record.position - position);
    record.pShape    = this;

    // 交差した.
    return true;
}

//-------------------------------------------------------------------------------------
//      マテリアルタイプを取得します.
//-------------------------------------------------------------------------------------
MATERIAL_TYPE Sphere::GetMaterialType()
{ return material; }

//-------------------------------------------------------------------------------------
//      マテリアルカラーを取得します.
//-------------------------------------------------------------------------------------
Color Sphere::GetColor()
{ return color; }

//-------------------------------------------------------------------------------------
//      自己発行カラーを取得します.
//-------------------------------------------------------------------------------------
Color Sphere::GetEmissive()
{ return emissive; }


///////////////////////////////////////////////////////////////////////////////////////
// Triangle structure
///////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------
Triangle::Triangle
(
    const Vector3&      _p0,
    const Vector3&      _p1,
    const Vector3&      _p2,
    const Color&        _color,
    const Color&        _emissive,
    const MATERIAL_TYPE _material
)
: p0        ( _p0 )
, p1        ( _p1 )
, p2        ( _p2 )
, color     ( _color )
, emissive  ( _emissive )
, material  ( _material )
{
    normal = Vector3::ComputeNormal( p0, p1, p2 );
}

//-------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------
bool Triangle::IsHit( const Ray& ray, HitRecord& record )
{
#if 0
    // 背面カリング.
    if ( Vector3::Dot( ray.dir, normal ) > 0.0 )
    { return false; }
#endif

    Vector3 e1 = p1 - p0;
    Vector3 e2 = p2 - p0;
    Vector3 dir = ray.dir;
    Vector3 s1 = Vector3::Cross( dir, e2 );
    register f64 div = Vector3::Dot( s1, e1 );
    
    if ( div == 0.0 )
    { return false; }
        
    Vector3 d = ray.pos - p0;
    register f64 b1 = Vector3::Dot( d, s1 ) / div;
    if ( b1 <= 0.0 || b1 >= 1.0 )
    { return false; }
    
    Vector3 s2 = Vector3::Cross( d, e1 );
    register f64 b2 = Vector3::Dot( dir, s2 ) / div;
    if ( b2 <= 0.0 || ( b1 + b2 ) >= 1.0 )
    { return false; }
    
    register f64 dist = Vector3::Dot( e2, s2 ) / div;
    
    if ( dist < D_EPS || dist > D_INF )
    { return false; }

    record.position = ray.pos + ray.dir * dist;
    record.distance = dist;
    record.normal   = normal;
    record.pShape   = this;

    return true;
}

//-------------------------------------------------------------------------------------
//      マテリアルタイプを取得します.
//-------------------------------------------------------------------------------------
MATERIAL_TYPE Triangle::GetMaterialType()
{ return material; }

//-------------------------------------------------------------------------------------
//      自己発光カラーを取得します.
//-------------------------------------------------------------------------------------
Color Triangle::GetEmissive()
{ return emissive; }

//-------------------------------------------------------------------------------------
//      マテリアルカラーを取得します.
//-------------------------------------------------------------------------------------
Color Triangle::GetColor()
{ return color; }


///////////////////////////////////////////////////////////////////////////////////////
// Quad structure
///////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------
Quad::Quad
(
    const Vector3&      _p0,
    const Vector3&      _p1,
    const Vector3&      _p2,
    const Vector3&      _p3,
    const Color&        _color,
    const Color&        _emissive,
    const MATERIAL_TYPE _material
)
: p0        ( _p0 )
, p1        ( _p1 )
, p2        ( _p2 )
, p3        ( _p3 )
, color     ( _color )
, emissive  ( _emissive )
, material  ( _material )
{
    normal = Vector3::ComputeQuadNormal( p0, p1, p2, p3 );
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
    HitRecord& record
)
{
#if 0
    // 背面カリング.
    if ( Vector3::Dot( ray.dir, normal ) > 0.0 )
    { return false; }
#endif

    Vector3 e1 = b - a;
    Vector3 e2 = c - a;
    Vector3 dir = ray.dir;
    Vector3 s1 = Vector3::Cross( dir, e2 );
    register f64 div = Vector3::Dot( s1, e1 );
    
    if ( div == 0.0 )
    { return false; }
        
    Vector3 d = ray.pos - a;
    register f64 b1 = Vector3::Dot( d, s1 ) / div;
    if ( b1 < 0.0 || b1 > 1.0 )
    { return false; }
    
    Vector3 s2 = Vector3::Cross( d, e1 );
    register f64 b2 = Vector3::Dot( dir, s2 ) / div;
    if ( b2 < 0.0 || ( b1 + b2 ) > 1.0 )
    { return false; }
    
    register f64 dist = Vector3::Dot( e2, s2 ) / div;
    
    if ( dist < D_EPS || dist > D_INF )
    { return false; }

    record.position = ray.pos + ray.dir * dist;
    record.distance = dist;
    record.normal   = normal;
    record.pShape   = this;

    return true;
}

//-------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------
bool Quad::IsHit( const Ray& ray, HitRecord& record )
{
    if ( IsHitTriangle( ray, p0, p1, p2, record ) )
    { return true; }
    else if ( IsHitTriangle( ray, p2, p3, p0, record ) )
    { return true; }

    return false;
}

//-------------------------------------------------------------------------------------
//      マテリアルタイプを取得します.
//-------------------------------------------------------------------------------------
MATERIAL_TYPE Quad::GetMaterialType()
{ return material; }

//-------------------------------------------------------------------------------------
//      自己発光カラーを取得します.
//-------------------------------------------------------------------------------------
Color Quad::GetEmissive()
{ return emissive; }

//-------------------------------------------------------------------------------------
//      マテリアルカラーを主特区します.
//-------------------------------------------------------------------------------------
Color Quad::GetColor()
{ return color; }


} // namespace s3d

