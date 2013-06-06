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
    unsigned int numVertices;               //!< ���_���ł�.
    unsigned int numIndices;                //!< �C���f�b�N�X���ł�.
    unsigned int numMaterials;              //!< �}�e���A�����ł�.
    unsigned int numSubsets;                //!< �T�u�Z�b�g���ł�.
    unsigned int vertexStructureSize;       //!< ���_�\���̂̃T�C�Y�ł�.
    unsigned int indexStructureSize;        //!< ���_�C���f�b�N�X�\���̂̃T�C�Y�ł�.
    unsigned int materialStructureSize;     //!< �}�e���A���\���̂̃T�C�Y�ł�.
    unsigned int subsetStructureSize;       //!< �T�u�Z�b�g�\���̂̃T�C�Y�ł�.
};

/////////////////////////////////////////////////////////////////////////////////
// MSH_FILE_HEADER structure
/////////////////////////////////////////////////////////////////////////////////
struct MSH_FILE_HEADER
{
    unsigned char   magic[ 4 ];             //!< �}�W�b�N�ł�.
    unsigned int    version;                //!< �t�@�C���o�[�W�����ł�.
    unsigned int    dataHeaderSize;         //!< �f�[�^�w�b�_�̃T�C�Y�ł�.
    MSH_DATA_HEADER dataHeader;             //!< �f�[�^�w�b�_�ł�.
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
    MSH_FVEC3   position;           //!< �ʒu���W�ł�.
    MSH_FVEC3   normal;             //!< �@���x�N�g���ł�.
    MSH_FVEC3   tangent;            //!< �ڃx�N�g���ł�.
    MSH_FVEC2   texcoord;           //!< �e�N�X�`�����W�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// MSH_MATERIAL structure
///////////////////////////////////////////////////////////////////////////////
struct MSH_MATERIAL
{
    MSH_FVEC3   ambient;                //!< ���F�ł�.
    MSH_FVEC3   diffuse;                //!< �g�U���ːF�ł�.
    MSH_FVEC3   specular;               //!< ���ʔ��ːF�ł�.
    MSH_FVEC3   emissive;               //!< ���ȏƖ��F�ł�.
    float       alpha;                  //!< ���ߓx�ł�.
    float       power;                  //!< ���ʔ��ˋ��x�ł�.
    char        ambientMap[ 256 ];      //!< �A���r�G���g�}�b�v���ł�.
    char        diffuseMap[ 256 ];      //!< �f�B�t���[�Y�}�b�v���ł�.
    char        specularMap[ 256 ];     //!< �X�y�L�����[�}�b�v���ł�.
    char        bumpMap[ 256 ];         //!< �o���v�}�b�v���ł�.
    char        displacementMap[ 256 ]; //!< �f�B�X�v���C�X�����g�}�b�v���ł�.
};

//////////////////////////////////////////////////////////////////////////////
// MSH_SUBSET structure
//////////////////////////////////////////////////////////////////////////////
struct MSH_SUBSET
{
    unsigned int    indexOffset;        //!< ���_�C���f�b�N�X�̃I�t�Z�b�g�ł�.
    unsigned int    indexCount;         //!< ���_�C���f�b�N�X���ł�.
    unsigned int    materialIndex;      //!< �}�e���A���C���f�b�N�X�ł�.
};

//!< �}�W�b�N�ł�.
static const unsigned char MSH_MAGIC[ 4 ]           = { 'M', 'S', 'H', '\0' };

//!< �t�@�C���o�[�W�����ł�.
static const unsigned int  MSH_VERSION              = 0x00000001;

//!< �f�[�^�w�b�_�̃T�C�Y�ł�.
static const unsigned int  MSH_DATA_HEADER_SIZE     = sizeof( MSH_DATA_HEADER );

//!< ���_�\���̂̃T�C�Y�ł�.
static const unsigned int  MSH_VERTEX_STRUCT_SIZE   = sizeof( MSH_VERTEX );

//!< ���_�C���f�b�N�X�\���̂̃T�C�Y�ł�.
static const unsigned int  MSH_INDEX_STRUCT_SIZE    = sizeof( unsigned int );

//!< �}�e���A���\���̂̃T�C�Y�ł�.
static const unsigned int  MSH_MATERIAL_STRUCT_SIZE = sizeof( MSH_MATERIAL );

//!< �T�u�Z�b�g�\���̂̃T�C�Y�ł�.
static const unsigned int  MSH_SUBSET_STRUCT_SIZE   = sizeof( MSH_SUBSET );



#endif//__MSH_DATA_H__