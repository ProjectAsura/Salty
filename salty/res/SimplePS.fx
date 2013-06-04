//------------------------------------------------------------------------------------------
// File : SimplePS.fx
// Desc : Simple Pixel Shader
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include "SimpleDecl.fxh"


PSOutput PSFunc( VSOutput input )
{
    PSOutput output = (PSOutput)0;

    float4 diffuseMapColor  = DiffuseMap.Sample( DiffuseSmp, input.TexCoord );
    clip( ( diffuseMapColor.a < 0.125f ) ? -1.0f : 1.0f );

    float4 specularMapColor = SpecularMap.Sample( SpecularSmp, input.TexCoord );

    float3 N = normalize( input.NormalWS );
    float3 L = normalize( input.LightDir );
    float3 V = normalize( input.ViewDir );

    float3 diffuse  = NormalizedLambert( Diffuse * diffuseMapColor.rgb, L, N );
    float3 specular = NormalizedPhong( Specular * specularMapColor.rgb, Power, V, N, L );

    // ƒKƒ“ƒ}•â³.
    output.Color0.xyz = diffuse + specular;
    output.Color0.a   = Alpha;

    return output;

}