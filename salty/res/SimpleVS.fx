//------------------------------------------------------------------------------------------
// File : SimpleVS.fx
// Desc : Simple Vertex Shader
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include "SimpleDecl.fxh"


VSOutput VSFunc( const VSInput input )
{
    VSOutput output = (VSOutput)0;
    
    float4 localPos = float4( input.Position, 1.0f );
    float4 worldPos = mul( World, localPos );
    float4 viewPos  = mul( View, worldPos );
    float4 projPos  = mul( Proj, viewPos );

    output.Position   = projPos;
    output.TexCoord   = input.TexCoord;
    output.NormalWS   = normalize( mul( (float3x3)World, input.Normal ) );
    output.TangentWS  = normalize( mul( (float3x3)World, input.Tangent ) );
    output.BinormalWS = normalize( cross( output.NormalWS, output.TangentWS ) );

    [flatten]
    if ( Light.w * Light.w > 0.0f )
    { output.LightDir = Light.xyz - worldPos.xyz; }
    else
    { output.LightDir = -Light.xyz; }

    output.ViewDir = GetViewDir( View );


    return output;
}
