#include "TexDiffuse.hlsli"

float4 main(VertexOut input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 color = diffuse * colorTex.Sample(samplerState, input.uv);
	float3 lighting = light.ambient.xyz;

	float NdotL = dot(-light.direction, input.normal);

	lighting += saturate(NdotL * light.diffuse.xyz) * computeShadow(input.lightSpacePos, NdotL);

	return float4(color.xyz * lighting, color.a);
}