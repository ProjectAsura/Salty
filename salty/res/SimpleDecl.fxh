//---------------------------------------------------------------------------------------
// File : SimpleLayout.fxh
// Desc : Layout of Simple Shader
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------

#define PI 3.1415926535f

//---------------------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// VSInput structure
/////////////////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3  Position    : POSITION;     //!< �ʒu���W�ł�.
    float3  Normal      : NORMAL;       //!< �@���x�N�g���ł�.
    float3  Tangent     : TANGENT;      //!< �ڃx�N�g���ł�.
    float2  TexCoord    : TEXCOORD;     //!< �e�N�X�`�����W�ł�.
};


/////////////////////////////////////////////////////////////////////////////////////////
// VSOutput structure
/////////////////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position     : SV_POSITION;  //!< �ˉe�ϊ���̈ʒu���W�ł�.
    float3 NormalWS     : NORMAL;       //!< ���[���h��Ԃł̖@���x�N�g���ł�.
    float3 TangentWS    : TANGNET;      //!< ���[���h��Ԃł̐ڃx�N�g���ł�
    float3 BinormalWS   : BINORMAL;     //!< ���[���h��Ԃł̏]�@���x�N�g���ł�.
    float2 TexCoord     : TEXCOORD0;    //!< �e�N�X�`�����W�ł�.
    float3 LightDir     : TEXCOORD1;    //!< ���C�g�x�N�g���ł�.
    float3 ViewDir      : TEXCOORD2;    //!< �����x�N�g���ł�.
};


/////////////////////////////////////////////////////////////////////////////////////////
// PSOutput structure
/////////////////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4 Color0       : SV_TARGET0;       //!< �J���[0�ł�.
};


//---------------------------------------------------------------------------------------
// Constant Buffers
//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// Material Buffer
/////////////////////////////////////////////////////////////////////////////////////////
cbuffer Material : register( b0 )
{
    float3 Diffuse  : packoffset( c0 );     //!< �g�U���ːF�ł�.
    float  Alpha    : packoffset( c0.w );   //!< ���ߓx�ł�.
    float3 Specular : packoffset( c1 );     //!< ���ʔ��ːF�ł�.
    float  Power    : packoffset( c1.w );   //!< ���ʔ��ˋ��x�ł�.
    float3 Emissive : packoffset( c2 );     //!< ���ȏƖ��F�ł�.
    float  Bump     : packoffset( c2.w );   //!< �o���v�}�b�s���O�t���O.
};


/////////////////////////////////////////////////////////////////////////////////////////
// TransformParam Buffer
/////////////////////////////////////////////////////////////////////////////////////////
cbuffer TransformParam : register( b1 )
{
    float4x4 World  : packoffset( c0 );     //!< ���[���h�s��ł�.
    float4x4 View   : packoffset( c4 );     //!< �r���[�s��ł�.
    float4x4 Proj   : packoffset( c8 );     //!< �ˉe�s��ł�.
    float4   Light  : packoffset( c12 );     //!< ���C�g�ł�(w=0�̏ꍇ�́C��������, w=1�̏ꍇ�͓_����).
};

Texture2D       DiffuseMap    : register( t0 );
Texture2D       SpecularMap   : register( t1 );
Texture2D       BumpMap       : register( t2 );
SamplerState    DiffuseSmp    : register( s0 );
SamplerState    SpecularSmp   : register( s1 );
SamplerState    BumpSmp       : register( s2 );

//---------------------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//! @brief      �r���[�s�񂩂王���x�N�g�����擾���܂�.
//!
//! @param [in]     view        �r���[�s��.
//! @return �����x�N�g����ԋp���܂�.
//! @note   1�s3��ڂ� ( CameraPosition.x - CameraTarget.x )
//!         2�s3��ڂ� ( CameraPosition.y - CameraTarget.y )
//!         3�s3��ڂ� ( CameraPosition.z - CameraTarget.z )
//!         �ł���ƁC���肵�������ɂȂ��Ă��܂�.
//---------------------------------------------------------------------------------------
float3 GetViewDir( float4x4 view )
{
    return normalize( float3( -view._13, -view._23, -view._33 ) );
}

//---------------------------------------------------------------------------------------
//! @brief      �����o�[�g���C�e�B���O���s���܂�.
//!
//! @param [in]     diffuse     �g�U���ːF.
//! @param [in]     lightDir    ���C�g�x�N�g��.
//! @param [in]     normal      �@���x�N�g��.
//! @return     �����o�[�g���C�e�B���O�������ʂ�ԋp���܂�.
//---------------------------------------------------------------------------------------
float3 Lambert( float3 diffuse, float3 lightDir, float3 normal )
{
    return diffuse * max( dot( normal, lightDir ), 0.0f );
}

float3 NormalizedLambert( float3 diffuse, float3 lightDir, float3 normal )
{
   return diffuse * max( dot( normal, lightDir ), 0.0f ) * ( 1.0f / PI );
}

//---------------------------------------------------------------------------------------
//! @brief      �n�[�t�����o�[�g���C�e�B���O���s���܂�.
//!
//! @param [in]     diffuse     �g�U���ːF.
//! @param [in]     lightDir    ���C�g�x�N�g��.
//! @param [in]     normal      �@���x�N�g��.
//! @return     �n�[�t�����o�[�g���C�e�B���O�������ʂ�ԋp���܂�.
//---------------------------------------------------------------------------------------
float3 HalfLambert( float3 diffuse, float3 lightDir, float3 normal )
{
    return diffuse * ( max( dot( normal, lightDir ), 0.0f ) * 0.5f + 0.5f );
}

float3 NormalizedHalfLambert( float3 diffuse, float3 lightDir, float3 normal )
{
    return diffuse * ( max( dot( normal, lightDir ), 0.0f ) * ( 1.0f / PI ) * 0.5f + 0.5f );
}

//---------------------------------------------------------------------------------------
//! @brief      �t�H�����C�e�B���O���s���܂�.
//!
//! @param [in]     specular    ���ʔ��ːF.
//! @param [in]     power       ���ʔ��ˋ��x.
//! @param [in]     viewDir     �����x�N�g��.
//! @param [in]     normal      �@���x�N�g��.
//! @parma [in]     lightDir    ���C�g�x�N�g��.
//! @return     �t�H�����C�e�B���O�̌��ʂ�ԋp���܂�.
//---------------------------------------------------------------------------------------
float3 Phong( float3 specular, float power, float3 viewDir, float3 normal, float3 lightDir )
{
    float3 R = -viewDir + ( 2.0f * dot( normal, viewDir ) * normal );

    return specular * pow( max ( dot( lightDir, R ), 0.0f ), power );
}

float3 NormalizedPhong( float3 specular, float power, float3 viewDir, float3 normal, float3 lightDir )
{
    float3 R = -viewDir + ( 2.0f * dot( normal, viewDir ) * normal );

    return specular * pow( max ( dot( lightDir, R ), 0.0f ), power ) * ( ( power + 1.0f )/ ( 2.0 * PI ) );
}

//----------------------------------------------------------------------------------------
//! @brief      �u�����t�H�����C�e�B���O���s���܂�.
//!
//! @param [in]     specular    ���ʔ��ːF.
//! @param [in]     power       ���ʔ��ˋ��x�ł�.
//! @param [in]     viewDir     �����x�N�g��.
//! @param [in]     lightDir    ���C�g�x�N�g��.
//! @return     �u�����t�H�����C�e�B���O�̌��ʂ�ԋp���܂�.
//----------------------------------------------------------------------------------------
float3 BlinnPhong( float3 specular, float power, float viewDir, float3 normal,float3 lightDir )
{
    float3 H = normalize( lightDir + viewDir );

    return specular * pow( max( dot( normal, H ), 0.0f ), power );
}

float3 NormalizedBlinnPhong( float3 specular, float power, float viewDir, float3 normal, float3 lightDir )
{
    float3 H = normalize( lightDir + viewDir );

    return specular * pow( max( dot( normal, H ), 0.0f ), power ) * ( ( power + 9.0f ) / ( 9.0 * PI ) );
}