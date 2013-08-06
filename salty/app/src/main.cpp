﻿//-------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : Salty Application Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <saltyApp.h>


//-------------------------------------------------------------------------------------------
//! @brief      アプリケーションのメインエントリーポイントです.
//!
//! @param [in]     argc        コマンドライン引数の数.
//! @param [in]     argv        コマンドラインの値.
//! @return     システムへ値を返却します.
//-------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    salty::App app;

    app.Run();

    return 0;
}