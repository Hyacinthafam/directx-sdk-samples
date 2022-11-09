//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR)
//{
//    VS_OUTPUT output = (VS_OUTPUT)0;
//    output.Pos = mul(Pos, World);
//    output.Pos = mul(output.Pos, View);
//    output.Pos = mul(output.Pos, Projection);
//    output.Color = Color;
//    return output;
//}

VS_OUTPUT VS_main(float4 Pos : POSITION, float4 Color : COLOR)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float4 inPos = Pos;
    float3 translation = float3(1.0f, 0.3f, 1.0f);
    float3 scale = float3(0.2f, 3.0f, 3.0f);

    float angle = 1.3748;

    float3x3 rotationMatrix = { cos(angle), 0.0f, sin(angle), 0.0f, 1.0f, 0.0f, -sin(angle), 0.0f, cos(angle) };

    inPos.xyz = (scale * (mul(inPos.xyz, rotationMatrix))) + translation;
    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Color = Color;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
    return input.Color;
}