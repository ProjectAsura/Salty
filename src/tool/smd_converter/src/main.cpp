//-------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : Msh Binary Converter.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include "OBJLoader.h"
#include <cstdio>
#include <cstring>
#include <string>


#ifndef ILOG
#define ILOG( x, ... ) printf_s( x"\n", ##__VA_ARGS__ )
#endif//ILOG


//-------------------------------------------------------------------------------------------
//! @brief      ヘルプを表示します.
//-------------------------------------------------------------------------------------------
void ShowHelp()
{
    ILOG( "//-------------------------------------------------------------------" );
    ILOG( "// smd_converter.exe" );
    ILOG( "// Copyright(c) Project Asura. All right reserved." );
    ILOG( "//-------------------------------------------------------------------" );
    ILOG( "[使い方] smd_converter.exe -i 入力ファイル名 -o 出力ファイル名" );
    ILOG( "" );

}

//-------------------------------------------------------------------------------------------
//! @brief      メインエントリーポイントです.
//-------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    if ( argc > 1 )
    {
        std::string inputFileName;
        std::string outputFileName;

        for( int i=0; i<argc; i++ )
        {
            if ( strcmp( argv[ i ], "-i" ) == 0 )
            {
                i++;
                if ( i < argc )
                { inputFileName = std::string( argv[ i ] ); }
            }
            else if ( strcmp( argv[ i ], "-o" ) == 0 )
            {
                i++;
                if ( i < argc )
                { outputFileName = std::string( argv[ i ] ); }
            }
            else if ( strcmp( argv[ i ], "-io" ) == 0 )
            {
                i++;
                if ( i < argc )
                {
                    inputFileName = std::string( argv[ i ] );
                    
                    std::string::size_type index = inputFileName.find_last_of( "." );
                    if ( index == std::string::npos )
                    {
                        outputFileName = inputFileName + ".smd";
                    }
                    else
                    {
                        outputFileName = inputFileName;
                        outputFileName = outputFileName.substr( 0, index );
                        outputFileName += ".smd";
                    }
                }
            }
        }

        if ( !inputFileName.empty() && !outputFileName.empty() )
        {
            OBJMESH mesh;
            if ( mesh.LoadFile( inputFileName.c_str() ) ) 
            {
                if ( mesh.SaveToBinary( outputFileName.c_str() ) )
                { printf_s( "Info : Binary Convert Success. filename = %s\n", outputFileName.c_str() ); }
                else
                { printf_s( "Error : Save File Failed. filename = %s\n", outputFileName.c_str() ); }
            }
            else
            { printf_s( "Error : Load File Failed. filename = %s\n", inputFileName.c_str() ); }

            mesh.Release();
        }
        else
        { printf_s( "Error : Invalid Argument. \n" ); }
    }
    else
    {
//#if defined(_DEBUG) || defined(DEBUG)
//        std::string inputFileName = std::string( "../res/sponza.obj" );
//        std::string outputFileName = std::string( "../res/sponza.msh" );
//
//        if ( !inputFileName.empty() && !outputFileName.empty() )
//        {
//            OBJMESH mesh;
//            if ( mesh.LoadFile( inputFileName.c_str() ) ) 
//            {
//                if ( mesh.SaveToBinary( outputFileName.c_str() ) )
//                { printf_s( "Info : Binary Convert Success. filename = %s\n", outputFileName.c_str() ); }
//                else
//                { printf_s( "Error : Save File Failed. filename = %s\n", outputFileName.c_str() ); }
//            }
//            else
//            { printf_s( "Error : Load File Failed. filename = %s\n", inputFileName.c_str() ); }
//
//            mesh.Release();
//        }
//        else
//        { printf_s( "Error : Invalid Argument. \n" ); }
//#else
        ShowHelp();
//#endif
    }

    return 0;
}