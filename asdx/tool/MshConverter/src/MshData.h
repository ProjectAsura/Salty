//-------------------------------------------------------------------------------
// File : MshData.h
// Desc : Mesh Data Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------

#ifndef __MSH_DATA_H__
#define __MSH_DATA_H__


/////////////////////////////////////////////////////////////////////////////////
// MSH_DATA_HEADER structure    
/////////////////////////////////////////////////////////////////////////////////
struct MSH_DATA_HEADER
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
// MSH_FILE_HEADER structure
/////////////////////////////////////////////////////////////////////////////////
struct MSH_FILE_HEADER
{
    unsigned char   magic[ 4 ];             //!< マジックです.
    unsigned int    version;                //!< ファイルバージョンです.
    unsigned int    dataHeaderSize;         //!< データヘッダのサイズです.
    MSH_DATA_HEADER dataHeader;             //!< データヘッダです.
};

////////////////////////////////////////////////////////////////////////////////
// MSH_FVEC2 structure
////////////////////////////////////////////////////////////////////////////////
struct MSH_FVEC2
{
    float   x;
    float   y;
};

////////////////////////////////////////////////////////////////////////////////
// MSH_FVEC3 structure
////////////////////////////////////////////////////////////////////////////////
struct MSH_FVEC3
{
    float   x;
    float   y;
    float   z;
};

////////////////////////////////////////////////////////////////////////////////
// MSH_FVEC4 structure
////////////////////////////////////////////////////////////////////////////////
struct MSH_FVEC4
{
    float   x;
    float   y;
    float   z;
    float   w;
};

////////////////////////////////////////////////////////////////////////////////
// MSH_IVEC2 structure
////////////////////////////////////////////////////////////////////////////////
struct MSH_IVEC2
{
    int     x;
    int     y;
};

////////////////////////////////////////////////////////////////////////////////
// MSH_IVEC3 structure
////////////////////////////////////////////////////////////////////////////////
struct MSH_IVEC3
{
    int     x;
    int     y;
    int     z;
};

////////////////////////////////////////////////////////////////////////////////
// MSH_IVEC4 structure
////////////////////////////////////////////////////////////////////////////////
struct MSH_IVEC4
{
    int     x;
    int     y;
    int     z;
    int     w;
};

////////////////////////////////////////////////////////////////////////////////
// MSH_BVEC2 structure
////////////////////////////////////////////////////////////////////////////////
struct MSH_BVEC2
{
    unsigned char x;
    unsigned char y;
};

////////////////////////////////////////////////////////////////////////////////
// MSH_BVEC3 structure
////////////////////////////////////////////////////////////////////////////////
struct MSH_BVEC3
{
    unsigned char x;
    unsigned char y;
    unsigned char z;
};

////////////////////////////////////////////////////////////////////////////////
// MSH_BVEC4 structure
////////////////////////////////////////////////////////////////////////////////
struct MSH_BVEC4
{
    unsigned char x;
    unsigned char y;
    unsigned char z;
    unsigned char w;
};

///////////////////////////////////////////////////////////////////////////////
// MSH_FMTX44 structure
///////////////////////////////////////////////////////////////////////////////
struct MSH_FMTX44
{
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
};

///////////////////////////////////////////////////////////////////////////////
// MSH_VERTEX structure
///////////////////////////////////////////////////////////////////////////////
struct MSH_VERTEX
{
    MSH_FVEC3   position;           //!< 位置座標です.
    MSH_FVEC3   normal;             //!< 法線ベクトルです.
    MSH_FVEC3   tangent;            //!< 接ベクトルです.
    MSH_FVEC2   texcoord;           //!< テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////
// MSH_MATERIAL structure
///////////////////////////////////////////////////////////////////////////////
struct MSH_MATERIAL
{
    MSH_FVEC3   ambient;                //!< 環境色です.
    MSH_FVEC3   diffuse;                //!< 拡散反射色です.
    MSH_FVEC3   specular;               //!< 鏡面反射色です.
    MSH_FVEC3   emissive;               //!< 自己照明色です.
    float       alpha;                  //!< 透過度です.
    float       power;                  //!< 鏡面反射強度です.
    char        ambientMap[ 256 ];      //!< アンビエントマップ名です.
    char        diffuseMap[ 256 ];      //!< ディフューズマップ名です.
    char        specularMap[ 256 ];     //!< スペキュラーマップ名です.
    char        bumpMap[ 256 ];         //!< バンプマップ名です.
    char        displacementMap[ 256 ]; //!< ディスプレイスメントマップ名です.
};

//////////////////////////////////////////////////////////////////////////////
// MSH_SUBSET structure
//////////////////////////////////////////////////////////////////////////////
struct MSH_SUBSET
{
    unsigned int    indexOffset;        //!< 頂点インデックスのオフセットです.
    unsigned int    indexCount;         //!< 頂点インデックス数です.
    unsigned int    materialIndex;      //!< マテリアルインデックスです.
};

//!< マジックです.
static const unsigned char MSH_MAGIC[ 4 ]           = { 'M', 'S', 'H', '\0' };

//!< ファイルバージョンです.
static const unsigned int  MSH_VERSION              = 0x00000001;

//!< データヘッダのサイズです.
static const unsigned int  MSH_DATA_HEADER_SIZE     = sizeof( MSH_DATA_HEADER );

//!< 頂点構造体のサイズです.
static const unsigned int  MSH_VERTEX_STRUCT_SIZE   = sizeof( MSH_VERTEX );

//!< 頂点インデックス構造体のサイズです.
static const unsigned int  MSH_INDEX_STRUCT_SIZE    = sizeof( unsigned int );

//!< マテリアル構造体のサイズです.
static const unsigned int  MSH_MATERIAL_STRUCT_SIZE = sizeof( MSH_MATERIAL );

//!< サブセット構造体のサイズです.
static const unsigned int  MSH_SUBSET_STRUCT_SIZE   = sizeof( MSH_SUBSET );



#endif//__MSH_DATA_H__