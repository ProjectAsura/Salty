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
    float3  Position    : POSITION;     //!< 位置座標です.
    float3  Normal      : NORMAL;       //!< 法線ベクトルです.
    float3  Tangent     : TANGENT;      //!< 接ベクトルです.
    float2  TexCoord    : TEXCOORD;     //!< テクスチャ座標です.
};


/////////////////////////////////////////////////////////////////////////////////////////
// VSOutput structure
/////////////////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position     : SV_POSITION;  //!< 射影変換後の位置座標です.
    float3 NormalWS     : NORMAL;       //!< ワールド空間での法線ベクトルです.
    float3 TangentWS    : TANGNET;      //!< ワールド空間での接ベクトルです
    float3 BinormalWS   : BINORMAL;     //!< ワールド空間での従法線ベクトルです.
    float2 TexCoord     : TEXCOORD0;    //!< テクスチャ座標です.
    float3 LightDir     : TEXCOORD1;    //!< ライトベクトルです.
    float3 ViewDir      : TEXCOORD2;    //!< 視線ベクトルです.
};


/////////////////////////////////////////////////////////////////////////////////////////
// PSOutput structure
/////////////////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4 Color0       : SV_TARGET0;       //!< カラー0です.
};


//---------------------------------------------------------------------------------------
// Constant Buffers
//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// Material Buffer
/////////////////////////////////////////////////////////////////////////////////////////
cbuffer Material : register( b0 )
{
    float3 Diffuse  : packoffset( c0 );     //!< 拡散反射色です.
    float  Alpha    : packoffset( c0.w );   //!< 透過度です.
    float3 Specular : packoffset( c1 );     //!< 鏡面反射色です.
    float  Power    : packoffset( c1.w );   //!< 鏡面反射強度です.
    float3 Emissive : packoffset( c2 );     //!< 自己照明色です.
    float  Bump     : packoffset( c2.w );   //!< バンプマッピングフラグ.
};


/////////////////////////////////////////////////////////////////////////////////////////
// TransformParam Buffer
/////////////////////////////////////////////////////////////////////////////////////////
cbuffer TransformParam : register( b1 )
{
    float4x4 World  : packoffset( c0 );     //!< ワールド行列です.
    float4x4 View   : packoffset( c4 );     //!< ビュー行列です.
    float4x4 Proj   : packoffset( c8 );     //!< 射影行列です.
    float4   Light  : packoffset( c12 );     //!< ライトです(w=0の場合は，方向光源, w=1の場合は点光源).
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
//! @brief      ビュー行列から視線ベクトルを取得します.
//!
//! @param [in]     view        ビュー行列.
//! @return 視線ベクトルを返却します.
//! @note   1行3列目に ( CameraPosition.x - CameraTarget.x )
//!         2行3列目に ( CameraPosition.y - CameraTarget.y )
//!         3行3列目に ( CameraPosition.z - CameraTarget.z )
//!         であると，仮定した実装になっています.
//---------------------------------------------------------------------------------------
float3 GetViewDir( float4x4 view )
{
    return normalize( float3( -view._13, -view._23, -view._33 ) );
}

//---------------------------------------------------------------------------------------
//! @brief      ランバートライティングを行います.
//!
//! @param [in]     diffuse     拡散反射色.
//! @param [in]     lightDir    ライトベクトル.
//! @param [in]     normal      法線ベクトル.
//! @return     ランバートライティングした結果を返却します.
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
//! @brief      ハーフランバートライティングを行います.
//!
//! @param [in]     diffuse     拡散反射色.
//! @param [in]     lightDir    ライトベクトル.
//! @param [in]     normal      法線ベクトル.
//! @return     ハーフランバートライティングした結果を返却します.
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
//! @brief      フォンライティングを行います.
//!
//! @param [in]     specular    鏡面反射色.
//! @param [in]     power       鏡面反射強度.
//! @param [in]     viewDir     視線ベクトル.
//! @param [in]     normal      法線ベクトル.
//! @parma [in]     lightDir    ライトベクトル.
//! @return     フォンライティングの結果を返却します.
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
//! @brief      ブリンフォンライティングを行います.
//!
//! @param [in]     specular    鏡面反射色.
//! @param [in]     power       鏡面反射強度です.
//! @param [in]     viewDir     視線ベクトル.
//! @param [in]     lightDir    ライトベクトル.
//! @return     ブリンフォンライティングの結果を返却します.
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