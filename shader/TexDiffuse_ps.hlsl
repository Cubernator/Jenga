#include "TexDiffuse.hlsli"

float4 main(VertexOut input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 color = diffuse * colorTex.Sample(samplerState, input.uv);
	float4 finalColor = color * light.ambient;

	float NdotL = dot(-light.direction, input.normal);

	if (computeShadow(input.lightSpacePos, NdotL) > 0.0f) {
		finalColor += saturate(NdotL * light.diffuse * color);
	}

	return float4(finalColor.xyz, color.a);
}