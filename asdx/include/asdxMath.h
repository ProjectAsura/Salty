//-----------------------------------------------------------------------
// File : AsMath.h
// Desc : Asura Math
// Copyright(c) Project Asura All right reserved.
//-----------------------------------------------------------------------

#ifndef __ASDX_MATH_H__
#define __ASDX_MATH_H__

//-----------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------
#ifndef ASDX_INLINE
    #if (_MSC_VER >= 1200)
        #define ASDX_INLINE __forceinline
    #else
        #define ASDX_INLINE __inline
    #endif
#else
    #ifdef __cplusplus
        #define ASDX_INLINE inline
    #else
        #define ASDX_INLINE
    #endif
#endif

#ifndef ASDX_TEMPLATE
#define ASDX_TEMPLATE template< typename type >
#endif

#ifndef ASDX_TEMP_INLINE
#define ASDX_TEMP_INLINE ASDX_TEMPLATE ASDX_INLINE
#endif

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include <cmath>
#include <cfloat>
#include <cassert>
#include <cstring>
#include <DirectXMath.h>


namespace asdx {

//-----------------------------------------------------------------------
// Data Types
//-----------------------------------------------------------------------
typedef float           FLOAT;
typedef double          DOUBLE;
typedef signed char     SCHAR;
typedef unsigned char   UCHAR;
typedef signed short    SSHORT;
typedef unsigned short  USHORT;
typedef signed int      SINT;
typedef unsigned int    UINT;
typedef signed long     SLONG;
typedef unsigned long   ULONG;

//-----------------------------------------------------------------------
// Constant Variables
//-----------------------------------------------------------------------
const asdx::FLOAT     F_PI        = 3.1415926535897932384626433832795f;
const asdx::FLOAT     F_2PI       = 6.283185307179586476925286766559f;
const asdx::FLOAT     F_1DIVPI    = 0.31830988618379067153776752674503f;
const asdx::FLOAT     F_PIDIV2    = 1.5707963267948966192313216916398f;
const asdx::FLOAT     F_PIDIV3    = 1.0471975511965977461542144610932f;
const asdx::FLOAT     F_PIDIV4    = 0.78539816339744830961566084581988f;
const asdx::FLOAT     F_PIDIV6    = 0.52359877559829887307710723054658f;
const asdx::FLOAT     F_EPSILON   = FLT_EPSILON;
const asdx::DOUBLE    D_EPSILON   = DBL_EPSILON;


//-----------------------------------------------------------------------
// Function
//-----------------------------------------------------------------------

///----------------------------------------------------------------------
///<summary>
///ラジアンへ変換する
///</summary>
///<param="degree">変換する角度の値</param>
///<returns>ラジアンへと変換した結果を返却</returns>
///----------------------------------------------------------------------
ASDX_INLINE
float ToRadian( float degree )
{ return degree * ( F_PI / 180.0f ); }

///----------------------------------------------------------------------
///<summary>
///度へ変換する
///</summary>
///<param="radian">変換するラジアンの値</param>
///<returns>度へと変換した結果を返却</returns>
///----------------------------------------------------------------------
ASDX_INLINE
float ToDegree( float radian )
{ return radian * ( 180.0f / F_PI ); }

///----------------------------------------------------------------------
///<summary>
///ゼロに近いか確認する
///</summary>
///<param="value">確認したい数値</param>
///<returns>
///ゼロに近ければtrueを返却，そうでなければfalseを返却
///</returns>
///----------------------------------------------------------------------
ASDX_INLINE
bool IsNearZero( float value )
{ return ( -F_EPSILON <= value ) && ( value <= F_EPSILON ); }

///----------------------------------------------------------------------
///<summary>
///ゼロに近いか確認する
///</summary>
///<param="value">確認したい数値</param>
///<returns>
///ゼロに近ければtrueを返却，そうでなければfalseを返却
///</returns>
///----------------------------------------------------------------------
ASDX_INLINE
bool IsNearZero( double value )
{ return ( -D_EPSILON <= value ) && ( value <= D_EPSILON ); }

///----------------------------------------------------------------------
///<summary>
///</summary>
///<param="value1"></param>
///<param="value2"></param>
///<returns></returns>
///----------------------------------------------------------------------
ASDX_INLINE
bool IsNearlyEqual( float value1, float value2 )
{ return ( ( value1 - F_EPSILON ) <= value2 ) && ( value2 <= ( value1 + F_EPSILON ) ); }

///----------------------------------------------------------------------
///<summary>
///</summary>
///<param="value1"></param>
///<param="value2"></param>
///<returns></returns>
///----------------------------------------------------------------------
ASDX_INLINE
bool IsNearlyEqual( double value1, double value2 )
{ return ( ( value1 - D_EPSILON ) <= value2 ) && ( value2 <= ( value1 + D_EPSILON ) ); }

///----------------------------------------------------------------------
///<summary>
///最大値を返却する
///</summary>
///<param="a">比較数値a</param>
///<param="b">比較数値b</param>
///<returns>aとbを比較し、数値の大きいものを返却</returns>
///----------------------------------------------------------------------
ASDX_TEMP_INLINE
type Max( type a, type b )
{ return ( a > b ) ? a : b; }

///----------------------------------------------------------------------
///<summary>
///最小値を返却する
///</summary>
///<param="a">比較数値a</param>
///<param="b">比較数値b</param>
///<returns>aとbを比較し、数値の小さいものを返却</returns>
///----------------------------------------------------------------------
ASDX_TEMP_INLINE
type Min( type a, type b )
{ return ( a < b ) ? a : b; }

///----------------------------------------------------------------------
///<summary>
///数値を指定範囲内にクランプする
///</summary>
///<param="value">クランプしたい数値</param>
///<param="mini">最小値</param>
///<param="maxi">最大値</param>
///<returns>
///最小値から最大値の範囲内にクランプした結果を返却
///</returns>
///----------------------------------------------------------------------
ASDX_TEMP_INLINE
type Clamp( type value, type mini, type maxi )
{ return Max( mini, Min( maxi, value ) ); }

///----------------------------------------------------------------------
///<summary>
///数値を0から1の範囲内にクランプする
///</summary>
///<param="value">クランプしたい数値</param>
///<returns>0から1の範囲内にクランプした結果を返却する</returns>
///----------------------------------------------------------------------
ASDX_TEMP_INLINE
type Saturate( type value )
{ return Clamp( value, static_cast< type >( 0 ), static_cast< type >( 1 ) ); }

///----------------------------------------------------------------------
///<summary>
///符号を取得する
///</summary>
///<param="value">符号を確認したい数値</param>
///<returns>
///値がマイナスであれば-1を返却.
///値がプラスまたはゼロである場合は1を返却する.
///</returns>
///----------------------------------------------------------------------
ASDX_TEMP_INLINE
type Sign( type value )
{ return ( value < 0 ) ? -static_cast< type >( 1 ) : static_cast< type >( 1 ); }

///----------------------------------------------------------------------
///<summary>
///階乗計算を行う
///</summary>
///<param="number">階乗計算する数値n</param>
///<returns>nの階乗(n!)を計算した結果を返却する</returns>
///----------------------------------------------------------------------
ASDX_TEMP_INLINE
type Fact( int number )
{
    int result = 1;
    for( int i = 1; i <= number; ++i )
    { result *= i; }
    return static_cast< type >( result );
}

///----------------------------------------------------------------------
///<summary>
///2重階乗を行う
///</summary>
///<param="number">2重階乗計算する数値n</param>
///<returns>nの2重階乗(n!!)を計算した結果を返却する</returns>
///----------------------------------------------------------------------
ASDX_TEMP_INLINE
type DoubleFact( int number )
{
    int result = 1;
    int start = ( number % 2 == 0 ) ?  2 : 1;
    for( int i = start; i <= number; i += 2 )
    { result *= i; }
    return static_cast< type >( result );
}

/////////////////////////////////////////////////////////////////////////
// FLOAT2 structure
///////////////////////////////////////////////////////////////////////// 
typedef struct FLOAT2
{
    //-------------------------------------------------------------------
    // list of friend classes
    //-------------------------------------------------------------------
    /* NOTHING */

public:
    //-------------------------------------------------------------------
    // public variables
    //-------------------------------------------------------------------
    float x;
    float y;

    //-------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------

    // constructors
    FLOAT2();
    FLOAT2( const float* );
    FLOAT2( float x, float y );
    FLOAT2( const DirectX::XMFLOAT2& );

    // casting operators
    operator float* ();
    operator const float* () const;

    // assignment operators
    FLOAT2&         operator += ( const FLOAT2& );
    FLOAT2&         operator -= ( const FLOAT2& );
    FLOAT2&         operator *= ( float );
    FLOAT2&         operator /= ( float );
    FLOAT2&         operator =  ( const FLOAT2& );

    // unary operators
    FLOAT2          operator +  () const;
    FLOAT2          operator -  () const;

    // binary operators
    FLOAT2          operator +  ( FLOAT2 ) const;
    FLOAT2          operator -  ( FLOAT2 ) const;
    FLOAT2          operator *  ( float ) const;
    FLOAT2          operator /  ( float ) const;
    bool            operator == ( const FLOAT2& ) const;
    bool            operator != ( const FLOAT2& ) const;
    friend FLOAT2   operator*   ( float, const FLOAT2& );

    FLOAT2&         operator =  ( const DirectX::XMFLOAT2& );

    // Length and Normalize Methods
    float   Length() const;
    float   LengthSquared() const;
    FLOAT2& Normalize();

protected:
    //-------------------------------------------------------------------
    // protected variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // protected methods
    //-------------------------------------------------------------------
    /* NOTHING */

private:
    //-------------------------------------------------------------------
    // private variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------
    /* NOTHING */
} FLOAT2;

/////////////////////////////////////////////////////////////////////////
// FLOAT3 structure
/////////////////////////////////////////////////////////////////////////
typedef struct FLOAT3
{
    //-------------------------------------------------------------------
    // list of friend classes
    //-------------------------------------------------------------------
    /* NOTHING */

public:
    //-------------------------------------------------------------------
    // public variables
    //-------------------------------------------------------------------
    float x;
    float y;
    float z;

    //-------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------
    
    // constructors
    FLOAT3();
    FLOAT3( const float * );
    FLOAT3( FLOAT2& vec, float z );
    FLOAT3( float x, float y, float z );
    FLOAT3( const DirectX::XMFLOAT3& );

    // casting operators
    operator float* ();
    operator const float* () const;

    // assignment operators
    FLOAT3&         operator += ( const FLOAT3& );
    FLOAT3&         operator -= ( const FLOAT3& );
    FLOAT3&         operator *= ( float );
    FLOAT3&         operator /= ( float );
    FLOAT3&         operator =  ( const FLOAT3& );

    // unary operators
    FLOAT3          operator +  () const;
    FLOAT3          operator -  () const;

    // binary operators
    FLOAT3          operator +  ( FLOAT3 ) const;
    FLOAT3          operator -  ( FLOAT3 ) const;
    FLOAT3          operator *  ( float ) const;
    FLOAT3          operator /  ( float ) const;
    bool            operator == ( const FLOAT3& ) const;
    bool            operator != ( const FLOAT3& ) const;
    friend FLOAT3   operator *  ( float, const FLOAT3& );

    FLOAT3&         operator =  ( const DirectX::XMFLOAT3& );

    // Length and Normalize methods
    float   Length() const;
    float   LengthSquared() const;
    FLOAT3& Normalize();

protected:
    //-------------------------------------------------------------------
    // protected variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // protected methods
    //-------------------------------------------------------------------
    /* NOTHING */

private:
    //-------------------------------------------------------------------
    // private variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------
    /* NOTHING */
} FLOAT3;

/////////////////////////////////////////////////////////////////////////
// FLOAT3 structure
/////////////////////////////////////////////////////////////////////////
typedef struct FLOAT4
{
    //-------------------------------------------------------------------
    // list of friend classes
    //-------------------------------------------------------------------
    /* NOTHING */

public:
    //-------------------------------------------------------------------
    // public variables
    //-------------------------------------------------------------------
    float x;
    float y;
    float z;
    float w;

    //-------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------
    
    // costructors
    FLOAT4();
    FLOAT4( const float* );
    FLOAT4( FLOAT2& vec, float z, float w );
    FLOAT4( FLOAT3& vec, float w );
    FLOAT4( float x, float y, float z, float w );
    FLOAT4( const DirectX::XMFLOAT4& );

    // casting operators
    operator float* ();
    operator const float* () const;

    // assignment operators
    FLOAT4&         operator += ( const FLOAT4& );
    FLOAT4&         operator -= ( const FLOAT4& );
    FLOAT4&         operator *= ( float );
    FLOAT4&         operator /= ( float );
    FLOAT4&         operator =  ( const FLOAT4& );

    // unary operators
    FLOAT4          operator +  () const;
    FLOAT4          operator -  () const;

    // binary operators
    FLOAT4          operator +  ( FLOAT4 ) const;
    FLOAT4          operator -  ( FLOAT4 ) const;
    FLOAT4          operator *  ( float ) const;
    FLOAT4          operator /  ( float ) const;
    bool            operator == ( const FLOAT4& ) const;
    bool            operator != ( const FLOAT4& ) const;
    friend FLOAT4   operator *  ( float, const FLOAT4& );

    FLOAT4&         operator =  ( const DirectX::XMFLOAT4& );

    // Length and Normalize methods
    float   Length() const;
    float   LengthSquared() const;
    FLOAT4& Normalize();

protected:
    //-------------------------------------------------------------------
    // protected variables
    //-------------------------------------------------------------------
    /* NOTHING */
    
    //-------------------------------------------------------------------
    // protected methods
    //-------------------------------------------------------------------
    /* NOTHING */

private:
    //-------------------------------------------------------------------
    // private variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------
    /* NOTHING */
} FLOAT4;

/////////////////////////////////////////////////////////////////////////
// MATRIX structure
/////////////////////////////////////////////////////////////////////////
typedef struct MATRIX 
{
    //-------------------------------------------------------------------
    // list of friend classes
    //-------------------------------------------------------------------
    /* NOTHING */

public:
    //-------------------------------------------------------------------
    // public variables
    //-------------------------------------------------------------------
    union
    {
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };

    //-------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------
    
    // constructors
    MATRIX();
    MATRIX( const float* );
    MATRIX( 
        float _11, float _12, float _13, float _14,
        float _21, float _22, float _23, float _24,
        float _31, float _32, float _33, float _34,
        float _41, float _42, float _43, float _44 );
    MATRIX( const DirectX::XMFLOAT4X4& );

    // access grants
    float& operator () ( unsigned int row, unsigned int col );
    float  operator () ( unsigned int row, unsigned int col ) const;

    // casting operators
    operator float* ();
    operator const float* () const;

    // assignment operators
    MATRIX& operator *= ( const MATRIX& );
    MATRIX& operator += ( const MATRIX& );
    MATRIX& operator -= ( const MATRIX& );
    MATRIX& operator *= ( float );
    MATRIX& operator /= ( float );
    MATRIX& operator =  ( const MATRIX& );

    // unary operators
    MATRIX  operator + () const;
    MATRIX  operator - () const;

    // binary operators
    MATRIX  operator * ( MATRIX ) const;
    MATRIX  operator + ( MATRIX ) const;
    MATRIX  operator - ( MATRIX ) const;
    MATRIX  operator * ( float ) const;
    MATRIX  operator / ( float ) const;
    bool    operator == ( const MATRIX& ) const;
    bool    operator != ( const MATRIX& ) const;
    friend MATRIX operator * ( float, const MATRIX& );

    MATRIX& operator =  ( const DirectX::XMFLOAT4X4& );

    // Determinant and Identity methods
    float   Determinant() const;
    void    Identity();

protected:
    //-------------------------------------------------------------------
    // protected variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // protected methods
    //-------------------------------------------------------------------
    /* NOTHING */

private:
    //-------------------------------------------------------------------
    // private variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------
    /* NOTHING */

} MATRIX;

/////////////////////////////////////////////////////////////////////////
// QUATERNION structure
/////////////////////////////////////////////////////////////////////////
typedef struct QUATERNION
{
    //-------------------------------------------------------------------
    // list of friend classes
    //-------------------------------------------------------------------
    /* NOTHING */

public:
    //-------------------------------------------------------------------
    // public variables
    //-------------------------------------------------------------------
    float x;
    float y;
    float z;
    float w;

    //-------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------

    // constructors
    QUATERNION();
    QUATERNION( const float* );
    QUATERNION( float x, float y, float z, float w );

    // casting operators
    operator float* ();
    operator const float* () const;

    // assignment operators
    QUATERNION& operator += ( const QUATERNION& );
    QUATERNION& operator -= ( const QUATERNION& );
    QUATERNION& operator *= ( const QUATERNION& );
    QUATERNION& operator *= ( float );
    QUATERNION& operator /= ( float );

    // unary operators
    QUATERNION  operator + () const;
    QUATERNION  operator - () const;

    // binary operators
    QUATERNION  operator * ( QUATERNION ) const;
    QUATERNION  operator + ( QUATERNION ) const;
    QUATERNION  operator - ( QUATERNION ) const;
    QUATERNION  operator * ( float ) const;
    QUATERNION  operator / ( float ) const;
    bool operator == ( const QUATERNION& ) const;
    bool operator != ( const QUATERNION& ) const;
    friend QUATERNION operator * ( float, const QUATERNION& );

    // Length and Normalize mathods
    float      Length() const;
    float      LengthSquared() const;
    void       Conjugate();
    QUATERNION& Normalize();

} QUATERNION;

/////////////////////////////////////////////////////////////////////////
// PLANE structure
/////////////////////////////////////////////////////////////////////////
typedef struct PLANE
{
    //-------------------------------------------------------------------
    // list of friend classes
    //-------------------------------------------------------------------
    /* NOTHING */

public:
    //-------------------------------------------------------------------
    // public variables
    //-------------------------------------------------------------------
    float a;
    float b;
    float c;
    float d;

    //-------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------
    
    // constructors
    PLANE();
    PLANE( const float* );
    PLANE( float a, float b, float c, float d );

    // casting operators
    operator float* ();
    operator const float* () const;

    // assignment operators
    PLANE& operator *= ( float );
    PLANE& operator /= ( float );

    // unary operators
    PLANE  operator + () const;
    PLANE  operator - () const;

    // binary operators
    PLANE  operator * ( float ) const;
    PLANE  operator / ( float ) const;
    bool   operator == ( const PLANE& ) const;
    bool   operator != ( const PLANE& ) const;
    friend PLANE operator * ( float, PLANE& );

    // Normalize method
    PLANE& Normalize();
    PLANE& FromPointNormal( const FLOAT3 &point, const FLOAT3 &normal );
    PLANE& FromPoints( const FLOAT3 &p0, const FLOAT3 &p1, const FLOAT3 &p2 );
    bool   Inside( const FLOAT3 &point ) const;
    bool   Inside( const FLOAT3 &poont, const float radius ) const;

protected:
    //-------------------------------------------------------------------
    // protected variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // protected methods
    //-------------------------------------------------------------------
    /* NOTHING */

private:
    //-------------------------------------------------------------------
    // private variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------
    /* NOTHING */
} PLANE;

/////////////////////////////////////////////////////////////////////////
// BOUNDINGBOX structure
/////////////////////////////////////////////////////////////////////////
typedef struct BOUNDINGBOX
{
    //-------------------------------------------------------------------
    // list of friend classes
    //-------------------------------------------------------------------
    /* NOTHING */

public:
    //-------------------------------------------------------------------
    // public variables
    //-------------------------------------------------------------------
    FLOAT3 minimum;
    FLOAT3 maximum;
    
    //-------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------
    BOUNDINGBOX();
    BOUNDINGBOX( FLOAT3 minmum, FLOAT3 maximum );
    bool operator == ( BOUNDINGBOX& ) const;
    bool operator != ( BOUNDINGBOX& ) const;
    void Merge( FLOAT3 value );
    void Merge( int size, FLOAT3 value[] );

protected:
    //-------------------------------------------------------------------
    // protected variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // protected methods
    //-------------------------------------------------------------------
    /* NOTHING */

private:
    //-------------------------------------------------------------------
    // private variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------
    /* NOTHING */
    
} BOUNDINGBOX;

/////////////////////////////////////////////////////////////////////////
// BOUNDINGSPHERE strcture
/////////////////////////////////////////////////////////////////////////
typedef struct BOUNDINGSPHERE
{
    //-------------------------------------------------------------------
    // list of friend classes
    //-------------------------------------------------------------------
    /* NOTHING */

public:
    //-------------------------------------------------------------------
    // public variables
    //-------------------------------------------------------------------
    FLOAT3 position;
    float   radius;

    //-------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------
    BOUNDINGSPHERE();
    BOUNDINGSPHERE( FLOAT3 position, float radius );
    BOUNDINGSPHERE( BOUNDINGBOX box );
    bool operator == ( BOUNDINGSPHERE& ) const;
    bool operator != ( BOUNDINGSPHERE& ) const;
    void Merge( FLOAT3 value );
    void Merge( int size, FLOAT3* value );

protected:
    //-------------------------------------------------------------------
    // protected variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // protected methods
    //-------------------------------------------------------------------
    /* NOTHING */

private:
    //-------------------------------------------------------------------
    // private variables
    //-------------------------------------------------------------------
    /* NOTHING */

    //-------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------
    /* NOTHING */

} BOUNDINGSPHERE;

/////////////////////////////////////////////////////////////////////////
// Vector Methods
/////////////////////////////////////////////////////////////////////////

// Abs
FLOAT2  Abs( FLOAT2 v );
FLOAT3  Abs( FLOAT3 v );
FLOAT4  Abs( FLOAT4 v );
void    Abs( FLOAT2& v, FLOAT2& result );
void    Abs( FLOAT3& v, FLOAT3& result );
void    Abs( FLOAT4& v, FLOAT4& result );
// Clamp
FLOAT2  Clamp( FLOAT2 v, FLOAT2 mini, FLOAT2 maxi );
FLOAT3  Clamp( FLOAT3 v, FLOAT3 mini, FLOAT3 maxi );
FLOAT4  Clamp( FLOAT4 v, FLOAT4 mini, FLOAT4 maxi );
void    Clamp( FLOAT2& v, FLOAT2& mini, FLOAT2& maxi, FLOAT2& result );
void    Clamp( FLOAT3& v, FLOAT3& mini, FLOAT3& maxi, FLOAT3& result );
void    Clamp( FLOAT4& v, FLOAT4& mini, FLOAT4& maxi, FLOAT4& result );
// Distance
float   Distance( FLOAT2 v1, FLOAT2 v2 );
float   Distance( FLOAT3 v1, FLOAT3 v2 );
float   Distance( FLOAT4 v1, FLOAT4 v2 );
void    Distance( FLOAT2& v1, FLOAT2& v2, float& result );
void    Distance( FLOAT3& v1, FLOAT3& v2, float& result );
void    Distance( FLOAT4& v1, FLOAT4& v2, float& result );
// DistanceSquared
float   DistanceSquared( FLOAT2 v1, FLOAT2 v2 );
float   DistanceSquared( FLOAT3 v1, FLOAT3 v2 );
float   DistanceSquared( FLOAT4 v1, FLOAT4 v2 );
void    DistanceSquared( FLOAT2& v1, FLOAT2& v2, float& result );
void    DistanceSquared( FLOAT3& v1, FLOAT3& v2, float& reuslt );
void    DistanceSquared( FLOAT4& v1, FLOAT4& v2, float& result );
// Dot
float   Dot( FLOAT2 v1, FLOAT2 v2 );
float   Dot( FLOAT3 v1, FLOAT3 v2 );
float   Dot( FLOAT4 v1, FLOAT4 v2 );
void    Dot( FLOAT2& v1, FLOAT2& v2, float& result );
void    Dot( FLOAT3& v1, FLOAT3& v2, float& result );
void    Dot( FLOAT4& v1, FLOAT4& v2, float& result );
// Cross
FLOAT3  Cross( FLOAT3 v1, FLOAT3 v2 );
void    Cross( FLOAT3& v1, FLOAT3& v2, FLOAT3& result );
// Normalize
FLOAT2  Normalize( FLOAT2 v );
FLOAT3  Normalize( FLOAT3 v );
FLOAT4  Normalize( FLOAT4 v );
void    Normalize( FLOAT2& v, FLOAT2& result );
void    Normalize( FLOAT3& v, FLOAT2& result );
void    Normalize( FLOAT4& v, FLOAT4& result );
// ComputeNormal
FLOAT3  ComputeNormal( FLOAT3 p1, FLOAT3 p2, FLOAT3 p3 );
void    ComputeNormal( FLOAT3& p1, FLOAT3& p2, FLOAT3& p3, FLOAT3& result );
// ComputeQuadNormal
FLOAT3  ComputeQuadNormal( FLOAT3 p1, FLOAT3 p2, FLOAT3 p3, FLOAT3 p4 );
void    ComputeQuadNormal( FLOAT3& p1, FLOAT3& p2, FLOAT3& p3, FLOAT3& p4, FLOAT3& result );
// ComputeTangent
FLOAT3  ComputeTangent( FLOAT3 p1, FLOAT3 p2, FLOAT3 p3, FLOAT2 t1, FLOAT2 t2, FLOAT2 t3 );
void    ComputeTangent( FLOAT3& p1, FLOAT3& p2, FLOAT3& p3, FLOAT2& t1, FLOAT2& t2, FLOAT2& t3, FLOAT3& result );
// CompuateBinormal
FLOAT3 ComputeBinormal( FLOAT3 normal, FLOAT3 tangent );
void    ComputeBinormal( FLOAT3& normal, FLOAT3& tangent, FLOAT3& result );
// ComputeCrossingAngle
float   ComputeCrossingAngle( FLOAT2 v1, FLOAT2 v2 );
float   ComputeCrossingAngle( FLOAT3 v1, FLOAT3 v2 );
float   ComputeCrossingAngle( FLOAT4 v1, FLOAT4 v2 );
void    ComputeCrossingAngle( FLOAT2& v1, FLOAT2& v2, float& result );
void    ComputeCrossingAngle( FLOAT3& v1, FLOAT3& v2, float& result );
void    ComputeCrossingAngle( FLOAT4& v1, FLOAT4& v2, float& result );
// Min
FLOAT2  Min( FLOAT2 v1, FLOAT2 v2 );
FLOAT3  Min( FLOAT3 v1, FLOAT3 v2 );
FLOAT4  Min( FLOAT4 v1, FLOAT4 v2 );
void    Min( FLOAT2& v1, FLOAT2& v2, FLOAT2& result );
void    Min( FLOAT3& v1, FLOAT3& v2, FLOAT3& result );
void    Min( FLOAT4& v1, FLOAT4& v2, FLOAT4& result );
// Max
FLOAT2  Max( FLOAT2 v1, FLOAT2 v2 );
FLOAT3  Max( FLOAT3 v1, FLOAT3 v2 );
FLOAT4  Max( FLOAT4 v1, FLOAT4 v2 );
void    Max( FLOAT2& v1, FLOAT2& v2, FLOAT2& result );
void    Max( FLOAT3& v1, FLOAT3& v2, FLOAT3& result );
void    Max( FLOAT4& v1, FLOAT4& v2, FLOAT4& result );
// Reflect
FLOAT2  Reflect( FLOAT2 i, FLOAT2 n );
FLOAT3  Reflect( FLOAT3 i, FLOAT3 n );
void    Reflect( FLOAT2& i, FLOAT2& n, FLOAT2& result );
void    Reflect( FLOAT3& i, FLOAT3& n, FLOAT3& result );
// Refract
FLOAT3  Refract( FLOAT3 i, FLOAT3 n, float eta );
void    Refract( FLOAT3& i, FLOAT3& n, float eta, FLOAT3& result );
// Barycentric
FLOAT2  Barycentric( FLOAT2 v1, FLOAT2 v2, FLOAT2 v3, float f, float g );
FLOAT3  Barycentric( FLOAT3 v1, FLOAT3 v2, FLOAT3 v3, float f, float g );
FLOAT4  Barycentric( FLOAT4 v1, FLOAT4 v2, FLOAT4 v3, float f, float g );
void    Barycentric( FLOAT2 &v1, FLOAT2 &v2, FLOAT2 &v3, float f, float g, FLOAT2 &result );
void    Barycentric( FLOAT3 &v1, FLOAT3 &v2, FLOAT3 &v3, float f, float g, FLOAT3 &result );
void    Barycentric( FLOAT4 &v1, FLOAT4 &v2, FLOAT4 &v3, float f, float g, FLOAT4 &result );
// Lerp
FLOAT2  Lerp( FLOAT2 v1, FLOAT2 v2, float t );
FLOAT3  Lerp( FLOAT3 v1, FLOAT3 v2, float t );
FLOAT4  Lerp( FLOAT4 v1, FLOAT4 v2, float t );
void    Lerp( FLOAT2 &v1, FLOAT2 &v2, float t, FLOAT2 &result );
void    Lerp( FLOAT3 &v1, FLOAT3 &v2, float t, FLOAT3 &reuslt );
void    Lerp( FLOAT4 &v1, FLOAT4 &v2, float r, FLOAT4 &result );
// SmoothStep
FLOAT2  SmoothStep( FLOAT2 v1, FLOAT2 v2, float t );
FLOAT3  SmoothStep( FLOAT3 v1, FLOAT3 v2, float t );
FLOAT4  SmoothStep( FLOAT4 v1, FLOAT4 v2, float t );
void    SmoothStep( FLOAT2 &v1, FLOAT2 &v2, float t, FLOAT2 &result );
void    SmoothStep( FLOAT3 &v1, FLOAT3 &v2, float t, FLOAT3 &result );
void    SmoothStep( FLOAT4 &v1, FLOAT4 &v2, float t, FLOAT4 &result );
// Transform
FLOAT2  Transform( FLOAT2 position, MATRIX matrix );
FLOAT3  Transform( FLOAT3 position, MATRIX matrix );
FLOAT4  Transform( FLOAT4 position, MATRIX matrix );
void    Transform( FLOAT2 &position, MATRIX &matrix, FLOAT2 &result );
void    Transform( FLOAT3 &position, MATRIX &matrix, FLOAT3 &result );
void    Transform( FLOAT4 &position, MATRIX &matrix, FLOAT4 &result );
// TransformNormal
FLOAT2  TransformNormal( FLOAT2 normal, MATRIX matrix );
FLOAT3  TransformNormal( FLOAT3 normal, MATRIX matrix );
void    TransformNormal( FLOAT2 &normal, MATRIX &matrix, FLOAT2 &result );
void    TransformNormal( FLOAT3 &normal, MATRIX &matrix, FLOAT3 &result );
// TransformCoordinate
FLOAT2 TransformCoordinate( FLOAT2 coords, MATRIX matrix );
FLOAT3 TransformCoordiante( FLOAT3 coords, MATRIX matrix );
void    TransformCoordinate( FLOAT2 &coords, MATRIX &matrix, FLOAT2 &result );
void    TransformCoordinate( FLOAT3 &coords, MATRIX &matrix, FLOAT3& result );


/////////////////////////////////////////////////////////////////////////
// MATRIX Methods
/////////////////////////////////////////////////////////////////////////

// Identity
MATRIX& Identity( MATRIX &value );
// IsIdentity
bool    IsIdentity( MATRIX &value );
// Transpose
MATRIX  Transpose( MATRIX value );
void    Transpose( MATRIX &value, MATRIX &result );
// Multiply
MATRIX  Multiply( MATRIX value1, MATRIX value2 );
MATRIX  Multiply( MATRIX value, float scaleFactor );
void    Multiply( MATRIX &value1, MATRIX &value2, MATRIX &result );
void    Multiply( MATRIX &value, float scaleFactor, MATRIX &result );
// MultiplyTranspose
MATRIX  MultiplyTranspose( MATRIX value1, MATRIX value2 );
void    MultiplyTranspose( MATRIX &value1, MATRIX &value2, MATRIX &result );
// Invert
MATRIX  Invert( MATRIX value );
void    Invert( MATRIX &value, MATRIX &result );
// CreateFromQuaternion
MATRIX  CreateFromQuaternion( QUATERNION qua );
void    CreateFromQuaternion( QUATERNION &qua, MATRIX &result );
// CreateScale
MATRIX  CreateScale( float scale );
MATRIX  CreateScale( float xScale, float yScale, float zScale );
MATRIX  CreateScale( FLOAT3 scales );
void    CreateScale( float scale, MATRIX &result );
void    CreateScale( float xScale, float yScale, float zScale, MATRIX &result );
void    CreateScale( FLOAT3 &scales, MATRIX &result );
// CreateTranslation
MATRIX  CreateTranslation( float xPos, float yPos, float zPos );
MATRIX  CreateTranslation( FLOAT3 position );
void    CreateTranslation( float xPos, float yPos, float zPos, MATRIX &result );
void    CreateTranslation( FLOAT3 &position, MATRIX &result );
// CreateRotationX
MATRIX  CreateRotationX( float radian );
void    CreateRotationX( float radian, MATRIX &result );
// CreateRotationY
MATRIX  CreateRotationY( float radian );
void    CreateRotationY( float radian, MATRIX &result );
// CreateRotationZ
MATRIX  CreateRotationZ( float radian );
void    CreateRotationZ( float radian, MATRIX &result );
// CreateFromAxisAngle
MATRIX  CreateFromAxisAngle( FLOAT3 axis, float radian );
void    CreateFromAxisAngle( FLOAT3 &axis, float radian, MATRIX &result );
// CreateLookAt
MATRIX  CreateLookAt( FLOAT3 cameraPosition, FLOAT3 cameraTarget, FLOAT3 cameraUpVector );
void    CreateLookAt( FLOAT3 &cameraPosition, FLOAT3 &cameraTarget, FLOAT3 &cameraUpVector, MATRIX &result );
// CreatePespective
MATRIX  CreatePerspective( float width, float height, float nearPlaneDistance, float farPlaneDistance );
void    CreatePerspective( float width, float height, float nearPlaneDistance, float farPlaneDistance, MATRIX &result );
// CreatePerspectiveFieldOfView
MATRIX  CreatePerspectiveFieldOfView( float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance );
void    CreatePerspectiveFieldOfView( float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance, MATRIX &result );
// CreatePerspectiveOffcenter
MATRIX  CreatePerspectiveOffcenter( float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance );
void    CreatePerspectiveOffcenter( float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance, MATRIX &result );
// CreateOrthographic
MATRIX  CreateOrthographic( float width, float height, float nearPlaneDistance, float farPlaneDistance );
void    CreateOrthographic( float width, float height, float nearPlaneDistance, float farPlaneDistance, MATRIX &result );
// CreateOrthographicOffcenter
MATRIX  CreateOrthographicOffcenter( float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance );
void    CreateOrthographicOffcenter( float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance, MATRIX &result );
// CreateShadow
MATRIX  CreateShadow( FLOAT4 lightDirection, PLANE plane );
void    CreateShadow( FLOAT4 &lightDirection, PLANE &plane, MATRIX &result );
// CreateReflection
MATRIX  CreateReflection( PLANE plane );
void    CreateReflection( PLANE &plane, MATRIX &result );
// Lerp
MATRIX  Lerp( MATRIX v1, MATRIX v2, float t );
void    Lerp( MATRIX &v1, MATRIX &v2, float t, MATRIX &result );


/////////////////////////////////////////////////////////////////////////
// QUATERNION Functions
/////////////////////////////////////////////////////////////////////////

// Identity
QUATERNION& Identity( QUATERNION &value );
// IsIdentity
bool        IsIdentity( QUATERNION &value );
// Multiply
QUATERNION  Multiply( QUATERNION q1, QUATERNION q2 );
void        Multiply( QUATERNION &q1, QUATERNION &q2, QUATERNION &result );
// Dot
float       Dot( QUATERNION q1, QUATERNION q2 );
void        Dot( QUATERNION &q1, QUATERNION &q2, float &result );
// Conjugate
QUATERNION  Conjugate( QUATERNION qua );
void        Conjugate( QUATERNION &qua, QUATERNION &result );
// Normalize
QUATERNION  Normalize( QUATERNION qua );
void        Normalize( QUATERNION &qua, QUATERNION &result );
// Inverse
QUATERNION  Inverse( QUATERNION qua );
void        Inverse( QUATERNION &qua, QUATERNION &result );
// CreateFromYawPitchRoll
QUATERNION  CreateFromYawPitchRoll( float yaw, float pitch, float roll );
void        CreateFromYawPitchRoll( float yaw, float pitch, float roll, QUATERNION &result );
// Slerp
QUATERNION  Slerp( QUATERNION q1, QUATERNION q2, float t );
void        Slerp( QUATERNION &q1, QUATERNION &q2, float t, QUATERNION &result );
// Squad
QUATERNION  Squad( QUATERNION qua, QUATERNION a, QUATERNION b, QUATERNION c, float t );
void        Squad( QUATERNION &qua, QUATERNION &a, QUATERNION &b, QUATERNION &c, float t, QUATERNION &result );


/////////////////////////////////////////////////////////////////////////
// PLANE Functions
/////////////////////////////////////////////////////////////////////////

// Dot
float Dot( PLANE p, FLOAT4 v );
void  Dot( PLANE &p, FLOAT4 &v, float &result );
// DotCoordinate
float DotCoordinate( PLANE p, FLOAT3 v );
void  DotCoordinate( PLANE &p, FLOAT3 &v, float &result );
// DotNormal
float DotNormal( PLANE p, FLOAT3 v );
void  DotNormal( PLANE &p, FLOAT3 &v, float &result );
// Normalize
PLANE Normalize( PLANE p );
void  Normalize( PLANE &p, PLANE &result );

} // namespace asdx

//-----------------------------------------------------------------------
// Inline Files
//-----------------------------------------------------------------------
#include "asdxMath.inl"


#endif//__PHANTOM_MATH_H__