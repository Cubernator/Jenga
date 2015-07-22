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
Texture2D bumpMap;
SamplerState samplerState;

struct VertexIn
{
	float3 position : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float4 lightSpacePos : POSITION;
	float2 uv : TEXCOORD0;
	float3 viewDir : TEXCOORD1;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};
