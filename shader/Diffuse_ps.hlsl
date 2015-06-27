#include "Diffuse.hlsli"

float4 main(VertexOut input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(-light.direction, input.normal) * light.diffuse * diffuse);

	return float4(finalColor.xyz, diffuse.a);
}