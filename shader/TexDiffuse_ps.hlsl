#include "TexDiffuse.hlsli"

float4 main(VertexOut input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 color = diffuse * colorTex.Sample(samplerState, input.uv);
	float4 finalColor = color * light.ambient;
	finalColor += saturate(dot(-light.direction, input.normal) * light.diffuse * color);

	return float4(finalColor.xyz, color.a);
}