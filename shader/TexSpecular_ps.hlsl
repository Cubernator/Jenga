#include "TexSpecular.hlsli"

float4 main(VertexOut input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 tex = colorTex.Sample(samplerState, input.uv);
	float4 color = tex * diffuse;

	float NdotL = dot(-light.direction, input.normal);

	// diffuse lighting
	float3 lighting = saturate(NdotL * light.diffuse.xyz * color.xyz);

	if (NdotL > 0.0f) {

		// use texture alpha channel as specular map
		float specInt = specIntensity * tex.a;

		// specular lighting
		float3 R = normalize(reflect(light.direction, input.normal));
		float3 V = normalize(input.viewDir);
		lighting += specular.xyz * pow(saturate(dot(R, V)), specPower) * specInt;
	}

	// shadow
	lighting *= computeShadow(input.lightSpacePos, NdotL);

	return float4(saturate(color.xyz * light.ambient.xyz + lighting), diffuse.a);
}