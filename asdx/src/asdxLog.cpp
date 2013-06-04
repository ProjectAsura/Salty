//--------------------------------------------------------------------------
// File : asdxLog.h
// Desc : Log Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------
#include <asdxLog.h>
#include <cstdio>
#include <cstdarg>
#include <Windows.h>


namespace asdx {

//--------------------------------------------------------------------------
//      ログを出力します.
//--------------------------------------------------------------------------
int ConsoleLogA( const char* format, ... )
{
    int result = 0;
    va_list arg;

    va_start( arg, format );
    result = vprintf_s( format, arg );
    va_end( arg );

    return result;
}

//--------------------------------------------------------------------------
//      ログを出力します.
//--------------------------------------------------------------------------
int ConsoleLogW( const wchar_t* format, ... )
{
    int result = 0;
    va_list arg;

    va_start( arg, format );
    result = vwprintf_s( format, arg );
    va_end( arg );

    return result;
}

//--------------------------------------------------------------------------
//      デバッグログを出力します.
//--------------------------------------------------------------------------
int DebugLogA( const char* format, ... )
{
    int result = 0;
    char msg[ 2048 ] = "\0";
    va_list arg;

    va_start( arg, format );
    result = vsprintf_s( msg, format, arg );
    va_end( arg );

    OutputDebugStringA( msg );

    CONSOLE_SCREEN_BUFFER_INFO screenBuffer;
    HANDLE hStdout;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &screenBuffer);
    SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf_s( msg );
    SetConsoleTextAttribute(hStdout, screenBuffer.wAttributes );

    return result;
}

//--------------------------------------------------------------------------
//      デバッグログを出力します.
//--------------------------------------------------------------------------
int DebugLogW( const wchar_t* format, ... )
{
    int result = 0;
    wchar_t msg[ 2048 ] = L"\0";
    va_list arg;

    va_start( arg, format );
    result = vswprintf_s( msg, format, arg );
    va_end( arg );

    OutputDebugStringW( msg );

    CONSOLE_SCREEN_BUFFER_INFO screenBuffer;
    HANDLE hStdout;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &screenBuffer);
    SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    wprintf_s( msg );
    SetConsoleTextAttribute(hStdout, screenBuffer.wAttributes );

    return result;
}

//--------------------------------------------------------------------------
//      エラーログを出力します.
//--------------------------------------------------------------------------
int ErrorLogA( const char* format, ... )
{
    CONSOLE_SCREEN_BUFFER_INFO screenBuffer;
    HANDLE hStdout;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &screenBuffer);
    SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);

    int result = 0;
    va_list arg;

    va_start( arg, format );
    result = vfprintf_s( stderr, format, arg );
    va_end( arg );

    SetConsoleTextAttribute(hStdout, screenBuffer.wAttributes );

    return result;
}

//--------------------------------------------------------------------------
//      エラーログを出力します.
//--------------------------------------------------------------------------
int ErrorLogW( const wchar_t* format, ... )
{
    CONSOLE_SCREEN_BUFFER_INFO screenBuffer;
    HANDLE hStdout;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &screenBuffer);
    SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);

    int result = 0;
    va_list arg;

    va_start( arg, format );
    result = vfwprintf_s( stderr, format, arg );
    va_end( arg );

    SetConsoleTextAttribute(hStdout, screenBuffer.wAttributes );

    return result;
}

} // namespace asdx
