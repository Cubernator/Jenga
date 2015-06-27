#include "TexDiffuse.hlsli"

VertexOut main(VertexIn input)
{
	VertexOut vOut;

	vOut.position = mul(mvp, float4(input.position, 1.0f));
	vOut.normal = normalize(mul(world, float4(input.normal, 0.0f)).xyz);
	vOut.uv = input.uv;

	return vOut;
}