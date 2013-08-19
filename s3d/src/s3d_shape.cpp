﻿//--------------------------------------------------------------------------------------
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
    IMaterial*          _pMaterial
)
: radius    ( _radius )
, position  ( _position )
, pMaterial ( _pMaterial )
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

    f64 theta = acos( record.normal.z );
    f64 phi   = atan2( record.normal.y, record.normal.x );
    if ( phi < 0.0 )
    { phi += D_2PI; }

    record.texcoord = Vector2( phi * D_1DIV2PI, ( D_PI - theta ) * D_1DIVPI );

    // 交差した.
    return true;
}

//-------------------------------------------------------------------------------------
//      マテリアルを取得します.
//-------------------------------------------------------------------------------------
IMaterial* Sphere::GetMaterial()
{ return pMaterial; }

//-------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------
BoundingBox Sphere::GetBox()
{
    Vector3 min( position.x + radius, position.y + radius, position.z + radius );
    Vector3 max( position.y - radius, position.y - radius, position.z - radius );

    return BoundingBox( min, max );
}


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
    IMaterial*          _pMaterial,
    const Vector2&      _uv0,
    const Vector2&      _uv1,
    const Vector2&      _uv2
)
: p0        ( _p0 )
, p1        ( _p1 )
, p2        ( _p2 )
, pMaterial ( _pMaterial )
, uv0       ( _uv0 )
, uv1       ( _uv1 )
, uv2       ( _uv2 )
{
    normal = Vector3::ComputeNormal( p0, p1, p2 );
}

//-------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------
bool Triangle::IsHit( const Ray& ray, HitRecord& record )
{
    Vector3 e1  = p1 - p0;
    Vector3 e2  = p2 - p0;
    Vector3 dir = ray.dir;
    Vector3 s1  = Vector3::Cross( dir, e2 );
    register f64 div = Vector3::Dot( s1, e1 );
    
    if ( div == 0.0 )
    { return false; }

    Vector3 d = ray.pos - p0;
    register f64 beta = Vector3::Dot( d, s1 ) / div;
    if ( beta <= 0.0 || beta >= 1.0 )
    { return false; }
    
    Vector3 s2 = Vector3::Cross( d, e1 );
    register f64 gamma = Vector3::Dot( dir, s2 ) / div;
    if ( gamma <= 0.0 || ( beta + gamma ) >= 1.0 )
    { return false; }

    register f64 dist = Vector3::Dot( e2, s2 ) / div;

    if ( dist < D_EPS || dist > D_INF )
    { return false; }

    record.position = ray.pos + ray.dir * dist;
    record.distance = dist;
    record.normal   = normal;
    record.pShape   = this;

    f64 alpha = 1.0 - beta - gamma;
    record.texcoord = Vector2(
        uv0.x * alpha + uv1.x * beta + uv2.x * gamma,
        uv0.y * alpha + uv1.y * beta + uv2.y * gamma );

    return true;
}

//-------------------------------------------------------------------------------------
//! @brief      マテリアルを取得します.
//-------------------------------------------------------------------------------------
IMaterial* Triangle::GetMaterial()
{ return pMaterial; }

//-------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------
BoundingBox Triangle::GetBox()
{
    Vector3 min = Vector3::Min( p0, p1 );
    min = Vector3::Min( min, p2 );

    Vector3 max = Vector3::Max( p0, p1 );
    max = Vector3::Max( max, p2 );

    return BoundingBox( min, max );
}


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
    IMaterial*          _pMaterial,
    const Vector2&      _uv0,
    const Vector2&      _uv1,
    const Vector2&      _uv2,
    const Vector2&      _uv3
)
: p0        ( _p0 )
, p1        ( _p1 )
, p2        ( _p2 )
, p3        ( _p3 )
, pMaterial ( _pMaterial )
, uv0       ( _uv0 )
, uv1       ( _uv1 )
, uv2       ( _uv2 )
, uv3       ( _uv3 )
{
    normal = Vector3::ComputeQuadNormal( p0, p1, p2, p3 );
}

//--------------------------------------------------------------------------------
//      三角形ポリゴンと衝突しているかチェックします.
//--------------------------------------------------------------------------------
bool Quad::IsHitTriangle
(
    const Ray&      ray,
    const Vector3&  a,
    const Vector3&  b,
    const Vector3&  c,
    const Vector2&  st0,
    const Vector2&  st1,
    const Vector2&  st2,
    HitRecord&      record
)
{
    Vector3 e1 = b - a;
    Vector3 e2 = c - a;
    Vector3 dir = ray.dir;
    Vector3 s1 = Vector3::Cross( dir, e2 );
    register f64 div = Vector3::Dot( s1, e1 );
    
    if ( div == 0.0 )
    { return false; }
        
    Vector3 d = ray.pos - a;
    register f64 beta = Vector3::Dot( d, s1 ) / div;
    if ( beta <= 0.0 || beta >= 1.0 )
    { return false; }
    
    Vector3 s2 = Vector3::Cross( d, e1 );
    register f64 gamma = Vector3::Dot( dir, s2 ) / div;
    if ( gamma <= 0.0 || ( beta + gamma ) >= 1.0 )
    { return false; }
    
    register f64 dist = Vector3::Dot( e2, s2 ) / div;
    
    if ( dist < D_EPS || dist > D_INF )
    { return false; }

    record.position = ray.pos + ray.dir * dist;
    record.distance = dist;
    record.normal   = normal;
    record.pShape   = this;

    f64 alpha = 1.0 - beta - gamma;
    record.texcoord = Vector2(
        st0.x * alpha + st1.x * beta + st2.x * gamma,
        st0.y * alpha + st1.y * beta + st2.y * gamma );

    return true;
}

//-------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------
bool Quad::IsHit( const Ray& ray, HitRecord& record )
{
    if ( IsHitTriangle( ray, p0, p1, p2,  uv0, uv1, uv2, record ) )
    { return true; }
    else if ( IsHitTriangle( ray, p2, p3, p0, uv2, uv3, uv0, record ) )
    { return true; }

    return false;
}

//-------------------------------------------------------------------------------------
//      マテリアルを取得します.
//-------------------------------------------------------------------------------------
IMaterial* Quad::GetMaterial()
{ return pMaterial; }

//-------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------
BoundingBox Quad::GetBox()
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

