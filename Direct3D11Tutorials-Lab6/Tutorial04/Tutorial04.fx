//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

Texture2D txWoodColor : register(t0);
Texture2D txtileColor : register(t1);
SamplerState txWoodsamSampler : register(s0);

cbuffer ConstantBuffer : register( b0 )
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


struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
    //float3 Norm : NORMAL;
    //float3 Tang : TANGENT;
    //float3 Binorm : BINORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 Norm : TEXCOORD1;
    float3 viewDirInTang : TEXCOORD2;
    float3 lightDirInTang : TEXCOORD3;
    float3 tangent: TANGENT;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;
    //output.Norm = mul(float4(input.Norm, 1), World).xyz;

    return output;
}
//PixelInputType BumpMapVertexShader(VertexInputType input)
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS(PS_INPUT input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
        // float    x = BumpDensity * input.Tex.x,
        //y = BumpDensity * input.Tex.y;
        //float3Normal = bumpNormal(x, y);



        //float3  bumpNormal(float2 xy) {
        //    float3 N = vec3(0.0, 0.0, 1.0);
        //    //map xyto [-1, 1]x[-1, 1]:
        //    float2 st = 2.0 * fract(xy) - 1.0;
        //    float R2 = radius * radius - dot(st, st);
        //    if (R2 > 0.0) {
        //        N.xy = st / sqrt(R2);
        //        return normalize(N);
        //    }
    //float3 NormalAsCol = tex2D(bumpMap, Texcoord).xyz;
    //float3 NormalFrCol = normalize(2.0 * NormalAsCol - 1.0);
    float4 stoneColor = txWoodColor.Sample(txWoodsamSampler, input.Tex);
    float4 bmpFile = txWoodColor.Sample(txWoodsamSampler, input.Tex);
    float4 newColor = stoneColor * bmpFile;
    float4 dir = float4(0.0f, 0.0f, -1.0f, 1.0f);
    float4 col = float4(0.5f, 0.0f, 0.0f, 1.0f);
    newColor += saturate(dot((float3) dir, input.Norm) * col);
    return newColor;
}
