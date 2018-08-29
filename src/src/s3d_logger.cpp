//-------------------------------------------------------------------------------------------------
// File : s3d_logger.cpp
// Desc : Logger Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_logger.h>
#include <Windows.h>
#include <cstdio>
#include <cstdarg>


namespace /* anonymous */ {

///////////////////////////////////////////////////////////////////////////////////////////////////
// LOG_LEVEL enum
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LOG_LEVEL
{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR,
};

//-------------------------------------------------------------------------------------------------
// Gloval Variables
//-------------------------------------------------------------------------------------------------
CONSOLE_SCREEN_BUFFER_INFO  g_ScreenBuffer;  //!< スクリーンバッファ情報.


//-------------------------------------------------------------------------------------------------
//      カラーを設定します.
//-------------------------------------------------------------------------------------------------
void BindColor( LOG_LEVEL level )
{
    auto handle = GetStdHandle( STD_OUTPUT_HANDLE );
    GetConsoleScreenBufferInfo( handle, &g_ScreenBuffer );

    auto attribute = g_ScreenBuffer.wAttributes;
    switch( level )
    {
    case LOG_LEVEL_INFO:
        attribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;

    case LOG_LEVEL_DEBUG:
        attribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;

    case LOG_LEVEL_ERROR:
        attribute = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    }

    SetConsoleTextAttribute( handle, attribute );
}

//-------------------------------------------------------------------------------------------------
//      カラー設定を解除します.
//-------------------------------------------------------------------------------------------------
void UnBindColor()
{
    HANDLE handle = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( handle, g_ScreenBuffer.wAttributes );
}

}// namespace /* anonymous */ 


namespace s3d {

//-------------------------------------------------------------------------------------------------
// Static Initializer
//-------------------------------------------------------------------------------------------------
SystemLogger SystemLogger::s_Instance;


///////////////////////////////////////////////////////////////////////////////////////////////////
// SystemLogger class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------------
SystemLogger::SystemLogger()
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------------
SystemLogger::~SystemLogger()
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------------
//      唯一のインスタンスを取得します.
//-------------------------------------------------------------------------------------------------
SystemLogger& SystemLogger::GetInstance()
{ return s_Instance; }

//-------------------------------------------------------------------------------------------------
//      デバッグログです.
//-------------------------------------------------------------------------------------------------
void SystemLogger::DebugLog( char* format, ... )
{
    BindColor( LOG_LEVEL_DEBUG );

    // ログ出力.
    {
        char msg[ 2048 ] = "\0";
        va_list arg;

        va_start( arg, format );
        vsprintf_s( msg, format, arg );
        va_end( arg );

        printf_s( msg );

        OutputDebugStringA( msg );
    }

    UnBindColor();
}

//-------------------------------------------------------------------------------------------------
//      インフォメーションログです.
//-------------------------------------------------------------------------------------------------
void SystemLogger::InfoLog( char* format, ... )
{
    BindColor( LOG_LEVEL_INFO );

    // ログ出力.
    {
        char msg[ 2048 ] = "\0";
        va_list arg;

        va_start( arg, format );
        vsprintf_s( msg, format, arg );
        va_end( arg );

        fprintf_s( stdout, msg );

        OutputDebugStringA( msg );
    }

    UnBindColor();
}

//-------------------------------------------------------------------------------------------------
//      エラーログです.
//-------------------------------------------------------------------------------------------------
void SystemLogger::ErrorLog( char* format, ... )
{
    BindColor( LOG_LEVEL_ERROR );

    // ログ出力.
    {
        char msg[ 2048 ] = "\0";
        va_list arg;

        va_start( arg, format );
        vsprintf_s( msg, format, arg );
        va_end( arg );

        fprintf_s( stderr, msg );
        
        OutputDebugStringA( msg );
    }

    UnBindColor();
}

} // namespace s3d
