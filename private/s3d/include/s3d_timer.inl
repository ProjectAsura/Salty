//--------------------------------------------------------------------------------
// File : s3d_timer.inl
// Desc : Timer Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------

#ifndef __S3D_TIMER_INL__
#define __S3D_TIMER_INL__


namespace s3d {

//////////////////////////////////////////////////////////////////////////////////
// Timer class
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//      コンストラクタです.
//--------------------------------------------------------------------------------
S3D_INLINE
Timer::Timer()
: m_StartTime( 0 )
, m_StopTime ( 0 )
{
    LARGE_INTEGER qwTime = { 0 };
    QueryPerformanceFrequency( &qwTime );
    s64 ticksPerSec = qwTime.QuadPart;
    m_InvTicksPerSec = 1.0 / static_cast<f64>( ticksPerSec );
}

//--------------------------------------------------------------------------------
//      時間計測を開始します.
//--------------------------------------------------------------------------------
S3D_INLINE
void Timer::Start()
{
    LARGE_INTEGER qwTime = { 0 };
    QueryPerformanceCounter( &qwTime );
    m_StartTime = qwTime.QuadPart;
}

//--------------------------------------------------------------------------------
//      時間計測を終了します.
//--------------------------------------------------------------------------------
S3D_INLINE
void Timer::Stop()
{
    LARGE_INTEGER qwTime = { 0 };
    QueryPerformanceCounter( &qwTime );
    m_StopTime = qwTime.QuadPart;
}

//--------------------------------------------------------------------------------
//      経過時間をミリ秒単位で取得します.
//--------------------------------------------------------------------------------
S3D_INLINE
f64 Timer::GetElapsedTimeMsec() const
{ return ( m_StopTime - m_StartTime ) * 1000.0 * m_InvTicksPerSec; }

//--------------------------------------------------------------------------------
//      経過時間を秒単位で取得します.
//--------------------------------------------------------------------------------
S3D_INLINE
f64 Timer::GetElapsedTimeSec() const
{ return ( m_StopTime - m_StartTime ) * m_InvTicksPerSec; }

//--------------------------------------------------------------------------------
//      経過時間を分単位で取得します.
//--------------------------------------------------------------------------------
S3D_INLINE
f64 Timer::GetElapsedTimeMin() const
{ return ( ( m_StopTime - m_StartTime ) * m_InvTicksPerSec ) / 60.0; }

//--------------------------------------------------------------------------------
//      経過時間を時間単位で取得します.
//--------------------------------------------------------------------------------
S3D_INLINE
f64 Timer::GetElapsedTimeHour() const
{ return ( ( m_StopTime - m_StartTime ) * m_InvTicksPerSec ) / 3600.0; }



} // namespace s3d

#endif//__S3D_TIMER_INL__
