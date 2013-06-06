//-------------------------------------------------------------------------------
// File : asdxLog.h
// Desc : Log Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------

#ifndef __ASDX_LOG_H__
#define __ASDX_LOG_H__

namespace asdx {

////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//! @brief      ���O���o�͂��܂�.
//!
//! @param [in]     format          �����w��q.
//--------------------------------------------------------------------------
int ConsoleLogA( const char* format, ... );

//--------------------------------------------------------------------------
//! @brief      ���O���o�͂��܂�.
//!
//! @param [in]     format          �����w��q.
//--------------------------------------------------------------------------
int ConsoleLogW( const wchar_t* format, ... );

//--------------------------------------------------------------------------
//! @brief      �f�o�b�O���O���o�͂��܂�.
//!
//! @param [in]     format          �����w��q.
//! @note       �g�p�ł���ő啶������2048�ł�.
//--------------------------------------------------------------------------
int DebugLogA( const char* format, ... );

//--------------------------------------------------------------------------
//! @brief      �f�o�b�O���O���o�͂��܂�.
//!
//! @param [in]     format          �����w��q.
//! @note       �g�p�ł���ő啶������2048�ł�.
//--------------------------------------------------------------------------
int DebugLogW( const wchar_t* format, ... );

//--------------------------------------------------------------------------
//! @brief      �G���[���O���o�͂��܂�.
//!
//! @param [in]     format          �����w��q.
//--------------------------------------------------------------------------
int ErrorLogA( const char* format, ... );

//--------------------------------------------------------------------------
//! @brief      �G���[���O���o�͂��܂�.
//!
//! @param [in]     format          �����w��q.
//--------------------------------------------------------------------------
int ErrorLogW( const wchar_t* format, ... );


#ifndef __ASDX_WIDE
#define __ASDX_WIDE( _string )      L ## _string 
#endif//__ASDX_WIDE

#ifndef ASDX_WIDE
#define ASDX_WIDE( _string )        __ASDX_WIDE( _string )
#endif//ASDX_WIDE


#ifndef DLOG
#if defined(DEBUG) || defined(_DEBUG)
#define DLOG( fmt, ... )       asdx::DebugLogA( "[File: %s, Line: %d] "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#else
#define DLOG( fmt, ... )      ((void)0)
#endif//defined(DEBUG) || defined(_DEBUG)
#endif//DLOG

#ifndef DLOGW
#if defined(DEBUG) || defined(_DEBUG)
#define DLOGW( fmt, ... )       asdx::DebugLogW( ASDX_WIDE("[File: %s, Line: %d] ")ASDX_WIDE(fmt)ASDX_WIDE("\n"), ASDX_WIDE(__FILE__), __LINE__, ##__VA_ARGS )
#else
#define DLOGW( fmt, ... )       ((void)0)
#endif
#endif

#ifndef ILOG
#define ILOG( fmt, ... )      asdx::ConsoleLogA( fmt"\n", ##__VA_ARGS__ )
#endif//ILOG

#ifndef ILOGW
#define ILOGW( fmt, ... )     asdx::ConsoleLogW( ASDX_WIDE(fmt)ASDX_WIDE("\n"), ##__VA_ARGS__ )
#endif

#ifndef ELOG
#define ELOG( fmt, ... )      asdx::ErrorLogA( "[File: %s, Line: %d] "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG

#ifndef ELOGW
#define ELOGW( fmt, ... )     asdx::ErrorLogW( ASDX_WIDE("[File: %s, Line: %d] ")ASDX_WIDE(fmt)ASDX_WIDE("\n"), ASDX_WIDE(__FILE__), __LINE__, ##__VA_ARGS__ )
#endif



} // namespace asdx

#endif//__ASDX_LOG_H__