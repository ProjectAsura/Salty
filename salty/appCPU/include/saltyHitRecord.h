//--------------------------------------------------------------------------------
// File : saltyHitRecord.h
// Desc : Salty Hit Record Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------

#ifndef __SALTY_HIT_RECORD_H__
#define __SALTY_HIT_RECORD_H__

//--------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------
#include <asdxTypedef.h>
#include <asdxMath.h>


namespace salty {

//--------------------------------------------------------------------------------
// Forward Declaration.
//--------------------------------------------------------------------------------
struct IShape;


//////////////////////////////////////////////////////////////////////////////////
// HitRecord structure.
//////////////////////////////////////////////////////////////////////////////////
struct HitRecord
{
    f32                 Distance;       //!< 距離です.
    asdx::Vector3       Normal;         //!< 法線ベクトルです.
    asdx::Vector3       Position;       //!< 位置座標です.
    IShape*             pShape;         //!< シェイプです.

    HitRecord();

    HitRecord( const HitRecord& value );

    HitRecord( 
        const f32           distance, 
        const asdx::Vector3 normal,
        const asdx::Vector3 position,
        IShape*             pShape );

    HitRecord& operator = ( const HitRecord& value );
};


} // namespace salty

#endif//__SALTY_HIT_RECORD_H__