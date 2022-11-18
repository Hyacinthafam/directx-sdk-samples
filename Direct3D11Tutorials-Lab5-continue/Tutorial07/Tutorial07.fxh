//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txWoodColor : register(t0);
Texture2D txtileColor : register(t1);
SamplerState txWoodsamSampler : register(s0);

cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
    float4 vMeshColor;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    //float4 Pos : POSITION;
    //float2 Tex : TEXCOORD0;

    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    //float4 Pos : SV_POSITION;
    //float2 Tex : TEXCOORD0;

    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 Norm : TEXCOORD1;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;

    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float4 tileColor = txtileColor.Sample(txWoodsamSampler, input.Tex);
    float4 woodColor = txWoodColor.Sample(txWoodsamSampler, input.Tex);
    float4 newColor = tileColor * woodColor * 2.0;
    newColor = saturate(newColor);

    //return newColor;
    return woodColor;
}
