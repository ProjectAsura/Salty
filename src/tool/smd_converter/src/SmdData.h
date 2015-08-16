//-------------------------------------------------------------------------------
// File : SmdData.h
// Desc : S3d Mesh Data Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------

#ifndef __SMD_DATA_H__
#define __SMD_DATA_H__


/////////////////////////////////////////////////////////////////////////////////
// SMD_DATA_HEADER structure    
/////////////////////////////////////////////////////////////////////////////////
struct SMD_DATA_HEADER
{
    unsigned int numTriangles;              //!< 三角形数です.
    unsigned int numMaterials;              //!< マテリアル数です.
    unsigned int numTextures;               //!< テクスチャ数です.
    unsigned int triangleStructureSize;     //!< 三角形構造体のサイズです.
    unsigned int materialStructureSize;     //!< マテリアル構造体のサイズです.
    unsigned int textureStructureSize;      //!< テクスチャ構造体のサイズです.
};

/////////////////////////////////////////////////////////////////////////////////
// SMD_FILE_HEADER structure
/////////////////////////////////////////////////////////////////////////////////
struct SMD_FILE_HEADER
{
    unsigned char   magic[ 4 ];             //!< マジックです.
    unsigned int    version;                //!< ファイルバージョンです.
    unsigned int    dataHeaderSize;         //!< データヘッダのサイズです.
    SMD_DATA_HEADER dataHeader;             //!< データヘッダです.
};

////////////////////////////////////////////////////////////////////////////////
// SMD_DVEC2 structure
////////////////////////////////////////////////////////////////////////////////
struct SMD_DVEC2
{
    double   x;
    double   y;
};

////////////////////////////////////////////////////////////////////////////////
// SMD_DVEC3 structure
////////////////////////////////////////////////////////////////////////////////
struct SMD_DVEC3
{
    double   x;
    double   y;
    double   z;
};

////////////////////////////////////////////////////////////////////////////////
// SMD_DVEC4 structure
////////////////////////////////////////////////////////////////////////////////
struct SMD_DVEC4
{
    double   x;
    double   y;
    double   z;
    double   w;
};

struct SMD_FVEC2
{
    float   x;
    float   y;
};

struct SMD_FVEC3
{
    float   x;
    float   y;
    float   z;
};

struct SMD_FVEC4
{
    float   x;
    float   y;
    float   z;
    float   w;
};

////////////////////////////////////////////////////////////////////////////////
// SMD_IVEC2 structure
////////////////////////////////////////////////////////////////////////////////
struct SMD_IVEC2
{
    int     x;
    int     y;
};

////////////////////////////////////////////////////////////////////////////////
// SMD_IVEC3 structure
////////////////////////////////////////////////////////////////////////////////
struct SMD_IVEC3
{
    int     x;
    int     y;
    int     z;
};

////////////////////////////////////////////////////////////////////////////////
// SMD_IVEC4 structure
////////////////////////////////////////////////////////////////////////////////
struct SMD_IVEC4
{
    int     x;
    int     y;
    int     z;
    int     w;
};

////////////////////////////////////////////////////////////////////////////////
// SMD_BVEC2 structure
////////////////////////////////////////////////////////////////////////////////
struct SMD_BVEC2
{
    unsigned char x;
    unsigned char y;
};

////////////////////////////////////////////////////////////////////////////////
// SMD_BVEC3 structure
////////////////////////////////////////////////////////////////////////////////
struct SMD_BVEC3
{
    unsigned char x;
    unsigned char y;
    unsigned char z;
};

////////////////////////////////////////////////////////////////////////////////
// SMD_BVEC4 structure
////////////////////////////////////////////////////////////////////////////////
struct SMD_BVEC4
{
    unsigned char x;
    unsigned char y;
    unsigned char z;
    unsigned char w;
};

///////////////////////////////////////////////////////////////////////////////
// SMD_DMTX44 structure
///////////////////////////////////////////////////////////////////////////////
struct SMD_DMTX44
{
    double _11, _12, _13, _14;
    double _21, _22, _23, _24;
    double _31, _32, _33, _34;
    double _41, _42, _43, _44;
};

struct SMD_FMTX44
{
    float   _11, _12, _13, _14;
    float   _21, _22, _23, _24;
    float   _31, _32, _33, _34;
    float   _41, _42, _43, _44;
};

///////////////////////////////////////////////////////////////////////////////
// SMD_VERTEX structure
///////////////////////////////////////////////////////////////////////////////
struct SMD_VERTEX
{
    SMD_FVEC3   position;           //!< 位置座標です.
    SMD_FVEC3   normal;             //!< 法線ベクトルです.
    SMD_FVEC2   texcoord;           //!< テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////
// SMD_TRIANGLE structure
///////////////////////////////////////////////////////////////////////////////
struct SMD_TRIANGLE
{
    SMD_VERTEX    vertex[3];        //!< 頂点座標です.
    int           materialId;       //!< マテリアルインデックスです.
};

///////////////////////////////////////////////////////////////////////////////
// SMD_MATERIAL structure
///////////////////////////////////////////////////////////////////////////////
struct SMD_MATERIAL
{
    SMD_FVEC3   diffuse;                //!< 拡散反射色です.
    SMD_FVEC3   specular;               //!< 鏡面反射色です.
    float       power;                  //!< 鏡面反射強度です.
    SMD_FVEC3   emissive;               //!< 自己照明色です.
    int         diffuseMap;             //!< ディフューズマップです.
    int         specularMap;            //!< スペキュラーマップです.
};

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
struct SMD_TEXTURE
{
    char      filename[ 256 ];        //!< ファイル名です.
};

//////////////////////////////////////////////////////////////////////////////
// SMD_SUBSET structure
//////////////////////////////////////////////////////////////////////////////
struct SMD_SUBSET
{
    unsigned int    indexOffset;        //!< 頂点インデックスのオフセットです.
    unsigned int    indexCount;         //!< 頂点インデックス数です.
    unsigned int    materialIndex;      //!< マテリアルインデックスです.
};

//!< マジックです.
static const unsigned char SMD_MAGIC[ 4 ]           = { 'S', 'M', 'D', '\0' };

//!< ファイルバージョンです.
static const unsigned int  SMD_VERSION              = 0x00000001;

//!< データヘッダのサイズです.
static const unsigned int  SMD_DATA_HEADER_SIZE     = sizeof( SMD_DATA_HEADER );

//!< 頂点構造体のサイズです.
static const unsigned int  SMD_VERTEX_STRUCT_SIZE   = sizeof( SMD_VERTEX );

static const unsigned int  SMD_TRIANGLE_STRUCT_SIZE = sizeof( SMD_TRIANGLE );

//!< 頂点インデックス構造体のサイズです.
static const unsigned int  SMD_INDEX_STRUCT_SIZE    = sizeof( unsigned int );

//!< マテリアル構造体のサイズです.
static const unsigned int  SMD_MATERIAL_STRUCT_SIZE = sizeof( SMD_MATERIAL );

//!< サブセット構造体のサイズです.
static const unsigned int  SMD_SUBSET_STRUCT_SIZE   = sizeof( SMD_SUBSET );

//!< テクスチャ構造体のサイズです.
static const unsigned int  SMD_TEXTURE_STRUCT_SIZE  = sizeof( SMD_TEXTURE );



#endif//__MSH_DATA_H__