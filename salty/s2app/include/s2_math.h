//---------------------------------------------------------------------------------------
// File : s2_math.h
// Desc : Math Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------

#ifndef __S2_MATH_H__
#define __S2_MATH_H__


//---------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------
#include <cmath>
#include <cfloat>
#include <cassert>


//---------------------------------------------------------------------------------------
// Constant Values
//---------------------------------------------------------------------------------------
static const double PI  = 3.14159265358979323846;
//static const double INF = 1e20;
//static const double EPS = 1e-6;


//---------------------------------------------------------------------------------------
// Forward Declarations.
//---------------------------------------------------------------------------------------
struct MTX44;


template< typename T> inline T Max( T a, T b )
{ return ( a > b ) ? a : b; }

template< typename T> inline T Min( T a, T b )
{ return ( a < b ) ? a : b; }

template< typename T> inline T Clamp( T value, T a, T b )
{ return ( value > b ) ? b : ( value < a ) ? a : value; }


//---------------------------------------------------------------------------------------
//! @brief      XOR Shift法による乱数発生器.
//---------------------------------------------------------------------------------------
class XORSHIFT
{
    //===================================================================================
    // list of friend classes and methods.
    //===================================================================================
    /* NOTHING */

public:
    //===================================================================================
    // public variables.
    //===================================================================================
    /* NOTHING */

    //===================================================================================
    // public methods.
    //===================================================================================
    XORSHIFT( const unsigned int seed )
    {
        x = 123456789;
        y = 362436069;
        z = 521288629;
        w = ( seed <= 0 ) ? 88675123 : seed;
    }

    inline unsigned int GetAsU32()
    {
        unsigned int t = x ^ ( x << 1 );
        x = y;
        y = z;
        z = w;
        w = ( w ^ ( w >> 19 ) ) ^ ( t ^ ( t >> 8 ) );
        return w;
    }

    inline double GetAsF64()
    { return static_cast<double>( GetAsU32() ) / 0xffffffffui32; }

protected:
    //===================================================================================
    // protected variables.
    //===================================================================================
    /* NOTHING */

    //===================================================================================
    // protected methods.
    //===================================================================================
    /* NOTHING */

private:
    //===================================================================================
    // private variables.
    //===================================================================================
    unsigned int    x;
    unsigned int    y;
    unsigned int    z;
    unsigned int    w;

    //===================================================================================
    // private methods.
    //===================================================================================
    /* NOTHING */
};


//---------------------------------------------------------------------------------------
//! @brief      ３次元ベクトル構造体です
//---------------------------------------------------------------------------------------
struct VEC3
{
    double x;
    double y;
    double z;

    VEC3()
    { /* DO_NOTHING */  }

    VEC3(
        const double x_, 
        const double y_,
        const double z_
    )
    : x( x_ )
    , y( y_ )
    , z( z_ )
    { /* DO_NOTHING */ }

    VEC3( const VEC3& value )
    : x( value.x )
    , y( value.y )
    , z( value.z )
    { /* DO_NOTHING */ }

    inline VEC3 operator + () const
    { return (*this); }

    inline VEC3 operator - () const
    { return VEC3( -x, -y, -z ); }

    inline VEC3 operator + ( const VEC3 &b ) const
    { return VEC3( x + b.x, y + b.y, z + b.z ); }

    inline VEC3 operator - ( const VEC3 &b ) const 
    { return VEC3( x - b.x, y - b.y, z - b.z ); }

    inline VEC3 operator * ( const double b ) const
    { return VEC3( x * b, y * b, z * b ); }

    inline VEC3 operator * ( const VEC3& b ) const
    { return VEC3( x * b.x, y * b.y, z * b.z ); }

    inline VEC3 operator / ( const double b ) const
    { return VEC3( x / b, y / b, z / b ); }

    inline VEC3& operator += ( const VEC3& b )
    {
        x += b.x;
        y += b.y;
        z += b.z;
        return (*this);
    }

    inline VEC3& operator -= ( const VEC3& b )
    {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        return (*this);
    }

    inline const double LengthSq() const 
    { return x * x + y * y + z * z; }

    inline const double Length() const
    { return sqrt( x * x + y * y + z * z ); }

    inline void Normalize()
    {
        register double mag = sqrt( x * x + y * y + z * z );
        assert( mag != 0.0 );
        x /= mag;
        y /= mag;
        z /= mag;
    }

    bool operator == ( const VEC3& value ) const
    {
        return ( x == value.x )
            && ( y == value.y )
            && ( z == value.z );
    }

    bool operator != ( const VEC3& value ) const
    {
        return ( x != value.x )
            || ( y != value.y )
            || ( z != value.z );
    }
};
typedef VEC3 COLOR;

inline VEC3 operator * ( double f, const VEC3 &v )
{ return VEC3( v.x * f, v.y * f, v.z * f ); }

inline VEC3 Normalize( const VEC3 &v ) 
{
    register double mag = sqrt( v.x * v.x + v.y * v.y + v.z * v.z );
    assert( mag != 0.0 );
    return VEC3( v.x / mag, v.y / mag, v.z / mag );
}

inline const double Dot( const VEC3 &v1, const VEC3 &v2 ) 
{ return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

inline const VEC3 Cross( const VEC3 &v1, const VEC3 &v2 )
{
    return VEC3(
        (v1.y * v2.z) - (v1.z * v2.y),
        (v1.z * v2.x) - (v1.x * v2.z),
        (v1.x * v2.y) - (v1.y * v2.x));
}

inline VEC3 ComputeNormal( const VEC3& v0, const VEC3& v1, const VEC3& v2 )
{
    VEC3 e0 = v1 - v0;
    VEC3 e1 = v2 - v0;
    return Cross( e0, e1 );
}

inline VEC3 ComputeQuadNormal( const VEC3& v0, const VEC3& v1, const VEC3& v2, const VEC3& v3 )
{
    VEC3 n1a = ComputeNormal( v0, v1, v2 );
    VEC3 n1b = ComputeNormal( v0, v2, v3 );
    VEC3 n2a = ComputeNormal( v1, v2, v3 );
    VEC3 n2b = ComputeNormal( v1, v3, v2 );
    if ( Dot( n1a, n1b ) > Dot( n2a, n2b ) )
    {
        VEC3 normal = n1a + n1b;
        return Normalize( normal );
    }
    else
    {
        VEC3 normal = n2a + n2b;
        return Normalize( normal );
    }
}


//---------------------------------------------------------------------------------------
//! @brief      レイ構造体です
//---------------------------------------------------------------------------------------
struct RAY
{
    VEC3 org;
    VEC3 dir;

    RAY()
    { /* DO_NOTHING */ }

    RAY( const RAY& value )
    : org( value.org )
    , dir( value.dir )
    { /* DO_NOTHING */ }

    RAY( const VEC3 org_, const VEC3 &dir_) 
    : org(org_)
    , dir(dir_)
    { /* DO_NOTHING */ }
};


inline bool IntersectTriangle
(
    const RAY&  ray,
    const VEC3& v0,
    const VEC3& v1,
    const VEC3& v2,
    double&     distance
)
{
    VEC3 e0 = v1 - v0;
    VEC3 e1 = v2 - v0;

    VEC3 u = Cross( ray.dir, e1 );

    double det = Dot( e0, u );

    if ( det > -DBL_EPSILON && det < DBL_EPSILON )
    {
        return false;
    }

    VEC3 diff = ray.org - v0;
    double beta = Dot( diff, u );
    beta /= det;

    if ( beta < 0.0 || beta > 1.0 )
    {
        return false;
    }

    VEC3 v;
    v = Cross( diff, e0 );

    double gamma = Dot( ray.dir, v );
    gamma /= det;

    if ( gamma < 0.0 || gamma + beta > 1.0 )
    {
        return false;
    }

    double dist = Dot( e1, v );
    dist /= det;

    if ( dist < 0.0 )
    {
        return false;
    }

    distance = dist;

    return true;
}


//---------------------------------------------------------------------------------------
//! @brief      4行4列の行列です.
//---------------------------------------------------------------------------------------
struct MTX44
{
    double _11, _12, _13, _14;
    double _21, _22, _23, _24;
    double _31, _32, _33, _34;
    double _41, _42, _43, _44;

    MTX44()
    {}

    MTX44(
        const double m11, const double m12, const double m13, const double m14,
        const double m21, const double m22, const double m23, const double m24,
        const double m31, const double m32, const double m33, const double m34,
        const double m41, const double m42, const double m43, const double m44 )
    : _11( m11 ), _12( m12 ), _13( m13 ), _14( m14 )
    , _21( m21 ), _22( m22 ), _23( m23 ), _24( m24 )
    , _31( m31 ), _32( m32 ), _33( m33 ), _34( m34 )
    , _41( m41 ), _42( m42 ), _43( m43 ), _44( m44 )
    {
    }

    inline MTX44 operator * ( const MTX44& value )
    {
        return MTX44(
            ( _11 * value._11 ) + ( _12 * value._21 ) + ( _13 * value._31 ) + ( _14 * value._41 ),
            ( _11 * value._12 ) + ( _12 * value._22 ) + ( _13 * value._32 ) + ( _14 * value._42 ),
            ( _11 * value._13 ) + ( _12 * value._23 ) + ( _13 * value._33 ) + ( _14 * value._43 ),
            ( _11 * value._14 ) + ( _12 * value._24 ) + ( _13 * value._34 ) + ( _14 * value._44 ),

            ( _21 * value._11 ) + ( _22 * value._21 ) + ( _23 * value._31 ) + ( _24 * value._41 ),
            ( _21 * value._12 ) + ( _22 * value._22 ) + ( _23 * value._32 ) + ( _24 * value._42 ),
            ( _21 * value._13 ) + ( _22 * value._23 ) + ( _23 * value._33 ) + ( _24 * value._43 ),
            ( _21 * value._14 ) + ( _22 * value._24 ) + ( _23 * value._34 ) + ( _24 * value._44 ),

            ( _31 * value._11 ) + ( _32 * value._21 ) + ( _33 * value._31 ) + ( _34 * value._41 ),
            ( _31 * value._12 ) + ( _32 * value._22 ) + ( _33 * value._32 ) + ( _34 * value._42 ),
            ( _31 * value._13 ) + ( _32 * value._23 ) + ( _33 * value._33 ) + ( _34 * value._43 ),
            ( _31 * value._14 ) + ( _32 * value._24 ) + ( _33 * value._34 ) + ( _34 * value._44 ),

            ( _41 * value._11 ) + ( _42 * value._21 ) + ( _43 * value._31 ) + ( _44 * value._41 ),
            ( _41 * value._12 ) + ( _42 * value._22 ) + ( _43 * value._32 ) + ( _44 * value._42 ),
            ( _41 * value._13 ) + ( _42 * value._23 ) + ( _43 * value._33 ) + ( _44 * value._43 ),
            ( _41 * value._14 ) + ( _42 * value._24 ) + ( _43 * value._34 ) + ( _44 * value._44 )
        );
    }

    inline void Identity()
    {
        _11 = 1.0; _12 = 0.0; _13 = 0.0; _14 = 0.0;
        _21 = 0.0; _22 = 1.0; _23 = 0.0; _24 = 0.0;
        _31 = 0.0; _32 = 0.0; _33 = 1.0; _34 = 0.0;
        _41 = 0.0; _42 = 0.0; _43 = 0.0; _44 = 1.0;
    }

    inline double Det() const
    {
        return (
            _11*_22*_33*_44 + _11*_23*_34*_42 +
            _11*_24*_32*_43 + _12*_21*_34*_43 +
            _12*_23*_31*_44 + _12*_24*_33*_41 +
            _13*_21*_32*_44 + _13*_22*_34*_41 +
            _13*_24*_31*_42 + _14*_21*_33*_42 +
            _14*_22*_31*_43 + _14*_23*_32*_41 -
            _11*_22*_34*_43 - _11*_23*_32*_44 -
            _11*_24*_33*_42 - _12*_21*_33*_44 -
            _12*_23*_34*_41 - _12*_24*_31*_43 -
            _13*_21*_34*_42 - _13*_22*_31*_44 -
            _13*_24*_32*_41 - _14*_21*_32*_43 -
            _14*_22*_33*_41 - _14*_23*_31*_42
        );
    }
};

inline MTX44 LookAt( const VEC3& position, const VEC3& target, const VEC3& upward )
{
    VEC3 zAxis(
        position.x - target.x,
        position.y - target.y,
        position.z - target.z );
    zAxis.Normalize();

    VEC3 xAxis = Cross( zAxis, upward );
    xAxis.Normalize();

    VEC3 yAxis = Cross( zAxis, xAxis );
    yAxis.Normalize();

    return MTX44(
        xAxis.x, yAxis.x, zAxis.x, 0.0f,
        xAxis.y, yAxis.y, zAxis.y, 0.0f,
        xAxis.z, yAxis.z, zAxis.z, 0.0f,
        -Dot( xAxis, position ), -Dot( yAxis, position ), -Dot( zAxis, position ), 1.0f );
}

inline MTX44 PerspectiveFov( const double fov, const double aspectRatio, const double nearClip, const double farClip )
{
    register double diff = nearClip - farClip;
    register double tanFov = tan( fov * 0.5 );
    assert( aspectRatio != 0.0 );
    assert( diff        != 0.0 );
    assert( tanFov      != 0.0 );
    register double yScale  = 1.0 / tanFov;
    register double xScale  = yScale / aspectRatio;
    register double invDiff = 1.0 / diff;

    return MTX44(
        xScale,     0.0f,       0.0f,       0.0f,
        0.0f,       yScale,     0.0f,       0.0f,
        0.0f,       0.0f,       nearClip * invDiff, -1.0f,
        0.0f,       0.0f,       ( nearClip * farClip ) * invDiff,   0.0f  );
}

inline MTX44 Inverse( const MTX44& value )
{
    MTX44 result;
    register double det = value.Det();
    assert( det != 0.0 );
    register double invDet = 1.0 / det;

    result._11 = ( value._22 * value._33 * value._44 ) + ( value._23 * value._34 * value._42 ) + ( value._24 * value._32 * value._43 )
               - ( value._22 * value._34 * value._43 ) - ( value._23 * value._32 * value._44 ) - ( value._24 * value._33 * value._42 );
    result._12 = ( value._12 * value._34 * value._43 ) + ( value._13 * value._32 * value._44 ) + ( value._14 * value._33 * value._42 )
               - ( value._12 * value._33 * value._44 ) - ( value._13 * value._34 * value._42 ) - ( value._14 * value._32 * value._43 );
    result._13 = ( value._12 * value._23 * value._44 ) + ( value._13 * value._24 * value._42 ) + ( value._14 * value._22 * value._43 )
               - ( value._12 * value._24 * value._43 ) - ( value._13 * value._22 * value._44 ) - ( value._14 * value._23 * value._42 );
    result._14 = ( value._12 * value._24 * value._33 ) + ( value._13 * value._22 * value._34 ) + ( value._14 * value._23 * value._32 )
               - ( value._12 * value._23 * value._34 ) - ( value._13 * value._24 * value._32 ) - ( value._14 * value._22 * value._33 );

    result._21 = ( value._21 * value._34 * value._43 ) + ( value._23 * value._31 * value._44 ) + ( value._24 * value._33 * value._41 )
               - ( value._21 * value._33 * value._44 ) - ( value._23 * value._34 * value._41 ) - ( value._24 * value._31 * value._43 );
    result._22 = ( value._11 * value._33 * value._44 ) + ( value._13 * value._34 * value._41 ) + ( value._14 * value._31 * value._43 )
               - ( value._11 * value._34 * value._43 ) - ( value._13 * value._31 * value._44 ) - ( value._14 * value._33 * value._41 );
    result._23 = ( value._11 * value._24 * value._43 ) + ( value._13 * value._21 * value._44 ) + ( value._14 * value._23 * value._41 )
               - ( value._11 * value._23 * value._44 ) - ( value._13 * value._24 * value._41 ) - ( value._14 * value._21 * value._43 );
    result._24 = ( value._11 * value._23 * value._34 ) + ( value._13 * value._24 * value._31 ) + ( value._14 * value._21 * value._33 )
               - ( value._11 * value._24 * value._33 ) - ( value._13 * value._21 * value._34 ) - ( value._14 * value._23 * value._31 );

    result._31 = ( value._21 * value._32 * value._44 ) + ( value._22 * value._34 * value._41 ) + ( value._24 * value._31 * value._42 )
               - ( value._21 * value._34 * value._42 ) - ( value._22 * value._31 * value._44 ) - ( value._24 * value._32 * value._41 );
    result._32 = ( value._11 * value._34 * value._42 ) + ( value._12 * value._31 * value._44 ) + ( value._14 * value._32 * value._41 )
               - ( value._11 * value._32 * value._44 ) - ( value._12 * value._34 * value._41 ) - ( value._14 * value._31 * value._42 );
    result._33 = ( value._11 * value._22 * value._44 ) + ( value._12 * value._24 * value._41 ) + ( value._14 * value._21 * value._42 )
               - ( value._11 * value._24 * value._42 ) - ( value._12 * value._21 * value._44 ) - ( value._14 * value._22 * value._41 );
    result._34 = ( value._11 * value._24 * value._32 ) + ( value._12 * value._21 * value._34 ) + ( value._14 * value._22 * value._31 )
               - ( value._11 * value._22 * value._34 ) - ( value._12 * value._24 * value._31 ) - ( value._14 * value._21 * value._32 );

    result._41 = ( value._21 * value._33 * value._42 ) + ( value._22 * value._31 * value._43 ) + ( value._23 * value._32 * value._41 )
               - ( value._21 * value._32 * value._43 ) - ( value._22 * value._33 * value._41 ) - ( value._23 * value._31 * value._42 );
    result._42 = ( value._11 * value._32 * value._43 ) + ( value._12 * value._33 * value._41 ) + ( value._13 * value._31 * value._42 )
               - ( value._11 * value._33 * value._42 ) - ( value._12 * value._31 * value._43 ) - ( value._13 * value._32 * value._41 );
    result._43 = ( value._11 * value._23 * value._42 ) + ( value._12 * value._21 * value._43 ) + ( value._13 * value._22 * value._41 )
               - ( value._11 * value._22 * value._43 ) - ( value._12 * value._23 * value._41 ) - ( value._13 * value._21 * value._42 );
    result._44 = ( value._11 * value._22 * value._33 ) + ( value._12 * value._23 * value._31 ) + ( value._13 * value._21 * value._32 )
               - ( value._11 * value._23 * value._32 ) - ( value._12 * value._21 * value._33 ) - ( value._13 * value._22 * value._31 );

    result._11 *= invDet;
    result._12 *= invDet;
    result._13 *= invDet;
    result._14 *= invDet;

    result._21 *= invDet;
    result._22 *= invDet;
    result._23 *= invDet;
    result._24 *= invDet;

    result._31 *= invDet;
    result._32 *= invDet;
    result._33 *= invDet;
    result._34 *= invDet;

    result._41 *= invDet;
    result._42 *= invDet;
    result._43 *= invDet;
    result._44 *= invDet;

    return result;
}


inline VEC3 TransformCoord( const VEC3& coord, const MTX44& matrix )
{
    register double X = ( coord.x * matrix._11 ) + ( coord.y * matrix._21 ) + ( coord.z * matrix._31 ) + matrix._41;
    register double Y = ( coord.x * matrix._12 ) + ( coord.y * matrix._22 ) + ( coord.z * matrix._32 ) + matrix._42;
    register double Z = ( coord.x * matrix._13 ) + ( coord.y * matrix._23 ) + ( coord.z * matrix._33 ) + matrix._43;
    register double W = ( coord.x * matrix._14 ) + ( coord.y * matrix._24 ) + ( coord.z * matrix._34 ) + matrix._44;
    assert( W != 0.0 );
    return VEC3( X / W, Y / W, Z / W );
}


#endif//__S2_MATH_H__
