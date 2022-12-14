//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
    float4 lightPos;
   
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
VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR, float3 Norm : NORMAL, float3 N : NORMAL )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(Pos, World);
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    //output.Color = Color;


    float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
    float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
    float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);
    float3 lightDir = normalize(lightPos.xyz - Pos.xyz);
    float3 normal = normalize(N);
    float diff = max(0.0, dot(lightDir, normal));
    output.Color = (materialAmb + diff * materialDiff) * lightCol;


    if (Norm.z == -1.0f)
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
    }
    
    return output;
}

VS_OUTPUT VS_main(float4 Pos : POSITION, float4 Color : COLOR)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float4 inPos = Pos;

    //Translation
    float3 T = float3(1, 0.3, 1.0);
    inPos.xyz += T;

    //Scaling
    float3 S = float3(0.2, 3, 3);
    inPos.xyz *= S;

    //Rotation
    float angle = 150.0f;
    float3x3 rMatrix = float3x3(
        cos(angle), 0, -sin(angle), 
        0, 1, 0,
        sin(angle), 0, cos(angle)
        );

    inPos.xyz = mul(rMatrix, inPos.xyz);

    // resulting matrix
    float4x4  ModelViewProjectionMatrix = mul(mul(World, View), Projection);
    output.Pos = mul(inPos, ModelViewProjectionMatrix);

}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 colors = float4(-1.0f,1.0f,0.0f,1.0f);
    return colors;
}
float4 PS2(VS_OUTPUT input) : SV_Target
{
    
    float4 colors = float4(1.5f,1.5f,0.0f,1.0f);
    return colors;
}

float4 PS3(VS_OUTPUT input) : SV_Target
{
    float4 colors = float4(0.7f,0.5f,0.3f,0.6f);
    return colors;
}
