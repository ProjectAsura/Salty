//-----------------------------------------------------------------------------------------
// File : XLoader.h
// Desc : DirectX X File Loader Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------------------

#ifndef __X_LOADER_H__
#define __X_LOADER_H__

//-----------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------
#include <cstdio>
#include "Tokenizer.h"


//-----------------------------------------------------------------------------------------
// Constant Values
//-----------------------------------------------------------------------------------------
const static unsigned X_NAME_LENGTH = 256;


///////////////////////////////////////////////////////////////////////////////////////////
// XFVEC2 structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XFVEC2
{
    float x;
    float y;
};

///////////////////////////////////////////////////////////////////////////////////////////
// XFVEC3 structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XFVEC3
{
    float x;
    float y;
    float z;
};

///////////////////////////////////////////////////////////////////////////////////////////
// XFVEC4 structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XFVEC4
{
    float x;
    float y;
    float z;
    float w;
};

///////////////////////////////////////////////////////////////////////////////////////////
// XIVEC2 structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XIVEC2
{
    int   x;
    int   y;
};

///////////////////////////////////////////////////////////////////////////////////////////
// XIVEC3 structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XIVEC3
{
    int   x;
    int   y;
    int   z;
};

///////////////////////////////////////////////////////////////////////////////////////////
// XIVEC4 structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XIVEC4
{
    int   x;
    int   y;
    int   z;
    int   w;
};

///////////////////////////////////////////////////////////////////////////////////////////
// XFMTX44 structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XFMTX44
{
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
};

///////////////////////////////////////////////////////////////////////////////////////////
// XSTRING structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XSTRING
{
    char c_str[ X_NAME_LENGTH ];
};

///////////////////////////////////////////////////////////////////////////////////////////
// XMATERIAL structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XMATERIAL
{
    XFVEC4   faceColor;     //!< 面カラーです.
    float    power;         //!< 鏡面反射強度です.
    XFVEC3   specular;      //!< 鏡面反射色です.
    XFVEC3   emissive;      //!< 自己照明色です.
    XSTRING  textureMap;    //!< テクスチャファイル名です.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XVERTEX structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XVERTEX
{
    XFVEC3   position;      //!< 位置座標です.
    XFVEC3   normal;        //!< 法線ベクトルです.
    XFVEC3   tangent;       //!< 接ベクトルです.
    XFVEC3   binormal;      //!< 従法線ベクトルです.
    XFVEC2   texcoord;      //!< テクスチャ座標です.
    XIVEC4   blendIndex;    //!< ブレンドインデックスです.
    XFVEC4   blendWeight;   //!< ブレンドウェイトです.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XSUBSET structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XSUBSET
{
    unsigned int materialIndex;     //!< マテリアル番号です.
    unsigned int faceStart;         //!< 面開始番号です.
    unsigned int faceCount;         //!< 面数です.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XBONE structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XBONE
{
    int         parentIndex;        //!< 親ボーンの番号です.
    XFMTX44     invBindPose;        //!< 逆バインドポーズです.
    XFMTX44     bindPose;           //!< バインドポーズです.
    XFMTX44     localTransform;     //!< ローカル変換行列です.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XKEYPOSE structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XKEYPOSE
{
    unsigned int time;              //!< 時間です.
    XFMTX44      transform;         //!< 変換行列です.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XANIMATION structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XANIMATION
{
    unsigned int boneIndex;         //!< ボーン番号です.
    unsigned int numKeys;           //!< キー数です.
    XKEYPOSE*    pKeys;             //!< キーです.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XANIMATIONCLIP structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XANIMATIONCLIP
{
    XSTRING         name;           //!< クリップ名です.
    float           duration;       //!< 継続時間です.
    unsigned int    numKeyFrames;   //!< キーフレーム数です.
    XANIMATION*     pKeyFrame;      //!< キーフレームです.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XMESH class
///////////////////////////////////////////////////////////////////////////////////////////
class XMESH
{
    //=====================================================================================
    // list of friend classes and methods.
    //=====================================================================================
    /* NOTHING */

private:
    //=====================================================================================
    // private variables.
    //=====================================================================================
    unsigned int    m_NumVertex;            //!< 頂点数です.
    unsigned int    m_NumMaterial;          //!< マテリアル数です.
    unsigned int    m_NumSubset;            //!< サブセット数です.
    unsigned int    m_NumBone;              //!< ボーン数です.
    unsigned int    m_NumIndex;             //!< 頂点インデックス数です.
    unsigned int    m_NumClip;              //!< アニメーションクリップ数です.
    XVERTEX*        m_pVertices;            //!< 頂点です.
    XMATERIAL*      m_pMaterials;           //!< マテリアルです.
    XSUBSET*        m_pSubsets;             //!< サブセットです.
    XBONE*          m_pBones;               //!< ボーンです.
    unsigned*       m_pIndices;             //!< 頂点インデックスです.
    XANIMATIONCLIP* m_pClips;               //!< アニメーションクリップです.
    XSTRING         m_DirectionPath;        //!< ディレクトリパスです.
    Tokenizer*      m_pTokenizer;           //!< トーカナイザーです.
    int             m_CurParentID;          //!< 現在の親ボーンの番号です.
    int             m_CurFrameID;           //!< 現在のフレームの番号です.
    bool            m_IsExistTexCoord;      //!< テクスチャ座標の存在フラグ.
    bool            m_IsExistNormal;        //!< 法線ベクトルの存在フラグ.

    //====================================================================================
    // private methods.
    //====================================================================================
    bool ComputeNormal();
    bool ComputeTangent();
    void WriteDirect  ( FILE* pFile );
    void WriteOptimize( FILE* pFile );
    bool ParseFrame();
    bool ParseMeshVertex();
    bool ParseMeshNormal();
    bool ParseMeshTexCoord();
    bool ParseMeshMaterial();
    bool ParseMeshMaterialList();
    bool ParseSkinHeader();
    bool ParseMesh();
    bool ParseAnimationKey();
    bool ParseAnimationSet();
    bool ParseNode();
    bool SkipNode();
    int  SkipToBlock();
    bool SkipToBlockStart();
    bool SkipToBlockEnd();
    bool CheckHeader();
    int  CheckNode();

protected:
    //====================================================================================
    // protected varaibles.
    //====================================================================================
    /* NOTHING */

    //====================================================================================
    // protected methods.
    //====================================================================================
    /* NOTHING */

public:
    //====================================================================================
    // public variables.
    //====================================================================================
    /* NOTHING */

    //====================================================================================
    // public methods.
    //====================================================================================
    XMESH();
    ~XMESH();

    bool LoadFile    ( const char* filename );
    bool SaveToBinary( const char* filename );
    void Release();

    unsigned int        GetNumVertices();
    unsigned int        GetNumSubsets();
    unsigned int        GetNumMaterials();
    unsigned int        GetNumIndices();
    unsigned int        GetNumBones();
    unsigned int        GetNumClips();
    unsigned int        GetIndex( unsigned int index );
    unsigned int*       GetIndices();
    XVERTEX             GetVertex( unsigned int index );
    XVERTEX*            GetVertices();
    XSUBSET             GetSubset( unsigned int index );
    XSUBSET*            GetSubsets();
    XMATERIAL           GetMaterial( unsigned int index );
    XMATERIAL*          GetMaterials();
    XBONE               GetBone( unsigned int index );
    XBONE*              GetBones();
    XANIMATIONCLIP      GetClip( unsigned int index );
    XANIMATIONCLIP*     GetClips();
};



#endif//__X_LOADER_H__