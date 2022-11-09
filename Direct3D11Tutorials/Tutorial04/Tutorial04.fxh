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


//VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR )
//{
//    VS_OUTPUT output = (VS_OUTPUT)0;
//    output.Pos = mul( Pos, World );
//    output.Pos = mul( output.Pos, View );
//    output.Pos = mul( output.Pos, Projection );
//    output.Color = Color;
//    return output;
//}

VS_OUTPUT VS_main(float4 Pos : POSITION, float4 Color : COLOR)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
<<<<<<< HEAD
    float4 inPos = Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
=======
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    float4 inPos = Pos;
>>>>>>> 564388833b5dc6432149b78208c9cac793a9b4ff
    output.Color = Color;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    return input.Color;
}
