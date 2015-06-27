#include "common.hlsli"

cbuffer PerMaterial : register(b3)
{
	float4 diffuse;
}

Texture2D colorTex;
SamplerState samplerState;

struct VertexIn
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};
