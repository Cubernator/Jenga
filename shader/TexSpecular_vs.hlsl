#include "TexSpecular.hlsli"

VertexOut main(VertexIn input)
{
	VertexOut vOut;

	float4 pos = float4(input.position, 1.0f);
	float4 worldPos = mul(world, pos);

	vOut.position = mul(mvp, pos);
	vOut.lightSpacePos = mul(lightVP, worldPos);
	vOut.normal = normalize(mul(world, float4(input.normal, 0.0f)).xyz);
	vOut.uv = input.uv;
	vOut.viewDir = cameraPosition.xyz - worldPos.xyz;

	return vOut;
}