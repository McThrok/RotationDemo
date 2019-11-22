cbuffer perObjectBuffer : register(b0)
{
	float4x4 wvpMatrix;
	float4x4 worldMatrix;
	float4 color;
};

struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float3 inNormal : NORMAL;
	float3 inWorldPos : WORLD_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return color;
}