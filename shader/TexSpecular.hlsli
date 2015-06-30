#include "common.hlsli"
#include "shadow.hlsli"

cbuffer PerMaterial : register(b3)
{
	float4 diffuse;
	float4 specular;
	float specPower;
	float specIntensity;
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
	float4 lightSpacePos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
	float3 viewDir : TEXCOORD1;
};
