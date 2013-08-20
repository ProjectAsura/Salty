﻿//----------------------------------------------------------------------------------
// File : s3d_box.h
// Desc : Bounding Box Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------

#ifndef __S3D_BOX_H__
#define __S3D_BOX_H__

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <s3d_math.h>


namespace s3d {

/////////////////////////////////////////////////////////////////////////////////////
// BoundingBox structure
/////////////////////////////////////////////////////////////////////////////////////
struct BoundingBox
{
public:
    Vector3 min;        //!< 最小値です.
    Vector3 max;        //!< 最大値です.

    //-------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------------
    BoundingBox();

    //-------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //-------------------------------------------------------------------------------
    BoundingBox( const Vector3&, const Vector3& );

    //-------------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //-------------------------------------------------------------------------------
    BoundingBox( const BoundingBox& );

    //-------------------------------------------------------------------------------
    //! @brief      交差判定を行います.
    //-------------------------------------------------------------------------------
    bool IsHit( const Ray& ) const;

    //-------------------------------------------------------------------------------
    //! @brief      2つのバウンディングボックスをマージします.
    //-------------------------------------------------------------------------------
    static BoundingBox Merge( const BoundingBox&, const BoundingBox& );
};

} // namespace s3d


//-----------------------------------------------------------------------------------
// Inlines
//-----------------------------------------------------------------------------------
#include <s3d_box.inl>


#endif//__S3D_BOX_H__