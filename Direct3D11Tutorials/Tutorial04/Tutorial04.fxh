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
int Norm;
VS_OUTPUT VS_main(float4 Pos : POSITION, float4 Color : COLOR)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

   /* if (Norm.z == -1.0f)
    {
        output.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if (Norm.z == 1.0f)
    {
        output.Color = float4(0.4f, 0.1f, 0.7f, 1.0f);
    }
    else if (Norm.y == -1.0f)
    {
        output.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if (Norm.y == 1.0f)
    {
        output.Color = float4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    else if (Norm.x == -1.0f)
    {
        output.Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else if (Norm.x == 1.0f)
    {
        output.Color = float4(1.0f, 0.5f, 0.5f, 1.0f);
    }*/

    //float4 materialAmb = float4(0.1f, 0.2f, 0.3f, 1.0f);
    //float4 materialDif = float4(1.0f, 0.0f, 0.0f, 1.0f);
    //float4 lightCol = float4;
    //float4 lightDir = normalize(LightPos.xyz - Pos.xyz);
    //float diff = max(0.0f, dot(lightDir, normal));
    //
    //
    //float4 inPos = Pos;
    //float3 translation = float3(1.0f, 0.3f, 1.0f);
    //float3 scale = float3(0.2f, 3.0f, 3.0f);

    //float angle = 1.3748;

    //float3x3 rotationMatrix = { cos(angle), 0.0f, sin(angle), 0.0f, 1.0f, 0.0f, -sin(angle), 0.0f, cos(angle) };

    //inPos.xyz = (scale * (mul(inPos.xyz, rotationMatrix))) + translation;
    //
    float4 inPos = Pos;
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