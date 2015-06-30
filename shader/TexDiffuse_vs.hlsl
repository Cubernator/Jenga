#include "TexDiffuse.hlsli"

VertexOut main(VertexIn input)
{
	VertexOut vOut;

	float4 pos = float4(input.position, 1.0f);

	vOut.position = mul(mvp, pos);
	vOut.lightSpacePos = mul(lightVP, mul(world, pos));
	vOut.normal = normalize(mul(world, float4(input.normal, 0.0f)).xyz);
	vOut.uv = input.uv;

	return vOut;
}