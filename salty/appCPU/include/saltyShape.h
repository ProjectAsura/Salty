﻿//------------------------------------------------------------------------------
// File : saltyShape.h
// Desc : Salty Shape Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------

#ifndef __SALTY_SHAPE_H__
#define __SALTY_SHAPE_H__

//-------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------
#include <asdxMath.h>
#include <asdxGeometry.h>
#include <asdxResMesh.h>
#include <saltyHitRecord.h>
#include <saltyMaterial.h>


namespace salty {

//////////////////////////////////////////////////////////////////////////////////
// IShaper interface
//////////////////////////////////////////////////////////////////////////////////
struct IShape 
{
    //----------------------------------------------------------------------------
    //! @brief      レイとの交差判定を行います.
    //!
    //! @param [in]     ray     レイです.
    //! @param [out]    record  交差点の情報です.
    //! @retval true    交差しています.
    //! @retval false   交差はありません.
    //----------------------------------------------------------------------------
    virtual bool        Intersects( const asdx::Ray& ray, HitRecord& record ) = 0;

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを設定します.
    //!
    //! @param [in]     pMaterial       設定するマテリアル.
    //----------------------------------------------------------------------------
    virtual void        SetMaterial( IMaterial* pMaterial ) = 0;

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを取得します.
    //----------------------------------------------------------------------------
    virtual IMaterial*  GetMaterial() = 0;

#if defined(DEBUG) || defined(_DEBUG)
    virtual s32 GetType() = 0;
#endif
};


///////////////////////////////////////////////////////////////////////////////////
// Box structure
///////////////////////////////////////////////////////////////////////////////////
class Box : public IShape
{
    //=============================================================================
    // list of friend classes and methods.
    //=============================================================================
    /* NOTHING */

public:
    //=============================================================================
    // public variables
    //=============================================================================
    /* NOTHING */

    //=============================================================================
    // public methods.
    //=============================================================================

    //-----------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-----------------------------------------------------------------------------
    Box();

    //-----------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //-----------------------------------------------------------------------------
    Box( const Box& value );

    //-----------------------------------------------------------------------------
    //! @brief      引数つきコンストラクタです.
    //!
    //! @param [in]     position        位置座標です.
    //! @param [in]     size            サイズです.
    //-----------------------------------------------------------------------------
    Box( const asdx::Vector3 position, const asdx::Vector3 size );
 
    //----------------------------------------------------------------------------
    //! @brief      レイとの交差判定を行います.
    //!
    //! @param [in]     ray     レイです.
    //! @param [out]    record  交差点の情報です.
    //! @retval true    交差しています.
    //! @retval false   交差はありません.
    //----------------------------------------------------------------------------
    bool        Intersects  ( const asdx::Ray& ray, HitRecord& record );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを設定します.
    //!
    //! @param [in]     pMaterial       設定するマテリアル.
    //----------------------------------------------------------------------------
    void        SetMaterial ( IMaterial* pMaterial );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを取得します.
    //----------------------------------------------------------------------------
    IMaterial*  GetMaterial ();

    //----------------------------------------------------------------------------
    //! @brief      代入演算子です.
    //----------------------------------------------------------------------------
    Box& operator = ( const Box& value );

#if defined(DEBUG) || defined(_DEBUG)
    s32 GetType()
    { return 0; }
#endif

protected:
    //=============================================================================
    // protected variables.
    //=============================================================================
    /* NOTHING */

    //=============================================================================
    // protected methods.
    //=============================================================================
    /* NOTHING */

private:
    //=============================================================================
    // private variables.
    //=============================================================================
    IMaterial*          m_pMaterial;        //!< マテリアルです.
    asdx::BoundingBox   m_Box;              //!< 境界箱です.

    //=============================================================================
    // private methods.
    //=============================================================================
    /* NOTHING */
};


///////////////////////////////////////////////////////////////////////////////////
// Sphere class
///////////////////////////////////////////////////////////////////////////////////
class Sphere : public IShape
{
    //=============================================================================
    // list of friend classes and methods.
    //=============================================================================
    /* NOTHING */

public:
    //=============================================================================
    // public variables.
    //=============================================================================
    /* NOTHING */

    //=============================================================================
    // public methods.
    //=============================================================================

    //-----------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-----------------------------------------------------------------------------
    Sphere();

    //-----------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //-----------------------------------------------------------------------------
    Sphere( const Sphere& value );

    //-----------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //!
    //! @param [in]     position    位置座標です.
    //! @param [in]     radius      半径です.
    //-----------------------------------------------------------------------------
    Sphere( const asdx::Vector3 position, const f32 radius );

    //----------------------------------------------------------------------------
    //! @brief      レイとの交差判定を行います.
    //!
    //! @param [in]     ray     レイです.
    //! @param [out]    record  交差点の情報です.
    //! @retval true    交差しています.
    //! @retval false   交差はありません.
    //----------------------------------------------------------------------------
    bool        Intersects( const asdx::Ray& ray, HitRecord& record );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを設定します.
    //!
    //! @param [in]     pMaterial       設定するマテリアル.
    //----------------------------------------------------------------------------
    void        SetMaterial( IMaterial* pMaterial );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを取得します.
    //----------------------------------------------------------------------------
    IMaterial*  GetMaterial();

    //----------------------------------------------------------------------------
    //! @brief      代入演算子です.
    //----------------------------------------------------------------------------
    Sphere& operator = ( const Sphere& value );

#if defined(DEBUG) || defined(_DEBUG)
    s32 GetType()
    { return 1; }
#endif

protected:
    //============================================================================
    // protected variables.
    //============================================================================
    /* NOTHING */

    //============================================================================
    // protected methods.
    //============================================================================
    /* NOTHING */

private:
    //============================================================================
    // private variables.
    //============================================================================
    IMaterial*              m_pMaterial;        //!< マテリアルです.
    asdx::BoundingSphere    m_Sphere;           //!< 境界球です.

    //============================================================================
    // private methods.
    //============================================================================
    /* NOTHING */
};


//////////////////////////////////////////////////////////////////////////////////
// Plane class
//////////////////////////////////////////////////////////////////////////////////
class Plane : public IShape
{
    //============================================================================
    // list of friend classes and methdos.
    //============================================================================
    /* NOTHING */

public:
    //============================================================================
    // public variables.
    //============================================================================
    /* NOTHING */

    //============================================================================
    // public methods.
    //============================================================================

    //----------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //----------------------------------------------------------------------------
    Plane();

    //----------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //----------------------------------------------------------------------------
    Plane( const Plane& value );

    //----------------------------------------------------------------------------
    //! @brief      引数つきコンストラクタです.
    //!
    //! @param [in]     normal      法線ベクトルです.
    //! @param [in]     distance    原点からの距離です.
    //----------------------------------------------------------------------------
    Plane( const asdx::Vector3 normal, const f32 distance );

    //----------------------------------------------------------------------------
    //! @brief      レイとの交差判定を行います.
    //!
    //! @param [in]     ray     レイです.
    //! @param [out]    record  交差点の情報です.
    //! @retval true    交差しています.
    //! @retval false   交差はありません.
    //----------------------------------------------------------------------------
    bool Intersects( const asdx::Ray& ray, HitRecord& record );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを設定します.
    //!
    //! @param [in]     pMaterial       設定するマテリアル.
    //----------------------------------------------------------------------------
    void SetMaterial( IMaterial* pMaterial );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを取得します.
    //----------------------------------------------------------------------------
    IMaterial* GetMaterial();

    //----------------------------------------------------------------------------
    //! @brief      代入演算子です.
    //----------------------------------------------------------------------------
    Plane& operator = ( const Plane& value );

#if defined(DEBUG) || defined(_DEBUG)
    s32 GetType()
    { return 2; }
#endif

protected:
    //============================================================================
    // protected variables.
    //============================================================================
    /* NOTHING */

    //============================================================================
    // protected methods.
    //============================================================================
    /* NOTHING */

private:
    //============================================================================
    // private variables.
    //============================================================================
    IMaterial*          m_pMaterial;
    asdx::Plane         m_Plane;

    //============================================================================
    // private methods.
    //============================================================================
    /* NOTHING */
};

///////////////////////////////////////////////////////////////////////////////////
// Triangle structure
///////////////////////////////////////////////////////////////////////////////////
class Triangle : public IShape
{
    //=============================================================================
    // list of friend classes and methods.
    //=============================================================================
    /* NOTHING */

public:
    //=============================================================================
    // public variables.
    //=============================================================================
    /* NOTHING */

    //=============================================================================
    // public methods.
    //=============================================================================

    //-----------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-----------------------------------------------------------------------------
    Triangle();

    //-----------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //-----------------------------------------------------------------------------
    Triangle( const Triangle& value );

    //-----------------------------------------------------------------------------
    //! @brief      引数つきコンストラクタです.
    //!             法線ベクトルは計算を行い，算出します.
    //!
    //! @param [in]     v0      三角形を構成する頂点です.
    //! @param [in]     v1      三角形を構成する頂点です.
    //! @param [in]     v2      三角形を構成する頂点です.
    //-----------------------------------------------------------------------------
    Triangle(
        const asdx::Vector3 v0,
        const asdx::Vector3 v1,
        const asdx::Vector3 v2 );

    //-----------------------------------------------------------------------------
    //! @brief      引数つきコンストラクタです.
    //!
    //! @param [in]     v0      三角形を構成する頂点です.
    //! @param [in]     v1      三角形を構成する頂点です.
    //! @param [in]     v2      三角形を構成する頂点です.
    //! @param [in]     n       法線ベクトルです.
    //-----------------------------------------------------------------------------
    Triangle( 
        const asdx::Vector3 v0,
        const asdx::Vector3 v1,
        const asdx::Vector3 v2, 
        const asdx::Vector3 n );

    //----------------------------------------------------------------------------
    //! @brief      レイとの交差判定を行います.
    //!
    //! @param [in]     ray     レイです.
    //! @param [out]    record  交差点の情報です.
    //! @retval true    交差しています.
    //! @retval false   交差はありません.
    //----------------------------------------------------------------------------
    bool        Intersects( const asdx::Ray& ray, HitRecord& record );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを設定します.
    //!
    //! @param [in]     pMaterial       設定するマテリアル.
    //----------------------------------------------------------------------------
    void        SetMaterial( IMaterial* pMaterial );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを取得します.
    //----------------------------------------------------------------------------
    IMaterial*  GetMaterial();

    //----------------------------------------------------------------------------
    //! @brief      代入演算子です.
    //----------------------------------------------------------------------------
    Triangle& operator = ( const Triangle& value );

#if defined(DEBUG) || defined(_DEBUG)
    s32 GetType()
    { return 3; }
#endif

protected:
    //============================================================================
    // protected variables.
    //============================================================================
    /* NOTHING */

    //============================================================================
    // protected methods.
    //============================================================================
    /* NOTHING */

private:
    //============================================================================
    // private variables.
    //============================================================================
    IMaterial*      m_pMaterial;        //!< マテリアルです.
    asdx::Vector3   m_Vertex[3];        //!< 頂点です.
    asdx::Vector3   m_Normal;           //!< 法線ベクトルです.

    //=============================================================================
    // private methods.
    //=============================================================================
    /* NOTHING */
};

///////////////////////////////////////////////////////////////////////////////////
// Quad structure
///////////////////////////////////////////////////////////////////////////////////
class Quad : public IShape
{
    //=============================================================================
    // list of friend classes and methods.
    //=============================================================================
    /* NOTHING */

public:
    //=============================================================================
    // public variables.
    //=============================================================================
    /* NOTHING */

    //=============================================================================
    // public methods.
    //=============================================================================

    //-----------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-----------------------------------------------------------------------------
    Quad();

    //-----------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //-----------------------------------------------------------------------------
    Quad( const Quad& value );

    //-----------------------------------------------------------------------------
    //! @brief      引数つきコンストラクタです.
    //!             法線ベクトルは計算を行い，算出します.
    //!
    //! @param [in]     v0      四角形を構成する頂点です.
    //! @param [in]     v1      四角形を構成する頂点です.
    //! @param [in]     v2      四角形を構成する頂点です.
    //! @param [in]     v3      四角形を構成する頂点です.
    //-----------------------------------------------------------------------------
    Quad(
        const asdx::Vector3 v0,
        const asdx::Vector3 v1,
        const asdx::Vector3 v2,
        const asdx::Vector3 v3 );

    //-----------------------------------------------------------------------------
    //! @brief      引数つきコンストラクタです.
    //!
    //! @param [in]     v0      四角形を構成する頂点です.
    //! @param [in]     v1      四角形を構成する頂点です.
    //! @param [in]     v2      四角形を構成する頂点です.
    //! @param [in]     v3      四角形を構成する頂点です.
    //! @param [in]     n       法線ベクトルです.
    //-----------------------------------------------------------------------------
    Quad( 
        const asdx::Vector3 v0,
        const asdx::Vector3 v1,
        const asdx::Vector3 v2,
        const asdx::Vector3 v3,
        const asdx::Vector3 n );

    //----------------------------------------------------------------------------
    //! @brief      レイとの交差判定を行います.
    //!
    //! @param [in]     ray     レイです.
    //! @param [out]    record  交差点の情報です.
    //! @retval true    交差しています.
    //! @retval false   交差はありません.
    //----------------------------------------------------------------------------
    bool        Intersects( const asdx::Ray& ray, HitRecord& record );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを設定します.
    //!
    //! @param [in]     pMaterial       設定するマテリアル.
    //----------------------------------------------------------------------------
    void        SetMaterial( IMaterial* pMaterial );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを取得します.
    //----------------------------------------------------------------------------
    IMaterial*  GetMaterial();

    //----------------------------------------------------------------------------
    //! @brief      代入演算子です.
    //----------------------------------------------------------------------------
    Quad& operator = ( const Quad& value );

#if defined(DEBUG) || defined(_DEBUG)
    s32 GetType()
    { return 4; }
#endif

protected:
    //============================================================================
    // protected variables.
    //============================================================================
    /* NOTHING */

    //============================================================================
    // protected methods.
    //============================================================================
    /* NOTHING */

private:
    //============================================================================
    // private variables.
    //============================================================================
    IMaterial*      m_pMaterial;        //!< マテリアルです.
    asdx::Vector3   m_Vertex[4];        //!< 頂点です.
    asdx::Vector3   m_Normal;           //!< 法線ベクトルです.

    //=============================================================================
    // private methods.
    //=============================================================================
    /* NOTHING */
};

///////////////////////////////////////////////////////////////////////////////////
// Mesh structure
///////////////////////////////////////////////////////////////////////////////////
class Mesh : public IShape
{
    //=============================================================================
    // list of friend classes and methods
    //=============================================================================
    /* NOTHING */

public:
    //=============================================================================
    // public variables.
    //=============================================================================
    /* NOTHING */

    //-----------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-----------------------------------------------------------------------------
    Mesh();

    //-----------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //-----------------------------------------------------------------------------
    Mesh( const Mesh& value );

    //-----------------------------------------------------------------------------
    //! @brief      引数つきコンストラクタです.
    //-----------------------------------------------------------------------------
    Mesh( const asdx::ResMesh& mesh );

    //----------------------------------------------------------------------------
    //! @brief      レイとの交差判定を行います.
    //!
    //! @param [in]     ray     レイです.
    //! @param [out]    record  交差点の情報です.
    //! @retval true    交差しています.
    //! @retval false   交差はありません.
    //----------------------------------------------------------------------------
    bool        Intersects( const asdx::Ray& ray, HitRecord& record );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを設定します.
    //----------------------------------------------------------------------------
    void        SetMaterial( IMaterial* pMaterial );

    //----------------------------------------------------------------------------
    //! @brief      マテリアルを取得します.
    //----------------------------------------------------------------------------
    IMaterial*  GetMaterial();

    //----------------------------------------------------------------------------
    //! @brief      代入演算子です.
    //----------------------------------------------------------------------------
    Mesh& operator = ( const Mesh& value );

#if defined(DEBUG) || defined(_DEBUG)
    s32 GetType()
    { return 5; }
#endif

protected:
    //============================================================================
    // protected variables.
    //============================================================================
    /* NOTHING */

    //============================================================================
    // protected methods.
    //============================================================================
    /* NOTHING */

private:
    //============================================================================
    // private variables.
    //============================================================================
    Triangle*       m_pTriangles;       //!< 三角形リストです.

    //============================================================================
    // private methods.
    //============================================================================
    /* NOTHING */
};




} // namespace salty

#endif//__SALTY_SHAPE_H__