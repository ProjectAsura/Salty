﻿//-------------------------------------------------------------------------------------------------
// File : s3d_instance.cpp
// Desc : Shape Instance Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_instance.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Instance class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
Instance::Instance( IShape* shape, const Matrix& world )
: m_Count       ( 1 )
, m_pShape      ( shape )
, m_World       ( world )
, m_InvWorld    ( Matrix::Invert( world ) )
, m_WorldBox    ( BoundingBox::Transform( m_pShape->GetBox(), world ) )
, m_WorldCenter ( Vector3::Transform( m_pShape->GetCenter(), world ) )
{
    m_pShape->AddRef();
    m_WorldCenter = ( m_WorldBox.maxi + m_WorldBox.maxi ) * 0.5f;
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
Instance::~Instance()
{ SafeRelease( m_pShape ); }

//-------------------------------------------------------------------------------------------------
//      参照カウントを増やします.
//-------------------------------------------------------------------------------------------------
void Instance::AddRef()
{ m_Count++; }

//-------------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------------
void Instance::Release()
{
    m_Count--;
    if ( m_Count == 0 )
    { delete this; }
}

//-------------------------------------------------------------------------------------------------
//      参照カウントを取得します.
//-------------------------------------------------------------------------------------------------
u32 Instance::GetCount() const
{ return m_Count; }

//-------------------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------------------
bool Instance::IsHit( const RaySet& raySet, HitRecord& record ) const
{
    auto pos = Vector3::TransformCoord ( raySet.ray.pos, m_InvWorld );
    auto dir = Vector3::TransformNormal( raySet.ray.dir, m_InvWorld );
    auto localRaySet = MakeRaySet( pos, Vector3::UnitVector( dir ) );

    if ( m_pShape->IsHit( localRaySet, record ) )
    {
        //record.position = Vector3::Transform( record.position, m_World );
        //record.normal   = Vector3::TransformNormal( record.normal, Matrix::Transpose( m_InvWorld ) );
        return true;
    }

    return false;
}

void Instance::CalcParam(const Vector3& pos, const Vector2& barycentric, Vector3* normal, Vector2* texcoord) const
{
    m_pShape->CalcParam(pos, barycentric, normal, texcoord);
    *normal = Vector3::TransformNormal(*normal, Matrix::Transpose(m_InvWorld));
}

//-------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------------
BoundingBox Instance::GetBox() const
{ return m_WorldBox; }

//-------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//-------------------------------------------------------------------------------------------------
Vector3 Instance::GetCenter() const
{ return m_WorldCenter; }

//-------------------------------------------------------------------------------------------------
//      生成処理を行います.
//-------------------------------------------------------------------------------------------------
IShape* Instance::Create(IShape* pShape, const Matrix& world)
{ return new(std::nothrow) Instance(pShape, world); }

} // namespace s3d
