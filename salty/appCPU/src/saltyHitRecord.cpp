//-------------------------------------------------------------------------------
// File : saltyHitRecord.cpp
// Desc : Salty Hit Record Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------
#include <saltyHitRecord.h>


namespace salty {

/////////////////////////////////////////////////////////////////////////////////
// HitRecord structure
/////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//-------------------------------------------------------------------------------
HitRecord::HitRecord()
: Distance  ( F32_MAX )
, Normal    ()
, Position  ()
, pShape    ( nullptr )
{
    /* DO_NOTHING */
}

//------------------------------------------------------------------------------
//      �R�s�[�R���X�g���N�^�ł�.
//------------------------------------------------------------------------------
HitRecord::HitRecord( const HitRecord& value )
: Distance  ( value.Distance )
, Normal    ( value.Normal )
, Position  ( value.Position )
, pShape    ( value.pShape )
{
    /* DO_NOTHING */
}

//------------------------------------------------------------------------------
//      �����t���R���X�g���N�^�ł�.
//------------------------------------------------------------------------------
HitRecord::HitRecord
(
    const f32           distance,
    const asdx::Vector3 normal,
    const asdx::Vector3 position,
    IShape*             shape
)
: Distance  ( distance )
, Normal    ( normal )
, Position  ( position )
, pShape    ( shape )
{
    /* DO_NOTHING */
}

//-----------------------------------------------------------------------------
//      ������Z�q�ł�.
//-----------------------------------------------------------------------------
HitRecord& HitRecord::operator = ( const HitRecord& value )
{
    Distance = value.Distance;
    Normal   = value.Normal;
    Position = value.Position;
    pShape   = value.pShape;

    return (*this);
}

} // namespace salty