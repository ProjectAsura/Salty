//-------------------------------------------------------------------------------------------------
// File : s3d_sphere.cpp
// Desc : Sphere Shape Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_sphere.h>
#include <s3d_material.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Sphere class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
Sphere::Sphere(f32 radius, const Vector3& center, IMaterial* pMaterial)
: m_Count       ( 1 )
, m_Radius      ( radius )
, m_Center      ( center )
, m_pMaterial   ( pMaterial )
{
    Vector3 min( m_Center.x - m_Radius, m_Center.y - m_Radius, m_Center.z - m_Radius );
    Vector3 max( m_Center.x + m_Radius, m_Center.y + m_Radius, m_Center.z + m_Radius );
    m_Box = BoundingBox( min, max );
    m_pMaterial->AddRef();
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
Sphere::~Sphere()
{ SafeRelease(m_pMaterial); }

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void Sphere::AddRef()
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------------
void Sphere::Release()
{
    m_Count--;
    if ( m_Count == 0 )
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 Sphere::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------------------
bool Sphere::IsHit(const RaySet &raySet, HitRecord& record ) const
{
    const auto po = m_Center - raySet.ray.pos;
    const auto b  = Vector3::Dot(po, raySet.ray.dir);
    const auto D4 = b * b - Vector3::Dot(po, po) + m_Radius * m_Radius;

    if ( D4 < 0.0f )
    { return false; }   // 交差しなかった.

    const auto sqrt_D4 = sqrt(D4);
    const auto t1 = b - sqrt_D4;
    const auto t2 = b + sqrt_D4;

    if (t1 < F_HIT_MIN && t2 < F_HIT_MIN)
    { return false; }   // 交差しなかった.

    auto dist = ( t1 > F_HIT_MIN ) ? t1 : t2;
    if ( dist > record.distance )
    { return false; }

    record.distance  = dist;
    record.pShape    = this;
    record.pMaterial = m_pMaterial;

    // 交差した.
    return true;
}

void Sphere::CalcParam(const Vector3& pos, const Vector2& barycentric, Vector3* pNormal, Vector2* pTexCoord) const
{
    *pNormal = Vector3::UnitVector(pos - m_Center);

    auto theta = acosf( pNormal->y );
    auto phi   = atan2f( pNormal->x, pNormal->z );
    if ( phi < 0.0f )
    { phi += F_2PI; }

    *pTexCoord = Vector2( phi * F_1DIV2PI, ( F_PI - theta ) * F_1DIVPI );
}

void Sphere::Sample(PCG& random, Vector3* pPosition, float* pdf)
{
    const auto r1 = F_2PI * random.GetAsF32();
    const auto r2 = 1.0f - 2.0f * random.GetAsF32();
    const auto r3 = sqrt(1.0f - r2 * r2);
    const auto light_pos = m_Center + (m_Radius + 1e-1f) * Vector3::SafeUnitVector(Vector3(r3 * cos(r1), r3 * sin(r1), r2));

    *pPosition = light_pos;
    *pdf = 1.0f / (4.0f * F_PI * m_Radius * m_Radius);
}

//-------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------------
BoundingBox Sphere::GetBox() const
{ return m_Box; }

//-------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//-------------------------------------------------------------------------------------------------
Vector3 Sphere::GetCenter() const
{ return m_Center; }

//-------------------------------------------------------------------------------------------------
//      生成処理を行います.
//-------------------------------------------------------------------------------------------------
IShape* Sphere::Create(f32 radius, const Vector3& center, IMaterial* pMaterial)
{ return new(std::nothrow) Sphere(radius, center, pMaterial); }

} // namespace s3d
