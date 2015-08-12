﻿//-------------------------------------------------------------------------------------------------
// File : s3d_instance.cpp
// Desc : Instance Module.
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
Instance::Instance( const IShape* shape, const Matrix& world )
: m_pShape      ( shape )
, m_World       ( world )
, m_InvWorld    ( Matrix::Invert( world ) )
, m_WorldBox    ( BoundingBox::Transform( m_pShape->GetBox(), world ) )
, m_WorldCenter ( Vector3::Transform( m_pShape->GetCenter(), world ) )
{
    m_WorldCenter = ( m_WorldBox.maxi + m_WorldBox.maxi ) * 0.5f;
}

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
Instance::~Instance()
{ m_pShape = nullptr; }

//-------------------------------------------------------------------------------------------------
//      交差判定を行います.
//-------------------------------------------------------------------------------------------------
bool Instance::IsHit( const Ray& ray, HitRecord& record ) const
{
    auto pos = Vector3::TransformCoord ( ray.pos, m_InvWorld );
    auto dir = Vector3::TransformNormal( ray.dir, m_InvWorld );
    Ray localRay( pos, Vector3::UnitVector( dir ) );

    if ( m_pShape->IsHit( localRay, record ) )
    {
        record.position = Vector3::Transform( record.position, m_World );
        record.normal   = Vector3::TransformNormal( record.normal, Matrix::Transpose( m_InvWorld ) );
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
//      基本図形かどうかチェックします.
//-------------------------------------------------------------------------------------------------
bool Instance::IsPrimitive() const
{ return m_pShape->IsPrimitive(); }

//-------------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------------
BoundingBox Instance::GetBox() const
{ return m_WorldBox; }

//-------------------------------------------------------------------------------------------------
//      マテリアルを取得します.
//-------------------------------------------------------------------------------------------------
const IMaterial* Instance::GetMaterial() const
{ return m_pShape->GetMaterial(); }

//-------------------------------------------------------------------------------------------------
//      中心座標を取得します.
//-------------------------------------------------------------------------------------------------
Vector3 Instance::GetCenter() const
{ return m_WorldCenter; }

} // namespace s3d