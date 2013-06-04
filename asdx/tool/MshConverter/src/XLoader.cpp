
#define _CRT_SECURE_NO_WARNINGS 1

#include "XLoader.h"
#include <cassert>


//////////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
// Name : Vec3Add()
// Desc : ベクトルを足します.
//-----------------------------------------------------------------------
inline
XFVEC3 Vec3Add( const XFVEC3& a, const XFVEC3& b )
{
    XFVEC3 result;
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
XFVEC3 Vec3Sub( const XFVEC3& a, const XFVEC3& b )
{
    XFVEC3 result;
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
float Vec3Dot( const XFVEC3& a, const XFVEC3& b )
{
    return ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z );
}

//-----------------------------------------------------------------------
// Name : Vec3Cross()
// Desc : ベクトルの外積を求めます.
//-----------------------------------------------------------------------
inline
XFVEC3 Vec3Cross( const XFVEC3& a, const XFVEC3& b )
{
    XFVEC3 result;
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
XFVEC3 Vec3SafeNormalize( const XFVEC3& v )
{
    XFVEC3 result;

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
char* XGetDirectoryPath( const char* filename )
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
char* XCutDirectoryPath( const char* filename )
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
char* XSetDirectoryPath( char* dest, char* directory )
{
    char* str;
    str = new char[strlen(directory) + strlen(dest) + 1];
    strcpy( str, directory );
    strcat( str, dest );
    return str;
}

enum
{
    BLOCK_START = 1,
    BLOCK_END   = -1,
    BLOCK_EOF   = 0
};

enum NODE_TYPE
{
    NODE_UNKNOWN = 0,       //!< 解析対象外.
    NODE_FRAME,             //!< フレーム"Frame".
    NODE_MESH,              //!< メッシュ"Mesh".
    NODE_MESH_NORMAL,       //!< 法線ベクトル"MeshNormals".
    NODE_MESH_TEXCOORD,     //!< テクスチャ座標"MeshTextureCoords".
    NODE_MESH_MATERIAL,     //!< マテリアル"Material".
    NODE_MESH_MATERIALLIST, //!< マテリアルリスト"MeshMaterialLists".
    NODE_MESH_SKINHEADER,   //!< スキンヘッダー"XMeshSkinHeader".
    NODE_MESH_SKINWEIGHTS,  //!< スキンウェイト"SkinWeights".
    NODE_ANIMATION,         //!< アニメーション"Animation".
    NODE_ANIMATIONKEY,      //!< アニメーションキー"AnimationKey".
    NODE_ANIMATIONSET,      //!< アニメーションセット"AnimationSet".
    NODE_BLOCK_BEGIN,       //!< ブロック開始"{"
    NODE_BLOCK_END,         //!< ブロック終了"}"
    NODE_EOF                //!< EOF.
};


XMESH::XMESH()
: m_NumVertex   ( 0 )
, m_NumMaterial ( 0 )
, m_NumSubset   ( 0 )
, m_NumBone     ( 0 )
, m_NumIndex    ( 0 )
, m_NumClip     ( 0 )
, m_pVertices   ( nullptr )
, m_pMaterials  ( nullptr )
, m_pSubsets    ( nullptr )
, m_pBones      ( nullptr )
, m_pIndices    ( nullptr )
, m_pClips      ( nullptr )
, m_pTokenizer  ( nullptr )
, m_CurFrameID  ( -1 )
, m_CurParentID ( -1 )
, m_IsExistTexCoord ( false )
, m_IsExistNormal   ( false )
{
    std::memset( &m_DirectionPath, 0, sizeof( XSTRING ) );
}


XMESH::~XMESH()
{
    Release();
}

void XMESH::Release()
{
    if ( m_pVertices )
    {
        delete [] m_pVertices;
        m_pVertices = nullptr;
    }

    if ( m_pMaterials )
    {
        delete [] m_pMaterials;
        m_pMaterials = nullptr;
    }

    if ( m_pSubsets )
    {
        delete [] m_pSubsets;
        m_pSubsets = nullptr;
    }

    if ( m_pBones )
    {
        delete [] m_pBones;
        m_pBones = nullptr;
    }

    if ( m_pIndices )
    {
        delete [] m_pIndices;
        m_pIndices = nullptr;
    }

    if ( m_pClips )
    {
        for( unsigned int i=0; i<m_NumClip; ++i )
        {
            if ( m_pClips[ i ].pKeyFrame )
            {
                for( unsigned int j=0; j<m_pClips[ i ].numKeyFrames; ++j )
                {
                    delete [] m_pClips[ i ].pKeyFrame[ j ].pKeys;
                    m_pClips[ i ].pKeyFrame[ j ].pKeys = nullptr;
                }

                delete [] m_pClips[ i ].pKeyFrame;
                m_pClips[ i ].pKeyFrame = nullptr;
            }
        }
        delete [] m_pClips;
        m_pClips = nullptr;
    }

    if ( m_pTokenizer )
    {
        delete m_pTokenizer;
        m_pTokenizer = nullptr;
    }

    m_NumVertex     = 0;
    m_NumMaterial   = 0;
    m_NumSubset     = 0;
    m_NumBone       = 0;
    m_NumIndex      = 0;
    m_NumClip       = 0;
    m_CurFrameID    = -1;
    m_CurParentID   = -1;

    m_IsExistNormal   = false;
    m_IsExistTexCoord = false;
}


//-----------------------------------------------------------------------
// Name : ComputeNormal()
// Desc : 法線ベクトルを計算します.
//-----------------------------------------------------------------------
bool XMESH::ComputeNormal()
{
    if ( m_NumVertex == 0 )
    { return false; }

    if ( m_NumIndex == 0 )
    { return false; }

    if ( m_IsExistNormal )
    { return true; }

    // 法線データを初期化.
    for( unsigned int i=0; i<m_NumVertex; ++i )
    {
        m_pVertices[ i ].normal.x = 0.0f;
        m_pVertices[ i ].normal.y = 0.0f;
        m_pVertices[ i ].normal.z = 0.0f;
    }

    for( unsigned int i=0; i<m_NumIndex; i+=3 )
    {
        unsigned int i0 = m_pIndices[ i + 0 ];
        unsigned int i1 = m_pIndices[ i + 1 ];
        unsigned int i2 = m_pIndices[ i + 2 ];

        XFVEC3 p0 = m_pVertices[ i0 ].position;
        XFVEC3 p1 = m_pVertices[ i1 ].position;
        XFVEC3 p2 = m_pVertices[ i2 ].position;

        XFVEC3 e0 = Vec3Sub( p1, p0 );
        XFVEC3 e1 = Vec3Sub( p2, p0 );

        // 面法線を算出.
        XFVEC3 n = Vec3Cross( e0, e1 );
        XFVEC3 normal = Vec3SafeNormalize( n );

        // 面法線を加算.
        m_pVertices[ i0 ].normal.x += normal.x;
        m_pVertices[ i0 ].normal.y += normal.y;
        m_pVertices[ i0 ].normal.z += normal.z;

        m_pVertices[ i1 ].normal.x += normal.x;
        m_pVertices[ i1 ].normal.y += normal.y;
        m_pVertices[ i1 ].normal.z += normal.z;

        m_pVertices[ i2 ].normal.x += normal.x;
        m_pVertices[ i2 ].normal.y += normal.y;
        m_pVertices[ i2 ].normal.z += normal.z;
    }

    // 加算した法線を正規化し，頂点法線を求める.
    for( unsigned int i=0; i<m_NumVertex; ++i )
    {
        XFVEC3 n = Vec3SafeNormalize( m_pVertices[ i ].normal );
        m_pVertices[ i ].normal = n;
    }

    // 法線存在フラグを立てる.
    m_IsExistNormal = true;

    return true;
}

//-----------------------------------------------------------------------
// Name : ComputeTangent()
// Desc : 接ベクトルを計算します.
//-----------------------------------------------------------------------
bool XMESH::ComputeTangent()
{
    if ( m_NumVertex == 0 )
    { return false; }

    if ( m_NumIndex == 0 )
    { return false; }

    if ( !m_IsExistNormal )
    { return false; }

    if ( !m_IsExistTexCoord )
    { return false; }

    // 接ベクトルと従法線ベクトルを初期化.
    for( unsigned int i=0; i<m_NumVertex; ++i )
    {
        m_pVertices[ i ].tangent.x = 0.0f;
        m_pVertices[ i ].tangent.y = 0.0f;
        m_pVertices[ i ].tangent.z = 0.0f;

        m_pVertices[ i ].binormal.x = 0.0f;
        m_pVertices[ i ].binormal.y = 0.0f;
        m_pVertices[ i ].binormal.z = 0.0f;
    }

    for( unsigned int i=0; i<m_NumIndex; i+=3 )
    {
        unsigned int i0 = m_pIndices[ i + 0 ];
        unsigned int i1 = m_pIndices[ i + 1 ];
        unsigned int i2 = m_pIndices[ i + 2 ];

        XFVEC3 p0 = m_pVertices[ i0 ].position;
        XFVEC3 p1 = m_pVertices[ i1 ].position;
        XFVEC3 p2 = m_pVertices[ i2 ].position;

        XFVEC2 t0 = m_pVertices[ i0 ].texcoord;
        XFVEC2 t1 = m_pVertices[ i1 ].texcoord;
        XFVEC2 t2 = m_pVertices[ i2 ].texcoord;

        float x1 = p1.x - p0.x;
        float x2 = p2.x - p0.x;

        float y1 = p1.y - p0.y;
        float y2 = p2.y - p0.y;

        float z1 = p1.z - p0.z;
        float z2 = p2.z - p0.z;

        float u1 = t1.x - t0.x;
        float u2 = t2.x - t0.x;

        float v1 = t1.y - t0.y;
        float v2 = t2.y - t0.y;

        float r = ( u1 * v2 - v2 * u1 );
        if ( r != 0.0f )
        { r = 1.0f / r; }

        // 面の接ベクトルを求める.
        XFVEC3 tangent;
        tangent.x = ( v2 * x1 - v1 * x2 ) * r;
        tangent.y = ( v2 * y1 - v1 * y2 ) * r;
        tangent.z = ( v2 * z1 - v1 * z2 ) * r;
        tangent = Vec3SafeNormalize( tangent );

        // 面の従法線ベクトルを求める.
        XFVEC3 binormal;
        binormal.x = ( u1 * x2 - u2 * x1 ) * r;
        binormal.y = ( u1 * y2 - u2 * y1 ) * r;
        binormal.z = ( u1 * z2 - u2 * z1 ) * r;
        binormal = Vec3SafeNormalize( binormal );
        
        // 接ベクトルを加算.
        m_pVertices[ i0 ].tangent.x += tangent.x;
        m_pVertices[ i0 ].tangent.y += tangent.y;
        m_pVertices[ i0 ].tangent.z += tangent.z;

        m_pVertices[ i1 ].tangent.x += tangent.x;
        m_pVertices[ i1 ].tangent.y += tangent.y;
        m_pVertices[ i1 ].tangent.z += tangent.z;

        m_pVertices[ i2 ].tangent.x += tangent.x;
        m_pVertices[ i2 ].tangent.y += tangent.y;
        m_pVertices[ i2 ].tangent.z += tangent.z;

        // 従法線ベクトルを加算.
        m_pVertices[ i0 ].binormal.x += binormal.x;
        m_pVertices[ i0 ].binormal.y += binormal.y;
        m_pVertices[ i0 ].binormal.z += binormal.z;

        m_pVertices[ i1 ].binormal.x += binormal.x;
        m_pVertices[ i1 ].binormal.y += binormal.y;
        m_pVertices[ i1 ].binormal.z += binormal.z;

        m_pVertices[ i2 ].binormal.x += binormal.x;
        m_pVertices[ i2 ].binormal.y += binormal.y;
        m_pVertices[ i2 ].binormal.z += binormal.z;
    }

    for( unsigned int i=0; i<m_NumVertex; ++i )
    {
        XFVEC3 normal   = m_pVertices[ i ].normal;
        XFVEC3 tangent  = m_pVertices[ i ].tangent;
        XFVEC3 binormal = m_pVertices[ i ].binormal;

        float dots = Vec3Dot( normal, tangent );
        tangent.x = ( tangent.x - normal.x * dots );
        tangent.y = ( tangent.y - normal.y * dots );
        tangent.z = ( tangent.z - normal.z * dots );
        m_pVertices[ i ].tangent = Vec3SafeNormalize( tangent );

        XFVEC3 crs = Vec3Cross( normal, m_pVertices[ i ].tangent );
        float sign = 1.0f;

        // 座標系を合わせる.
        if ( Vec3Dot( crs, binormal ) < 0.0f )
        {
            sign = -1.0f;
            m_pVertices[ i ].tangent.x = -m_pVertices[ i ].tangent.x;
            m_pVertices[ i ].tangent.y = -m_pVertices[ i ].tangent.y;
            m_pVertices[ i ].tangent.z = -m_pVertices[ i ].tangent.z;
        }

        binormal.x = crs.x * sign;
        binormal.y = crs.y * sign;
        binormal.z = crs.z * sign;

        m_pVertices[ i ].binormal = Vec3SafeNormalize( binormal );
    }

    return true;
}

unsigned int XMESH::GetNumVertices()
{ return m_NumVertex; }

unsigned int XMESH::GetNumSubsets()
{ return m_NumSubset; }

unsigned int XMESH::GetNumMaterials()
{ return m_NumMaterial; }

unsigned int XMESH::GetNumIndices()
{ return m_NumIndex; }

unsigned int XMESH::GetNumBones()
{ return m_NumBone; }

unsigned int XMESH::GetNumClips()
{ return m_NumClip; }

unsigned int XMESH::GetIndex( unsigned int index )
{
    assert( index < m_NumIndex );
    return m_pIndices[ index ];
}

unsigned int* XMESH::GetIndices()
{ return m_pIndices; }

XVERTEX XMESH::GetVertex( unsigned int index )
{
    assert( index < m_NumVertex );
    return m_pVertices[ index ];
}

XVERTEX* XMESH::GetVertices() 
{ return m_pVertices; }

XSUBSET XMESH::GetSubset( unsigned int index )
{
    assert( index < m_NumSubset );
    return m_pSubsets[ index ];
}

XSUBSET* XMESH::GetSubsets()
{ return m_pSubsets; }

XMATERIAL XMESH::GetMaterial( unsigned int index )
{
    assert( index < m_NumMaterial );
    return m_pMaterials[ index ];
}

XMATERIAL* XMESH::GetMaterials()
{ return m_pMaterials; }

XBONE XMESH::GetBone( unsigned int index )
{
    assert( index < m_NumBone );
    return m_pBones[ index ];
}

XBONE* XMESH::GetBones()
{ return m_pBones; }

XANIMATIONCLIP XMESH::GetClip( unsigned int index )
{
    assert( index < m_NumClip );
    return m_pClips[ index ];
}

XANIMATIONCLIP* XMESH::GetClips()
{ return m_pClips; }


bool XMESH::LoadFile( const char* filename )
{
    FILE* pFile;

    // ファイルを開く.
    errno_t ret = fopen_s( &pFile, filename, "rb" );

    // チェック.
    if ( ret != 0 )
    {
        // エラーログ出力.
        printf_s( "Error : File Open Failed. filename = %s\n", filename );

        // 異常終了.
        return false;
    }

    // 最後まで移動.
    fseek( pFile, 0, SEEK_END );

    // ファイルサイズを取得.
    unsigned int bufSize = static_cast< unsigned int >( ftell( pFile ) );

    // 先頭に戻す.
    fseek( pFile, 0, SEEK_SET );

    // バッファを確保.
    char* pBuf = new char[ bufSize + 1 ];

    // NULLチェック.
    if ( pBuf == nullptr )
    {
        // ファイルを閉じる.
        fclose( pFile );

        // エラーログ出力.
        printf_s( "Error : Memory Allocate Failed. \n" );

        // 異常終了.
        return false;
    }

    // ゼロクリア.
    memset( pBuf, 0, bufSize );

    // NULL終端にする.
    pBuf[ bufSize ] = '\0';

    if ( m_pTokenizer != nullptr )
    {
        delete m_pTokenizer;
        m_pTokenizer = nullptr;
    }

    // 一括読み込み.
    unsigned int readCount = fread( pBuf, sizeof( unsigned char ), bufSize, pFile );

    // ファイルを閉じる.
    fclose( pFile );

    // 読み込みサイズをチェック.
    if ( readCount != bufSize )
    {
    }
}