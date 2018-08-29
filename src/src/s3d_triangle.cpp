//-------------------------------------------------------------------------------------------------
// File : s3d_triangle.cpp
// Desc : Triangle Shape Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_triangle.h>
#include <s3d_material.h>

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
    m_pMaterial->AddRef();

    m_Edge[0] = m_Vertex[1].Position - m_Vertex[0].Position;
    m_Edge[1] = m_Vertex[2].Position - m_Vertex[0].Position;
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
Triangle::~Triangle()
{ SafeRelease(m_pMaterial); }

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
bool Triangle::IsHit(const RaySet& raySet, HitRecord& record) const
{
    auto s1  = Vector3::Cross( raySet.ray.dir, m_Edge[1] );
    auto div = Vector3::Dot( s1, m_Edge[0] );

    if ( abs(div) <= FLT_EPSILON )
    { return false; }

    auto d = raySet.ray.pos - m_Vertex[0].Position;
    auto beta = Vector3::Dot( d, s1 ) / div;
    if ( beta <= 0.0 || beta >= 1.0 )
    { return false; }

    auto s2 = Vector3::Cross( d, m_Edge[0] );
    auto gamma = Vector3::Dot( raySet.ray.dir, s2 ) / div;
    if ( gamma <= 0.0 || ( beta + gamma ) >= 1.0 )
    { return false; }

    auto dist = Vector3::Dot( m_Edge[1], s2 ) / div;
    if ( dist < F_HIT_MIN || dist > F_HIT_MAX )
    { return false; }

    if ( dist >= record.distance )
    { return false; }

//    record.position  = raySet.ray.pos + raySet.ray.dir * dist;
    record.distance  = dist;
    record.pShape    = this;
    record.pMaterial = m_pMaterial;
    record.barycentric.x = beta;
    record.barycentric.y = gamma;

    //auto alpha = 1.0f - beta - gamma;
    //record.normal = Vector3(
    //    m_Vertex[0].Normal.x * alpha + m_Vertex[1].Normal.x * beta + m_Vertex[2].Normal.x * gamma,
    //    m_Vertex[0].Normal.y * alpha + m_Vertex[1].Normal.y * beta + m_Vertex[2].Normal.y * gamma,
    //    m_Vertex[0].Normal.z * alpha + m_Vertex[1].Normal.z * beta + m_Vertex[2].Normal.z * gamma );
    //record.normal.SafeNormalize();

    //record.texcoord = Vector2(
    //    m_Vertex[0].TexCoord.x * alpha + m_Vertex[1].TexCoord.x * beta + m_Vertex[2].TexCoord.x * gamma,
    //    m_Vertex[0].TexCoord.y * alpha + m_Vertex[1].TexCoord.y * beta + m_Vertex[2].TexCoord.y * gamma );

    return true;
}

void Triangle::CalcParam(const Vector3& position, const Vector2& barycentric, Vector3* pOutNormal, Vector2* pOutTexCoord) const
{
    auto beta = barycentric.x;
    auto gamma = barycentric.y;
    auto alpha = 1.0f - beta - gamma;
    *pOutNormal = Vector3(
        m_Vertex[0].Normal.x * alpha + m_Vertex[1].Normal.x * beta + m_Vertex[2].Normal.x * gamma,
        m_Vertex[0].Normal.y * alpha + m_Vertex[1].Normal.y * beta + m_Vertex[2].Normal.y * gamma,
        m_Vertex[0].Normal.z * alpha + m_Vertex[1].Normal.z * beta + m_Vertex[2].Normal.z * gamma );
    pOutNormal->SafeNormalize();

    *pOutTexCoord = Vector2(
        m_Vertex[0].TexCoord.x * alpha + m_Vertex[1].TexCoord.x * beta + m_Vertex[2].TexCoord.x * gamma,
        m_Vertex[0].TexCoord.y * alpha + m_Vertex[1].TexCoord.y * beta + m_Vertex[2].TexCoord.y * gamma );
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
