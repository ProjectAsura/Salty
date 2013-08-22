//------------------------------------------------------------------------------------------
// File : OBJLoader.cpp
// Desc : Wavefront OBJ File Loader Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS 1

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "OBJLoader.h"
#include "SmdData.h"
#include <cassert>


//------------------------------------------------------------------------------------------
// Macro
//------------------------------------------------------------------------------------------

// RtlZeroMemory Macro
#ifndef RtlZeroMemory
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif

// ZeroMemory Macro
#ifndef ZeroMemory
#define ZeroMemory RtlZeroMemory
#endif

// SAFE_DELETE_ARRAY
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { if (x) { delete [] (x); (x) = NULL; } }
#endif


//------------------------------------------------------------------------------------------
// Using Statements
//------------------------------------------------------------------------------------------
using namespace std;

const float F_PI = 3.1415926535f;


//////////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////////
inline
float DegToRad( const float deg )
{
    return  deg * (F_PI/180.0f);
}

inline
float RadToDeg( const float rad )
{
    return rad * (180.0f/F_PI);
}

//-----------------------------------------------------------------------
// Name : Vec3Add()
// Desc : ベクトルを足します.
//-----------------------------------------------------------------------
inline
OBJVEC3 Vec3Add( const OBJVEC3& a, const OBJVEC3& b )
{
    OBJVEC3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}


//-----------------------------------------------------------------------
// Name : Vec3Sub()
// Desc : ベクトルを引きます.
//-----------------------------------------------------------------------
inline 
OBJVEC3 Vec3Sub( const OBJVEC3& a, const OBJVEC3& b )
{
    OBJVEC3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

//-----------------------------------------------------------------------
// Name : Vec3Dot()
// Desc : ベクトルの内積を求めます.
//-----------------------------------------------------------------------
inline 
float Vec3Dot( const OBJVEC3& a, const OBJVEC3& b )
{
    return ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z );
}

//-----------------------------------------------------------------------
// Name : Vec3Cross()
// Desc : ベクトルの外積を求めます.
//-----------------------------------------------------------------------
inline
OBJVEC3 Vec3Cross( const OBJVEC3& a, const OBJVEC3& b )
{
    OBJVEC3 result;
    result.x = ( a.y * b.z ) - ( a.z * b.y );
    result.y = ( a.z * b.x ) - ( a.x * b.z );
    result.z = ( a.x * b.y ) - ( a.y * b.x );
    return result;
}

//-----------------------------------------------------------------------
// Name : Vec3SafeNormalize()
// Desc : 安全に正規化を行います.
//-----------------------------------------------------------------------
inline
OBJVEC3 Vec3SafeNormalize( const OBJVEC3& v )
{
    OBJVEC3 result;

    float mag = sqrtf( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) );
    if ( mag != 0.0f )
    {
        float invMag = 1.0f / mag;
        result.x = v.x * invMag;
        result.y = v.y * invMag;
        result.z = v.z * invMag;
        return result;
    }

    result.x = v.x;
    result.y = v.y;
    result.z = v.z;
    return result;
}

//------------------------------------------------------------------------
// Name : GetDirectionPath()
// Desc : 文字列からディレクトリを取得
//------------------------------------------------------------------------
char* GetDirectoryPath( const char* filename )
{
    char *strings = NULL;
    char *dir;
    dir = new char [strlen(filename)+1];
    strcpy(dir, filename);
    if ( strings = strrchr(dir, '/' ) ) strings[1] = '\0';
    else if ( strings = strrchr(dir, '\\') ) strings[1] ='\0';
    else dir[0] = '\0';
    return dir;
}

//-----------------------------------------------------------------------
// Name : CutDirectoryPath()
// Desc : 文字列からディレクトリを削除
//-----------------------------------------------------------------------
char* CutDirectoryPath( const char* filename )
{
    char *s=NULL;
    char *base=NULL;
    base = new char [strlen(filename)+1];
    strcpy( base, filename );
    if ( s = strrchr(base, '\\') )
    {
        s++;
        return s;
    }
    else if ( s = strrchr(base, '/') )
    {
        s++;
        return s;
    }
    else
    { 
        return base;
    }
}

//-----------------------------------------------------------------------
// Name : SetDirectoryPath
// Desc : ディレクトリを前に付加して文字列を返す
//-----------------------------------------------------------------------
char* SetDirectoryPath( char* dest, char* directory )
{
    char* str;
    str = new char[strlen(directory) + strlen(dest) + 1];
    strcpy( str, directory );
    strcat( str, dest );
    return str;
}


//-----------------------------------------------------------------------
// Name : InitMaterial()
// Desc : マテリアルの初期化
//-----------------------------------------------------------------------
void InitMaterial( OBJMATERIAL* pMaterial )
{
    ZeroMemory( pMaterial, sizeof( OBJMATERIAL ) );
    pMaterial->ambient   = OBJVEC3( 0.2f, 0.2f, 0.2f );
    pMaterial->diffuse   = OBJVEC3( 0.8f, 0.8f, 0.8f );
    pMaterial->specular  = OBJVEC3( 1.0f, 1.0f, 1.0f );
    pMaterial->emissive  = OBJVEC3( 0.0f, 0.0f, 0.0f );
    pMaterial->shininess = 0.0f;
    pMaterial->alpha     = 1.0f;

}

//-----------------------------------------------------------------------
// Name : OBJVEC3Scale()
// Desc : スケール倍する
//-----------------------------------------------------------------------
OBJVEC3 OBJVEC3Scale( OBJVEC3 v, float scale )
{ return OBJVEC3( v.x * scale, v.y * scale, v.z * scale ); }

//-----------------------------------------------------------------------
// Name : OBJVEC3Add()
// Desc : 足し算する
//-----------------------------------------------------------------------
OBJVEC3 OBJVEC3Add( OBJVEC3 v1, OBJVEC3 v2 )
{ return OBJVEC3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z ); }

//-----------------------------------------------------------------------
// Name : OBJVEC3Substract()
// Desc : 引き算する
//-----------------------------------------------------------------------
OBJVEC3 OBJVEC3Substract( OBJVEC3 v1, OBJVEC3 v2 )
{ return OBJVEC3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z ); }

//-----------------------------------------------------------------------
// Name : OBJVEC3Length()
// Desc : 長さを求める
//-----------------------------------------------------------------------
float OBJVEC3Length( OBJVEC3 v )
{ return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }

//-----------------------------------------------------------------------
// Name : OBJVEC3Max()
// Desc : 最大値を求める
//-----------------------------------------------------------------------
OBJVEC3 OBJVEC3Max( OBJVEC3 v, OBJVEC3 _max )
{
    OBJVEC3 result;
    result.x = ( v.x > _max.x ? v.x : _max.x );
    result.y = ( v.y > _max.y ? v.y : _max.y );
    result.z = ( v.z > _max.z ? v.z : _max.z );
    return result;
}

//------------------------------------------------------------------------
// Name : OBJVEC3Min()
// Desc : 最小値を求める
//------------------------------------------------------------------------
OBJVEC3 OBJVEC3Min( OBJVEC3 v, OBJVEC3 _min )
{
    OBJVEC3 result;
    result.x = ( v.x < _min.x ? v.x : _min.x );
    result.y = ( v.y < _min.y ? v.y : _min.y );
    result.z = ( v.z < _min.z ? v.z : _min.z );
    return result;
}

//////////////////////////////////////////////////////////////////////////
// OBJVEC2
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// Name : OBJVEC2()
// Desc : コンストラクタ
//------------------------------------------------------------------------
OBJVEC2::OBJVEC2( float nx, float ny )
: x(nx), y(ny)
{
}

//-----------------------------------------------------------------------
// operator float*
//-----------------------------------------------------------------------
OBJVEC2::operator float * () 
{ return (float*)&x; }

//-----------------------------------------------------------------------
// operator const float*
//-----------------------------------------------------------------------
OBJVEC2::operator const float *() const 
{ return (const float*)&x; }


//////////////////////////////////////////////////////////////////////////
// OBJVEC3
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// Name : OBJVEC3()
// Desc : コンストラクタ
//------------------------------------------------------------------------
OBJVEC3::OBJVEC3( float nx, float ny, float nz )
: x(nx), y(ny), z(nz)
{
}

//------------------------------------------------------------------------
// operator float*
//------------------------------------------------------------------------
OBJVEC3::operator float *() 
{ return (float*)&x; }

//------------------------------------------------------------------------
// operator const float*
//------------------------------------------------------------------------
OBJVEC3::operator const float *() const
{ return (const float*)&x; }

//////////////////////////////////////////////////////////////////////////
// OBJBOUNDINGBOX
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
// Name : OBJBOUNDINGBOX()
// Desc : コンストラクタ
//-----------------------------------------------------------------------
OBJBOUNDINGBOX::OBJBOUNDINGBOX( OBJVEC3 value )
: minimum( value ), maximum( value )
{
}

//-----------------------------------------------------------------------
// Name : Merage()
// Desc : 
//-----------------------------------------------------------------------
void OBJBOUNDINGBOX::Merge(OBJVEC3 value)
{
    maximum = OBJVEC3Max( value, maximum );
    minimum = OBJVEC3Min( value, minimum );
    size = OBJVEC3Substract( maximum, minimum );
}

//////////////////////////////////////////////////////////////////////////
// OBJBOUNDINGSPHERE
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// Name : Create()
// Desc :
//------------------------------------------------------------------------
void OBJBOUNDINGSPHERE::Create( OBJBOUNDINGBOX box )
{
    center = OBJVEC3Add( box.maximum, box.minimum );
    center = OBJVEC3Scale( center, 0.5f );

    OBJVEC3 half;
    half = OBJVEC3Substract( box.maximum, center );
    radius = OBJVEC3Length( half );
}


/////////////////////////////////////////////////////////////////////////
// OBJMESH
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
// Name : OBJMESH()
// Desc : コンストラクタ
//-----------------------------------------------------------------------
OBJMESH::OBJMESH()
{
    m_NumVertices  = 0;
    m_NumMaterials = 0;
    m_NumSubsets   = 0;
    m_NumIndices   = 0;

    m_Vertices  = 0;
    m_Materials = 0;
    m_Subsets   = 0;
    m_Indices   = 0;

    m_IsExistNormal   = false;
    m_IsExistTexCoord = false;
}

//-----------------------------------------------------------------------
// Name : ~OBJMESH()
// Desc : デストラクタ
//-----------------------------------------------------------------------
OBJMESH::~OBJMESH()
{
    Release();
};

//-----------------------------------------------------------------------
// Name : Release()
// Desc : メモリを破棄
//-----------------------------------------------------------------------
void OBJMESH::Release()
{
    SAFE_DELETE_ARRAY( m_Vertices );
    SAFE_DELETE_ARRAY( m_Materials );
    SAFE_DELETE_ARRAY( m_Subsets );
    SAFE_DELETE_ARRAY( m_Indices );
    
    m_NumVertices  = 0;
    m_NumMaterials = 0;
    m_NumSubsets   = 0;
    m_NumIndices   = 0;

    m_IsExistNormal   = false;
    m_IsExistTexCoord = false;
}

//-----------------------------------------------------------------------
// Name : LoadOBJFile()
// Desc : OBJファイルの読み込み
//-----------------------------------------------------------------------
bool OBJMESH::LoadOBJFile(const char *filename)
{
    ifstream file;

    m_IsExistNormal   = false;
    m_IsExistTexCoord = false;

    char mtlFileName[OBJ_NAME_LENGTH] = {0};
    char buf[OBJ_BUFFER_LENGTH] = {0};
    vector<OBJVEC3>      positions;
    vector<OBJVEC3>      normals;
    vector<OBJVEC2>      texcoords;
    vector<OBJVERTEX>    t_vertices;
    vector<OBJSUBSET>    t_subsets;
    vector<unsigned int> t_indices;
    bool initBox = false;
    int prevSize = 0;

    unsigned long total = 0;

    OBJMATERIAL  material;
    unsigned int dwFaceIndex = 0;
    unsigned int dwFaceCount = 0;
    unsigned int dwCurSubset = 0;

    //　ディレクトリを切り取り
    strcpy_s( m_directoryPath, GetDirectoryPath( filename ) );

    //　ファイルを開く
    file.open( filename, ios::in );

    //　チェック
    if ( !file.is_open() )
    {
        cerr << "Error : ファイルオープンに失敗\n";
        cerr << "File Name : " << filename << endl;
        return false;
    }

    //　ループ
    for( ;; )
    {
        file >> buf;
        if ( !file )
            break;

        //　コメント
        if ( 0 == strcmp( buf, "#" ) )
        {
        }

        //　頂点座標
        else if ( 0 == strcmp( buf, "v" ) )
        {
            float x, y, z;
            file >> x >> y >> z;
            OBJVEC3 v( x, y, z );
            positions.push_back( v );

            //　バウンディングボックスの初期化
            if ( !initBox )
            {
                m_Box = OBJBOUNDINGBOX( v );
                initBox = true;
            }

            //　バウンディングボックスの算出
            m_Box.Merge( v );
        }

        //　テクスチャ座標
        else if ( 0 == strcmp( buf, "vt" ) )
        {
            float u, v;
            file >> u >> v;
            texcoords.push_back( OBJVEC2( u, v ) );
        }

        //　法線ベクトル
        else if ( 0 == strcmp( buf, "vn" ) )
        {
            float x, y, z;
            file >> x >> y >> z;
            normals.push_back( OBJVEC3( x, y, z) );
        }

        //　面
        else if ( 0 == strcmp( buf, "f" ) )
        {
            unsigned int iPosition, iTexCoord, iNormal;
            unsigned int p[4] = {-1}, t[4] = {-1}, n[4] = {-1};    
            OBJVERTEX vertex;
            dwFaceIndex++;
            dwFaceCount++;
            int count = 0;
            unsigned int index = 0;
        
            //　三角形・四角形のみ対応
            for ( int iFace = 0; iFace < 4; iFace++ )
            {
                count++;    //　頂点数を数える
                ZeroMemory( &vertex, sizeof( OBJVERTEX ) );

                file >> iPosition;
                vertex.position = positions[ iPosition - 1 ];
                p[iFace] = iPosition -1;
                
                if ( '/' == file.peek() )
                {
                    file.ignore();

                    //　テクスチャ座標インデックス
                    if ( '/' != file.peek() )
                    {
                        file >> iTexCoord;
                        vertex.texcoord = texcoords[ iTexCoord - 1 ];
                        t[iFace] = iTexCoord -1;
                    }

                    //　法線ベクトルインデックス
                    if ( '/' == file.peek() )
                    {
                        file.ignore();

                        file >> iNormal;
                        vertex.normal = normals[ iNormal - 1 ];
                        n[iFace] = iNormal -1;
                    }
                }

                //　カウントが3未満
                if ( iFace < 3 )
                {
                    t_vertices.push_back( vertex );
                    index = t_vertices.size()-1;
                    t_indices.push_back( index );
                }

                //　次が改行だったら終了
                if ( ( '\n' == file.peek() )
                  || ( '\r' == file.peek() ) )
                {
                    break;
                }

            }

            //　四角形ポリゴンの場合，三角形を追加する
            if ( count > 3 )
            {
                //　カウント
                dwFaceIndex++;
                dwFaceCount++;

                //　頂点とインデックスを追加
                for ( int iFace = 1; iFace < 4; iFace++ )
                {
                    int j = (iFace+1)%4;
                    ZeroMemory( &vertex, sizeof( OBJVERTEX ) );

                    if ( p[j] != -1 ) vertex.position = positions[ p[j] ];
                    if ( t[j] != -1 ) vertex.texcoord = texcoords[ t[j] ];
                    if ( n[j] != -1 ) vertex.normal   = normals[ n[j] ];

                    t_vertices.push_back( vertex );
                    index = t_vertices.size() - 1;
                    t_indices.push_back( index );
                }

            }
        }

        //　マテリアルファイル
        else if ( 0 == strcmp( buf, "mtllib" ) )
        {
            file >> mtlFileName;
            //　マテリアルファイルの読み込み
            if ( mtlFileName[0] )
            {
                if ( !LoadMTLFile( SetDirectoryPath(mtlFileName, m_directoryPath) ) )
                {
                    cerr << "Error : マテリアルのロードに失敗\n";
                    return false;
                }
            }
        }

        //　マテリアル
        else if ( 0 == strcmp( buf, "usemtl" ) )
        {
            char strName[OBJ_NAME_LENGTH] = {0};
            file >> strName;
            OBJSUBSET subset;
            subset.faceCount = 1;

            for ( unsigned int i = 0; i < m_NumMaterials; i++ )
            {
                if ( 0 == strcmp( m_Materials[i].name, strName ) )
                {
                    dwCurSubset = i;
                    break;
                }
            }

            subset.materialIndex = dwCurSubset;
            subset.faceStart     = dwFaceIndex*3;
            prevSize             = t_subsets.size();
            t_subsets.push_back( subset );
            if ( t_subsets.size() > 1 )
            {
                t_subsets[prevSize-1].faceCount = dwFaceCount*3;
                dwFaceCount = 0;
            }
        }

        file.ignore( OBJ_BUFFER_LENGTH, '\n' );
    }

    //　サブセット
    if ( t_subsets.size() > 0 )
    {
        int maxSize = t_subsets.size();
        t_subsets[maxSize-1].faceCount = dwFaceCount*3;
    }

    //　ファイルを閉じる
    file.close();

    //　頂点データをコピー
    m_NumVertices = t_vertices.size();
    m_Vertices = new OBJVERTEX[ m_NumVertices ];
    for ( unsigned int i = 0; i<m_NumVertices; i++ )
    { m_Vertices[i] = t_vertices[i]; }

    //　サブセットデータをコピー
    m_NumSubsets = t_subsets.size();
    m_Subsets = new OBJSUBSET[ m_NumSubsets ];
    for ( unsigned int i =0; i<m_NumSubsets; i++ )
    { m_Subsets[i] = t_subsets[i]; }

    //　インデックスデータをコピー
    m_NumIndices = t_indices.size();
    m_Indices = new unsigned int [ m_NumIndices ];
    for ( unsigned int i = 0; i<m_NumIndices; i++ )
    { m_Indices[i] = t_indices[i]; }

    //　バウンディングスフィアの作成
    m_Sphere.Create( m_Box );

    if ( normals.size() != 0 )
    { m_IsExistNormal = true; }
    if ( texcoords.size() != 0 )
    { m_IsExistTexCoord = true; }

    //　メモリ破棄
    positions .clear();
    normals   .clear();
    texcoords .clear();
    t_vertices.clear();
    t_subsets .clear();
    t_indices .clear();

    if ( !m_IsExistNormal )
    {
        if ( !ComputeNormal() )
        {
            printf_s( "Error : ComputeNormal() Failed. \n" );
        }
    }

    if ( m_IsExistNormal && m_IsExistTexCoord )
    {
        if ( !ComputeTangent() )
        {
            printf_s( "Error : ComputeTangent() Failed.\n" );
        }
    }
    else
    {
        printf_s( "Info : Tangent Cannot Compute." );
    }

    if ( !m_IsExistTexCoord )
    {
        printf_s( "Error : Texture Coordinate does not exist! please check model data.\n" );
    }

    //　正常終了
    return true;
}

//-----------------------------------------------------------------------
// Name : LoadMTLFile()
// Desc : MTLファイルの読み込み
//-----------------------------------------------------------------------
bool OBJMESH::LoadMTLFile( const char* filename )
{
    char buf[OBJ_BUFFER_LENGTH] = {0};
    int iMtlCount = -1;
    ifstream file;
    vector<OBJMATERIAL> t_materials;
    OBJMATERIAL material;
    InitMaterial( &material );

    //　ファイルを開く
    file.open( filename, ios::in );

    //　チェック
    if ( !file.is_open() )
    {
        cerr << "Error : ファイルオープンに失敗しました\n";
        cerr << "File Name : " << filename << endl;
        return false;
    }

    //　ループ
    for( ;; )
    {
        file >> buf;
        if ( !file )
            break;

        // New Material
        if ( 0 == strcmp( buf, "newmtl" ) )
        {
            iMtlCount++;
            t_materials.push_back( material );
            char strName[OBJ_NAME_LENGTH] = {0};
            file >> strName;
            strcpy( t_materials[iMtlCount].name, strName );
        }
        // Ambient Color
        else if ( 0 == strcmp( buf, "Ka" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            t_materials[iMtlCount].ambient = OBJVEC3( r, g, b );
        }
        // Diffuse Color
        else if ( 0 == strcmp( buf, "Kd" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            t_materials[iMtlCount].diffuse = OBJVEC3( r, g, b );
        }
        // Specular Color
        else if ( 0 == strcmp( buf, "Ks" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            t_materials[iMtlCount].specular = OBJVEC3( r, g, b );
        }
        else if ( 0 == strcmp( buf, "Ke" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            t_materials[iMtlCount].emissive = OBJVEC3( r, g, b );
        }
        // Alpha
        else if ( 0 == strcmp( buf, "d" ) ||
                  0 == strcmp( buf, "Tr" ) )
        {
            file >> t_materials[iMtlCount].alpha;
        }
        // Shininess
        else if ( 0 == strcmp( buf, "Ns" ) )
        {
            file >> t_materials[iMtlCount].shininess;
        }
        // Ambient Map
        else if ( 0 == strcmp( buf, "map_Ka" ) )
        {
            char mapKaName[OBJ_NAME_LENGTH];
            file >> mapKaName;
            //SetDirectoryPath( mapKaName, m_directoryPath );
            strcpy( t_materials[iMtlCount].ambientMapName, mapKaName );
        }
        // Diffuse Map
        else if ( 0 == strcmp( buf, "map_Kd" ) )
        {
            char mapKdName[OBJ_NAME_LENGTH];
            file >> mapKdName;
            //SetDirectoryPath( mapKdName, m_directoryPath );
            strcpy( t_materials[iMtlCount].diffuseMapName, mapKdName );
        }
        // Specular Map
        else if ( 0 == strcmp( buf, "map_Ks" ) )
        {
            char mapKsName[OBJ_NAME_LENGTH];
            file >> mapKsName;
            //SetDirectoryPath( mapKsName, m_directoryPath );
            strcpy( t_materials[iMtlCount].specularMapName, mapKsName );
        }
        // Bump Map
        else if ( ( 0 == strcmp( buf, "map_Bump" ) )
               || ( 0 == strcmp( buf, "map_bump" ) ) 
               || ( 0 == strcmp( buf, "bump"     ) ) )
        {
            char mapBumpName[OBJ_NAME_LENGTH];
            file >> mapBumpName;
            //SetDirectoryPath( mapBumpName, m_directoryPath );
            strcpy( t_materials[iMtlCount].bumpMapName, mapBumpName );
        }
        else if ( 0 == strcmp( buf, "disp" ) )
        {
            char displacementMapName[OBJ_NAME_LENGTH];
            file >> displacementMapName;
            //SetDirectoryPath( displacementMapName, m_directoryPath );
            strcpy( t_materials[iMtlCount].displacementMapName, displacementMapName );
        }

        file.ignore( OBJ_BUFFER_LENGTH, '\n' );
    }

    //　ファイルを閉じる
    file.close();

    //　マテリアルデータをコピー
    m_NumMaterials = t_materials.size();
    m_Materials = new OBJMATERIAL[ m_NumMaterials ];
    for ( unsigned int i = 0; i<m_NumMaterials; i++ )
        m_Materials[i] = t_materials[i];

    //　正常終了
    return true;
}

//-----------------------------------------------------------------------
// Name : LoadFile()
// Desc : メッシュファイルの読み込み
//-----------------------------------------------------------------------
bool OBJMESH::LoadFile( const char* filename )
{
    //　OBJ, MTLファイルを読み込み
    if ( !LoadOBJFile( filename ) )
    {
        cerr << "Error : メッシュファイルの読み込みに失敗しました\n";
        return false;
    }

    //　正常終了
    return true;
}

//-----------------------------------------------------------------------
// Name : ComputeNormal()
// Desc : 法線ベクトルを計算します.
//-----------------------------------------------------------------------
bool OBJMESH::ComputeNormal()
{
    if ( m_NumVertices == 0 )
    { return false; }

    if ( m_NumIndices == 0 )
    { return false; }

    if ( m_IsExistNormal )
    { return true; }

    OBJVEC3* pVertexNormal = new OBJVEC3 [ m_NumVertices ];
    if ( pVertexNormal == nullptr )
    {
        printf_s( "Error : Memory Allocate Failed.\n" );
        return false;
    }

    // 法線データを初期化.
    for( unsigned int i=0; i<m_NumVertices; ++i )
    {
        m_Vertices[ i ].normal.x = 0.0f;
        m_Vertices[ i ].normal.y = 0.0f;
        m_Vertices[ i ].normal.z = 0.0f;

        pVertexNormal[ i ].x = 0.0f;
        pVertexNormal[ i ].y = 0.0f;
        pVertexNormal[ i ].z = 0.0f;
    }

    for( unsigned int i=0; i<m_NumIndices; i+=3 )
    {
        unsigned int i0 = m_Indices[ i + 0 ];
        unsigned int i1 = m_Indices[ i + 1 ];
        unsigned int i2 = m_Indices[ i + 2 ];

        OBJVEC3 p0 = m_Vertices[ i0 ].position;
        OBJVEC3 p1 = m_Vertices[ i1 ].position;
        OBJVEC3 p2 = m_Vertices[ i2 ].position;

        OBJVEC3 e0 = Vec3Sub( p1, p0 );
        OBJVEC3 e1 = Vec3Sub( p2, p0 );

        // 面法線を算出.
        OBJVEC3 n = Vec3Cross( e0, e1 );
        OBJVEC3 normal = Vec3SafeNormalize( n );

        // 面法線を加算.
        pVertexNormal[ i0 ].x += normal.x;
        pVertexNormal[ i0 ].y += normal.y;
        pVertexNormal[ i0 ].z += normal.z;

        pVertexNormal[ i1 ].x += normal.x;
        pVertexNormal[ i1 ].y += normal.y;
        pVertexNormal[ i1 ].z += normal.z;

        pVertexNormal[ i2 ].x += normal.x;
        pVertexNormal[ i2 ].y += normal.y;
        pVertexNormal[ i2 ].z += normal.z;
    }

    // 加算した法線を正規化し，頂点法線を求める.
    for( unsigned int i=0; i<m_NumVertices; ++i )
    {
        OBJVEC3 n = Vec3SafeNormalize( pVertexNormal[ i ] );
        pVertexNormal[ i ] = n;
    }

    const float SMOOTHING_ANGLE = 59.7f;
    float cosSmooth = cosf( DegToRad( SMOOTHING_ANGLE ) );

    // スムージング処理
    for( unsigned int i=0; i<m_NumIndices; i+=3 )
    {
        unsigned int i0 = m_Indices[ i + 0 ];
        unsigned int i1 = m_Indices[ i + 1 ];
        unsigned int i2 = m_Indices[ i + 2 ];

        OBJVEC3 p0 = m_Vertices[ i0 ].position;
        OBJVEC3 p1 = m_Vertices[ i1 ].position;
        OBJVEC3 p2 = m_Vertices[ i2 ].position;

        OBJVEC3 e0 = Vec3Sub( p1, p0 );
        OBJVEC3 e1 = Vec3Sub( p2, p0 );

        // 面法線を算出.
        OBJVEC3 n = Vec3Cross( e0, e1 );
        OBJVEC3 faceNormal = Vec3SafeNormalize( n );

        // 頂点法線と面法線のなす角度を算出.
        float cosAngle0 = Vec3Dot( pVertexNormal[ i0 ], faceNormal );
        float cosAngle1 = Vec3Dot( pVertexNormal[ i1 ], faceNormal );
        float cosAngle2 = Vec3Dot( pVertexNormal[ i2 ], faceNormal );

        // スムージング処理.
        if ( cosAngle0 >= cosSmooth ) { m_Vertices[ i0 ].normal = pVertexNormal[ i0 ]; }
        else                          { m_Vertices[ i0 ].normal = faceNormal; }

        if( cosAngle1 >= cosSmooth )  { m_Vertices[ i1 ].normal = pVertexNormal[ i1 ]; }
        else                          { m_Vertices[ i1 ].normal = faceNormal; }

        if ( cosAngle2 >= cosSmooth ) { m_Vertices[ i2 ].normal = pVertexNormal[ i2 ]; }
        else                          { m_Vertices[ i2 ].normal = faceNormal; }
    }

    delete [] pVertexNormal;
    pVertexNormal = nullptr;

    // 法線存在フラグを立てる.
    m_IsExistNormal = true;

    return true;

}

//-----------------------------------------------------------------------
// Name : ComputeTangent()
// Desc : 接ベクトルを計算します.
//-----------------------------------------------------------------------
bool OBJMESH::ComputeTangent()
{
    if ( m_NumVertices == 0 )
    { return false; }

    if ( m_NumIndices == 0 )
    { return false; }

    if ( !m_IsExistNormal )
    { return false; }

    if ( !m_IsExistTexCoord )
    { return false; }

    // 接ベクトルと従法線ベクトルを初期化.
    for( unsigned int i=0; i<m_NumVertices; ++i )
    {
        m_Vertices[ i ].tangent.x = 0.0f;
        m_Vertices[ i ].tangent.y = 0.0f;
        m_Vertices[ i ].tangent.z = 0.0f;

        m_Vertices[ i ].binormal.x = 0.0f;
        m_Vertices[ i ].binormal.y = 0.0f;
        m_Vertices[ i ].binormal.z = 0.0f;
    }

    for( unsigned int i=0; i<m_NumIndices; i+=3 )
    {
        unsigned int i0 = m_Indices[ i + 0 ];
        unsigned int i1 = m_Indices[ i + 1 ];
        unsigned int i2 = m_Indices[ i + 2 ];

        OBJVEC3 p0 = m_Vertices[ i0 ].position;
        OBJVEC3 p1 = m_Vertices[ i1 ].position;
        OBJVEC3 p2 = m_Vertices[ i2 ].position;

        OBJVEC2 t0 = m_Vertices[ i0 ].texcoord;
        OBJVEC2 t1 = m_Vertices[ i1 ].texcoord;
        OBJVEC2 t2 = m_Vertices[ i2 ].texcoord;

        OBJVEC3 e0;
        OBJVEC3 e1;
        e0.x = p1.x - p0.x;
        e0.y = t1.x - t0.x;
        e0.z = t1.y - t0.y;

        e1.x = p2.x - p0.x;
        e1.y = t2.x - t0.x;
        e1.z = t2.y - t0.y;

        OBJVEC3 tan0;
        OBJVEC3 tan1;
        OBJVEC3 tan2;

        OBJVEC3 bin0;
        OBJVEC3 bin1;
        OBJVEC3 bin2;

        OBJVEC3 crs;
        
        crs = Vec3Cross( e0, e1 );
        crs = Vec3SafeNormalize( crs );
        if ( fabs( crs.x ) < 1e-4f )
        { crs.x = 1.0f; }
        
        float tanX = -crs.y / crs.x;
        float binX = -crs.z / crs.x;

        tan0.x = tanX;
        tan1.x = tanX;
        tan2.x = tanX;

        bin0.x = binX;
        bin1.x = binX;
        bin2.x = binX;

        e0.x = p1.y - p0.y;
        e1.x = p2.y - p0.y;
        crs = Vec3Cross( e0, e1 );
        crs = Vec3SafeNormalize( crs );
        if ( fabs( crs.x ) < 1e-4f ) 
        { crs.x = 1.0f; }
        
        float tanY = -crs.y / crs.x;
        float binY = -crs.z / crs.x;
        tan0.y = tanY;
        tan1.y = tanY;
        tan2.y = tanY;

        bin0.y = binY;
        bin1.y = binY;
        bin2.y = binY;

        e0.x = p1.z - p0.z;
        e1.x = p2.z - p0.z;
        crs = Vec3Cross( e0, e1 );
        crs = Vec3SafeNormalize( crs );
        if ( fabs( crs.x ) < 1e-4f )
        { crs.x = 1.0f; }

        float tanZ = -crs.y / crs.x;
        float binZ = -crs.z / crs.x;
        tan0.z = tanZ;
        tan1.z = tanZ;
        tan2.z = tanZ;

        bin0.z = binZ;
        bin1.z = binZ;
        bin2.z = binZ;

        float dp0 = Vec3Dot( tan0, m_Vertices[ i0 ].normal );
        float dp1 = Vec3Dot( tan1, m_Vertices[ i1 ].normal );
        float dp2 = Vec3Dot( tan2, m_Vertices[ i2 ].normal );

        tan0.x -= m_Vertices[ i0 ].normal.x * dp0;
        tan0.y -= m_Vertices[ i0 ].normal.y * dp0;
        tan0.z -= m_Vertices[ i0 ].normal.z * dp0;

        tan1.x -= m_Vertices[ i1 ].normal.x * dp1;
        tan1.y -= m_Vertices[ i1 ].normal.y * dp1;
        tan1.z -= m_Vertices[ i1 ].normal.z * dp1;
        
        tan2.x -= m_Vertices[ i2 ].normal.x * dp2;
        tan2.y -= m_Vertices[ i2 ].normal.y * dp2;
        tan2.z -= m_Vertices[ i2 ].normal.z * dp2;

        tan0 = Vec3SafeNormalize( tan0 );
        tan1 = Vec3SafeNormalize( tan1 );
        tan2 = Vec3SafeNormalize( tan2 );


        dp0 = Vec3Dot( bin0, m_Vertices[ i0 ].normal );
        dp1 = Vec3Dot( bin1, m_Vertices[ i1 ].normal );
        dp2 = Vec3Dot( bin2, m_Vertices[ i2 ].normal );

        bin0.x -= m_Vertices[ i0 ].normal.x * dp0;
        bin0.y -= m_Vertices[ i0 ].normal.y * dp0;
        bin0.z -= m_Vertices[ i0 ].normal.z * dp0;

        bin1.x -= m_Vertices[ i1 ].normal.x * dp1;
        bin1.y -= m_Vertices[ i1 ].normal.y * dp1;
        bin1.z -= m_Vertices[ i1 ].normal.z * dp1;
        
        bin2.x -= m_Vertices[ i2 ].normal.x * dp2;
        bin2.y -= m_Vertices[ i2 ].normal.y * dp2;
        bin2.z -= m_Vertices[ i2 ].normal.z * dp2;

        bin0 = Vec3SafeNormalize( bin0 );
        bin1 = Vec3SafeNormalize( bin1 );
        bin2 = Vec3SafeNormalize( bin2 );


        m_Vertices[ i0 ].tangent = tan0;
        m_Vertices[ i1 ].tangent = tan1;
        m_Vertices[ i2 ].tangent = tan2;

        m_Vertices[ i0 ].binormal = bin0;
        m_Vertices[ i1 ].binormal = bin1;
        m_Vertices[ i2 ].binormal = bin2;
    }

    return true;
}

//------------------------------------------------------------------------
// Name : GetNumVertices()
// Desc : 頂点数を取得
//------------------------------------------------------------------------
unsigned int OBJMESH::GetNumVertices()
{ return m_NumVertices; }

//-----------------------------------------------------------------------
// Name : GetNumSubsets()
// Desc : サブセット数を取得
//-----------------------------------------------------------------------
unsigned int OBJMESH::GetNumSubsets()
{ return m_NumSubsets; }

//-----------------------------------------------------------------------
// Name : GetNumMaterials()
// Desc : マテリアル数を取得
//-----------------------------------------------------------------------
unsigned int OBJMESH::GetNumMaterials()
{ return m_NumMaterials; }

//-----------------------------------------------------------------------
// Name : GetNumIndices()
// Desc : インデックス数を取得
//-----------------------------------------------------------------------
unsigned int OBJMESH::GetNumIndices()
{ return m_NumIndices; }

//-----------------------------------------------------------------------
// Name : GetIndexData()
// Desc : 指定されたインデックスデータを取得
//-----------------------------------------------------------------------
unsigned int OBJMESH::GetIndexData( unsigned int index )
{ return m_Indices[index]; }

//-----------------------------------------------------------------------
// Name : GetVertex()
// Desc : 指定された頂点データを取得
//-----------------------------------------------------------------------
OBJVERTEX OBJMESH::GetVertex( unsigned int index )
{ return m_Vertices[index]; }

//-----------------------------------------------------------------------
// Name : GetSubsets()
// Desc : 指定されたサブセットデータを取得
//-----------------------------------------------------------------------
OBJSUBSET OBJMESH::GetSubset( unsigned int index )
{ return m_Subsets[index]; }

//-----------------------------------------------------------------------
// Name : GetMaterial()
// Desc : 指定されたマテリアルデータを取得
//-----------------------------------------------------------------------
OBJMATERIAL OBJMESH::GetMaterial( unsigned int index )
{ return m_Materials[index]; }

//-----------------------------------------------------------------------
// Name : GetVertices()
// Desc : 頂点データを取得
//-----------------------------------------------------------------------
OBJVERTEX* OBJMESH::GetVertices()
{ return m_Vertices; }

//-----------------------------------------------------------------------
// Name : GetSubsets()
// Desc : サブセットデータを取得
//-----------------------------------------------------------------------
OBJSUBSET* OBJMESH::GetSubsets()
{ return m_Subsets; }

//-----------------------------------------------------------------------
// Name : GetMaterails()
// Desc : マテリアルデータを取得
//-----------------------------------------------------------------------
OBJMATERIAL* OBJMESH::GetMaterials()
{ return m_Materials; }

//-----------------------------------------------------------------------
// Name : GetIndices()
// Desc : インデックスデータを取得
//-----------------------------------------------------------------------
unsigned int* OBJMESH::GetIndices()
{ return m_Indices; }
        
//-----------------------------------------------------------------------
// Name : GetBox()
// Desc : バウンディングボックスを取得
//-----------------------------------------------------------------------
OBJBOUNDINGBOX OBJMESH::GetBox()
{ return m_Box; }

//-----------------------------------------------------------------------
// Name : GetSphere()
// Desc : バウンディングスフィアを取得
//-----------------------------------------------------------------------
OBJBOUNDINGSPHERE OBJMESH::GetSphere()
{ return m_Sphere; }


//-----------------------------------------------------------------------
// Name : WriteDirect()
// Desc : 最適化せず直接バイナリ書き込みをします.
//-----------------------------------------------------------------------
void OBJMESH::WriteDirect( FILE* pFile )
{
    // データヘッダーを設定.
    SMD_DATA_HEADER dataHeader;
    dataHeader.numVertices  = m_NumVertices;
    dataHeader.numIndices   = m_NumIndices;
    dataHeader.numMaterials = m_NumMaterials;
    dataHeader.numSubsets   = m_NumSubsets;
    dataHeader.vertexStructureSize   = SMD_VERTEX_STRUCT_SIZE;
    dataHeader.indexStructureSize    = SMD_INDEX_STRUCT_SIZE;
    dataHeader.materialStructureSize = SMD_MATERIAL_STRUCT_SIZE;
    dataHeader.subsetStructureSize   = SMD_SUBSET_STRUCT_SIZE;

    // ファイルヘッダーを設定.
    SMD_FILE_HEADER fileHeader;
    memcpy( &fileHeader.magic, SMD_MAGIC, sizeof( unsigned char ) * 4 );
    fileHeader.version        = SMD_VERSION;
    fileHeader.dataHeaderSize = SMD_DATA_HEADER_SIZE;
    fileHeader.dataHeader     = dataHeader;

#if 0
    //printf_s( "File Version  = 0x%08x\n", fileHeader.version );
    //printf_s( "Num Vertices  = %d\n", dataHeader.numVertices );
    //printf_s( "Num Indices   = %d\n", dataHeader.numIndices );
    //printf_s( "Num Materials = %d\n", dataHeader.numMaterials );
    //printf_s( "Num Subsets   = %d\n", dataHeader.numSubsets );
#endif

    // ヘッダーを書き込み.
    fwrite( &fileHeader, sizeof( SMD_FILE_HEADER ), 1, pFile );

    // 頂点データを書き込み.
    for( size_t i=0; i<m_NumVertices; ++i )
    {
        SMD_VERTEX vertex;
        ZeroMemory( &vertex, sizeof( SMD_VERTEX ) );

        // 位置座標.
        vertex.position.x = static_cast<double>( m_Vertices[ i ].position.x );
        vertex.position.y = static_cast<double>( m_Vertices[ i ].position.y );
        vertex.position.z = static_cast<double>( m_Vertices[ i ].position.z );
        
        // 法線ベクトル.
        vertex.normal.x   = static_cast<double>( m_Vertices[ i ].normal.x );
        vertex.normal.y   = static_cast<double>( m_Vertices[ i ].normal.y );
        vertex.normal.z   = static_cast<double>( m_Vertices[ i ].normal.z );

        // テクスチャ座標.
        vertex.texcoord.x = static_cast<double>( m_Vertices[ i ].texcoord.x );
        vertex.texcoord.y = static_cast<double>( m_Vertices[ i ].texcoord.y );

#if 0
        printf_s( "vertex[ %d ] : \n", i );
        printf_s( "position = ( %f, %f, %f )\n", vertex.position.x, vertex.position.y, vertex.position.z );
        printf_s( "normal   = ( %f, %f, %f )\n", vertex.normal.x, vertex.normal.y, vertex.normal.z );
        printf_s( "tangent  = ( %f, %f, %f )\n", vertex.tangent.x, vertex.tangent.y, vertex.tangent.z );
        printf_s( "texcoord = ( %f, %f )\n", vertex.texcoord.x, vertex.texcoord.y );
        printf_s( "\n" );
#endif

        fwrite( &vertex, sizeof( SMD_VERTEX ), 1, pFile );
    }

    // 頂点インデックスを書き込み.
    for( size_t i=0; i<m_NumIndices; ++i )
    {
#if 0
        //printf_s( "index[ %d ] = %d\n", i, m_Indices[ i ] );
#endif

        fwrite( &m_Indices[ i ], sizeof( unsigned int ), 1, pFile );
    }

    // マテリアルデータを書き込み.
    for( size_t i=0; i<m_NumMaterials; ++i )
    {
        SMD_MATERIAL material;
        ZeroMemory( &material, sizeof( SMD_MATERIAL ) );

        // 拡散反射色.
        material.diffuse.x = m_Materials[ i ].diffuse.x;
        material.diffuse.y = m_Materials[ i ].diffuse.y;
        material.diffuse.z = m_Materials[ i ].diffuse.z;

        // 自己照明.
        material.emissive.x = m_Materials[ i ].emissive.x;
        material.emissive.y = m_Materials[ i ].emissive.y;
        material.emissive.z = m_Materials[ i ].emissive.z;

        // 屈折率.
        material.refractivity = m_Materials[ i ].alpha;

        // テクスチャマップ名
        strcpy( material.diffuseMap, m_Materials[ i ].diffuseMapName );

#if 0
        //printf_s( "ambient  = ( %f, %f, %f )\n", material.ambientR, material.ambientG, material.ambientB );
        //printf_s( "diffuse  = ( %f, %f, %f )\n", material.diffuseR, material.diffuseG, material.diffuseB );
        //printf_s( "specular = ( %f, %f, %f )\n", material.specularR, material.specularG, material.specularB );
        //printf_s( "emissive = ( %f, %f, %f )\n", material.emissiveR, material.emissiveG, material.emissiveB );
        //printf_s( "power    = %f\n", material.power  );
        //printf_s( "alpha    = %f\n", material.bumpMap );
        //printf_s( "ambientmap  = %s\n", material.ambientMap );
        //printf_s( "diffusemap  = %s\n", material.diffuseMap );
        //printf_s( "specularmap = %s\n", material.specularMap );
        //printf_s( "bumpmap     = %s\n", material.bumpMap );
        //printf_s( "displacementmap = %s\n", material.displacementMap );
        //printf_s( "\n" );
#endif

        fwrite( &material, sizeof( SMD_MATERIAL ), 1, pFile );
    }

    // サブセットをデータを書き込み.
    for( size_t i=0; i<m_NumSubsets; ++i )
    {
        SMD_SUBSET subset;
        ZeroMemory( &subset, sizeof( SMD_SUBSET ) );

        subset.indexOffset   = m_Subsets[ i ].faceStart;
        subset.indexCount    = m_Subsets[ i ].faceCount;
        subset.materialIndex = m_Subsets[ i ].materialIndex;
    
#if 0
        //printf_s( "indexOffset  = %d\n", subset.indexOffset );
        //printf_s( "indexCount   = %d\n", subset.indexCount );
        //printf_s( "materalIndex = %d\n", subset.materialIndex );
        //printf_s( "\n" );
#endif

        fwrite( &subset, sizeof( SMD_SUBSET ), 1, pFile );
    }
}

//-----------------------------------------------------------------------
// Name : CompareSubset()
// Desc : サブセットの大小比較を行います.
//-----------------------------------------------------------------------
static int CompareSubset( const void* pA, const void* pB )
{
    OBJSUBSET* a = (OBJSUBSET*)pA;
    OBJSUBSET* b = (OBJSUBSET*)pB;

    // pAの方がマテリアル番号が大きい場合.
    if ( a->materialIndex > b->materialIndex )
    { return 1; }

    // マテリアル番号が同じ場合.
    if ( a->materialIndex == b->materialIndex )
    {
        // pAの開始面番号が大きい場合.
        if ( a->faceStart > b->faceStart )
        { return 1; }
        // pAとpBの開始面番号が同じ場合.
        else if ( a->faceStart == b->faceStart )
        { return 0; }
        // pAの開始面番号が小さい場合.
        else
        { return -1; }
    }

    // pAの方がマテリアル番号が小さい場合.
    if ( a->materialIndex < b->materialIndex )
    { return -1; }

    // ありえないけど0を返す.
    return 0;
};

//-----------------------------------------------------------------------
// Name : WriteOptimize()
// Desc : 最適化しながらバイナリ書き込みします.
//-----------------------------------------------------------------------
void OBJMESH::WriteOptimize( FILE* pFile )
{
    // サブセットをマテリアルインデックスでクイックソートする.
    OBJSUBSET* pSubsets = new OBJSUBSET[ m_NumSubsets ];
    for( size_t i=0; i<m_NumSubsets; ++i )
    {
        pSubsets[ i ] = m_Subsets[ i ];
    }
    qsort( pSubsets, m_NumSubsets, sizeof( OBJSUBSET ), CompareSubset );

#if 0
    //for( size_t i=0; i<m_NumSubsets; ++i )
    //{
    //    printf_s( "pSubset[ %d ]\n", i );
    //    printf_s( "    startIndex = %d\n", pSubsets[ i ].faceStart );
    //    printf_s( "    indexCount = %d\n", pSubsets[ i ].faceCount );
    //    printf_s( "    materialId = %d\n", pSubsets[ i ].materialIndex );
    //}
#endif

    // 整形後用のサブセットのメモリを確保.
    OBJSUBSET* pSortedSubsets = new OBJSUBSET[ m_NumMaterials ];

    for( size_t i=0; i<m_NumMaterials; ++i )
    {
        pSortedSubsets[ i ].faceCount = 0;
        pSortedSubsets[ i ].faceStart = 0;
        pSortedSubsets[ i ].materialIndex = 0;
    }

    unsigned int matIdx    = -1;
    unsigned int faceCount = 0;
    int idx = -1;

    // 整形後用のサブセットを構築.
    for( size_t i=0; i<m_NumSubsets; ++i )
    {
        if ( matIdx == (int)pSubsets[ i ].materialIndex )
        {
            faceCount += pSubsets[ i ].faceCount;
            pSortedSubsets[ idx ].faceCount += pSubsets[ i ].faceCount;
        }
        else
        {
            idx++;
            pSortedSubsets[ idx ].faceStart = faceCount;
            pSortedSubsets[ idx ].faceCount = pSubsets[ i ].faceCount;
            pSortedSubsets[ idx ].materialIndex = pSubsets[ i ].materialIndex;
            matIdx = pSubsets[ i ].materialIndex;
            faceCount += pSubsets[ i ].faceCount;

#if 0
            //printf_s( "idx = %d\n", idx );
            //printf_s( "faceCount = %d\n", faceCount );
            //printf_s( "matIdx = %d\n", matIdx );
#endif
        }
    }

#if 0
    //for( size_t i=0; i<m_NumMaterials; ++i )
    //{
    //    printf_s( "pSortedSubset[ %d ]\n", i );
    //    printf_s( "    startIndex = %d\n", pSortedSubsets[ i ].faceStart );
    //    printf_s( "    indexCount = %d\n", pSortedSubsets[ i ].faceCount );
    //    printf_s( "    materialId = %d\n", pSortedSubsets[ i ].materialIndex );
    //}
#endif

    // データヘッダーを設定.
    SMD_DATA_HEADER dataHeader;
    dataHeader.numVertices  = m_NumVertices;
    dataHeader.numIndices   = m_NumIndices;
    dataHeader.numMaterials = m_NumMaterials;
    dataHeader.numSubsets   = m_NumMaterials;
    dataHeader.vertexStructureSize   = SMD_VERTEX_STRUCT_SIZE;
    dataHeader.indexStructureSize    = SMD_INDEX_STRUCT_SIZE;
    dataHeader.materialStructureSize = SMD_MATERIAL_STRUCT_SIZE;
    dataHeader.subsetStructureSize   = SMD_SUBSET_STRUCT_SIZE;

    // ファイルヘッダーを設定.
    SMD_FILE_HEADER fileHeader;
    memcpy( &fileHeader.magic, SMD_MAGIC, sizeof( unsigned char ) * 4 );
    fileHeader.version        = SMD_VERSION;
    fileHeader.dataHeaderSize = SMD_DATA_HEADER_SIZE;
    fileHeader.dataHeader     = dataHeader;

#if 0
    //printf_s( "File Version  = 0x%08x\n", fileHeader.version );
    //printf_s( "Num Vertices  = %d\n", dataHeader.numVertices );
    //printf_s( "Num Indices   = %d\n", dataHeader.numIndices );
    //printf_s( "Num Materials = %d\n", dataHeader.numMaterials );
    //printf_s( "Num Subsets   = %d\n", dataHeader.numSubsets );
#endif

    // ヘッダーを書き込み.
    fwrite( &fileHeader, sizeof( SMD_FILE_HEADER ), 1, pFile );


    // 頂点データはそのまま書き込み.
    for( size_t i=0; i<m_NumVertices; ++i )
    {
        SMD_VERTEX vertex;
        ZeroMemory( &vertex, sizeof( SMD_VERTEX ) );

        // 位置座標.
        vertex.position.x = static_cast<double>( m_Vertices[ i ].position.x );
        vertex.position.y = static_cast<double>( m_Vertices[ i ].position.y );
        vertex.position.z = static_cast<double>( m_Vertices[ i ].position.z );
        
        // 法線ベクトル.
        vertex.normal.x   = static_cast<double>( m_Vertices[ i ].normal.x );
        vertex.normal.y   = static_cast<double>( m_Vertices[ i ].normal.y );
        vertex.normal.z   = static_cast<double>( m_Vertices[ i ].normal.z );

        // テクスチャ座標.
        vertex.texcoord.x = static_cast<double>( m_Vertices[ i ].texcoord.x );
        vertex.texcoord.y = static_cast<double>( m_Vertices[ i ].texcoord.y );

#if 0
        printf_s( "vertex[ %d ] : \n", i );
        printf_s( "position = ( %f, %f, %f )\n", vertex.position.x, vertex.position.y, vertex.position.z );
        printf_s( "normal   = ( %f, %f, %f )\n", vertex.normal.x, vertex.normal.y, vertex.normal.z );
        printf_s( "tangent  = ( %f, %f, %f )\n", vertex.tangent.x, vertex.tangent.y, vertex.tangent.z );
        printf_s( "texcoord = ( %f, %f )\n", vertex.texcoord.x, vertex.texcoord.y );
        printf_s( "\n" );
#endif

        fwrite( &vertex, sizeof( SMD_VERTEX ), 1, pFile );
    }

    // インデックスデータはクイックソートしたデータを利用して書き込み.
    for( size_t i=0; i<m_NumSubsets; ++i )
    {
        for( size_t j=0; j<pSubsets[ i ].faceCount; ++j )
        {
            unsigned int index = m_Indices[ pSubsets[ i ].faceStart + j ];
            fwrite( &index, sizeof( unsigned int ), 1, pFile );
        }
    }

    // マテリアルデータを書き込み.
    for( size_t i=0; i<m_NumMaterials; ++i )
    {
        SMD_MATERIAL material;
        ZeroMemory( &material, sizeof( SMD_MATERIAL ) );

        // 拡散反射色.
        material.diffuse.x = m_Materials[ i ].diffuse.x;
        material.diffuse.y = m_Materials[ i ].diffuse.y;
        material.diffuse.z = m_Materials[ i ].diffuse.z;

        // 自己照明.
        material.emissive.x = m_Materials[ i ].emissive.x;
        material.emissive.y = m_Materials[ i ].emissive.y;
        material.emissive.z = m_Materials[ i ].emissive.z;

        // 屈折率.
        material.refractivity = m_Materials[ i ].alpha;

        // テクスチャマップ名.
        strcpy( material.diffuseMap, m_Materials[ i ].diffuseMapName );

#if 0
        //printf_s( "ambient  = ( %f, %f, %f )\n", material.ambientR, material.ambientG, material.ambientB );
        //printf_s( "diffuse  = ( %f, %f, %f )\n", material.diffuseR, material.diffuseG, material.diffuseB );
        //printf_s( "specular = ( %f, %f, %f )\n", material.specularR, material.specularG, material.specularB );
        //printf_s( "emissive = ( %f, %f, %f )\n", material.emissiveR, material.emissiveG, material.emissiveB );
        //printf_s( "power    = %f\n", material.power  );
        //printf_s( "alpha    = %f\n", material.bumpMap );
        //printf_s( "ambientmap  = %s\n", material.ambientMap );
        //printf_s( "diffusemap  = %s\n", material.diffuseMap );
        //printf_s( "specularmap = %s\n", material.specularMap );
        //printf_s( "bumpmap     = %s\n", material.bumpMap );
        //printf_s( "displacementmap = %s\n", material.displacementMap );
        //printf_s( "\n" );
#endif

        fwrite( &material, sizeof( SMD_MATERIAL ), 1, pFile );
    }

    // サブセットをデータを書き込み.
    for( size_t i=0; i<m_NumMaterials; ++i )
    {
        SMD_SUBSET subset;
        ZeroMemory( &subset, sizeof( SMD_SUBSET ) );

        subset.indexOffset   = pSortedSubsets[ i ].faceStart;
        subset.indexCount    = pSortedSubsets[ i ].faceCount;
        subset.materialIndex = pSortedSubsets[ i ].materialIndex;
    
#if 0
        //printf_s( "indexOffset  = %d\n", subset.indexOffset );
        //printf_s( "indexCount   = %d\n", subset.indexCount );
        //printf_s( "materalIndex = %d\n", subset.materialIndex );
        //printf_s( "\n" );
#endif

        fwrite( &subset, sizeof( SMD_SUBSET ), 1, pFile );
    }

    // 不要なメモリを解放.
    delete [] pSortedSubsets;
    pSortedSubsets = nullptr;

    delete [] pSubsets;
    pSubsets = nullptr;
};

//-----------------------------------------------------------------------
// Name : SaveToBinary()
// Desc : バイナリ保存します.
//-----------------------------------------------------------------------
bool OBJMESH::SaveToBinary( const char* filename )
{
    FILE* pFile;

    // ファイルを開く.
    errno_t err = fopen_s( &pFile, filename, "wb" );
    if ( err != 0 )
    {
        printf_s( "Error : SaveToMsh() Failed. filename = %s", filename );
        return false;
    }

    // 最適化の必要がないのでそのまま書き込み.
    if ( m_NumMaterials == m_NumSubsets )
    {
        WriteDirect( pFile );
    }
    // 最適化しながら書き込み.
    else
    {
        WriteOptimize( pFile );
    }

    // ファイルを閉じる.
    fclose( pFile );
    pFile = nullptr;

    // 正常終了.
    return true;
}


