//-------------------------------------------------------------------------------------------------
// File : s3d_logger.h
// Desc : Logger Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#ifndef __S3D_LOGGER_H__
#define __S3D_LOGGER_H__

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_typedef.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// SystemLogger class
///////////////////////////////////////////////////////////////////////////////////////////////////
class SystemLogger
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
    static SystemLogger& GetInstance();
    void DebugLog( char* format, ... );
    void InfoLog ( char* format, ... );
    void ErrorLog( char* format, ... );

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    static SystemLogger s_Instance;

    //=============================================================================================
    // private methods.
    //=============================================================================================
    SystemLogger ();
    ~SystemLogger();
    SystemLogger             ( const SystemLogger& ) = delete;
    SystemLogger& operator = ( const SystemLogger& ) = delete;
};

} // namespace s3d

#ifndef DLOG
    #if S3D_IS_DEBUG
        #define DLOG(x, ...)    s3d::SystemLogger::GetInstance().DebugLog("[File: %s, Line: %d] " x "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
    #else
        #define DLOG(x, ...)    ((void)0)
    #endif//S3D_IS_DEBUG
#endif//DLOG

#ifndef ILOG
    #define ILOG(x, ...)        s3d::SystemLogger::GetInstance().InfoLog(x "\n", ##__VA_ARGS__)
#endif//ILOG

#ifndef ELOG
    #define ELOG(x, ...)        s3d::SystemLogger::GetInstance().ErrorLog("[File: %s, Line: %d] " x "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG


#endif//__G3D_LOGGER_H__
