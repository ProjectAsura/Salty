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
    const IMaterial*    _pMaterial
)
: radius    ( _radius )
, position  ( _position )
, pMaterial ( _pMaterial )
{ /* DO_NOTHING */ }


//--------------------------------------------------------------------------------------
//      交差判定を行います.
//--------------------------------------------------------------------------------------
bool Sphere::IsHit(const Ray &ray, HitRecord& record ) const
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

    f64 dist = 0.0;
    if (t1 > D_EPS) 
    { dist = t1; }
    else 
    { dist = t2; }

    if ( dist > record.distance )
    { return false; }

    record.distance  = dist;
    record.position  = ray.pos + record.distance * ray.dir;
    record.pShape    = this;

    f64 theta = acos( record.normal.y );
    f64 phi   = atan2( record.normal.x, record.normal.z );
    if ( phi < 0.0 )
    { phi += D_2PI; }

    record.texcoord = Vector2( phi * D_1DIV2PI, ( D_PI - theta ) * D_1DIVPI );

    // フラットシェーディング.
    record.normal    = Vector3::UnitVector(record.position - position);

    // 交差した.
    return true;
}

//-------------------------------------------------------------------------------------
//      マテリアルを取得します.
//-------------------------------------------------------------------------------------
const IMaterial* Sphere::GetMaterial() const
{ return pMaterial; }

//-------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------
BoundingBox Sphere::GetBox() const
{
    Vector3 min( position.x - radius, position.y - radius, position.z - radius );
    Vector3 max( position.y + radius, position.y + radius, position.z + radius );

    return BoundingBox( min, max );
}

//-------------------------------------------------------------------------------------
//      基本図形であるかどうか判定します.
//-------------------------------------------------------------------------------------
bool Sphere::IsPrimitive() const
{ return true; }


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
    const IMaterial*    _pMaterial,
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
bool Triangle::IsHit( const Ray& ray, HitRecord& record ) const
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

    if ( dist > record.distance )
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
//      マテリアルを取得します.
//-------------------------------------------------------------------------------------
const IMaterial* Triangle::GetMaterial() const
{ return pMaterial; }

//-------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------
BoundingBox Triangle::GetBox() const
{
    Vector3 min, max;
    min = Vector3::Min( p0, p1 );
    min = Vector3::Min( min, p2 );

    max = Vector3::Max( p0, p1 );
    max = Vector3::Max( max, p2 );

    return BoundingBox( min, max );
}

//-------------------------------------------------------------------------------------
//      基本図形であるかどうか判定します.
//-------------------------------------------------------------------------------------
bool Triangle::IsPrimitive() const
{ return true; }


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
    const IMaterial*    _pMaterial,
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
) const
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

    if ( dist > record.distance )
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
bool Quad::IsHit( const Ray& ray, HitRecord& record ) const
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
const IMaterial* Quad::GetMaterial() const
{ return pMaterial; }

//-------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------
BoundingBox Quad::GetBox() const
{
    Vector3 min, max;
    min = Vector3::Min( p0, p1 );
    min = Vector3::Min( min, p2 );
    min = Vector3::Min( min, p3 );

    max = Vector3::Max( p0, p1 );
    max = Vector3::Max( max, p2 );
    max = Vector3::Max( max, p3 );

    return BoundingBox( min, max );
}

//-------------------------------------------------------------------------------------
//      基本図形であるかどうかチェックします.
//-------------------------------------------------------------------------------------
bool Quad::IsPrimitive() const
{ return true; }


///////////////////////////////////////////////////////////////////////////////////////
// MeshTriangle structure
///////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------
MeshTriangle::MeshTriangle()
: pMaterial( nullptr )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-------------------------------------------------------------------------------------
MeshTriangle::MeshTriangle
(
    const Vector3&      p0,
    const Vector3&      p1,
    const Vector3&      p2,

    const Vector3&      n0,
    const Vector3&      n1,
    const Vector3&      n2,

    const Vector2&      t0,
    const Vector2&      t1,
    const Vector2&      t2,

    const IMaterial*    mat
)
: pMaterial( mat )
{
    p[ 0 ] = p0;
    p[ 1 ] = p1;
    p[ 2 ] = p2;

    n[ 0 ] = n0;
    n[ 1 ] = n1;
    n[ 2 ] = n2;

    t[ 0 ] = t0;
    t[ 1 ] = t1;
    t[ 2 ] = t2;
}


//-------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------
bool MeshTriangle::IsHit( const Ray& ray, HitRecord& record ) const
{
    Vector3 e1  = p[ 1 ] - p[ 0 ];
    Vector3 e2  = p[ 2 ] - p[ 0 ];
    Vector3 dir = ray.dir;
    Vector3 s1  = Vector3::Cross( dir, e2 );
    register f64 div = Vector3::Dot( s1, e1 );
    
    if ( div == 0.0 )
    { return false; }

    Vector3 d = ray.pos - p[ 0 ];
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

    if ( dist > record.distance )
    { return false; }

    record.position = ray.pos + ray.dir * dist;
    record.distance = dist;
    record.pShape   = this;

    f64 alpha = 1.0 - beta - gamma;
    record.texcoord.x = t[ 0 ].x * alpha + t[ 1 ].x * beta + t[ 2 ].x * gamma;
    record.texcoord.y = t[ 0 ].y * alpha + t[ 1 ].y * beta + t[ 2 ].y * gamma;

    // スムースシェーディング
    record.normal.x = n[ 0 ].x * alpha + n[ 1 ].x * beta + n[ 2 ].x * gamma;
    record.normal.y = n[ 0 ].y * alpha + n[ 1 ].y * beta + n[ 2 ].y * gamma;
    record.normal.z = n[ 0 ].z * alpha + n[ 1 ].z * beta + n[ 2 ].z * gamma;

    return true;
}

//-------------------------------------------------------------------------------------
//      マテリアルを取得します.
//-------------------------------------------------------------------------------------
const IMaterial* MeshTriangle::GetMaterial() const
{ return pMaterial; }

//-------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------
BoundingBox MeshTriangle::GetBox() const
{
    Vector3 min, max;
    min = Vector3::Min( p[ 0 ], p[ 1 ] );
    min = Vector3::Min( min,    p[ 2 ] );

    max = Vector3::Max( p[ 0 ], p[ 1 ] );
    max = Vector3::Max( max,    p[ 2 ] );

    return BoundingBox( min, max );
}

//-------------------------------------------------------------------------------------
//      基本図形であるかどうか判定します.
//-------------------------------------------------------------------------------------
bool MeshTriangle::IsPrimitive() const
{ return true; }


} // namespace s3d

