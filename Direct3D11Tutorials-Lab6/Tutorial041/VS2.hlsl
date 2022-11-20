
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{

	float4 inpos = pos;
	inpos.x += 3;
	inpos = mul(inpos, World);
	inpos = mul(inpos, View);
	inpos = mul(inpos, Projection);

	
	return inpos;
}