#include "Diffuse.hlsli"

VertexOut main(VertexIn input)
{
	VertexOut vOut;

	vOut.position = mul(mvp, float4(input.position, 1.0f));
	vOut.normal = mul(world, float4(input.normal, 0.0f)).xyz;

	return vOut;
}
