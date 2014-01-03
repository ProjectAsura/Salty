//----------------------------------------------------------------------------------
// File : s3d_box.inl
// Desc : Bounding Box Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

#ifndef __S3D_BOX_INL__
#define __S3D_BOX_INL__

#include <cstdio>

namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// BoundingBox structure
////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox::BoundingBox()
: min( F_MAX, F_MAX, F_MAX )
, max( F_MIN, F_MIN, F_MIN )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox::BoundingBox( const Vector3& value )
: min( value )
, max( value )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox::BoundingBox( const Vector3& mini, const Vector3& maxi )
: min( mini )
, max( maxi )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      コピーコンストラクタです.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox::BoundingBox( const BoundingBox& value )
: min( value.min )
, max( value.max )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      交差判定を行います.
//----------------------------------------------------------------------------------
S3D_INLINE
bool BoundingBox::IsHit( const Ray& ray ) const
{
    const Vector3* v[ 2 ] = { &min, &max };
    f64 intervalMin = F_HIT_MIN;
    f64 intervalMax = F_HIT_MAX;

    s32 idx0, idx1;
    f64 t0, t1;

    // X軸方向.
    idx0 = ray.sign[ 0 ];
    idx1 = 1 - idx0;
    t0 = ( v[ idx0 ]->x - ray.pos.x ) * ray.invDir.x;
    t1 = ( v[ idx1 ]->x - ray.pos.x ) * ray.invDir.x;
    intervalMin = ( t0 > intervalMin ) ? t0 : intervalMin;
    intervalMax = ( t1 < intervalMax ) ? t1 : intervalMax;
    if ( intervalMin > intervalMax )
    { return false; }

    // Y軸方向.
    idx0 = ray.sign[ 1 ];
    idx1 = 1 - idx0;
    t0 = ( v[ idx0 ]->y - ray.pos.y ) * ray.invDir.y;
    t1 = ( v[ idx1 ]->y - ray.pos.y ) * ray.invDir.y;
    intervalMin = ( t0 > intervalMin ) ? t0 : intervalMin;
    intervalMax = ( t1 < intervalMax ) ? t1 : intervalMax;
    if ( intervalMin > intervalMax )
    { return false; }

    // Z軸方向.
    idx0 = ray.sign[ 2 ];
    idx1 = 1 - idx0;
    t0 = ( v[ idx0 ]->z - ray.pos.z ) * ray.invDir.z;
    t1 = ( v[ idx1 ]->z - ray.pos.z ) * ray.invDir.z;
    intervalMin = ( t0 > intervalMin ) ? t0 : intervalMin;
    intervalMax = ( t1 < intervalMax ) ? t1 : intervalMax;
    if ( intervalMin > intervalMax )
    { return false; }

    return true;
}

//----------------------------------------------------------------------------------
//      2つのバウンディングボックスをマージします.
//----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox BoundingBox::Merge( const BoundingBox& a, const BoundingBox& b )
{
    Vector3 mini = Vector3::Min( a.min, b.min );
    Vector3 maxi = Vector3::Max( a.max, b.max );

    return BoundingBox( mini, maxi );
}

//-----------------------------------------------------------------------------------
//      バウンディングボックスと点をマージします.
//-----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox BoundingBox::Merge( const BoundingBox& box, const Vector3& p )
{
    Vector3 mini = Vector3::Min( box.min, p );
    Vector3 maxi = Vector3::Max( box.max, p );

    return BoundingBox( mini, maxi );
}

/////////////////////////////////////////////////////////////////////////////////////
// BoundingBoxQuad structure
/////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------
S3D_INLINE
BoundingBoxQuad::BoundingBoxQuad()
{
#if S3D_IS_SIMD
    value[0][0] = _mm_set1_ps( F_MAX );
    value[0][1] = _mm_set1_ps( F_MAX );
    value[0][2] = _mm_set1_ps( F_MAX );

    value[1][0] = _mm_set1_ps( F_MIN );
    value[1][1] = _mm_set1_ps( F_MIN );
    value[1][2] = _mm_set1_ps( F_MIN );
#else
    value[0][0].m128_f32[0] = F_MAX;
    value[0][0].m128_f32[1] = F_MAX;
    value[0][0].m128_f32[2] = F_MAX;
    value[0][0].m128_f32[3] = F_MAX;

    value[0][1].m128_f32[0] = F_MAX;
    value[0][1].m128_f32[1] = F_MAX;
    value[0][1].m128_f32[2] = F_MAX;
    value[0][1].m128_f32[3] = F_MAX;

    value[0][2].m128_f32[0] = F_MAX;
    value[0][2].m128_f32[1] = F_MAX;
    value[0][2].m128_f32[2] = F_MAX;
    value[0][2].m128_f32[3] = F_MAX;


    value[1][0].m128_f32[0] = F_MIN;
    value[1][0].m128_f32[1] = F_MIN;
    value[1][0].m128_f32[2] = F_MIN;
    value[1][0].m128_f32[3] = F_MIN;

    value[1][1].m128_f32[0] = F_MIN;
    value[1][1].m128_f32[1] = F_MIN;
    value[1][1].m128_f32[2] = F_MIN;
    value[1][1].m128_f32[3] = F_MIN;

    value[1][2].m128_f32[0] = F_MIN;
    value[1][2].m128_f32[1] = F_MIN;
    value[1][2].m128_f32[2] = F_MIN;
    value[1][2].m128_f32[3] = F_MIN;
#endif//S3D_IS_SIMD
}

S3D_INLINE 
BoundingBoxQuad::BoundingBoxQuad
(
    const BoundingBox& box0
)
{
#if S3D_IS_SIMD
    value[0][0] = _mm_set1_ps( box0.min.x );
    value[0][1] = _mm_set1_ps( box0.min.y );
    value[0][2] = _mm_set1_ps( box0.min.z );

    value[1][0] = _mm_set1_ps( box0.max.x );
    value[1][1] = _mm_set1_ps( box0.max.y );
    value[1][2] = _mm_set1_ps( box0.max.z );
#else
    value[0][0].m128_f32[0] = box0.min.x;
    value[0][0].m128_f32[1] = box0.min.x;
    value[0][0].m128_f32[2] = box0.min.x;
    value[0][0].m128_f32[3] = box0.min.x;

    value[0][1].m128_f32[0] = box0.min.y;
    value[0][1].m128_f32[1] = box0.min.y;
    value[0][1].m128_f32[2] = box0.min.y;
    value[0][1].m128_f32[3] = box0.min.y;

    value[0][2].m128_f32[0] = box0.min.z;
    value[0][2].m128_f32[1] = box0.min.z;
    value[0][2].m128_f32[2] = box0.min.z;
    value[0][2].m128_f32[3] = box0.min.z;


    value[1][0].m128_f32[0] = box0.max.x;
    value[1][0].m128_f32[1] = box0.max.x;
    value[1][0].m128_f32[2] = box0.max.x;
    value[1][0].m128_f32[3] = box0.max.x;

    value[1][1].m128_f32[0] = box0.max.y;
    value[1][1].m128_f32[1] = box0.max.y;
    value[1][1].m128_f32[2] = box0.max.y;
    value[1][1].m128_f32[3] = box0.max.y;

    value[1][2].m128_f32[0] = box0.max.z;
    value[1][2].m128_f32[1] = box0.max.z;
    value[1][2].m128_f32[2] = box0.max.z;
    value[1][2].m128_f32[3] = box0.max.z;
#endif//S3D_IS_SIMD
}


//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
S3D_INLINE
BoundingBoxQuad::BoundingBoxQuad
(
    const BoundingBox& box0,
    const BoundingBox& box1
)
{
#if S3D_IS_SIMD
    value[0][0] = _mm_set_ps( F_MAX, F_MAX, box1.min.x, box0.min.x );
    value[0][1] = _mm_set_ps( F_MAX, F_MAX, box1.min.y, box0.min.y );
    value[0][2] = _mm_set_ps( F_MAX, F_MAX, box1.min.z, box0.min.z );

    value[1][0] = _mm_set_ps( F_MIN, F_MIN, box1.max.x, box0.max.x );
    value[1][1] = _mm_set_ps( F_MIN, F_MIN, box1.max.y, box0.max.y );
    value[1][2] = _mm_set_ps( F_MIN, F_MIN, box1.max.z, box0.max.z );
#else
    value[0][0].m128_f32[0] = box0.min.x;
    value[0][0].m128_f32[1] = box1.min.x;
    value[0][0].m128_f32[2] = box0.min.x;
    value[0][0].m128_f32[3] = box0.min.x;

    value[0][1].m128_f32[0] = box0.min.y;
    value[0][1].m128_f32[1] = box1.min.y;
    value[0][1].m128_f32[2] = box0.min.y;
    value[0][1].m128_f32[3] = box0.min.y;

    value[0][2].m128_f32[0] = box0.min.z;
    value[0][2].m128_f32[1] = box1.min.z;
    value[0][2].m128_f32[2] = box0.min.z;
    value[0][2].m128_f32[3] = box0.min.z;


    value[1][0].m128_f32[0] = box0.max.x;
    value[1][0].m128_f32[1] = box1.max.x;
    value[1][0].m128_f32[2] = box0.max.x;
    value[1][0].m128_f32[3] = box0.max.x;

    value[1][1].m128_f32[0] = box0.max.y;
    value[1][1].m128_f32[1] = box1.max.y;
    value[1][1].m128_f32[2] = box0.max.y;
    value[1][1].m128_f32[3] = box0.max.y;

    value[1][2].m128_f32[0] = box0.max.z;
    value[1][2].m128_f32[1] = box1.max.z;
    value[1][2].m128_f32[2] = box0.max.z;
    value[1][2].m128_f32[3] = box0.max.z;
#endif//S3D_IS_SIMD
}


//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
S3D_INLINE
BoundingBoxQuad::BoundingBoxQuad
(
    const BoundingBox& box0,
    const BoundingBox& box1,
    const BoundingBox& box2
)
{
#if S3D_IS_SIMD
    value[0][0] = _mm_set_ps( F_MAX, box2.min.x, box1.min.x, box0.min.x );
    value[0][1] = _mm_set_ps( F_MAX, box2.min.y, box1.min.y, box0.min.y );
    value[0][2] = _mm_set_ps( F_MAX, box2.min.z, box1.min.z, box0.min.z );

    value[1][0] = _mm_set_ps( F_MIN, box2.max.x, box1.max.x, box0.max.x );
    value[1][1] = _mm_set_ps( F_MIN, box2.max.y, box1.max.y, box0.max.y );
    value[1][2] = _mm_set_ps( F_MIN, box2.max.z, box1.max.z, box0.max.z );
#else
    value[0][0].m128_f32[0] = box0.min.x;
    value[0][0].m128_f32[1] = box1.min.x;
    value[0][0].m128_f32[2] = box2.min.x;
    value[0][0].m128_f32[3] = box0.min.x;

    value[0][1].m128_f32[0] = box0.min.y;
    value[0][1].m128_f32[1] = box1.min.y;
    value[0][1].m128_f32[2] = box2.min.y;
    value[0][1].m128_f32[3] = box0.min.y;

    value[0][2].m128_f32[0] = box0.min.z;
    value[0][2].m128_f32[1] = box1.min.z;
    value[0][2].m128_f32[2] = box2.min.z;
    value[0][2].m128_f32[3] = box0.min.z;


    value[1][0].m128_f32[0] = box0.max.x;
    value[1][0].m128_f32[1] = box1.max.x;
    value[1][0].m128_f32[2] = box2.max.x;
    value[1][0].m128_f32[3] = box0.max.x;

    value[1][1].m128_f32[0] = box0.max.y;
    value[1][1].m128_f32[1] = box1.max.y;
    value[1][1].m128_f32[2] = box2.max.y;
    value[1][1].m128_f32[3] = box0.max.y;

    value[1][2].m128_f32[0] = box0.max.z;
    value[1][2].m128_f32[1] = box1.max.z;
    value[1][2].m128_f32[2] = box2.max.z;
    value[1][2].m128_f32[3] = box0.max.z;
#endif//S3D_IS_SIMD
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
S3D_INLINE
BoundingBoxQuad::BoundingBoxQuad
(
    const BoundingBox& box0,
    const BoundingBox& box1,
    const BoundingBox& box2,
    const BoundingBox& box3
)
{
#if S3D_IS_SIMD
    value[0][0] = _mm_set_ps( box3.min.x, box2.min.x, box1.min.x, box0.min.x );
    value[0][1] = _mm_set_ps( box3.min.y, box2.min.y, box1.min.y, box0.min.y );
    value[0][2] = _mm_set_ps( box3.min.z, box2.min.z, box1.min.z, box0.min.z );

    value[1][0] = _mm_set_ps( box3.max.x, box2.max.x, box1.max.x, box0.max.x );
    value[1][1] = _mm_set_ps( box3.max.y, box2.max.y, box1.max.y, box0.max.y );
    value[1][2] = _mm_set_ps( box3.max.z, box2.max.z, box1.max.z, box0.max.z );
#else
    value[0][0].m128_f32[0] = box0.min.x;
    value[0][0].m128_f32[1] = box1.min.x;
    value[0][0].m128_f32[2] = box2.min.x;
    value[0][0].m128_f32[3] = box3.min.x;

    value[0][1].m128_f32[0] = box0.min.y;
    value[0][1].m128_f32[1] = box1.min.y;
    value[0][1].m128_f32[2] = box2.min.y;
    value[0][1].m128_f32[3] = box3.min.y;

    value[0][2].m128_f32[0] = box0.min.z;
    value[0][2].m128_f32[1] = box1.min.z;
    value[0][2].m128_f32[2] = box2.min.z;
    value[0][2].m128_f32[3] = box3.min.z;


    value[1][0].m128_f32[0] = box0.max.x;
    value[1][0].m128_f32[1] = box1.max.x;
    value[1][0].m128_f32[2] = box2.max.x;
    value[1][0].m128_f32[3] = box3.max.x;

    value[1][1].m128_f32[0] = box0.max.y;
    value[1][1].m128_f32[1] = box1.max.y;
    value[1][1].m128_f32[2] = box2.max.y;
    value[1][1].m128_f32[3] = box3.max.y;

    value[1][2].m128_f32[0] = box0.max.z;
    value[1][2].m128_f32[1] = box1.max.z;
    value[1][2].m128_f32[2] = box2.max.z;
    value[1][2].m128_f32[3] = box3.max.z;
#endif//S3D_IS_SIMD
}

//-----------------------------------------------------------------------------------
//      引数付きコンストラクタです.
//-----------------------------------------------------------------------------------
S3D_INLINE
BoundingBoxQuad::BoundingBoxQuad
(
    const b128& miniX,
    const b128& miniY,
    const b128& miniZ,

    const b128& maxiX,
    const b128& maxiY,
    const b128& maxiZ
)
{
    value[0][0] = miniX;
    value[0][1] = miniY;
    value[0][2] = miniZ;

    value[1][0] = maxiX;
    value[1][1] = maxiY;
    value[1][2] = maxiZ;
}

//-----------------------------------------------------------------------------------
//      交差判定を行います.
//-----------------------------------------------------------------------------------
S3D_INLINE
bool BoundingBoxQuad::IsHit( const RayQuad& ray, s32& mask ) const
{
#if S3D_IS_SIMD
    b128 tmin = _mm_set1_ps( F_HIT_MIN );
    b128 tmax = _mm_set1_ps( F_HIT_MAX );

    s32 idx0, idx1;

    // X軸.
    idx0 = ray.sign[ 0 ];
    idx1 = 1 - idx0;
    tmin = _mm_max_ps( tmin, _mm_mul_ps( _mm_sub_ps( value[ idx0 ][ 0 ], ray.pos[ 0 ] ), ray.invDir[ 0 ] ) );
    tmax = _mm_min_ps( tmax, _mm_mul_ps( _mm_sub_ps( value[ idx1 ][ 0 ], ray.pos[ 0 ] ), ray.invDir[ 0 ] ) );

    // Y軸.
    idx0 = ray.sign[ 1 ];
    idx1 = 1 - idx0;
    tmin = _mm_max_ps( tmin, _mm_mul_ps( _mm_sub_ps( value[ idx0 ][ 1 ], ray.pos[ 1 ] ), ray.invDir[ 1 ] ) );
    tmax = _mm_min_ps( tmax, _mm_mul_ps( _mm_sub_ps( value[ idx1 ][ 1 ], ray.pos[ 1 ] ), ray.invDir[ 1 ] ) );

    // Z軸.
    idx0 = ray.sign[ 2 ];
    idx1 = 1 - idx0;
    tmin = _mm_max_ps( tmin, _mm_mul_ps( _mm_sub_ps( value[ idx0 ][ 2 ], ray.pos[ 2 ] ), ray.invDir[ 2 ] ) );
    tmax = _mm_min_ps( tmax, _mm_mul_ps( _mm_sub_ps( value[ idx1 ][ 2 ], ray.pos[ 2 ] ), ray.invDir[ 2 ] ) );

    mask = _mm_movemask_ps( _mm_cmpge_ps( tmax, tmin ) );
    return ( mask > 0 );
#else
    b128 tmin = { F_HIT_MIN, F_HIT_MIN, F_HIT_MIN, F_HIT_MIN };
    b128 tmax = { F_HIT_MAX, F_HIT_MAX, F_HIT_MAX, F_HIT_MAX };

    s32 idx0, idx1;

    // X軸
    idx0 = ray.sign[ 0 ];
    idx1 = 1 - idx0;
    for ( u32 i=0; i<4; ++i )
    {
        tmin.m128_f32[ i ] = s3d::Max( tmin.m128_f32[ i ], ( value[ idx0 ][ 0 ].m128_f32[ i ] - ray.pos[ 0 ].m128_f32[ i ] ) * ray.invDir[ 0 ].m128_f32[ i ] );
        tmax.m128_f32[ i ] = s3d::Min( tmax.m128_f32[ i ], ( value[ idx1 ][ 0 ].m128_f32[ i ] - ray.pos[ 0 ].m128_f32[ i ] ) * ray.invDir[ 0 ].m128_f32[ i ] );
    }

    // Y軸
    idx0 = ray.sign[ 1 ];
    idx1 = 1 - idx0;
    for ( u32 i=0; i<4; ++i )
    {
        tmin.m128_f32[ i ] = s3d::Max( tmin.m128_f32[ i ], ( value[ idx0 ][ 1 ].m128_f32[ i ] - ray.pos[ 1 ].m128_f32[ i ] ) * ray.invDir[ 1 ].m128_f32[ i ] );
        tmax.m128_f32[ i ] = s3d::Min( tmax.m128_f32[ i ], ( value[ idx1 ][ 1 ].m128_f32[ i ] - ray.pos[ 1 ].m128_f32[ i ] ) * ray.invDir[ 1 ].m128_f32[ i ] );
    }

    // Z軸
    idx0 = ray.sign[ 2 ];
    idx1 = 1 - idx0;
    for ( u32 i=0; i<4; ++i )
    {
        tmin.m128_f32[ i ] = s3d::Max( tmin.m128_f32[ i ], ( value[ idx0 ][ 2 ].m128_f32[ i ] - ray.pos[ 2 ].m128_f32[ i ] ) * ray.invDir[ 2 ].m128_f32[ i ] );
        tmax.m128_f32[ i ] = s3d::Min( tmax.m128_f32[ i ], ( value[ idx1 ][ 2 ].m128_f32[ i ] - ray.pos[ 2 ].m128_f32[ i ] ) * ray.invDir[ 2 ].m128_f32[ i ] );
    }

    b128 flg;
    flg.m128_u32[0] = ( tmax.m128_f32[ 0 ] >= tmin.m128_f32[ 0 ] ) ? 0xffffffff : 0x0;
    flg.m128_u32[1] = ( tmax.m128_f32[ 1 ] >= tmin.m128_f32[ 1 ] ) ? 0xffffffff : 0x0;
    flg.m128_u32[2] = ( tmax.m128_f32[ 2 ] >= tmin.m128_f32[ 2 ] ) ? 0xffffffff : 0x0;
    flg.m128_u32[3] = ( tmax.m128_f32[ 3 ] >= tmin.m128_f32[ 3 ] ) ? 0xffffffff : 0x0;

    mask = ( Sign(flg.m128_u32[3]) << 3 | Sign(flg.m128_u32[2]) << 2 | Sign(flg.m128_u32[1]) << 1 | Sign(flg.m128_u32[0]) );
    return ( mask > 0 );
#endif
}

//-----------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-----------------------------------------------------------------------------------
S3D_INLINE
BoundingBox BoundingBoxQuad::GetBox() const
{
    Vector3 tmin( value[0][0].m128_f32[0], value[0][1].m128_f32[0], value[0][2].m128_f32[0] );
    Vector3 tmax( value[1][0].m128_f32[0], value[1][1].m128_f32[0], value[1][2].m128_f32[0] );

    for( u32 i=1; i<4; ++i )
    {
        tmin = Vector3::Min( tmin, Vector3( value[0][0].m128_f32[i], value[0][1].m128_f32[i], value[0][2].m128_f32[i] ) );
        tmax = Vector3::Max( tmax, Vector3( value[1][0].m128_f32[i], value[1][1].m128_f32[i], value[1][2].m128_f32[i] ) );
    }

    return BoundingBox( tmin, tmax );
}

//-----------------------------------------------------------------------------------
//      2つの4バウンディングボックスをマージします.
//-----------------------------------------------------------------------------------
S3D_INLINE
BoundingBoxQuad BoundingBoxQuad::Merge( const BoundingBoxQuad& a, const BoundingBoxQuad& b )
{
#if S3D_IS_SIMD
    b128 miniX = _mm_min_ps( a.value[0][0], b.value[0][0] );
    b128 miniY = _mm_min_ps( a.value[0][1], b.value[0][1] );
    b128 miniZ = _mm_min_ps( a.value[0][2], b.value[0][2] );

    b128 maxiX = _mm_max_ps( a.value[1][0], b.value[1][0] );
    b128 maxiY = _mm_max_ps( a.value[1][1], b.value[1][1] );
    b128 maxiZ = _mm_max_ps( a.value[1][2], b.value[1][2] );

    return BoundingBoxQuad( miniX, miniY, miniZ, maxiX, maxiY, maxiZ );
#else
    b128 miniX = { 
        ( a.value[0][0].m128_f32[0] < b.value[0][0].m128_f32[0] ) ? a.value[0][0].m128_f32[0] : b.value[0][0].m128_f32[0],
        ( a.value[0][0].m128_f32[1] < b.value[0][0].m128_f32[1] ) ? a.value[0][0].m128_f32[1] : b.value[0][0].m128_f32[1],
        ( a.value[0][0].m128_f32[2] < b.value[0][0].m128_f32[2] ) ? a.value[0][0].m128_f32[2] : b.value[0][0].m128_f32[2],
        ( a.value[0][0].m128_f32[3] < b.value[0][0].m128_f32[3] ) ? a.value[0][0].m128_f32[3] : b.value[0][0].m128_f32[3]
    };
    b128 miniY = { 
        ( a.value[0][1].m128_f32[0] < b.value[0][1].m128_f32[0] ) ? a.value[0][1].m128_f32[0] : b.value[0][1].m128_f32[0],
        ( a.value[0][1].m128_f32[1] < b.value[0][1].m128_f32[1] ) ? a.value[0][1].m128_f32[1] : b.value[0][1].m128_f32[1],
        ( a.value[0][1].m128_f32[2] < b.value[0][1].m128_f32[2] ) ? a.value[0][1].m128_f32[2] : b.value[0][1].m128_f32[2],
        ( a.value[0][1].m128_f32[3] < b.value[0][1].m128_f32[3] ) ? a.value[0][1].m128_f32[3] : b.value[0][1].m128_f32[3]
    };
    b128 miniZ = { 
        ( a.value[0][2].m128_f32[0] < b.value[0][2].m128_f32[0] ) ? a.value[0][2].m128_f32[0] : b.value[0][2].m128_f32[0],
        ( a.value[0][2].m128_f32[1] < b.value[0][2].m128_f32[1] ) ? a.value[0][2].m128_f32[1] : b.value[0][2].m128_f32[1],
        ( a.value[0][2].m128_f32[2] < b.value[0][2].m128_f32[2] ) ? a.value[0][2].m128_f32[2] : b.value[0][2].m128_f32[2],
        ( a.value[0][2].m128_f32[3] < b.value[0][2].m128_f32[3] ) ? a.value[0][2].m128_f32[3] : b.value[0][2].m128_f32[3]
    };

    b128 maxiX = { 
        ( a.value[1][0].m128_f32[0] > b.value[1][0].m128_f32[0] ) ? a.value[1][0].m128_f32[0] : b.value[1][0].m128_f32[0],
        ( a.value[1][0].m128_f32[1] > b.value[1][0].m128_f32[1] ) ? a.value[1][0].m128_f32[1] : b.value[1][0].m128_f32[1],
        ( a.value[1][0].m128_f32[2] > b.value[1][0].m128_f32[2] ) ? a.value[1][0].m128_f32[2] : b.value[1][0].m128_f32[2],
        ( a.value[1][0].m128_f32[3] > b.value[1][0].m128_f32[3] ) ? a.value[1][0].m128_f32[3] : b.value[1][0].m128_f32[3]
    };
    b128 maxiY = { 
        ( a.value[1][1].m128_f32[0] > b.value[1][1].m128_f32[0] ) ? a.value[1][1].m128_f32[0] : b.value[1][1].m128_f32[0],
        ( a.value[1][1].m128_f32[1] > b.value[1][1].m128_f32[1] ) ? a.value[1][1].m128_f32[1] : b.value[1][1].m128_f32[1],
        ( a.value[1][1].m128_f32[2] > b.value[1][1].m128_f32[2] ) ? a.value[1][1].m128_f32[2] : b.value[1][1].m128_f32[2],
        ( a.value[1][1].m128_f32[3] > b.value[1][1].m128_f32[3] ) ? a.value[1][1].m128_f32[3] : b.value[1][1].m128_f32[3]
    };
    b128 maxiZ = { 
        ( a.value[1][2].m128_f32[0] > b.value[1][2].m128_f32[0] ) ? a.value[1][2].m128_f32[0] : b.value[1][2].m128_f32[0],
        ( a.value[1][2].m128_f32[1] > b.value[1][2].m128_f32[1] ) ? a.value[1][2].m128_f32[1] : b.value[1][2].m128_f32[1],
        ( a.value[1][2].m128_f32[2] > b.value[1][2].m128_f32[2] ) ? a.value[1][2].m128_f32[2] : b.value[1][2].m128_f32[2],
        ( a.value[1][2].m128_f32[3] > b.value[1][2].m128_f32[3] ) ? a.value[1][2].m128_f32[3] : b.value[1][2].m128_f32[3]
    };

    return BoundingBoxQuad( miniX, miniY, miniZ, maxiX, maxiY, maxiZ );
#endif//S3D_IS_SIMD
}

} // namespace s3d


#endif//__S3D_BOX_INL__
