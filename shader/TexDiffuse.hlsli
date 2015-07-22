#include "common.hlsli"
#include "shadow.hlsli"

cbuffer PerMaterial : register(b3)
{
	float4 diffuse;
}

Texture2D colorTex : register(t0);
SamplerState samplerState;

struct VertexIn
{
	float3 position : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float4 lightSpacePos : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};
