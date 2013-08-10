//-------------------------------------------------------------------------------
// File : s3d_rand.inl
// Desc : Random Generator Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------

#ifndef __S3D_RAND_INL__
#define __S3D_RAND_INL__

namespace s3d {

S3D_INLINE
Random::Random()
{
    SetSeed( 17320508 );
}

S3D_INLINE
Random::Random( const u32 seed )
{
    SetSeed( seed );
}

S3D_INLINE
void Random::SetSeed( const u32 seed )
{
    // 超越数設定.
    m_X = 123456789;
    m_Y = 362436069;
    m_Z = 521288629;
    m_W = ( seed <= 0 ) ? 88675123 : seed;
}

S3D_INLINE
u32 Random::GetAsU32()
{
    u32 t = m_X ^ ( m_X << 11 );
    m_X   = m_Y;
    m_Y   = m_Z;
    m_Z   = m_W;
    m_W   = ( m_W ^ ( m_W >> 19 ) ) ^ ( t ^ ( t >> 8 ) );

    return m_W;
}

S3D_INLINE
f64 Random::GetAsF64()
{
    return static_cast<f64>( GetAsU32() ) / 0xffffffffui32;
}

} // namespace s3d


#endif//__S3D_RAND_INL__