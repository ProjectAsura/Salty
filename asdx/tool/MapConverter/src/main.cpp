//-------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : Application Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <cstring>
#include <string>
#include "DDSLoader.h"



//-------------------------------------------------------------------------------------------
//! @brief      �w���v��\�����܂�.
//-------------------------------------------------------------------------------------------
void ShowHelp()
{
    printf_s( "//-------------------------------------------------------------------\n" );
    printf_s( "// MapConverter.exe\n" );
    printf_s( "// Copyright(c) Project Asura. All right reserved.\n" );
    printf_s( "//-------------------------------------------------------------------\n" );
    printf_s( "[�g����] MapConverter.exe -i ���̓t�@�C���� -o �o�̓t�@�C����\n" );
    printf_s( "\n" );
}


//-------------------------------------------------------------------------------------------
//! @brief      ���C���G���g���[�|�C���g�ł�.
//!
//! @param [in]     argc
//-------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    std::string input_name;
    std::string output_name;

    for( int i=0; i<argc; ++i )
    {
        // ���̓t�@�C����.
        if( strcmp( argv[ i ], "-i" ) == 0 )
        {
            if ( i+1 <= argc )
            {
                i++;
                input_name = std::string( argv[ i ] );
            }
        }
        // �o�̓t�@�C����.
        else if ( strcmp( argv[ i ], "-o" ) == 0 )
        {
            if ( i+1 <= argc )
            {
                i++;
                output_name = std::string( argv[ i ] );
            }
        }
        // ���o�͖��𓯈��.
        else if ( strcmp( argv[ i ], "-io" ) == 0 )
        {
            if ( i+1 <= argc )
            {
                i++;
                input_name = std::string( argv[ i ] );

                std::string::size_type index = input_name.find_last_of( "." );
                if ( index == std::string::npos )
                {
                    output_name = input_name + ".map";
                }
                else
                {
                    output_name = input_name;
                    output_name = output_name.substr( 0, index );
                    output_name += ".map";
                }

            }
        }
        // �w���v�\��.
        else if ( strcmp( argv[ i ], "-h" ) == 0 )
        {
            ShowHelp();
            return 0;
        }
    }

    // �t�@�C���������͂��ꂽ���`�F�b�N.
    if( !input_name.empty() && !output_name.empty() )
    {
        DDSTexture texture;
        
        // �e�N�X�`���t�@�C�������[�h.
        if ( !texture.LoadFromFile( input_name.c_str() ) )
        {
            // �G���[���O�o��.
            printf_s( "Error : Texture File Load Failed. filename = %s\n", input_name.c_str() );

            // ��������N���A.
            input_name.clear();
            output_name.clear();

            texture.Release();

            return -1;
        }

        // MAP�t�@�C���ɕۑ�.
        if ( !texture.SaveToBinary( output_name.c_str() ) )
        {
            // �G���[���O�o��.
            printf_s( "Error : Binary Save Failed. filename = %s\n", output_name.c_str() );

            // ��������N���A.
            input_name.clear();
            output_name.clear();

            texture.Release();

            return -1;
        }

        // �����ƕ\��.
        printf_s( "Info : Binary Save Success! filename = %s\n", output_name.c_str() );

        texture.Release();
    }
    else
    {
        // �w���v�\��.
        ShowHelp();
    }

    // ��������N���A.
    input_name.clear();
    output_name.clear();

    return 0;
}