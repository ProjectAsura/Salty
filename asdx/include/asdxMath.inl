//-----------------------------------------------------------------------
// File : asdxMath.inl
// Desc : Math Module.
// Copyright(c) Project Asura All right reserved.
//-----------------------------------------------------------------------

namespace asdx {

/////////////////////////////////////////////////////////////////////////
// FLOAT2 structure
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE
FLOAT2::FLOAT2()
{
    x = y = 0;
}

ASDX_INLINE
FLOAT2::FLOAT2( const float* pf )
{
    assert( pf != 0 );
    x = pf[ 0 ];
    y = pf[ 1 ];    
}

ASDX_INLINE
FLOAT2::FLOAT2( float nx, float ny )
{
    x = nx;
    y = ny;
}

ASDX_INLINE
FLOAT2::operator float *()
{ return (float*)&x; }

ASDX_INLINE
FLOAT2::operator const float *() const
{ return (const float*)&x; }

ASDX_INLINE
FLOAT2& FLOAT2::operator += ( const FLOAT2& v )
{
    x += v.x;
    y += v.y;
    return (*this);
}

ASDX_INLINE
FLOAT2& FLOAT2::operator -= ( const FLOAT2& v )
{
    x -= v.x;
    y -= v.y;
    return (*this);
}

ASDX_INLINE
FLOAT2& FLOAT2::operator *= ( float f )
{
    x *= f;
    y *= f;
    return (*this);
}

ASDX_INLINE
FLOAT2& FLOAT2::operator /= ( float f )
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    return (*this);
}

ASDX_INLINE
FLOAT2& FLOAT2::operator = ( const FLOAT2& value )
{
    x = value.x;
    y = value.y;
    return (*this);
}

ASDX_INLINE
FLOAT2 FLOAT2::operator + () const
{ return (*this); }

ASDX_INLINE
FLOAT2 FLOAT2::operator - () const
{ return FLOAT2( -x, -y ); }

ASDX_INLINE
FLOAT2 FLOAT2::operator + ( FLOAT2 v ) const
{ return FLOAT2( x + v.x, y + v.y ); }

ASDX_INLINE
FLOAT2 FLOAT2::operator - ( FLOAT2 v ) const
{ return FLOAT2( x - v.x, y - v.y ); }

ASDX_INLINE
FLOAT2 FLOAT2::operator * ( float f ) const
{ return FLOAT2( x * f, y * f ); }

ASDX_INLINE
FLOAT2 FLOAT2::operator / ( float f ) const
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f / f;
    return FLOAT2( x * fInv, y * fInv );
}

ASDX_INLINE
FLOAT2 operator * ( float f, const FLOAT2& v )
{ return FLOAT2( f * v.x, f * v.y ); }

ASDX_INLINE
bool FLOAT2::operator == ( const FLOAT2& v ) const
{ return ( IsNearlyEqual( x, v.x ) && IsNearlyEqual( y, v.y ) ); }

ASDX_INLINE
bool FLOAT2::operator != ( const FLOAT2& v ) const
{ return ( !IsNearlyEqual( x, v.x ) || !IsNearlyEqual( y, v.y ) ); }

ASDX_INLINE
float FLOAT2::Length() const
{ return sqrtf( x * x + y * y ); }

ASDX_INLINE
float FLOAT2::LengthSquared() const
{ return ( x * x + y * y ); }

ASDX_INLINE
FLOAT2& FLOAT2::Normalize()
{
    float mag = Length();
    assert( !IsNearZero( mag ) );
    float invMag = 1.0f / mag;
    x *= invMag;
    y *= invMag;
    return (*this);
}

ASDX_INLINE
FLOAT2::FLOAT2( const DirectX::XMFLOAT2& value )
: x( value.x )
, y( value.y )
{ /* DO_NOTHING */ }

ASDX_INLINE
FLOAT2& FLOAT2::operator = ( const DirectX::XMFLOAT2& value )
{
    x = value.x;
    y = value.y;
    return (*this);
}



/////////////////////////////////////////////////////////////////////////
// FLOAT3 structure
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE
FLOAT3::FLOAT3()
{
    x = y = z= 0;
}

ASDX_INLINE
FLOAT3::FLOAT3( const float* pf )
{
    assert( pf != 0 );
    x = pf[ 0 ];
    y = pf[ 1 ];
    z = pf[ 2 ];
}

ASDX_INLINE
FLOAT3::FLOAT3( FLOAT2& v, float nz )
{
    x = v.x;
    y = v.y;
    z = nz;
}

ASDX_INLINE
FLOAT3::FLOAT3( float nx, float ny, float nz )
{
    x = nx;
    y = ny;
    z = nz;
}

ASDX_INLINE
FLOAT3::operator float *()
{ return (float*)&x; }

ASDX_INLINE
FLOAT3::operator const float *() const
{ return (const float*)&x; }

ASDX_INLINE
FLOAT3& FLOAT3::operator += ( const FLOAT3& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    return (*this);
}

ASDX_INLINE
FLOAT3& FLOAT3::operator -= ( const FLOAT3& v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return (*this);
}

ASDX_INLINE
FLOAT3& FLOAT3::operator *= ( float f )
{
    x *= f;
    y *= f;
    z *= f;
    return (*this);
}

ASDX_INLINE
FLOAT3& FLOAT3::operator /= ( float f )
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
    return (*this);
}

ASDX_INLINE
FLOAT3& FLOAT3::operator = ( const FLOAT3& value )
{
    x = value.x;
    y = value.y;
    z = value.z;
    return (*this);
}

ASDX_INLINE
FLOAT3 FLOAT3::operator + () const
{ return (*this); }

ASDX_INLINE
FLOAT3 FLOAT3::operator - () const
{ return FLOAT3( -x, -y, -z ); }

ASDX_INLINE
FLOAT3 FLOAT3::operator + ( FLOAT3 v ) const
{ return FLOAT3( x + v.x, y + v.y, z + v.z ); }

ASDX_INLINE
FLOAT3 FLOAT3::operator - ( FLOAT3 v ) const
{ return FLOAT3( x - v.x, y - v.y, z - v.z ); }

ASDX_INLINE
FLOAT3 FLOAT3::operator * ( float f ) const
{ return FLOAT3( x * f, y * f, z * f ); }

ASDX_INLINE
FLOAT3 FLOAT3::operator / ( float f ) const
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f / f;
    return FLOAT3( x * fInv, y * fInv, z * fInv );
}

ASDX_INLINE
FLOAT3 operator * ( float f, const FLOAT3& v )
{ return FLOAT3( f * v.x, f * v.y, f * v.z ); }

ASDX_INLINE
bool FLOAT3::operator == ( const FLOAT3& v ) const
{
    return ( IsNearlyEqual( x, v.x ) 
          && IsNearlyEqual( y, v.y ) 
          && IsNearlyEqual( z, v.z ) );
}

ASDX_INLINE
bool FLOAT3::operator != ( const FLOAT3& v ) const
{ 
    return ( !IsNearlyEqual( x, v.x ) 
          || !IsNearlyEqual( y, v.y ) 
          || !IsNearlyEqual( z, v.z ) ); 
}

ASDX_INLINE
float FLOAT3::Length() const
{ return sqrtf( x * x + y * y + z * z); }

ASDX_INLINE
float FLOAT3::LengthSquared() const
{ return ( x * x + y * y + z * z); }

ASDX_INLINE
FLOAT3& FLOAT3::Normalize()
{
    float mag = Length();
    assert( !IsNearZero( mag ) );
    float invMag = 1.0f / mag;
    x *= invMag;
    y *= invMag;
    z *= invMag;
    return (*this);
}

ASDX_INLINE
FLOAT3::FLOAT3( const DirectX::XMFLOAT3& value )
: x( value.x )
, y( value.y )
, z( value.z )
{ /* DO_NOTHING */ }

ASDX_INLINE
FLOAT3& FLOAT3::operator = ( const DirectX::XMFLOAT3& value )
{
    x = value.x;
    y = value.y;
    z = value.z;
    return (*this);
}


/////////////////////////////////////////////////////////////////////////
// FLOAT4 structure
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE
FLOAT4::FLOAT4()
{
    x = y = z= 0;
}

ASDX_INLINE
FLOAT4::FLOAT4( const float* pf )
{
    assert( pf != 0 );
    x = pf[ 0 ];
    y = pf[ 1 ];
    z = pf[ 2 ];
    w = pf[ 3 ];
}

ASDX_INLINE
FLOAT4::FLOAT4( FLOAT2& v, float nz, float nw )
{
    x = v.x;
    y = v.y;
    z = nz;
    w = nw;
}

ASDX_INLINE
FLOAT4::FLOAT4( FLOAT3& v, float nw )
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = nw;
}

ASDX_INLINE
FLOAT4::FLOAT4( float nx, float ny, float nz, float nw )
{
    x = nx;
    y = ny;
    z = nz;
    w = nw;
}

ASDX_INLINE
FLOAT4::operator float *()
{ return (float*)&x; }

ASDX_INLINE
FLOAT4::operator const float *() const
{ return (const float*)&x; }

ASDX_INLINE
FLOAT4& FLOAT4::operator += ( const FLOAT4& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return (*this);
}

ASDX_INLINE
FLOAT4& FLOAT4::operator -= ( const FLOAT4& v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return (*this);
}

ASDX_INLINE
FLOAT4& FLOAT4::operator *= ( float f )
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return (*this);
}

ASDX_INLINE
FLOAT4& FLOAT4::operator /= ( float f )
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
    w *= fInv;
    return (*this);
}

ASDX_INLINE
FLOAT4& FLOAT4::operator = ( const FLOAT4& value )
{
    x = value.x;
    y = value.y;
    z = value.z;
    w = value.w;
    return (*this);
}

ASDX_INLINE
FLOAT4 FLOAT4::operator + () const
{ return (*this); }

ASDX_INLINE
FLOAT4 FLOAT4::operator - () const
{ return FLOAT4( -x, -y, -z, -w ); }

ASDX_INLINE
FLOAT4 FLOAT4::operator + ( FLOAT4 v ) const
{ return FLOAT4( x + v.x, y + v.y, z + v.z, w + v.w ); }

ASDX_INLINE
FLOAT4 FLOAT4::operator - ( FLOAT4 v ) const
{ return FLOAT4( x - v.x, y - v.y, z - v.z, w - v.w ); }

ASDX_INLINE
FLOAT4 FLOAT4::operator * ( float f ) const
{ return FLOAT4( x * f, y * f, z * f, w * f ); }

ASDX_INLINE
FLOAT4 FLOAT4::operator / ( float f ) const
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f / f;
    return FLOAT4( x * fInv, y * fInv, z * fInv, w * fInv );
}

ASDX_INLINE
FLOAT4 operator * ( float f, const FLOAT4& v )
{ return FLOAT4( f * v.x, f * v.y, f * v.z, f * v.w ); }

ASDX_INLINE
bool FLOAT4::operator == ( const FLOAT4& v ) const
{
    return ( IsNearlyEqual( x, v.x ) 
          && IsNearlyEqual( y, v.y ) 
          && IsNearlyEqual( z, v.z )
          && IsNearlyEqual( w, v.w ) );
}

ASDX_INLINE
bool FLOAT4::operator != ( const FLOAT4& v ) const
{ 
    return ( !IsNearlyEqual( x, v.x ) 
          || !IsNearlyEqual( y, v.y ) 
          || !IsNearlyEqual( z, v.z )
          || !IsNearlyEqual( w, v.w ) ); 
}

ASDX_INLINE
float FLOAT4::Length() const
{ return sqrtf( x * x + y * y + z * z + w * w ); }

ASDX_INLINE
float FLOAT4::LengthSquared() const
{ return ( x * x + y * y + z * z + w * w ); }

ASDX_INLINE
FLOAT4& FLOAT4::Normalize()
{
    float mag = Length();
    assert( !IsNearZero( mag ) );
    float invMag = 1.0f / mag;
    x *= invMag;
    y *= invMag;
    z *= invMag;
    w *= invMag;
    return (*this);
}

ASDX_INLINE
FLOAT4::FLOAT4( const DirectX::XMFLOAT4& value )
: x( value.x )
, y( value.y )
, z( value.z )
, w( value.w )
{ /* DO_NOTHING */ }

ASDX_INLINE
FLOAT4& FLOAT4::operator = ( const DirectX::XMFLOAT4& value )
{
    x = value.x;
    y = value.y;
    z = value.z;
    w = value.w;
    return (*this);
}


/////////////////////////////////////////////////////////////////////////
// Vector Methods
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE
FLOAT2 Abs( FLOAT2 v )
{ return FLOAT2( fabs( v.x ), fabs( v.y ) ); }

ASDX_INLINE
FLOAT3 Abs( FLOAT3 v )
{ return FLOAT3( fabs( v.x ), fabs( v.y ), fabs( v.z ) ); }

ASDX_INLINE
FLOAT4 Abs( FLOAT4 v )
{ return FLOAT4( fabs( v.x ), fabs( v.y ), fabs( v.z ), fabs( v.w ) ); }

ASDX_INLINE
void Abs( FLOAT2 &v, FLOAT2 &result )
{ result = Abs( v ); }

ASDX_INLINE
void Abs( FLOAT3 &v, FLOAT3 &result )
{ result = Abs( v ); }

ASDX_INLINE
void Abs( FLOAT4 &v, FLOAT4 &result )
{ result = Abs( v ); }

ASDX_INLINE
FLOAT2 Clamp( FLOAT2 v, FLOAT2 mini, FLOAT3 maxi )
{
    return FLOAT2( Clamp< float >( v.x, mini.x, maxi.x ),
                    Clamp< float >( v.y, mini.y, maxi.y ) );
}

ASDX_INLINE
FLOAT3 Clamp( FLOAT3 v, FLOAT3 mini, FLOAT3 maxi )
{
    return FLOAT3( Clamp< float >( v.x, mini.x, maxi.x ),
                    Clamp< float >( v.y, mini.y, maxi.y ),
                    Clamp< float >( v.z, mini.z, maxi.z ) );
}

ASDX_INLINE
FLOAT4 Clamp( FLOAT4 v, FLOAT4 mini, FLOAT4 maxi )
{
    return FLOAT4( Clamp< float >( v.x, mini.x, maxi.x ),
                    Clamp< float >( v.y, mini.y, maxi.y ),
                    Clamp< float >( v.z, mini.z, maxi.z ),
                    Clamp< float >( v.w, mini.w, maxi.w ) );
}

ASDX_INLINE
void Clamp( FLOAT2 &v, FLOAT2 &mini, FLOAT2 &maxi, FLOAT2 &result )
{ result = Clamp( v, mini, maxi ); }

ASDX_INLINE
void Clamp( FLOAT3 &v, FLOAT3 &mini, FLOAT3 &maxi, FLOAT3 &result )
{ result = Clamp( v, mini, maxi ); }

ASDX_INLINE
void Clamp( FLOAT4 &v, FLOAT4 &mini, FLOAT4 &maxi, FLOAT4 &result )
{ result = Clamp( v, mini, maxi ); }

ASDX_INLINE
float Distance( FLOAT2 v1, FLOAT2 v2 )
{
    FLOAT2 v = v2 - v1;
    return v.Length();
}

ASDX_INLINE
float Distance( FLOAT3 v1, FLOAT3 v2 )
{
    FLOAT3 v = v2 - v1;
    return v.Length();
}

ASDX_INLINE
float Distance( FLOAT4 v1, FLOAT4 v2 )
{
    FLOAT4 v = v2 - v1;
    return v.Length();
}

ASDX_INLINE
void Distance( FLOAT2 &v1, FLOAT2 &v2, float &result )
{ result = Distance( v1, v2 ); }

ASDX_INLINE
void Distance( FLOAT3 &v1, FLOAT3 &v2, float &result )
{ result = Distance( v1, v2 ); }

ASDX_INLINE
void Distance( FLOAT4 &v1, FLOAT4 &v2, float &result )
{ result = Distance( v1, v2 ); }

ASDX_INLINE
float DistanceSquared( FLOAT2 v1, FLOAT2 v2 )
{
    FLOAT2 v = v2 - v1;
    return v.LengthSquared();
}

ASDX_INLINE
float DistanceSquared( FLOAT3 v1, FLOAT3 v2 )
{
    FLOAT3 v = v2 - v1;
    return v.LengthSquared();
}

ASDX_INLINE
float DistanceSquared( FLOAT4 v1, FLOAT4 v2 )
{
    FLOAT4 v = v2 - v1;
    return v.LengthSquared();
}

ASDX_INLINE
void DistanceSquared( FLOAT2 &v1, FLOAT2 &v2, float &result )
{ result = DistanceSquared( v1, v2 ); }

ASDX_INLINE
void DistanceSquared( FLOAT3 &v1, FLOAT3 &v2, float &result )
{ result = DistanceSquared( v1, v2 ); }

ASDX_INLINE
void DistanceSquared( FLOAT4 &v1, FLOAT4 &v2, float &result )
{ result = DistanceSquared( v1, v2 ); }

ASDX_INLINE
float Dot( FLOAT2 v1, FLOAT2 v2 )
{ return ( v1.x * v2.x + v1.y * v2.y ); }

ASDX_INLINE
float Dot( FLOAT3 v1, FLOAT3 v2 )
{ return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z ); }

ASDX_INLINE
float Dot( FLOAT4 v1, FLOAT4 v2 )
{ return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w ); }

ASDX_INLINE
void Dot( FLOAT2 &v1, FLOAT2 &v2, float &result )
{ result = Dot( v1, v2 ); }

ASDX_INLINE
void Dot( FLOAT3 &v1, FLOAT3 &v2, float &result )
{ result = Dot( v1, v2 ); }

ASDX_INLINE
void Dot( FLOAT4 &v1, FLOAT4 &v2, float &result )
{ result = Dot( v1, v2 ); }

ASDX_INLINE
FLOAT3 Cross( FLOAT3 v1, FLOAT3 v2 )
{
    return FLOAT3(    ( v1.y * v2.z ) - ( v1.z * v2.y ),
                    ( v1.z * v2.x ) - ( v1.x * v2.z ),
                    ( v1.x * v2.y ) - ( v1.y * v2.x ) );
}

ASDX_INLINE
void Cross( FLOAT3 &v1, FLOAT3 &v2, FLOAT3 &result )
{ result = Cross( v1, v2 ); }

ASDX_INLINE
FLOAT2 Normalize( FLOAT2 v )
{
    float mag = v.Length();
    assert( !IsNearZero( mag ) );
    float invMag = 1.0f / mag;
    return FLOAT2( v.x * invMag, v.y * invMag );
}

ASDX_INLINE
FLOAT3 Normalize( FLOAT3 v )
{
    float mag = v.Length();
    assert( !IsNearZero( mag ) );
    float invMag = 1.0f / mag;
    return FLOAT3( v.x * invMag, v.y * invMag, v.z * invMag ); 
}

ASDX_INLINE
FLOAT4 Normalize( FLOAT4 v )
{
    float mag = v.Length();
    assert( !IsNearZero( mag ) );
    float invMag = 1.0f / mag;
    return FLOAT4( v.x * invMag, v.y * invMag, v.z * invMag, v.w * invMag );
}

ASDX_INLINE
void Normalize( FLOAT2 &v, FLOAT2 &result )
{ result = Normalize( v ); }

ASDX_INLINE
void Normalize( FLOAT3 &v, FLOAT3 &result )
{ result = Normalize( v ); }

ASDX_INLINE
void Normalize( FLOAT4 &v, FLOAT4 &result )
{ result = Normalize( v ); }

ASDX_INLINE
FLOAT3 ComputeNormal( FLOAT3 p1, FLOAT3 p2, FLOAT3 p3 )
{
    FLOAT3 v1 = p2 - p1;
    FLOAT3 v2 = p3 - p1;
    FLOAT3 result = Cross( v1, v2 );
    return result.Normalize();
}

ASDX_INLINE
void ComputeNormal( FLOAT3 &p1, FLOAT3 &p2, FLOAT3 &p3, FLOAT3 &result )
{ result = ComputeNormal( p1, p2, p3 ); }

ASDX_INLINE
FLOAT3 ComputeQuadNormal( FLOAT3 p1, FLOAT3 p2, FLOAT3 p3, FLOAT3 p4 )
{
    FLOAT3 result;
    FLOAT3 n1a = ComputeNormal( p1, p2, p3 );
    FLOAT3 n1b = ComputeNormal( p1, p3, p4 );
    FLOAT3 n2a = ComputeNormal( p2, p3, p4 );
    FLOAT3 n2b = ComputeNormal( p2, p4, p1 );
    if ( Dot( n1a, n1b ) > Dot( n2a, n2b ) )
    {
        result = n1a + n1b;
        result.Normalize();
    }
    else
    {
        result = n2a + n2b;
        result.Normalize();
    }
    return result;
}

ASDX_INLINE
void ComputeQuadNormal( FLOAT3 &p1, FLOAT3 &p2, FLOAT3 &p3, FLOAT3 &p4, FLOAT3 &result )
{ result = ComputeQuadNormal( p1, p2, p3, p4 ); }

ASDX_INLINE
FLOAT3 CompuateTangent( FLOAT3 p1, FLOAT3 p2, FLOAT3 p3,
                         FLOAT2 t1, FLOAT2 t2, FLOAT2 t3 )
{
    FLOAT3 result = -( t3.x - t1.x ) * ( p2 - p1 ) + ( t2.x - t1.x ) * ( p3 - p1 );
    return result;
}

ASDX_INLINE
void ComputeTangent( FLOAT3 &p1, FLOAT3 &p2, FLOAT3 &p3,
                     FLOAT2 &t1, FLOAT2 &t2, FLOAT2 &t3, FLOAT3 &result )
{ result = ComputeTangent( p1, p2, p3, t1, t2, t3 ); }

ASDX_INLINE
FLOAT3 ComputeBinormal( FLOAT3 normal, FLOAT3 tangent )
{ return Cross( normal, tangent ); }

ASDX_INLINE
void ComputeBinormal( FLOAT3 &normal, FLOAT3 &tangent, FLOAT3 &result )
{ result = ComputeBinormal( normal, tangent ); }

ASDX_INLINE
float ComputeCrossingAngle( FLOAT2 v1, FLOAT2 v2 )
{
    float d = v1.Length() * v2.Length();
    if ( IsNearZero( d ) ) { return 0; }
    float c = Dot( v1, v2 ) / d;
    if ( c >= 1.0f )  { return 0; }
    if ( c <= -1.0f ) { return F_PI; }
    return acosf( c );
}

ASDX_INLINE
float ComputeCrossingAngle( FLOAT3 v1, FLOAT3 v2 )
{
    float d = v1.Length() * v2.Length();
    if ( IsNearZero( d ) ) { return 0; }
    float c = Dot( v1, v2 ) / d;
    if ( c >= 1.0f )  { return 0; }
    if ( c <= -1.0f ) { return F_PI; }
    return acosf( c );
}

ASDX_INLINE
float ComputeCrossingAngle( FLOAT4 v1, FLOAT4 v2 )
{
    float d = v1.Length() * v2.Length();
    if ( IsNearZero( d ) ) { return 0; }
    float c = Dot( v1, v2 ) / d;
    if ( c >= 1.0f )  { return 0; }
    if ( c <= -1.0f ) { return F_PI; }
    return acosf( c );
}

ASDX_INLINE
void ComputeCrossingAngle( FLOAT2 &v1, FLOAT2 &v2, float &result )
{ result = ComputeCrossingAngle( v1, v2 ); }

ASDX_INLINE
void ComputeCrossingAngle( FLOAT3 &v1, FLOAT3 &v2, float &result )
{ result = ComputeCrossingAngle( v1, v2 ); }

ASDX_INLINE
void ComputeCrossingAngle( FLOAT4 &v1, FLOAT4 &v2, float &result )
{ result = ComputeCrossingAngle( v1, v2 ); }

ASDX_INLINE
FLOAT2 Min( FLOAT2 v1, FLOAT2 v2 )
{ 
    return FLOAT2( Min< float >( v1.x, v2.x ),
                    Min< float >( v1.y, v2.y ) );
}

ASDX_INLINE
FLOAT3 Min( FLOAT3 v1, FLOAT3 v2 )
{ 
    return FLOAT3( Min< float >( v1.x, v2.x ),
                    Min< float >( v1.y, v2.y ),
                    Min< float >( v1.z, v2.z ) );
}

ASDX_INLINE
FLOAT4 Min( FLOAT4 v1, FLOAT4 v2 )
{ 
    return FLOAT4( Min< float >( v1.x, v2.x ),
                    Min< float >( v1.y, v2.y ),
                    Min< float >( v1.z, v2.z ),
                    Min< float >( v1.w, v2.w ) );
}

ASDX_INLINE
void Min( FLOAT2 &v1, FLOAT2 &v2, FLOAT2 &result )
{ result = Min( v1, v2 ); }

ASDX_INLINE
void Min( FLOAT3 &v1, FLOAT3 &v2, FLOAT3 &result )
{ result = Min( v1, v2 ); }

ASDX_INLINE
void Min( FLOAT4 &v1, FLOAT4 &v2, FLOAT4 &result )
{ result = Min( v1, v2 ); }

ASDX_INLINE
FLOAT2 Max( FLOAT2 v1, FLOAT2 v2 )
{
    return FLOAT2( Max< float >( v1.x, v2.x ),
                    Max< float >( v1.y, v2.y ) );
}

ASDX_INLINE
FLOAT3 Max( FLOAT3 v1, FLOAT3 v2 )
{
    return FLOAT3( Max< float >( v1.x, v2.x ),
                    Max< float >( v1.y, v2.y ),
                    Max< float >( v1.z, v2.z ) );
}

ASDX_INLINE
FLOAT4 Max( FLOAT4 v1, FLOAT4 v2 )
{
    return FLOAT4( Max< float >( v1.x, v2.x ),
                    Max< float >( v1.y, v2.y ),
                    Max< float >( v1.z, v2.z ),
                    Max< float >( v1.w, v2.w ) );
}

ASDX_INLINE
void Max( FLOAT2 &v1, FLOAT2 &v2, FLOAT2 &result )
{ result = Max( v1, v2 ); }

ASDX_INLINE
void Max( FLOAT3 &v1, FLOAT3 &v2, FLOAT3 &result )
{ result = Max( v1, v2 ); }

ASDX_INLINE
void Max( FLOAT4 &v1, FLOAT4 &v2, FLOAT4 &result )
{ result = Max( v1, v2 ); }

ASDX_INLINE
FLOAT2 Reflect( FLOAT2 i, FLOAT2 n )
{
    FLOAT2 result;
    float dot = Dot( n, i );
    result.x = i.x - ( 2.0f * n.x ) * dot;
    result.y = i.y - ( 2.0f * n.y ) * dot;
    return result;
}

ASDX_INLINE
FLOAT3 Reflect( FLOAT3 i, FLOAT3 n )
{
    FLOAT3 result;
    float dot = Dot( n, i );
    result.x = i.x - ( 2.0f * n.x ) * dot;
    result.y = i.y - ( 2.0f * n.y ) * dot;
    result.z = i.z - ( 2.0f * n.z ) * dot;
    return result;
}

ASDX_INLINE
void Reflect( FLOAT2 &i, FLOAT2 &n, FLOAT2 &result )
{ result = Reflect( i, n ); }

ASDX_INLINE
void Reflect( FLOAT3 &i, FLOAT3 &n, FLOAT3 &result )
{ result = Reflect( i, n ); }

ASDX_INLINE
FLOAT3 Refract( FLOAT3 i, FLOAT3 n, float eta )
{
    float cosi = Dot( -i, n );
    float cost2 = 1.0f - eta * eta * ( 1.0f - cosi * cosi );
    FLOAT3 result = eta * i + ( ( eta * cosi - sqrtf( fabs( cost2 ) ) ) * n );
    return result * Sign< float >( cost2 );
}

ASDX_INLINE
void Refract( FLOAT3 &i, FLOAT3 &n, float eta, FLOAT3 &result )
{ result = Refract( i, n, eta ); }

ASDX_INLINE
FLOAT2 Barycentric( FLOAT2 v1, FLOAT2 v2, FLOAT2 v3, float f, float g )
{
    FLOAT2 result = v1 + f * ( v2 - v1 ) + g * ( v3 - v1 );
    return result;
}

ASDX_INLINE
FLOAT3 Barycentric( FLOAT3 v1, FLOAT3 v2, FLOAT3 v3, float f, float g )
{
    FLOAT3 result = v1 + f * ( v2 - v1 ) + g * ( v3 - v1 );
    return result;
}

ASDX_INLINE
FLOAT4 Barycentric( FLOAT4 v1, FLOAT4 v2, FLOAT4 v3, float f, float g )
{
    FLOAT4 result = v1 + f * ( v2 - v1 ) + g * ( v3 - v1 );
    return result;
}

ASDX_INLINE
void Barycentric( FLOAT2 &v1, FLOAT2 &v2, FLOAT2 &v3, float f, float g, FLOAT2 &result )
{ result = Barycentric( v1, v2, v3, f, g ); }

ASDX_INLINE
void Barycentric( FLOAT3 &v1, FLOAT3 &v2, FLOAT3 &v3, float f, float g, FLOAT3 &result )
{ result = Barycentric( v1, v2, v3, f, g ); }

ASDX_INLINE
void Barycentric( FLOAT4 &v1, FLOAT4 &v2, FLOAT4 &v3, float f, float g , FLOAT4 &result )
{ result = Barycentric( v1, v2, v3, f, g ); }

ASDX_INLINE
FLOAT2 Lerp( FLOAT2 v1, FLOAT2 v2, float t )
{
    FLOAT2 result = ( 1.0f - t ) * v1 + t * v2;
    return result;
}

ASDX_INLINE
FLOAT3 Lerp( FLOAT3 v1, FLOAT3 v2, float t )
{
    FLOAT3 result = ( 1.0f - t ) * v1 + t * v2;
    return result;
}

ASDX_INLINE
FLOAT4 Lerp( FLOAT4 v1, FLOAT4 v2, float t )
{
    FLOAT4 result = ( 1.0f - t ) * v1 + t * v2;
    return result;
}

ASDX_INLINE
void Lerp( FLOAT2 &v1, FLOAT2 &v2, float t, FLOAT2 &result )
{ result = Lerp( v1, v2, t ); }

ASDX_INLINE
void Lerp( FLOAT3 &v1, FLOAT3 &v2, float t, FLOAT3 &result )
{ result = Lerp( v1, v2, t ); }

ASDX_INLINE
void Lerp( FLOAT4 &v1, FLOAT4 &v2, float t, FLOAT4 &result )
{ result = Lerp( v1, v2, t ); }

ASDX_INLINE
FLOAT2 SmoothStep( FLOAT2 v1, FLOAT2 v2, float t )
{
    float s = Clamp< float >( t, 0.0f, 1.0f );
    return Lerp( v1, v2, ( s * s ) + ( 3.0f - ( 2.0f * s ) ) );
}

ASDX_INLINE
FLOAT3 SmoothStep( FLOAT3 v1, FLOAT3 v2, float t )
{
    float s = Clamp< float >( t, 0.0f, 1.0f );
    return Lerp( v1, v2, ( s * s ) + ( 3.0f - ( 2.0f * s ) ) );
}

ASDX_INLINE
FLOAT4 SmoothStep( FLOAT4 v1, FLOAT4 v2, float t )
{
    float s = Clamp< float >( t, 0.0f, 1.0f );
    return Lerp( v1, v2, ( s * s ) + ( 3.0f - ( 2.0f * s ) ) );
}

ASDX_INLINE
void SmoothStep( FLOAT2 &v1, FLOAT2 &v2, float t, FLOAT2 &result )
{ result = SmoothStep( v1, v2, t ); }

ASDX_INLINE
void SmoothStep( FLOAT3 &v1, FLOAT3 &v2, float t, FLOAT3 &result )
{ result = SmoothStep( v1, v2, t ); }

ASDX_INLINE
void SmoothStep( FLOAT4 &v1, FLOAT4 &v2, float t, FLOAT4 &result )
{ result = SmoothStep( v1, v2, t ); }

ASDX_INLINE
FLOAT2 Transform( FLOAT2 position, MATRIX matrix )
{
    return FLOAT2(
        ((position.x * matrix._11) + (position.y * matrix._21)) + matrix._41,
        ((position.x * matrix._12) + (position.y * matrix._22)) + matrix._42 );
}

ASDX_INLINE
FLOAT3 Transform( FLOAT3 position, MATRIX matrix )
{
    return FLOAT3(
        ( ((position.x * matrix._11) + (position.y * matrix._21)) + (position.z * matrix._31)) + matrix._41,
        ( ((position.x * matrix._12) + (position.y * matrix._22)) + (position.z * matrix._32)) + matrix._42,
        ( ((position.x * matrix._13) + (position.y * matrix._23)) + (position.z * matrix._33)) + matrix._43 );
}

ASDX_INLINE
FLOAT4 Transform( FLOAT4 position, MATRIX matrix )
{
    return FLOAT4(
        ( ( ((position.x * matrix._11) + (position.y * matrix._21)) + (position.z * matrix._31) ) + (position.w * matrix._41)),
        ( ( ((position.x * matrix._12) + (position.y * matrix._22)) + (position.z * matrix._32) ) + (position.w * matrix._42)),
        ( ( ((position.x * matrix._13) + (position.y * matrix._23)) + (position.z * matrix._33) ) + (position.w * matrix._43)),
        ( ( ((position.x * matrix._14) + (position.y * matrix._24)) + (position.z * matrix._43) ) + (position.w * matrix._44)) );
}

ASDX_INLINE
void Transform( FLOAT2 &position, MATRIX &matrix, FLOAT2 &result )
{ result = Transform( position, matrix ); }

ASDX_INLINE
void Transform( FLOAT3 &position, MATRIX &matrix, FLOAT3 &result )
{ result = Transform( position, matrix ); }

ASDX_INLINE
void Transform( FLOAT4 &position, MATRIX &matrix, FLOAT4 &result )
{ result = Transform( position, matrix ); }

ASDX_INLINE
FLOAT2 TransformNormal( FLOAT2 normal, MATRIX matrix )
{
    return FLOAT2(
        (normal.x * matrix._11) + (normal.y * matrix._21),
        (normal.x * matrix._12) + (normal.y * matrix._22) );
}

ASDX_INLINE
FLOAT3 TransformNormal( FLOAT3 normal, MATRIX matrix )
{
    return FLOAT3(
        ((normal.x * matrix._11) + (normal.y * matrix._21)) + (normal.z * matrix._31),
        ((normal.y * matrix._12) + (normal.y * matrix._22)) + (normal.z * matrix._32),
        ((normal.z * matrix._13) + (normal.y * matrix._23)) + (normal.z * matrix._33) );
}

ASDX_INLINE
void TransformNormal( FLOAT2 &normal, MATRIX &matrix, FLOAT2 &result )
{ result = TransformNormal( normal, matrix ); }

ASDX_INLINE
void TransformNormal( FLOAT3 &normal, MATRIX &matrix, FLOAT3 &result )
{ result = TransformNormal( normal, matrix ); }

ASDX_INLINE
FLOAT2 TransformCoordinate( FLOAT2 coords, MATRIX matrix )
{
    FLOAT4 tmp( coords.x, coords.y, 0, 1 );
    FLOAT4 result = Transform( tmp, matrix );
    return FLOAT2( result.x, result.y );
}

ASDX_INLINE
FLOAT3 TransformCoordinate( FLOAT3 coords, MATRIX matrix )
{
    FLOAT4 tmp( coords.x, coords.y, coords.z, 1 );
    FLOAT4 result = Transform( tmp, matrix );
    return FLOAT3( result.x, result.y, result.z );
}

ASDX_INLINE
void TransformCoordinate( FLOAT2 &coords, MATRIX &matrix, FLOAT2 &result )
{ result = TransformCoordinate( coords, matrix ); }

ASDX_INLINE
void TransformCoordinate( FLOAT3 &coords, MATRIX &matrix, FLOAT3 &result )
{ result = TransformCoordinate( coords, matrix ); }




/////////////////////////////////////////////////////////////////////////
// MATRIX structure
/////////////////////////////////////////////////////////////////////////
ASDX_INLINE
MATRIX::MATRIX()
{ /* DO_NOTHING */ }

ASDX_INLINE
MATRIX::MATRIX( const float* pf )
{
    assert( pf != 0 );
    memcpy( &_11, pf, sizeof(MATRIX) );
}

ASDX_INLINE
MATRIX::MATRIX( float f11, float f12, float f13, float f14,
                float f21, float f22, float f23, float f24,
                float f31, float f32, float f33, float f34,
                float f41, float f42, float f43, float f44 )
{
    _11 = f11; _12 = f12; _13 = f13; _14 = f14;
    _21 = f21; _22 = f22; _23 = f23; _24 = f24;
    _31 = f31; _32 = f32; _33 = f33; _34 = f34;
    _41 = f41; _42 = f42; _43 = f43; _44 = f44;
}

ASDX_INLINE float&
MATRIX::operator () ( unsigned int iRow, unsigned int iCol )
{ return m[iRow][iCol]; }

ASDX_INLINE float
MATRIX::operator () ( unsigned int iRow, unsigned int iCol ) const
{ return m[iRow][iCol]; }

ASDX_INLINE
MATRIX::operator float* ()
{ return (float*)&_11; }

ASDX_INLINE
MATRIX::operator const float* () const 
{ return (const float*)&_11; }

ASDX_INLINE MATRIX&
MATRIX::operator *= ( const MATRIX &mat )
{
    MATRIX tmp = (*this);
    (*this) = Multiply( tmp, mat );
    return (*this);
}

ASDX_INLINE MATRIX&
MATRIX::operator += ( const MATRIX& mat )
{
    _11 += mat._11; _12 += mat._12; _13 += mat._13; _14 += mat._14;
    _21 += mat._21; _22 += mat._22; _23 += mat._23; _24 += mat._24;
    _31 += mat._31; _32 += mat._32; _33 += mat._33; _34 += mat._34;
    _41 += mat._41; _42 += mat._42; _43 += mat._43; _44 += mat._44;
    return (*this);
}

ASDX_INLINE MATRIX&
MATRIX::operator -= ( const MATRIX& mat )
{
    _11 -= mat._11; _12 -= mat._12; _13 -= mat._13; _14 -= mat._14;
    _21 -= mat._21; _22 -= mat._22; _23 -= mat._23; _24 -= mat._24;
    _31 -= mat._31; _32 -= mat._32; _33 -= mat._33; _34 -= mat._34;
    _41 -= mat._41; _42 -= mat._42; _43 -= mat._43; _44 -= mat._44;
    return (*this);
}

ASDX_INLINE MATRIX&
MATRIX::operator *= ( float f )
{
    _11 *= f; _12 *= f; _13 *= f; _14 *= f;
    _21 *= f; _22 *= f; _23 *= f; _24 *= f;
    _31 *= f; _32 *= f; _33 *= f; _34 *= f;
    _41 *= f; _42 *= f; _43 *= f; _44 *= f;
    return (*this);
}

ASDX_INLINE MATRIX&
MATRIX::operator /= ( float f )
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f/f;
    _11 *= fInv; _12 *= fInv; _13 *= fInv; _14 *= fInv;
    _21 *= fInv; _22 *= fInv; _23 *= fInv; _24 *= fInv;
    _31 *= fInv; _32 *= fInv; _33 *= fInv; _34 *= fInv;
    _41 *= fInv; _42 *= fInv; _43 *= fInv; _44 *= fInv;
    return (*this);
}

ASDX_INLINE MATRIX&
MATRIX::operator = ( const MATRIX& value )
{
    memcpy( &_11, &value._11, sizeof(MATRIX) );
    return (*this);
}

ASDX_INLINE MATRIX
MATRIX::operator + () const
{ 
    return MATRIX( _11, _12, _13, _14,
                   _21, _22, _23, _24,
                   _31, _32, _33, _34,
                   _41, _42, _43, _44 );
}

ASDX_INLINE MATRIX
MATRIX::operator - () const
{
    return MATRIX( -_11, -_12, -_13, -_14,
                   -_21, -_22, -_23, -_24,
                   -_31, -_32, -_33, -_34,
                   -_41, -_42, -_43, -_44 );
}

ASDX_INLINE MATRIX
MATRIX::operator * ( MATRIX mat ) const
{
    MATRIX tmp = (*this);
    MATRIX result = Multiply( tmp, mat );
    return result;
}

ASDX_INLINE MATRIX
MATRIX::operator + ( MATRIX mat ) const
{
    return MATRIX( _11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
                   _21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
                   _31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
                   _41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44 );
}

ASDX_INLINE MATRIX
MATRIX::operator - ( MATRIX mat ) const
{
    return MATRIX( _11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
                   _21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
                   _31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
                   _41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44 );
}

ASDX_INLINE MATRIX
MATRIX::operator * ( float f ) const
{
    return MATRIX( _11 * f, _12 * f, _13 * f, _14 * f,
                   _21 * f, _22 * f, _23 * f, _24 * f,
                   _31 * f, _32 * f, _33 * f, _34 * f,
                   _41 * f, _42 * f, _43 * f, _44 * f );
}

ASDX_INLINE MATRIX
MATRIX::operator / ( float f ) const
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f/f;
    return MATRIX( _11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv,
                   _21 * fInv, _22 * fInv, _23 * fInv, _24 * fInv,
                   _31 * fInv, _32 * fInv, _33 * fInv, _34 * fInv,
                   _41 * fInv, _42 * fInv, _43 * fInv, _44 * fInv );
}

ASDX_INLINE MATRIX
operator * ( float f, const MATRIX& mat )
{
    return MATRIX( f * mat._11, f * mat._12, f * mat._13, f * mat._14,
                   f * mat._21, f * mat._22, f * mat._23, f * mat._24,
                   f * mat._31, f * mat._32, f * mat._33, f * mat._34,
                   f * mat._41, f * mat._42, f * mat._43, f * mat._44 );
}

ASDX_INLINE bool
MATRIX::operator == ( const MATRIX& mat ) const
{ return ( 0 == memcmp( this, &mat, sizeof( MATRIX ) ) ); }

ASDX_INLINE bool
MATRIX::operator != ( const MATRIX& mat ) const
{ return ( 0 != memcmp( this, &mat, sizeof( MATRIX ) ) ); }

ASDX_INLINE float
MATRIX::Determinant() const
{
    float det =
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
        _14*_22*_33*_41 - _14*_23*_31*_42;
    return det;
}

ASDX_INLINE void
MATRIX::Identity()
{
    _11 = _22 = _33 = _44 = 1.0f;
    _12 = _13 = _14 =
    _21 = _23 = _24 =
    _31 = _32 = _34 =
    _41 = _42 = _43 = 0.0f;
}

ASDX_INLINE
MATRIX::MATRIX( const DirectX::XMFLOAT4X4& value )
{
    memcpy( &_11, &value._11, sizeof(float) * 16 );
}

ASDX_INLINE MATRIX&
MATRIX::operator = ( const DirectX::XMFLOAT4X4& value )
{
    memcpy( &_11, &value._11, sizeof(float) * 16 );
    return (*this);
}

/////////////////////////////////////////////////////////////////////////
// MATRIX Methods
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE
MATRIX& Identity( MATRIX &value )
{
    value.m[0][1] = value.m[0][2] = value.m[0][3] =
    value.m[1][0] = value.m[1][2] = value.m[1][3] =
    value.m[2][0] = value.m[2][1] = value.m[2][3] =
    value.m[3][0] = value.m[3][1] = value.m[3][2] = 0.0f;

    value.m[0][0] = value.m[1][1] = value.m[2][2] = value.m[3][3] = 1.0f;
    return value;
}

ASDX_INLINE
bool IsIdentity( const MATRIX &value )
{
    return (
        IsNearlyEqual( value.m[0][0], 1.0f ) && IsNearZero( value.m[0][1] ) && IsNearZero( value.m[0][2] ) && IsNearZero( value.m[0][3] ) &&
        IsNearZero( value.m[1][0] ) && IsNearlyEqual( value.m[1][1], 1.0f ) && IsNearZero( value.m[1][2] ) && IsNearZero( value.m[1][3] ) &&
        IsNearZero( value.m[2][0] ) && IsNearZero( value.m[2][1] ) && IsNearlyEqual( value.m[2][2] , 1.0f ) && IsNearZero( value.m[2][3] ) &&
        IsNearZero( value.m[3][0] ) && IsNearZero( value.m[3][1] ) && IsNearZero( value.m[3][2] ) && IsNearlyEqual( value.m[3][3], 1.0f ) );
}

ASDX_INLINE
MATRIX Transpose( MATRIX value )
{
    return MATRIX(
        value._11, value._21, value._31, value._41,
        value._12, value._22, value._32, value._42,
        value._13, value._23, value._33, value._43,
        value._14, value._24, value._34, value._44 );
}

ASDX_INLINE
void Transpose( MATRIX &value, MATRIX &result )
{ result = Transpose( value ); }

ASDX_INLINE
MATRIX Multiply( MATRIX value1, MATRIX value2 )
{
    MATRIX tmp;
    for ( int i=0; i<4; i++ )
        for ( int j=0; j<4; j++ )
            tmp.m[i][j] = value1.m[i][0] * value2.m[0][j] +
                          value1.m[i][1] * value2.m[1][j] +
                          value1.m[i][2] * value2.m[2][j] +
                          value1.m[i][3] * value2.m[3][j];
    return tmp;
}

ASDX_INLINE
void Multiply( MATRIX &value1, MATRIX &value2, MATRIX &result )
{ result = Multiply( value1, value2 ); }

ASDX_INLINE
MATRIX Multiply( MATRIX value, float scaleFactor )
{
    return MATRIX(
        value._11*scaleFactor, value._12*scaleFactor, value._13*scaleFactor, value._14*scaleFactor,
        value._21*scaleFactor, value._22*scaleFactor, value._23*scaleFactor, value._24*scaleFactor,
        value._31*scaleFactor, value._32*scaleFactor, value._33*scaleFactor, value._34*scaleFactor,
        value._41*scaleFactor, value._42*scaleFactor, value._43*scaleFactor, value._44*scaleFactor
        );
}

ASDX_INLINE
void Multiply( MATRIX &value, float scaleFactor, MATRIX &result )
{ result = Multiply( value, scaleFactor ); }

ASDX_INLINE
MATRIX MultiplyTranspose( MATRIX value1, MATRIX value2 )
{
    MATRIX result;
    result = Multiply( value1, value2 );
    result = Transpose( result );
    return result;
}

ASDX_INLINE
void MultiplyTranspose( MATRIX &value1, MATRIX &value2, MATRIX &result )
{ result = MultiplyTranspose( value1, value2 ); }

ASDX_INLINE 
MATRIX Invert( MATRIX value )
{
    MATRIX result;
    float det = value.Determinant();
    assert( !IsNearZero( det ) );
    float invDet = 1.0f / det;

    result._11 = value._22*value._33*value._44 + value._23*value._34*value._42 + value._24*value._32*value._43 - value._22*value._34*value._43 - value._23*value._32*value._44 - value._24*value._33*value._42;
    result._12 = value._12*value._34*value._43 + value._13*value._32*value._44 + value._14*value._33*value._42 - value._12*value._33*value._44 - value._13*value._34*value._42 - value._14*value._32*value._43;
    result._13 = value._12*value._23*value._44 + value._13*value._24*value._42 + value._14*value._22*value._43 - value._12*value._24*value._43 - value._13*value._22*value._44 - value._14*value._23*value._42;
    result._14 = value._12*value._24*value._33 + value._13*value._22*value._34 + value._14*value._23*value._32 - value._12*value._23*value._34 - value._13*value._24*value._32 - value._14*value._22*value._33;

    result._21 = value._21*value._34*value._43 + value._23*value._31*value._44 + value._24*value._33*value._41 - value._21*value._33*value._44 - value._23*value._34*value._41 - value._24*value._31*value._43;
    result._22 = value._11*value._33*value._44 + value._13*value._34*value._41 + value._14*value._31*value._43 - value._11*value._34*value._43 - value._13*value._31*value._44 - value._14*value._33*value._41;
    result._23 = value._11*value._24*value._43 + value._13*value._21*value._44 + value._14*value._23*value._41 - value._11*value._23*value._44 - value._13*value._24*value._41 - value._14*value._21*value._43;
    result._24 = value._11*value._23*value._34 + value._13*value._24*value._31 + value._14*value._21*value._33 - value._11*value._24*value._33 - value._13*value._21*value._34 - value._14*value._23*value._31;

    result._31 = value._21*value._32*value._44 + value._22*value._34*value._41 + value._24*value._31*value._42 - value._21*value._34*value._42 - value._22*value._31*value._44 - value._24*value._32*value._41;
    result._32 = value._11*value._34*value._42 + value._12*value._31*value._44 + value._14*value._32*value._41 - value._11*value._32*value._44 - value._12*value._34*value._41 - value._14*value._31*value._42;
    result._33 = value._11*value._22*value._44 + value._12*value._24*value._41 + value._14*value._21*value._42 - value._11*value._24*value._42 - value._12*value._21*value._44 - value._14*value._22*value._41;
    result._34 = value._11*value._24*value._32 + value._12*value._21*value._34 + value._14*value._22*value._31 - value._11*value._22*value._34 - value._12*value._24*value._31 - value._14*value._21*value._32;

    result._41 = value._21*value._33*value._42 + value._22*value._31*value._43 + value._23*value._32*value._41 - value._21*value._32*value._43 - value._22*value._33*value._41 - value._23*value._31*value._42;
    result._42 = value._11*value._32*value._43 + value._12*value._33*value._41 + value._13*value._31*value._42 - value._11*value._33*value._42 - value._12*value._31*value._43 - value._13*value._32*value._41;
    result._43 = value._11*value._23*value._42 + value._12*value._21*value._43 + value._13*value._22*value._41 - value._11*value._22*value._43 - value._12*value._23*value._41 - value._13*value._21*value._42;
    result._44 = value._11*value._22*value._33 + value._12*value._23*value._31 + value._13*value._21*value._32 - value._11*value._23*value._32 - value._12*value._21*value._33 - value._13*value._22*value._31;

    result = invDet * result;
    return result;
}

ASDX_INLINE
void Invert( MATRIX &value, MATRIX &result )
{ result = Invert( value ); }

ASDX_INLINE
MATRIX CreateScale( float scale )
{
    return MATRIX(
        scale, 0.0f, 0.0f, 0.0f,
        0.0f, scale, 0.0f, 0.0f,
        0.0f, 0.0f, scale, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f );
}

ASDX_INLINE 
void CreateScale( float scale, MATRIX &result )
{ result = CreateScale( scale ); }

ASDX_INLINE 
MATRIX CreateScale( float xScale, float yScale, float zScale )
{
    return MATRIX(
        xScale, 0.0f, 0.0f, 0.0f,
        0.0f, yScale, 0.0f, 0.0f,
        0.0f, 0.0f, zScale, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f );
}

ASDX_INLINE
void CreateScale( float xScale, float yScale, float zScale, MATRIX &result )
{ result = CreateScale( xScale, yScale, zScale ); }

ASDX_INLINE
MATRIX CreateScale( FLOAT3 scales )
{
    return MATRIX(
        scales.x, 0.0f, 0.0f, 0.0f,
        0.0f, scales.y, 0.0f, 0.0f,
        0.0f, 0.0f, scales.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f ); 
}

ASDX_INLINE
void CreateScale( FLOAT3 &scales, MATRIX &result )
{ result = CreateScale( scales ); }

ASDX_INLINE 
MATRIX CreateTranslation( float xPos, float yPos, float zPos )
{
    return MATRIX(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        xPos, yPos, zPos, 1.0f );
}

ASDX_INLINE
void CreateTranslation( float xPos, float yPos, float zPos, MATRIX &result )
{ result = CreateTranslation( xPos, yPos, zPos ); }

ASDX_INLINE
MATRIX CreateTranslation( FLOAT3 pos )
{
    return MATRIX(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        pos.x, pos.y, pos.z, 1.0f );
}

ASDX_INLINE
void CreateTranslation( FLOAT3 &pos, MATRIX &result )
{ result = CreateTranslation( pos ); }
 

ASDX_INLINE 
MATRIX CreateRotationX( float radian )
{
    return MATRIX(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cosf(radian), sinf(radian), 0.0f,
        0.0f, -sinf(radian), cosf(radian), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f );
}

ASDX_INLINE 
void CreateRotationX( float radian, MATRIX &result )
{ result = CreateRotationX( radian ); }

ASDX_INLINE 
MATRIX CreateRotationY( float radian )
{
    return MATRIX(
        cosf(radian), 0.0f, -sinf(radian), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sinf(radian), 0.0f, cosf(radian), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f );
}

ASDX_INLINE
void CreateRotationY( float radian, MATRIX &result )
{ result = CreateRotationY( radian ); }

ASDX_INLINE
MATRIX CreateRotationZ( float radian )
{
    return MATRIX( 
        cosf(radian), sinf(radian), 0.0f, 0.0f,
        -sinf(radian), cosf(radian), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f );
}

ASDX_INLINE
void CreateRotationZ( float radian, MATRIX &result )
{ result = CreateRotationZ( radian ); }

ASDX_INLINE
MATRIX CreateFromQuaternion( QUATERNION qua )
{
    MATRIX result;
    float xx = qua.x*qua.x;
    float yy = qua.y*qua.y;
    float zz = qua.z*qua.z;
    float xy = qua.x*qua.y;
    float yw = qua.y*qua.w;
    float yz = qua.y*qua.z;
    float xw = qua.x*qua.w;
    float zx = qua.z*qua.x;
    float zw = qua.z*qua.w;

    result._11 = 1.0f - (2.0f * (yy + zz));
    result._12 = 2.0f * (xy + zw);
    result._13 = 2.0f * (zx - yz);
    result._14 = 0.0f;

    result._21 = 2.0f * (xy - zw);
    result._22 = 1.0f - (2.0f * (zz + xx)); 
    result._23 = 2.0f * (yz + xw);
    result._24 = 0.0f;

    result._31 = 2.0f * (zx + yw); 
    result._32 = 2.0f * (yz - xw);
    result._33 = 1.0f - (2.0f * (yy + xx));
    result._34 = 0.0f;

    result._41 = 0.0f;
    result._42 = 0.0f;
    result._43 = 0.0f;
    result._44 = 1.0f;
    
    return result;
}

ASDX_INLINE
void CreateFromQuaternion( QUATERNION &qua, MATRIX &result )
{ result = CreateFromQuaternion( qua ); }

ASDX_INLINE
MATRIX CreateFromAxisAngle( FLOAT3 axis, float radian )
{
    MATRIX result;
    float sine = sinf(radian);
    float cosine = cosf(radian);
    float ab, bc, ca, tx, ty, tz, a = 1.0f -cosine;
    ab = axis.x * axis.y * a;
    bc = axis.y * axis.z * a;
    ca = axis.z * axis.x * a;
    tx = axis.x * axis.x;
    ty = axis.y * axis.y;
    tz = axis.z * axis.z;

    result._11 = tx + cosine * (1.0f - tx);
    result._12 = ab + axis.z * sine;
    result._13 = ca - axis.y * sine;
    result._14 = 0.0f;

    result._21 = ab - axis.z * sine;
    result._22 = ty + cosine * (1.0f - ty);
    result._23 = bc + axis.x * sine;
    result._24 = 0.0f;

    result._31 = ca + axis.y * sine;
    result._32 = bc - axis.x * sine;
    result._33 = tz + cosine * (1.0f - tz);
    result._34 = 0.0f;

    result._41 = 0.0f;
    result._42 = 0.0f;
    result._43 = 0.0f;
    result._44 = 1.0f;

    return result;
}

ASDX_INLINE
void CreateFromAxisAngle( FLOAT3 &axis, float radian, MATRIX &result )
{ result = CreateFromAxisAngle( axis, radian ); }

ASDX_INLINE
MATRIX CreateLookAt( FLOAT3 cameraPosition, FLOAT3 cameraTarget, FLOAT3 cameraUpVector )
{
    MATRIX result;
    FLOAT3 zaxis = cameraPosition - cameraTarget;
    zaxis.Normalize();

    FLOAT3 xaxis = Cross(cameraUpVector, zaxis);
    xaxis.Normalize();

    FLOAT3 yaxis = Cross(zaxis, xaxis);
    yaxis.Normalize();

    result._11 = xaxis.x;
    result._12 = yaxis.x;
    result._13 = zaxis.x;
    result._14 = 0.0f;

    result._21 = xaxis.y;
    result._22 = yaxis.y;
    result._23 = zaxis.y;
    result._24 = 0.0f;

    result._31 = xaxis.z;
    result._32 = yaxis.z;
    result._33 = zaxis.z;
    result._34 = 0.0f;

    result._41 = -Dot(xaxis, cameraPosition);
    result._42 = -Dot(yaxis, cameraPosition);
    result._43 = -Dot(zaxis, cameraPosition);
    result._44 = 1.0f;

    return result;
}

ASDX_INLINE
void CreateLookAt( FLOAT3 &cameraPosition, FLOAT3 &cameraTarget, FLOAT3 &cameraUpVector, MATRIX &result )
{ result = CreateLookAt( cameraPosition, cameraTarget, cameraUpVector ); }

ASDX_INLINE 
MATRIX CreatePerspective( float width, float height, float nearPlaneDistance, float farPlaneDistance )
{
    assert( !IsNearZero( width ) );
    assert( !IsNearZero( height ) );
    assert( !IsNearZero( nearPlaneDistance - farPlaneDistance ) );
    MATRIX result;
    result._11 = 2.0f*nearPlaneDistance/width;
    result._12 = 0.0f;
    result._13 = 0.0f;
    result._14 = 0.0f;

    result._21 = 0.0f;
    result._22 = 2.0f*nearPlaneDistance/height;
    result._23 = 0.0f;
    result._24 = 0.0f;

    result._31 = 0.0f;
    result._32 = 0.0f;
    result._33 = farPlaneDistance/(nearPlaneDistance - farPlaneDistance);
    result._34 = (nearPlaneDistance*farPlaneDistance)/(nearPlaneDistance -farPlaneDistance);
    
    result._41 = 0.0f;
    result._42 = 0.0f;
    result._43 = -1.0f;
    result._44 = 0.0f;

    return result;
}

ASDX_INLINE
void CreatePerspective( float width, float height, float nearPlaneDistance, float farPlaneDistance, MATRIX &result )
{ result = CreatePerspective( width, height, nearPlaneDistance, farPlaneDistance ); }

ASDX_INLINE
MATRIX CreatePerspectiveFieldOfView( float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance )
{
    assert( !IsNearZero( aspectRatio ) );
    assert( !IsNearZero( nearPlaneDistance - farPlaneDistance ) );
    MATRIX result;
    float yScale = 1.0f/tanf(fieldOfView/2.0f);
    float xScale = yScale/aspectRatio;
    result._11 = xScale;
    result._12 = 0.0f;
    result._13 = 0.0f;
    result._14 = 0.0f;

    result._21 = 0.0f;
    result._22 = yScale;
    result._23 = 0.0f;
    result._24 = 0.0f;

    result._31 = 0.0f;
    result._32 = 0.0f;
    result._33 = farPlaneDistance/(nearPlaneDistance - farPlaneDistance);
    result._34 = -1.0f;

    result._41 = 0.0f;
    result._42 = 0.0f;
    result._43 = (nearPlaneDistance*farPlaneDistance)/(nearPlaneDistance - farPlaneDistance);
    result._44 = 0.0f;

    return result;
}

ASDX_INLINE
void CreatePerspectiveFieldOfView( float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance, MATRIX &result )
{ result = CreatePerspectiveFieldOfView( fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance ); }

ASDX_INLINE
MATRIX CreatePerspectiveOffcenter( float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance )
{
    assert( !IsNearZero( right - left ) );
    assert( !IsNearZero( top - bottom ) );
    assert( !IsNearZero( nearPlaneDistance - farPlaneDistance ) );
    MATRIX result;
    result._11 = 2.0f*nearPlaneDistance/(right - left);
    result._12 = 0.0f;
    result._13 = 0.0f;
    result._14 = 0.0f;

    result._21 = 0.0f;
    result._22 = 2.0f*nearPlaneDistance/(top - bottom);
    result._23 = 0.0f;
    result._24 = 0.0f;

    result._31 = (left + right)/(right - left);
    result._32 = (top + bottom)/(top -bottom);
    result._33 = farPlaneDistance/(nearPlaneDistance - farPlaneDistance);
    result._34 = -1.0f;

    result._41 = 0.0f;
    result._42 = 0.0f;
    result._43 = nearPlaneDistance*farPlaneDistance/(nearPlaneDistance - farPlaneDistance);
    result._44 = 0.0f;

    return result;
}

ASDX_INLINE
void CreatePerspectiveOffcenter( float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance, MATRIX &result )
{ result = CreatePerspectiveOffcenter( left, right, bottom, top, nearPlaneDistance, farPlaneDistance ); }

ASDX_INLINE
MATRIX CreateOrthographic( float width, float height, float nearPlaneDistance, float farPlaneDistance )
{
    assert( !IsNearZero( width ) );
    assert( !IsNearZero( height ) );
    assert( !IsNearZero( nearPlaneDistance - farPlaneDistance ) );

    MATRIX result;
    result._11 = 2.0f/width;
    result._12 = 0.0f;
    result._13 = 0.0f;
    result._14 = 0.0f;
    
    result._21 = 0.0f;
    result._22 = 2.0f/height;
    result._23 = 0.0f;
    result._24 = 0.0f;

    result._31 = 0.0f;
    result._32 = 0.0f;
    result._33 = 1.0f/(nearPlaneDistance - farPlaneDistance);
    result._34 = 0.0f;

    result._41 = 0.0f;
    result._42 = 0.0f;
    result._43 = nearPlaneDistance/(nearPlaneDistance - farPlaneDistance);
    result._44 = 1.0f;

    return result;
}

ASDX_INLINE
void CreateOrthographic( float width, float height, float nearPlaneDistance, float farPlaneDistance, MATRIX &result )
{ result = CreateOrthographic( width, height, nearPlaneDistance, farPlaneDistance ); }

ASDX_INLINE
MATRIX CreateShadow( FLOAT4 lightDirection, PLANE plane )
{
    MATRIX result;
    PLANE p = Normalize( plane );
    FLOAT4 l = Normalize( lightDirection );
    float d = Dot( p, l );

    result._11 = p.a * l.x + d;
    result._12 = p.a * l.y;
    result._13 = p.a * l.z;
    result._14 = p.a * l.w;

    result._21 = p.b * l.x;
    result._22 = p.b * l.y + d;
    result._23 = p.b * l.z;
    result._24 = p.b * l.w;

    result._31 = p.c * l.x;
    result._32 = p.c + l.y;
    result._33 = p.c * l.z + d;
    result._34 = p.c * l.w;

    result._41 = p.d * l.x;
    result._42 = p.d * l.y;
    result._43 = p.d * l.z;
    result._44 = p.d * l.w + d;

    return result;
}

ASDX_INLINE
void CreateShadow( FLOAT4 &lightDirection, PLANE &plane, MATRIX &result )
{ result = CreateShadow( lightDirection, plane ); }

ASDX_INLINE
MATRIX CreateReflect( PLANE plane )
{
    MATRIX result;
    result._11 = -2.0f * plane.a * plane.a + 1.0f;
    result._12 = -2.0f * plane.b * plane.a;
    result._13 = -2.0f * plane.c * plane.a;
    result._14 = 0.0f;

    result._21 = -2.0f * plane.a * plane.b;
    result._22 = -2.0f * plane.b * plane.b + 1.0f;
    result._23 = -2.0f * plane.c * plane.b;
    result._24 = 0.0f;

    result._31 = -2.0f * plane.a * plane.c;
    result._32 = -2.0f * plane.b * plane.c;
    result._33 = -2.0f * plane.c * plane.c + 1.0f;
    result._34 = 0.0f;

    result._41 = -2.0f * plane.a * plane.d;
    result._42 = -2.0f * plane.b * plane.d;
    result._43 = -2.0f * plane.c * plane.d;
    result._44 = 1.0f;
    return result;
}

ASDX_INLINE
void CreateReflect( PLANE &plane, MATRIX &result )
{ result = CreateReflect( plane ); }

ASDX_INLINE
MATRIX Lerp( MATRIX value1, MATRIX value2, float amount )
{
    MATRIX result;
    result = (1.0f - amount) * value1 + amount * value2;
    return result;
}

ASDX_INLINE
void Lerp( MATRIX &value1, MATRIX &value2, float amount, MATRIX &result )
{ result = Lerp( value1, value2, amount ); }



/////////////////////////////////////////////////////////////////////////
// QUATERNION
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE
QUATERNION::QUATERNION()
{
    x = 0;
    y = 0;
    z = 0;
    w = 0;
}

ASDX_INLINE
QUATERNION::QUATERNION( const float* pf )
{
    assert( pf != 0 );
    x = pf[ 0 ];
    y = pf[ 1 ];
    z = pf[ 2 ];
    w = pf[ 3 ];
}

ASDX_INLINE
QUATERNION::QUATERNION( float fx, float fy, float fz, float fw )
{ 
    x = fx;
    y = fy;
    z = fz;
    w = fw;
}

ASDX_INLINE
QUATERNION::operator float* ()
{ return (float*)&x; }

ASDX_INLINE
QUATERNION::operator const float* () const
{ return (const float*)&x; }

ASDX_INLINE QUATERNION&
QUATERNION::operator += ( const QUATERNION& q )
{
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return (*this);
}

ASDX_INLINE QUATERNION&
QUATERNION::operator -= ( const QUATERNION& q )
{
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return (*this);
}

ASDX_INLINE QUATERNION&
QUATERNION::operator *= ( const QUATERNION& q )
{
    QUATERNION tmp = (*this);
    (*this) = Multiply( tmp, q );
    return (*this);
}

ASDX_INLINE QUATERNION&
QUATERNION::operator *= ( float f )
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return (*this);
}

ASDX_INLINE QUATERNION&
QUATERNION::operator /= ( float f )
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
    w *= fInv;
    return (*this);
}

ASDX_INLINE QUATERNION
QUATERNION::operator + () const
{ return (*this); }

ASDX_INLINE QUATERNION
QUATERNION::operator - () const
{ return QUATERNION( -x, -y, -z, -w ); }

ASDX_INLINE QUATERNION
QUATERNION::operator + ( QUATERNION q ) const
{ return QUATERNION( x + q.x, y + q.y, z + q.z, w + q.z ); }

ASDX_INLINE QUATERNION
QUATERNION::operator - ( QUATERNION q ) const
{ return QUATERNION( x - q.x, y - q.y, z - q.z, w - q.z ); }

ASDX_INLINE QUATERNION
QUATERNION::operator * ( QUATERNION q ) const
{ 
    QUATERNION qT;
    qT = Multiply( (*this), q );
    return qT;
}

ASDX_INLINE QUATERNION
QUATERNION::operator * ( float f ) const
{ return QUATERNION( x * f, y * f, z * f, w *f ); }

ASDX_INLINE QUATERNION
QUATERNION::operator / ( float f ) const
{ 
    assert( !IsNearZero( f ) );
    float fInv = 1.0f/f;
    return QUATERNION( x * fInv, y * fInv, z * fInv, w * fInv ); 
}

ASDX_INLINE QUATERNION
operator * ( float f, const QUATERNION& q )
{ return QUATERNION( f * q.x, f * q.y, f * q.z, f * q.w ); }

ASDX_INLINE bool
QUATERNION::operator == ( const QUATERNION& q ) const
{ return ( (x==q.x) && (y==q.y) && (z==q.z) && (w==q.w) ); }

ASDX_INLINE bool
QUATERNION::operator != ( const QUATERNION& q ) const
{ return ( (x!=q.x) || (y!=q.y) || (z!=q.z) || (w!=q.w) ); }

ASDX_INLINE float
QUATERNION::Length() const
{ return sqrtf( x * x + y * y + z * z + w * w ); }

ASDX_INLINE float
QUATERNION::LengthSquared() const
{ return ( x * x + y * y + z * z + w * w ); }

ASDX_INLINE void
QUATERNION::Conjugate()
{ 
    x = -x;
    y = -y;
    z = -z;
}

ASDX_INLINE QUATERNION&
QUATERNION::Normalize()
{
    float mag = Length();
    assert( !IsNearZero( mag ) );
    float invMag = 1.0f/mag;
    x *= invMag;
    y *= invMag;
    z *= invMag;
    w *= invMag;
    return (*this);
}

/////////////////////////////////////////////////////////////////////////
// QUATERNION Methods
//////////////////////////////////////////////////////////////////////////

ASDX_INLINE
float Dot( QUATERNION q1, QUATERNION q2 )
{ return (q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w); }

ASDX_INLINE
void Dot( QUATERNION &q1, QUATERNION &q2, float &result )
{ result = Dot( q1, q2 ); }

ASDX_INLINE
QUATERNION Conjugate( QUATERNION qua )
{ return QUATERNION( -qua.x, -qua.y, -qua.z, qua.w ); }

ASDX_INLINE
void Conjugate( QUATERNION &qua, QUATERNION &result )
{ result = Conjugate( qua ); }

ASDX_INLINE
QUATERNION Normalize( QUATERNION qua )
{
    QUATERNION result;
    float mag = qua.Length();
    assert( !IsNearZero( mag ) );
    float magInv = 1.0f/mag;
    result.x *= magInv;
    result.y *= magInv;
    result.z *= magInv;
    result.w *= magInv;
    return result;
}

ASDX_INLINE
void Normalize( QUATERNION &qua, QUATERNION &result )
{ result = Normalize( qua ); }

ASDX_INLINE
QUATERNION Inverse( QUATERNION qua )
{
    QUATERNION result( -qua.x, -qua.y, -qua.z, qua.w );
    result.Normalize();
    return result;
}

ASDX_INLINE
void Inverse( QUATERNION &qua, QUATERNION &result )
{ result = Inverse( qua ); }

ASDX_INLINE
QUATERNION CreateFromYawPitchRoll( float yaw, float pitch, float roll )
{
    QUATERNION result;
    float sr = sinf( roll * 0.5f );
    float cr = cosf( roll * 0.5f );
    float sp = sinf( pitch * 0.5f );
    float cp = cosf( pitch * 0.5f );
    float sy = sinf( yaw * 0.5f );
    float cy = cosf( yaw * 0.5f );

    result.x = -(sy*sp*cr) + (cy*cp*sr);
    result.y = (cy*sp*cr) + (sy*cp*sr);
    result.z = -(cy*sp*sr) + (sy*cp*cr);
    result.w = (cy*cp*cr) + (sy*sp*sr);
    return result;
}

ASDX_INLINE
void CreateFromYawPitchRoll( float yaw, float pitch, float roll, QUATERNION &result )
{ result = CreateFromYawPitchRoll( yaw, pitch, roll ); }
    
ASDX_INLINE
QUATERNION Slerp( QUATERNION q1, QUATERNION q2, float t )
{
    if ( t <= 0.0f ) return q1;
    if ( t >= 1.0f ) return q2;

    float cosOmega = Dot(q1, q2);
    float q1x = q1.x;
    float q1y = q1.y;
    float q1z = q1.z;
    float q1w = q1.w;

    if ( cosOmega < 0.0f )
    {
        q1w = -q1w;
        q1x = -q1x;
        q1y = -q1y;
        q1z = -q1z;
        cosOmega -= cosOmega;
    }

    float k1, k2;
    if ( cosOmega > 0.9999f )
    {
        k1 = 1.0f - t;
        k1 = t;
    }
    else
    {
        float sinOmega = sqrtf(1.0f - (cosOmega*cosOmega));
        float omega = atan2f(sinOmega, cosOmega);
        float sinOmegaInv  = 1.0f/sinOmega;
        k1 = sinf((1.0f-t)*omega)*sinOmegaInv;
        k2 = sinf(t*omega)*sinOmegaInv;
    }
    return QUATERNION( 
        (k1*q1.x) + (k2*q1.x),
        (k1*q1.y) + (k2*q1.y),
        (k1*q1.z) + (k2*q1.z),
        (k1*q1.w) + (k2*q1.w)
        );
}

ASDX_INLINE
void Slerp( QUATERNION &q1, QUATERNION &q2, float t, QUATERNION &result )
{ result = Slerp( q1, q2, t ); }

ASDX_INLINE
QUATERNION Squad( QUATERNION qua, QUATERNION a, QUATERNION b, QUATERNION c, float t )
{ return Slerp( Slerp(qua, c, t ), Slerp( a, b, t ), 2.0f * t * ( 1.0f - t ) ); }

ASDX_INLINE
void Squad( QUATERNION &qua, QUATERNION &a, QUATERNION &b, QUATERNION &c, float t, QUATERNION &result )
{ result = Squad( qua, a, b, c, t ); }



/////////////////////////////////////////////////////////////////////////
// PLANE
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE 
PLANE::PLANE( const float* pf )
{
    assert( pf != 0 );
    a = pf[ 0 ];
    b = pf[ 1 ];
    c = pf[ 2 ];
    d = pf[ 3 ];
}

ASDX_INLINE
PLANE::PLANE( float fa, float fb, float fc, float fd )
{
    a = fa;
    b = fb;
    c = fc;
    d = fd;
}

ASDX_INLINE
PLANE::operator float* ()
{ return (float*)&a; }

ASDX_INLINE
PLANE::operator const float* () const
{ return (const float*)&a; }

ASDX_INLINE PLANE&
PLANE::operator *= ( float f )
{ 
    a *= f;
    b *= f;
    c *= f;
    d *= f;
    return (*this);
}

ASDX_INLINE PLANE&
PLANE::operator /= ( float f )
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f/f;
    a *= fInv;
    b *= fInv;
    c *= fInv;
    d *= fInv;
    return (*this);
}

ASDX_INLINE PLANE
PLANE::operator + () const
{ return (*this); }

ASDX_INLINE PLANE
PLANE::operator - () const
{ return PLANE( -a, -b, -c, -d ); }

ASDX_INLINE PLANE
PLANE::operator * ( float f ) const
{ return PLANE( a * f, b * f, c * f, d * f ); }

ASDX_INLINE PLANE
PLANE::operator / ( float f ) const
{
    assert( !IsNearZero( f ) );
    float fInv = 1.0f/f;
    return PLANE( a * fInv, b * fInv, c * fInv, d * fInv ); 
}

ASDX_INLINE PLANE
operator * ( float f, const PLANE& p )
{ return PLANE( f * p.a, f * p.b, f * p.c, f * p.d ); }

ASDX_INLINE bool
PLANE::operator == ( const PLANE& p ) const
{ return ( (a==p.a) && (b==p.b) && (c==p.c) && (d==p.d) ); }

ASDX_INLINE bool
PLANE::operator != ( const PLANE& p ) const
{ return ( (a!=p.a) || (b!=p.b) || (c!=p.c) || (d!=p.d) ); }


ASDX_INLINE PLANE&
PLANE::Normalize()
{
    float mag = sqrtf( a*a + b*b + c*c );
    assert( !IsNearZero( mag ) );
    float magInv = 1.0f / mag;
    a *= magInv;
    b *= magInv;
    c *= magInv;
    d *= magInv;
    return (*this);
}

ASDX_INLINE PLANE&
PLANE::FromPointNormal( const FLOAT3 &point, const FLOAT3 &normal )
{
    a = normal.x;
    b = normal.y;
    c = normal.z;
    d = -Dot( point, normal );
   
    return (*this);
}

ASDX_INLINE PLANE&
PLANE::FromPoints( const FLOAT3 &p0, const FLOAT3 &p1, const FLOAT3 &p2 )
{
    FLOAT3 edge1, edge2, normal, Nnormal;
    
    edge1 = p1 - p0;
    edge2 = p2 - p0;
    normal = Cross( edge1, edge2 );
    Nnormal = normal.Normalize();
    return FromPointNormal( p0, Nnormal );
}

ASDX_INLINE bool
PLANE::Inside( const FLOAT3 &point ) const
{
    float result = DotCoordinate( (*this), point );
    return ( result >= 0.0f );
}

ASDX_INLINE bool
PLANE::Inside( const FLOAT3 &point, const float radius ) const
{
    float dist = DotCoordinate( (*this), point );
    return ( dist >= -radius );
}

/////////////////////////////////////////////////////////////////////////
// PLANE Methods
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE
float Dot( PLANE p, FLOAT4 v )
{ return (p.a*v.x + p.b*v.y + p.c*v.z + p.d*v.w); }

ASDX_INLINE
void Dot( PLANE &p, FLOAT4 &v, float &result )
{ result = Dot( p, v ); }

ASDX_INLINE
float DotCoordinate( PLANE p, FLOAT3 v )
{ return (p.a*v.x + p.b*v.y + p.c*v.z + p.d); }

ASDX_INLINE
void DotCoordinate( PLANE &p, FLOAT3 &v, float &result )
{ result = DotCoordinate( p, v ); }

ASDX_INLINE 
float DotNormal( PLANE p, FLOAT3 v )
{ return (p.a*v.x + p.b*v.y + p.c*v.z); }

ASDX_INLINE 
void DorNormal( PLANE &p, FLOAT3 &v, float &result )
{ result = DotNormal( p, v ); }

ASDX_INLINE 
PLANE Normalize( PLANE p )
{
    PLANE result = p;
    float mag = ( p.a*p.a + p.b*p.b + p.c*p.c );
    assert( !IsNearZero( mag ) );
    float magInv = 1.0f/mag;
    result.a *= magInv;
    result.b *= magInv;
    result.c *= magInv;
    result.d *= magInv;
    return result;
}

ASDX_INLINE
void Normalize( PLANE &p, PLANE &result )
{ result = Normalize( p ); }


/////////////////////////////////////////////////////////////////////////
// BOUNDINGBOX structure
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE
BOUNDINGBOX::BOUNDINGBOX()
{ /* DO_NOTHING */ }

ASDX_INLINE
BOUNDINGBOX::BOUNDINGBOX( FLOAT3 _minimum, FLOAT3 _maximum )
{
    minimum = _minimum;
    maximum = _maximum;
}

ASDX_INLINE
bool BOUNDINGBOX::operator == ( BOUNDINGBOX &box ) const
{ return ( maximum == box.maximum && minimum == box.minimum ); }

ASDX_INLINE
bool BOUNDINGBOX::operator != ( BOUNDINGBOX &box ) const
{ return ( maximum != box.maximum || minimum != box.minimum ); }

ASDX_INLINE
void BOUNDINGBOX::Merge( FLOAT3 value )
{
    maximum = Max( maximum, value );
    minimum = Min( minimum, value );
}

ASDX_INLINE
void BOUNDINGBOX::Merge( int size, FLOAT3 value[] )
{
    assert( value != 0 );
    for( int i = 0; i < size; ++i )
    { Merge( value[ i ] ); }
}

/////////////////////////////////////////////////////////////////////////
// BOUNDINGSPHERE structure
/////////////////////////////////////////////////////////////////////////

ASDX_INLINE
BOUNDINGSPHERE::BOUNDINGSPHERE()
{ /* DO_NOTING */ }

ASDX_INLINE
BOUNDINGSPHERE::BOUNDINGSPHERE( FLOAT3 _position, float _radius )
{
    position = _position;
    radius = _radius;
}

ASDX_INLINE
BOUNDINGSPHERE::BOUNDINGSPHERE( BOUNDINGBOX box )
{
    position = ( box.maximum + box.minimum ) * 0.5f;
    radius = Distance( box.maximum, box.minimum ) * 0.5f;
}

ASDX_INLINE
bool BOUNDINGSPHERE::operator == ( BOUNDINGSPHERE &sphere ) const
{ return ( position == sphere.position && radius == sphere.radius ); }

ASDX_INLINE
bool BOUNDINGSPHERE::operator != ( BOUNDINGSPHERE &sphere ) const
{ return ( position != sphere.position || radius != sphere.radius ); }

ASDX_INLINE
void BOUNDINGSPHERE::Merge( FLOAT3 value )
{
    FLOAT3 tmp = value - position;
    float r = tmp.Length();
    if ( r > radius ) 
    { 
        radius = r; 
        position = ( value + position ) * 0.5f;
    }
}

ASDX_INLINE
void BOUNDINGSPHERE::Merge( int size, FLOAT3* value )
{
    assert( value != 0 );
    for( int i = 0; i < size; ++i )
    { Merge( value[ i ] ); }
}

} // namespace asdx
