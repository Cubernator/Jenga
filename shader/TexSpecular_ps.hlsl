#include "TexSpecular.hlsli"

float4 main(VertexOut input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 color = diffuse * colorTex.Sample(samplerState, input.uv);

	float NdotL = dot(-light.direction, input.normal);

	// diffuse lighting
	float3 lighting = saturate(NdotL * light.diffuse.xyz * color);

	// specular lighting
	float3 R = normalize(reflect(light.direction, input.normal));
	float3 V = normalize(input.viewDir);
	lighting += specular.xyz * pow(saturate(dot(R, V)), specPower) * specIntensity;

	// shadow
	lighting *= computeShadow(input.lightSpacePos, NdotL);

	return float4(saturate(color.xyz * light.ambient.xyz + lighting), color.a);
}