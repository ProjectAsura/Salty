﻿//---------------------------------------------------------------------------
// File : s3d_bvh.h
// Desc : Bounding Volume Hierarchy Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------

#ifndef __S3D_BVH_H__
#define __S3D_BVH_H__

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include <s3d_box.h>
#include <s3d_shape.h>


namespace s3d {

//////////////////////////////////////////////////////////////////////////////
// BVH structure
//////////////////////////////////////////////////////////////////////////////
struct BVH : public IShape
{
public:
    BoundingBox box;            //!< バウンディングボックスです.
    IShape*     pLeft;          //!< 左のブランチです.
    IShape*     pRight;         //!< 右のブランチです.

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    BVH();

    //-------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //-------------------------------------------------------------------------
    BVH( IShape*, IShape* );

    //-------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //-------------------------------------------------------------------------
    BVH( IShape*, IShape*, const BoundingBox& );

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    virtual ~BVH();

    //-------------------------------------------------------------------------
    //! @brief      初期化を行います.
    //-------------------------------------------------------------------------
    void Init( IShape**, const u32 );

    //-------------------------------------------------------------------------
    //! @brief      交差判定を行います.
    //-------------------------------------------------------------------------
    bool IsHit( const Ray&, HitRecord& ) const;

    //-------------------------------------------------------------------------
    //! @brief      マテリアルを取得します.
    //-------------------------------------------------------------------------
    IMaterial* GetMaterial() const;

    //-------------------------------------------------------------------------
    //! @brief      バウンディングボックスを取得します.
    //-------------------------------------------------------------------------
    BoundingBox GetBox() const;

    //-------------------------------------------------------------------------
    //! @brief      基本図形であるか判定します.
    //-------------------------------------------------------------------------
    bool IsPrimitive() const;

    //-------------------------------------------------------------------------
    //! @brief      ブランチを構築します.
    //-------------------------------------------------------------------------
    static IShape* BuildBranch( IShape**, const s32 );

    //-------------------------------------------------------------------------
    //! @brief      ブランチを破棄します.
    //-------------------------------------------------------------------------
    static void DestroyBranch( BVH* );
};

} // namespace s3d

#endif//__S3D_BVH_H__
