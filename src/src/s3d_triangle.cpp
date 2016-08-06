//-------------------------------------------------------------------------------------------------
// File : s3d_triangle.cpp
// Desc : Triangle Shape Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_triangle.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Triangle class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
Triangle::Triangle(Vertex* pVertice, IMaterial* pMaterial)
: m_Count    (1)
, m_pMaterial(pMaterial)
{
    for(auto i=0; i<3; ++i)
    { m_Vertex[i] = pVertice[i]; }

    auto min = m_Vertex[0].Position;
    auto max = m_Vertex[0].Position;
    min = Vector3::Min( min, m_Vertex[1].Position );
    min = Vector3::Min( min, m_Vertex[2].Position );
    max = Vector3::Max( max, m_Vertex[1].Position );
    max = Vector3::Max( max, m_Vertex[2].Position );
    m_BoundingBox = BoundingBox( min, max );
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
Triangle::~Triangle()
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void Triangle::AddRef() 
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------------
void Triangle::Release()
{
    m_Count--;
    if (m_Count == 0)
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 Triangle::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------------------
bool Triangle::IsHit(const Ray& ray, HitRecord& record) const
{
    auto e1  = m_Vertex[1].Position - m_Vertex[0].Position;
    auto e2  = m_Vertex[2].Position - m_Vertex[0].Position;
    auto dir = ray.dir;
    auto s1  = Vector3::Cross( dir, e2 );
    auto div = Vector3::Dot( s1, e1 );

    if ( -FLT_EPSILON <= div && div <= FLT_EPSILON )
    { return false; }

    auto d = ray.pos - m_Vertex[0].Position;
    auto beta = Vector3::Dot( d, s1 ) / div;
    if ( beta <= 0.0 || beta >= 1.0 )
    { return false; }

    auto s2 = Vector3::Cross( d, e1 );
    auto gamma = Vector3::Dot( dir, s2 ) / div;
    if ( gamma <= 0.0 || ( beta + gamma ) >= 1.0 )
    { return false; }

    auto dist = Vector3::Dot( e2, s2 ) / div;
    if ( dist < F_HIT_MIN || dist > F_HIT_MAX )
    { return false; }

    if ( dist > record.distance )
    { return false; }

    record.position  = ray.pos + ray.dir * dist;
    record.distance  = dist;
    record.pShape    = this;
    record.pMaterial = m_pMaterial;

    auto alpha = 1.0f - beta - gamma;
    record.normal = Vector3(
        m_Vertex[0].Normal.x * alpha + m_Vertex[1].Normal.x * beta + m_Vertex[2].Normal.x * gamma,
        m_Vertex[0].Normal.y * alpha + m_Vertex[1].Normal.y * beta + m_Vertex[2].Normal.y * gamma,
        m_Vertex[0].Normal.z * alpha + m_Vertex[1].Normal.z * beta + m_Vertex[2].Normal.z * gamma );
    record.normal.Normalize();

    record.texcoord = Vector2(
        m_Vertex[0].TexCoord.x * alpha + m_Vertex[1].TexCoord.x * beta + m_Vertex[2].TexCoord.x * gamma,
        m_Vertex[0].TexCoord.y * alpha + m_Vertex[1].TexCoord.y * beta + m_Vertex[2].TexCoord.y * gamma );

    return true;
}

//-------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------------
BoundingBox Triangle::GetBox() const
{ return m_BoundingBox; }

//-------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//-------------------------------------------------------------------------------------------------
Vector3 Triangle::GetCenter() const
{ return m_BoundingBox.center; }

//-------------------------------------------------------------------------------------------------
//      生成処理を行います.
//-------------------------------------------------------------------------------------------------
IShape* Triangle::Create(Vertex* pVertices, IMaterial* pMaterial)
{  return new(std::nothrow) Triangle(pVertices, pMaterial); }

} // namespace s3d
