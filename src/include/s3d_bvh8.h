﻿//-------------------------------------------------------------------------------------------------
// File : s3d_bvh8.h
// Desc : Oct BVH Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Inlcudes
//-------------------------------------------------------------------------------------------------
#include <s3d_math.h>
#include <s3d_shape.h>
#include <vector>

#if 0
namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// BVH8 class
///////////////////////////////////////////////////////////////////////////////////////////////////
class BVH8 : IShape, IDisposable
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

public:
    //=============================================================================================
    // public variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // public methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      OBVHを構築します.
    //---------------------------------------------------------------------------------------------
    static IShape* Build(std::vector<IShape*>& pShapes);

    //---------------------------------------------------------------------------------------------
    //! @brief      破棄します.
    //---------------------------------------------------------------------------------------------
    void Dispose() override;

    //---------------------------------------------------------------------------------------------
    //! @brief      交差判定を行います.
    //---------------------------------------------------------------------------------------------
    bool IsHit(const Ray&, HitRecord& ) const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      バウンディングボックスを取得します.
    //---------------------------------------------------------------------------------------------
    BoundingBox GetBox() const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      基本図形かどうか判定します.
    //---------------------------------------------------------------------------------------------
    bool IsPrimitive() const override;

    //---------------------------------------------------------------------------------------------
    //! @brief      中心座標を取得します.
    //---------------------------------------------------------------------------------------------
    Vector3 GetCenter() const override;

private:
    //=============================================================================================
    // private varaibles.
    //=============================================================================================
    BoundingBox8    m_Box;
    IShape*         m_pNode[8];

    //=============================================================================================
    // private methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    BVH8( IShape** ppShapes, const BoundingBox8& box );

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    ~BVH8();
};

} // namespace s3d
#endif