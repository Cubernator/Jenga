#include "ShadowPass.hlsli"

VertexOut main(VertexIn input)
{
	VertexOut vOut;

	vOut.position = mul(mvp, float4(input.position, 1.0f));

	return vOut;
}