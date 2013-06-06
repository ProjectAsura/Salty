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
    XFVEC4   faceColor;     //!< �ʃJ���[�ł�.
    float    power;         //!< ���ʔ��ˋ��x�ł�.
    XFVEC3   specular;      //!< ���ʔ��ːF�ł�.
    XFVEC3   emissive;      //!< ���ȏƖ��F�ł�.
    XSTRING  textureMap;    //!< �e�N�X�`���t�@�C�����ł�.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XVERTEX structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XVERTEX
{
    XFVEC3   position;      //!< �ʒu���W�ł�.
    XFVEC3   normal;        //!< �@���x�N�g���ł�.
    XFVEC3   tangent;       //!< �ڃx�N�g���ł�.
    XFVEC3   binormal;      //!< �]�@���x�N�g���ł�.
    XFVEC2   texcoord;      //!< �e�N�X�`�����W�ł�.
    XIVEC4   blendIndex;    //!< �u�����h�C���f�b�N�X�ł�.
    XFVEC4   blendWeight;   //!< �u�����h�E�F�C�g�ł�.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XSUBSET structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XSUBSET
{
    unsigned int materialIndex;     //!< �}�e���A���ԍ��ł�.
    unsigned int faceStart;         //!< �ʊJ�n�ԍ��ł�.
    unsigned int faceCount;         //!< �ʐ��ł�.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XBONE structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XBONE
{
    int         parentIndex;        //!< �e�{�[���̔ԍ��ł�.
    XFMTX44     invBindPose;        //!< �t�o�C���h�|�[�Y�ł�.
    XFMTX44     bindPose;           //!< �o�C���h�|�[�Y�ł�.
    XFMTX44     localTransform;     //!< ���[�J���ϊ��s��ł�.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XKEYPOSE structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XKEYPOSE
{
    unsigned int time;              //!< ���Ԃł�.
    XFMTX44      transform;         //!< �ϊ��s��ł�.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XANIMATION structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XANIMATION
{
    unsigned int boneIndex;         //!< �{�[���ԍ��ł�.
    unsigned int numKeys;           //!< �L�[���ł�.
    XKEYPOSE*    pKeys;             //!< �L�[�ł�.
};

///////////////////////////////////////////////////////////////////////////////////////////
// XANIMATIONCLIP structure
///////////////////////////////////////////////////////////////////////////////////////////
struct XANIMATIONCLIP
{
    XSTRING         name;           //!< �N���b�v���ł�.
    float           duration;       //!< �p�����Ԃł�.
    unsigned int    numKeyFrames;   //!< �L�[�t���[�����ł�.
    XANIMATION*     pKeyFrame;      //!< �L�[�t���[���ł�.
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
    unsigned int    m_NumVertex;            //!< ���_���ł�.
    unsigned int    m_NumMaterial;          //!< �}�e���A�����ł�.
    unsigned int    m_NumSubset;            //!< �T�u�Z�b�g���ł�.
    unsigned int    m_NumBone;              //!< �{�[�����ł�.
    unsigned int    m_NumIndex;             //!< ���_�C���f�b�N�X���ł�.
    unsigned int    m_NumClip;              //!< �A�j���[�V�����N���b�v���ł�.
    XVERTEX*        m_pVertices;            //!< ���_�ł�.
    XMATERIAL*      m_pMaterials;           //!< �}�e���A���ł�.
    XSUBSET*        m_pSubsets;             //!< �T�u�Z�b�g�ł�.
    XBONE*          m_pBones;               //!< �{�[���ł�.
    unsigned*       m_pIndices;             //!< ���_�C���f�b�N�X�ł�.
    XANIMATIONCLIP* m_pClips;               //!< �A�j���[�V�����N���b�v�ł�.
    XSTRING         m_DirectionPath;        //!< �f�B���N�g���p�X�ł�.
    Tokenizer*      m_pTokenizer;           //!< �g�[�J�i�C�U�[�ł�.
    int             m_CurParentID;          //!< ���݂̐e�{�[���̔ԍ��ł�.
    int             m_CurFrameID;           //!< ���݂̃t���[���̔ԍ��ł�.
    bool            m_IsExistTexCoord;      //!< �e�N�X�`�����W�̑��݃t���O.
    bool            m_IsExistNormal;        //!< �@���x�N�g���̑��݃t���O.

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