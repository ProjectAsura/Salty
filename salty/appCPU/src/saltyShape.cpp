//-----------------------------------------------------------------------------------
// File : saltyShape.cpp
// Desc : Salty Spahe Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------
#include <saltyShape.h>
#include <cstdio>


namespace salty {

/////////////////////////////////////////////////////////////////////////////////////
// Box class
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------
Box::Box()
: m_pMaterial( nullptr )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------------
Box::Box( const Box& value )
: m_Box         ( value.m_Box )
, m_pMaterial   ( value.m_pMaterial )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
Box::Box( const asdx::Vector3 position, const asdx::Vector3 size )
: m_pMaterial   ( nullptr )
{
    asdx::Vector3 mini(
        position.x - size.x * 0.5f,
        position.y - size.y * 0.5f,
        position.z - size.z * 0.5f );

    asdx::Vector3 maxi(
        position.x + size.x * 0.5f,
        position.y + size.y * 0.5f,
        position.z + size.z * 0.5f );

    m_Box.min = mini;
    m_Box.max = maxi;
}

//-----------------------------------------------------------------------------------
//      レイとの交差判定を行います.
//-----------------------------------------------------------------------------------
bool Box::Intersects( const asdx::Ray& ray, HitRecord& record )
{
    f32 distance;
    if ( !m_Box.Intersects( ray, distance ) )
    { return false; }

    f32 dot[3];
    asdx::Vector3 normal[3] = {
        asdx::Vector3( 1.0f, 0.0f, 0.0f ),
        asdx::Vector3( 0.0f, 1.0f, 0.0f ),
        asdx::Vector3( 0.0f, 0.0f, 1.0f )
    };
    dot[0] = asdx::Vector3::Dot( ray.direction, normal[0] );
    dot[1] = asdx::Vector3::Dot( ray.direction, normal[1] );
    dot[2] = asdx::Vector3::Dot( ray.direction, normal[2] );

    int idx = 0;
    if ( fabs( dot[0] ) < fabs( dot[1] ) )
    { idx = 1; }
    if ( fabs( dot[0] ) < fabs( dot[2] ) )
    { idx = 2; }
    if ( fabs( dot[1] ) < fabs( dot[2] ) )
    { idx = 2; }

    record.Distance = distance;
    record.Position = ray.position + ray.direction * distance;
    record.Normal   = asdx::Sign( dot[idx] ) * normal[idx];
    record.pShape   = this;

    return true;
}

//-----------------------------------------------------------------------------------
//      マテリアルを設定します.
//-----------------------------------------------------------------------------------
void Box::SetMaterial( IMaterial* pMaterial )
{ m_pMaterial = pMaterial; }

//-----------------------------------------------------------------------------------
//      マテリアルを取得します.
//-----------------------------------------------------------------------------------
IMaterial* Box::GetMaterial()
{ return m_pMaterial; }

//-----------------------------------------------------------------------------------
//      代入演算子です.
//-----------------------------------------------------------------------------------
Box& Box::operator = ( const Box& value )
{
    m_Box       = value.m_Box;
    m_pMaterial = value.m_pMaterial;

    return (*this);
}


/////////////////////////////////////////////////////////////////////////////////////
// Sphere class
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------
Sphere::Sphere()
: m_pMaterial( nullptr )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------------
Sphere::Sphere( const Sphere& value )
: m_Sphere      ( value.m_Sphere )
, m_pMaterial   ( value.m_pMaterial )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
Sphere::Sphere( const asdx::Vector3 position, const f32 radius )
: m_Sphere      ( position, radius )
, m_pMaterial   ( nullptr )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      レイとの交差判定を行います.
//-----------------------------------------------------------------------------------
bool Sphere::Intersects( const asdx::Ray& ray, HitRecord& record )
{
    f32 distance;
    if ( !m_Sphere.Intersects( ray, distance ) )
    { return false; }

    record.Distance = distance;
    record.Position = ray.position + ray.direction * distance;
    record.pShape   = this;

    asdx::Vector3 normal = record.Position - m_Sphere.center;
    normal.Normalize();
    record.Normal = ( asdx::Vector3::Dot( ray.direction, normal ) < 0.0f ) ? normal : -normal;

    return true;
}

//-----------------------------------------------------------------------------------
//      マテリアルを設定します.
//-----------------------------------------------------------------------------------
void Sphere::SetMaterial( IMaterial* pMaterial )
{ m_pMaterial = pMaterial; }

//-----------------------------------------------------------------------------------
//      マテリアルを取得します.
//-----------------------------------------------------------------------------------
IMaterial* Sphere::GetMaterial()
{ return m_pMaterial; }

//-----------------------------------------------------------------------------------
//      代入演算子です.
//-----------------------------------------------------------------------------------
Sphere& Sphere::operator = ( const Sphere& value )
{
    m_Sphere    = value.m_Sphere;
    m_pMaterial = value.m_pMaterial;
    return (*this);
}


/////////////////////////////////////////////////////////////////////////////////////
// Plane class
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------
Plane::Plane()
: m_pMaterial( nullptr )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------------
Plane::Plane( const Plane& value )
: m_pMaterial( value.m_pMaterial )
, m_Plane    ( value.m_Plane )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
Plane::Plane( const asdx::Vector3 normal, const f32 distance )
: m_pMaterial( nullptr )
, m_Plane    ( normal, distance )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      レイとの交差判定を行います.
//-----------------------------------------------------------------------------------
bool Plane::Intersects( const asdx::Ray& ray, HitRecord& record )
{
    f32 distance;
    if ( !ray.Intersects( m_Plane, distance ) )
    { return false; }

    record.Distance = distance;
    record.Normal   = m_Plane.normal;
    record.Position = ray.position + ray.direction * distance;
    record.pShape   = this;

    return true;
}

//-----------------------------------------------------------------------------------
//      マテリアルを設定します.
//-----------------------------------------------------------------------------------
void Plane::SetMaterial( IMaterial* pMaterial )
{ m_pMaterial = pMaterial; }

//-----------------------------------------------------------------------------------
//      マテリアルを取得します.
//-----------------------------------------------------------------------------------
IMaterial* Plane::GetMaterial()
{ return m_pMaterial; }

//-----------------------------------------------------------------------------------
//      代入演算子です.
//-----------------------------------------------------------------------------------
Plane& Plane::operator = ( const Plane& value )
{
    m_pMaterial = value.m_pMaterial;
    m_Plane     = value.m_Plane;
    return (*this);
}


/////////////////////////////////////////////////////////////////////////////////////
// Triangle class
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------
Triangle::Triangle()
: m_pMaterial( nullptr )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------------
Triangle::Triangle( const Triangle& value )
: m_pMaterial   ( value.m_pMaterial )
, m_Normal      ( value.m_Normal )
{
    m_Vertex[0] = value.m_Vertex[0];
    m_Vertex[1] = value.m_Vertex[1];
    m_Vertex[2] = value.m_Vertex[2];
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
Triangle::Triangle
(
    const asdx::Vector3 v0,
    const asdx::Vector3 v1,
    const asdx::Vector3 v2
)
: m_pMaterial( nullptr )
{
    m_Vertex[0] = v0;
    m_Vertex[1] = v1;
    m_Vertex[2] = v2;
    m_Normal = asdx::Vector3::ComputeNormal( v0, v1, v2 );
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
Triangle::Triangle
(
    const asdx::Vector3 v0,
    const asdx::Vector3 v1,
    const asdx::Vector3 v2,
    const asdx::Vector3 n 
)
: m_pMaterial( nullptr )
, m_Normal   ( n )
{
    m_Vertex[0] = v0;
    m_Vertex[1] = v1;
    m_Vertex[2] = v2;
}

//-----------------------------------------------------------------------------------
//      レイとの交差判定を行います.
//-----------------------------------------------------------------------------------
bool Triangle::Intersects( const asdx::Ray& ray, HitRecord& record )
{
    f32 distance;
    if ( !ray.Intersects( m_Vertex[0], m_Vertex[1], m_Vertex[2], distance ) )
    { return false; }

    record.Distance = distance;
    record.Normal   = m_Normal;
    record.Position = ray.position + ray.direction * distance;
    record.pShape   = this;

    return true;
}

//-----------------------------------------------------------------------------------
//      マテリアルを設定します.
//-----------------------------------------------------------------------------------
void Triangle::SetMaterial( IMaterial* pMaterial )
{ m_pMaterial = pMaterial; }

//-----------------------------------------------------------------------------------
//      マテリアルを取得します.
//-----------------------------------------------------------------------------------
IMaterial* Triangle::GetMaterial()
{ return m_pMaterial; }

//-----------------------------------------------------------------------------------
//      代入演算子です.
//-----------------------------------------------------------------------------------
Triangle& Triangle::operator = ( const Triangle& value )
{
    m_Vertex[0] = value.m_Vertex[0];
    m_Vertex[1] = value.m_Vertex[1];
    m_Vertex[2] = value.m_Vertex[2];
    m_Normal    = value.m_Normal;
    m_pMaterial = value.m_pMaterial;
    return (*this);
}



/////////////////////////////////////////////////////////////////////////////////////
// Quad class
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------
Quad::Quad()
: m_pMaterial( nullptr )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------------
Quad::Quad( const Quad& value )
: m_pMaterial   ( value.m_pMaterial )
, m_Normal      ( value.m_Normal )
{
    m_Vertex[0] = value.m_Vertex[0];
    m_Vertex[1] = value.m_Vertex[1];
    m_Vertex[2] = value.m_Vertex[2];
    m_Vertex[3] = value.m_Vertex[3];
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
Quad::Quad
(
    const asdx::Vector3 v0,
    const asdx::Vector3 v1,
    const asdx::Vector3 v2,
    const asdx::Vector3 v3
)
: m_pMaterial( nullptr )
{
    m_Vertex[0] = v0;
    m_Vertex[1] = v1;
    m_Vertex[2] = v2;
    m_Vertex[3] = v3;
    m_Normal = asdx::Vector3::ComputeQuadNormal( v0, v1, v2, v3 );
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
Quad::Quad
(
    const asdx::Vector3 v0,
    const asdx::Vector3 v1,
    const asdx::Vector3 v2,
    const asdx::Vector3 v3,
    const asdx::Vector3 n 
)
: m_pMaterial( nullptr )
, m_Normal   ( n )
{
    m_Vertex[0] = v0;
    m_Vertex[1] = v1;
    m_Vertex[2] = v2;
    m_Vertex[3] = v3;
}

//-----------------------------------------------------------------------------------
//      レイとの交差判定を行います.
//-----------------------------------------------------------------------------------
bool Quad::Intersects( const asdx::Ray& ray, HitRecord& record )
{
    f32 distance;
    if ( ray.Intersects( m_Vertex[0], m_Vertex[1], m_Vertex[2], distance ) )
    {
        record.Distance = distance;
        record.Normal   = m_Normal;
        record.Position = ray.position + ray.direction * distance;
        record.pShape   = this;
        return true;
    }

    if ( ray.Intersects( m_Vertex[2], m_Vertex[3], m_Vertex[0], distance ) )
    {
        record.Distance = distance;
        record.Normal   = m_Normal;
        record.Position = ray.position + ray.direction * distance;
        record.pShape   = this;
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------
//      マテリアルを設定します.
//-----------------------------------------------------------------------------------
void Quad::SetMaterial( IMaterial* pMaterial )
{ m_pMaterial = pMaterial; }

//-----------------------------------------------------------------------------------
//      マテリアルを取得します.
//-----------------------------------------------------------------------------------
IMaterial* Quad::GetMaterial()
{ return m_pMaterial; }

//-----------------------------------------------------------------------------------
//      代入演算子です.
//-----------------------------------------------------------------------------------
Quad& Quad::operator = ( const Quad& value )
{
    m_Vertex[0] = value.m_Vertex[0];
    m_Vertex[1] = value.m_Vertex[1];
    m_Vertex[2] = value.m_Vertex[2];
    m_Vertex[3] = value.m_Vertex[3];
    m_Normal    = value.m_Normal;
    m_pMaterial = value.m_pMaterial;
    return (*this);
}


/////////////////////////////////////////////////////////////////////////////////////
// Mesh class
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------
Mesh::Mesh()
: m_pTriangles( nullptr )
{
}

//-----------------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------------
Mesh::Mesh( const Mesh& value )
{
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
Mesh::Mesh( const asdx::ResMesh& mesh )
{
}

//-----------------------------------------------------------------------------------
//      レイとの交差判定を行います.
//-----------------------------------------------------------------------------------
bool Mesh::Intersects( const asdx::Ray& ray, HitRecord& record )
{
    return false;
}

//-----------------------------------------------------------------------------------
//      マテリアルを設定します.
//-----------------------------------------------------------------------------------
void Mesh::SetMaterial( IMaterial* pMaterial )
{ ASDX_UNUSED_VAR( pMaterial ); }

//-----------------------------------------------------------------------------------
//      マテリアルを取得します.
//-----------------------------------------------------------------------------------
IMaterial* Mesh::GetMaterial()
{ return nullptr; }

//-----------------------------------------------------------------------------------
//      代入演算子です.
//-----------------------------------------------------------------------------------
Mesh& Mesh::operator = ( const Mesh& value )
{
 
    return (*this);
}

} // namespace salty
