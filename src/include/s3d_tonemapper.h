//------------------------------------------------------------------------------------------------
// File : s3d_tonemapper.h
// Desc : Tone Mapper Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------------
#include <s3d_typedef.h>
#include <s3d_math.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// TONE_MAPPING_TYPE
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TONE_MAPPING_TYPE
{
    TONE_MAPPING_REINHARD,          //!< Reinhard 方式.
    TONE_MAPPING_FILMIC,            //!< Uncharted2 方式.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ToneMapper class
///////////////////////////////////////////////////////////////////////////////////////////////////
class ToneMapper
{
public:
    static void Map( TONE_MAPPING_TYPE type, const s32 width, const s32 height, const Color* pPixels, Color* pResult );

private:
    static void ReinhardToneMapping( const s32 width, const s32 height, const Color* pPixels, Color* pResult );
    static void FilmicToneMapping  ( const s32 width, const s32 height, const Color* pPixels, Color* pResult );
};


} // namespace s3d

