#include "common.hlsli"

cbuffer PerMaterial : register(b3)
{
	float4 diffuse;
}

struct VertexIn
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float3 normal : TEXCOORD;
};
