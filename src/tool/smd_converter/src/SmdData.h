﻿//-------------------------------------------------------------------------------
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
    unsigned int numVertices;               //!< 頂点数です.
    unsigned int numIndices;                //!< インデックス数です.
    unsigned int numMaterials;              //!< マテリアル数です.
    unsigned int numSubsets;                //!< サブセット数です.
    unsigned int vertexStructureSize;       //!< 頂点構造体のサイズです.
    unsigned int indexStructureSize;        //!< 頂点インデックス構造体のサイズです.
    unsigned int materialStructureSize;     //!< マテリアル構造体のサイズです.
    unsigned int subsetStructureSize;       //!< サブセット構造体のサイズです.
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

///////////////////////////////////////////////////////////////////////////////
// SMD_VERTEX structure
///////////////////////////////////////////////////////////////////////////////
struct SMD_VERTEX
{
    SMD_DVEC3   position;           //!< 位置座標です.
    SMD_DVEC3   normal;             //!< 法線ベクトルです.
    SMD_DVEC2   texcoord;           //!< テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////
// SMD_MATERIAL structure
///////////////////////////////////////////////////////////////////////////////
struct SMD_MATERIAL
{
    SMD_DVEC3   diffuse;                //!< 拡散反射色です.
    SMD_DVEC3   emissive;               //!< 自己照明色です.
    double      refractivity;           //!< 屈折率です.
    double      roughness;              //!< 面の粗さです.
    char        diffuseMap[ 256 ];      //!< ディフューズマップ名です.
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

//!< 頂点インデックス構造体のサイズです.
static const unsigned int  SMD_INDEX_STRUCT_SIZE    = sizeof( unsigned int );

//!< マテリアル構造体のサイズです.
static const unsigned int  SMD_MATERIAL_STRUCT_SIZE = sizeof( SMD_MATERIAL );

//!< サブセット構造体のサイズです.
static const unsigned int  SMD_SUBSET_STRUCT_SIZE   = sizeof( SMD_SUBSET );



#endif//__MSH_DATA_H__