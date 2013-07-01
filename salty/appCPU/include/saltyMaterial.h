//--------------------------------------------------------------------------------
// File : saltyMaterial.h
// Desc : Salty Material Module
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------

#ifndef __SALTY_MATERIAL_H__
#define __SALTY_MATERIAL_H__

//--------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------
#include <asdxMath.h>


namespace salty {

//////////////////////////////////////////////////////////////////////////////////
// Material interface
//////////////////////////////////////////////////////////////////////////////////
struct IMaterial
{
    //----------------------------------------------------------------------------
    //! @brief      サンプルをとります.
    //!
    //! @param [in]     normal      法線ベクトル.
    //! @param [in]     indir       入射ベクトル.
    //! @param [out]    outdir      出射ベクトル.
    //----------------------------------------------------------------------------
    virtual asdx::Vector3 Sample(
        const asdx::Vector3& normal,
        const asdx::Vector3& indir,
        asdx::Vector3&       outdir ) = 0;

    virtual asdx::Vector3 GetColor() = 0;
    virtual asdx::Vector3 GetEmissive() = 0;
};

struct Lambert : public IMaterial
{
    asdx::Vector3       Color;
    asdx::Vector3       Emissive;

    Lambert();
    Lambert( const asdx::Vector3 color, const asdx::Vector3 emssive );

    asdx::Vector3 Sample(
        const asdx::Vector3& normal,
        const asdx::Vector3& indir,
        asdx::Vector3&       outdir );

    asdx::Vector3 GetColor();
    asdx::Vector3 GetEmissive();
};


////////////////////////////////////////////////////////////////////////////////////
//// Lambert structure
////////////////////////////////////////////////////////////////////////////////////
//struct Lambert : public IMaterial
//{
//    asdx::Vector3   Diffuse;        //!< 拡散反射色です.
//    f32             Alpha;          //!< 透過度です.
//
//    //----------------------------------------------------------------------------
//    //! @brief      コンストラクタです.
//    //----------------------------------------------------------------------------
//    Lambert();
//
//    //----------------------------------------------------------------------------
//    //! @brief      引数付きコンストラクタです.
//    //----------------------------------------------------------------------------
//    Lambert( 
//        const asdx::Vector3 diffuse,
//        const f32           alpha );
//
//    //----------------------------------------------------------------------------
//    //! @brief      サンプルをとります.
//    //!
//    //! @param [in]     normal      法線ベクトル.
//    //! @param [in]     input       入射ベクトル.
//    //! @param [out]    output      出射ベクトル.
//    //! @param [out]    color       カラー
//    //----------------------------------------------------------------------------
//    void Sample(
//        const asdx::Vector3& normal,
//        const asdx::Vector3& input,
//        asdx::Vector3&       output,
//        asdx::Vector4&       color );
//};
//
////////////////////////////////////////////////////////////////////////////////////
//// Phong structure
////////////////////////////////////////////////////////////////////////////////////
//struct Phong : public IMaterial
//{
//    asdx::Vector3  Diffuse;         //!< 拡散反射色です.
//    f32            Alpha;           //!< 透過度です.
//    asdx::Vector3  Specular;        //!< 鏡面反射色です.
//    f32            Power;           //!< 鏡面反射強度です.
//
//    //----------------------------------------------------------------------------
//    //! @brief      コンストラクタです.
//    //----------------------------------------------------------------------------
//    Phong();
//
//    //----------------------------------------------------------------------------
//    //! @brief      引数付きコンストラクタです.
//    //----------------------------------------------------------------------------
//    Phong( 
//        const asdx::Vector3 diffuse,
//        const f32           alpha,
//        const asdx::Vector3 specular,
//        const f32           power );
//
//    //----------------------------------------------------------------------------
//    //! @brief      サンプルをとります.
//    //!
//    //! @param [in]     normal      法線ベクトル.
//    //! @param [in]     input       入射ベクトル.
//    //! @param [out]    output      出射ベクトル.
//    //! @param [out]    color       カラー
//    //----------------------------------------------------------------------------
//    void Sample(
//        const asdx::Vector3& normal,
//        const asdx::Vector3& input,
//        asdx::Vector3&       output,
//        asdx::Vector4&       color );
//};
//
////////////////////////////////////////////////////////////////////////////////////
//// CookTorrance structure
////////////////////////////////////////////////////////////////////////////////////
//struct CookTorrance : public IMaterial
//{
//    asdx::Vector3   Diffuse;        //!< 拡散反射色です.
//    f32             Alpha;          //!< 透過度です.
//    asdx::Vector3   Specular;       //!< 鏡面反射色です.
//    f32             Power;          //!< 鏡面反射強度です.
//    f32             Refractive;     //!< 屈折率です.
//
//    //----------------------------------------------------------------------------
//    //! @brief      コンストラクタです.
//    //----------------------------------------------------------------------------
//    CookTorrance();
//
//    //----------------------------------------------------------------------------
//    //! @brief      引数付きコンストラクタです.
//    //----------------------------------------------------------------------------
//    CookTorrance(
//        const asdx::Vector3 diffuse,
//        const f32           alpha,
//        const asdx::Vector3 specular,
//        const f32           power,
//        const f32           refractive );
//
//    //----------------------------------------------------------------------------
//    //! @brief      サンプルをとります.
//    //!
//    //! @param [in]     normal      法線ベクトル.
//    //! @param [in]     input       入射ベクトル.
//    //! @param [out]    output      出射ベクトル.
//    //! @param [out]    color       カラー
//    //----------------------------------------------------------------------------
//    void Sample(
//        const asdx::Vector3& normal,
//        const asdx::Vector3& input,
//        asdx::Vector3&       output,
//        asdx::Vector4&       color );
//};
//
////////////////////////////////////////////////////////////////////////////////////
//// Mirror structure
////////////////////////////////////////////////////////////////////////////////////
//struct Mirror : public IMaterial
//{
//    asdx::Vector3       Color;      //!< 物体の色です.
//    f32                 Alpha;      //!< 透過度です.
//
//    //----------------------------------------------------------------------------
//    //! @brief      サンプルをとります.
//    //!
//    //! @param [in]     normal      法線ベクトル.
//    //! @param [in]     input       入射ベクトル.
//    //! @param [out]    output      出射ベクトル.
//    //! @param [out]    color       カラー
//    //----------------------------------------------------------------------------
//    void Sample(
//        const asdx::Vector3& normal,
//        const asdx::Vector3& input,
//        asdx::Vector3&       output,
//        asdx::Vector4&       color );
//};
//
////////////////////////////////////////////////////////////////////////////////////
//// Glass structure
////////////////////////////////////////////////////////////////////////////////////
//struct Glass : public IMaterial
//{
//    asdx::Vector3       Color;          //!< 物体の色です.
//    f32                 Alpha;          //!< 透過度.
//    f32                 Refractive;     //!< 屈折率です.
//
//    //----------------------------------------------------------------------------
//    //! @brief      サンプルをとります.
//    //!
//    //! @param [in]     normal      法線ベクトル.
//    //! @param [in]     input       入射ベクトル.
//    //! @param [out]    output      出射ベクトル.
//    //! @param [out]    color       カラー
//    //----------------------------------------------------------------------------
//    void Sample(
//        const asdx::Vector3& normal,
//        const asdx::Vector3& input,
//        asdx::Vector3&       output,
//        asdx::Vector4&       color );
//};

} // namespace salty

#endif//__SALTY_MATERIAL_H__