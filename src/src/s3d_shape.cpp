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
    const f32           _radius,
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
    const f32 b  = Vector3::Dot(po, ray.dir);
    const f32 D4 = b * b - Vector3::Dot(po, po) + radius * radius;

    if ( D4 <= 0.0f )
    { return false; }   // 交差しなかった.

    const f32 sqrt_D4 = sqrt(D4);
    const f32 t1 = b - sqrt_D4;
    const f32 t2 = b + sqrt_D4;

    if (t1 < F_HIT_MIN && t2 < F_HIT_MIN)
    { return false; }   // 交差しなかった.

    f32 dist = 0.0f;
    if (t1 > F_HIT_MIN) 
    { dist = t1; }
    else 
    { dist = t2; }

    if ( dist > record.distance )
    { return false; }

    record.distance  = dist;
    record.position  = ray.pos + record.distance * ray.dir;
    record.pShape    = this;

    f32 theta = acosf( record.normal.y );
    f32 phi   = atan2f( record.normal.x, record.normal.z );
    if ( phi < 0.0f )
    { phi += F_2PI; }

    record.texcoord = Vector2( phi * F_1DIV2PI, ( F_PI - theta ) * F_1DIVPI );

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
    const Vertex&       _v0,
    const Vertex&       _v1,
    const Vertex&       _v2,
    const IMaterial*    _pMaterial
)
: v0 ( _v0 )
, v1 ( _v1 )
, v2 ( _v2 )
, pMaterial ( _pMaterial )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------
Triangle::Triangle
(
    const Face3&        _face,
    const IMaterial*    _pMaterial
)
: v0 ( _face.v0.pos, _face.v0.texcoord, _face.normal )
, v1 ( _face.v1.pos, _face.v1.texcoord, _face.normal )
, v2 ( _face.v2.pos, _face.v2.texcoord, _face.normal )
, pMaterial ( _pMaterial )
{ /* DO_NOTHIGN */ }

//-------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------
bool Triangle::IsHit( const Ray& ray, HitRecord& record ) const
{
    Vector3 e1  = v1.pos - v0.pos;
    Vector3 e2  = v2.pos - v0.pos;
    Vector3 dir = ray.dir;
    Vector3 s1  = Vector3::Cross( dir, e2 );
    register f32 div = Vector3::Dot( s1, e1 );
    
    if ( -FLT_EPSILON <= div && div <= FLT_EPSILON )
    { return false; }

    Vector3 d = ray.pos - v0.pos;
    register f32 beta = Vector3::Dot( d, s1 ) / div;
    if ( beta <= 0.0 || beta >= 1.0 )
    { return false; }
    
    Vector3 s2 = Vector3::Cross( d, e1 );
    register f32 gamma = Vector3::Dot( dir, s2 ) / div;
    if ( gamma <= 0.0 || ( beta + gamma ) >= 1.0 )
    { return false; }

    register f32 dist = Vector3::Dot( e2, s2 ) / div;

    if ( dist < F_HIT_MIN || dist > F_HIT_MAX )
    { return false; }

    if ( dist > record.distance )
    { return false; }

    record.position = ray.pos + ray.dir * dist;
    record.distance = dist;
    record.pShape   = this;

    f32 alpha = 1.0f - beta - gamma;
    record.normal = Vector3(
        v0.normal.x * alpha + v1.normal.x * beta + v2.normal.x * gamma,
        v0.normal.y * alpha + v1.normal.y * beta + v2.normal.y * gamma,
        v0.normal.z * alpha + v1.normal.z * beta + v2.normal.z * gamma );
    record.normal.Normalize();

    record.texcoord = Vector2(
        v0.texcoord.x * alpha + v1.texcoord.x * beta + v2.texcoord.x * gamma,
        v0.texcoord.y * alpha + v1.texcoord.y * beta + v2.texcoord.y * gamma );

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
    Vector3 min = v0.pos;
    Vector3 max = v0.pos;
    min = Vector3::Min( min, v1.pos );
    min = Vector3::Min( min, v2.pos );

    max = Vector3::Max( max, v1.pos );
    max = Vector3::Max( max, v2.pos );

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
    const Vertex&       _v0,
    const Vertex&       _v1,
    const Vertex&       _v2,
    const Vertex&       _v3,
    const IMaterial*    _pMaterial
)
: v0        ( _v0 )
, v1        ( _v1 )
, v2        ( _v2 )
, v3        ( _v3 )
, pMaterial ( _pMaterial )
{ /* DO_NOTHING */ }


//-------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------
Quad::Quad
(
    const Face4&        _face,
    const IMaterial*    _pMaterial
)
: v0        ( _face.v0.pos, _face.v0.texcoord, _face.normal )
, v1        ( _face.v1.pos, _face.v1.texcoord, _face.normal )
, v2        ( _face.v2.pos, _face.v2.texcoord, _face.normal )
, v3        ( _face.v3.pos, _face.v3.texcoord, _face.normal )
, pMaterial ( _pMaterial )
{ /* DO_NOTHING */ }

//--------------------------------------------------------------------------------
//      三角形ポリゴンと衝突しているかチェックします.
//--------------------------------------------------------------------------------
bool Quad::IsHitTriangle
(
    const Ray&      ray,
    const Vertex&   a,
    const Vertex&   b,
    const Vertex&   c,
    HitRecord&      record
) const
{
    Vector3 e1 = b.pos - a.pos;
    Vector3 e2 = c.pos - a.pos;
    Vector3 s1 = Vector3::Cross( ray.dir, e2 );
    f32 div = Vector3::Dot( s1, e1 );
    
    if ( -FLT_EPSILON <= div && div <= FLT_EPSILON )
    { return false; }
        
    Vector3 d = ray.pos - a.pos;
    f32 beta = Vector3::Dot( d, s1 ) / div;
    if ( beta <= 0.0 || beta >= 1.0 )
    { return false; }
    
    Vector3 s2 = Vector3::Cross( d, e1 );
    f32 gamma = Vector3::Dot( ray.dir, s2 ) / div;
    if ( gamma <= 0.0 || ( beta + gamma ) >= 1.0 )
    { return false; }
    
    f32 dist = Vector3::Dot( e2, s2 ) / div;
    
    if ( dist < F_HIT_MIN || dist > F_HIT_MAX )
    { return false; }

    if ( dist > record.distance )
    { return false; }

    record.position = ray.pos + ray.dir * dist;
    record.distance = dist;
    record.pShape   = this;

    f32 alpha = 1.0f - beta - gamma;
    record.normal   = Vector3(
        a.normal.x * alpha + b.normal.x * beta + c.normal.x * gamma,
        a.normal.y * alpha + b.normal.y * beta + c.normal.y * gamma,
        a.normal.z * alpha + b.normal.z * beta + c.normal.z * gamma );
    record.normal.Normalize();

    record.texcoord = Vector2(
        a.texcoord.x * alpha + b.texcoord.x * beta + c.texcoord.x * gamma,
        a.texcoord.y * alpha + b.texcoord.y * beta + c.texcoord.y * gamma );

    return true;
}

//-------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------
bool Quad::IsHit( const Ray& ray, HitRecord& record ) const
{
    if ( IsHitTriangle( ray, v0, v1, v2, record ) )
    { return true; }
    
    if ( IsHitTriangle( ray, v2, v3, v0, record ) )
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
    Vector3 min = v0.pos;
    Vector3 max = v0.pos;
    min = Vector3::Min( min, v1.pos );
    min = Vector3::Min( min, v2.pos );
    min = Vector3::Min( min, v3.pos );

    max = Vector3::Max( max, v1.pos );
    max = Vector3::Max( max, v2.pos );
    max = Vector3::Max( max, v3.pos );

    return BoundingBox( min, max );
}

//-------------------------------------------------------------------------------------
//      基本図形であるかどうかチェックします.
//-------------------------------------------------------------------------------------
bool Quad::IsPrimitive() const
{ return true; }



} // namespace s3d

