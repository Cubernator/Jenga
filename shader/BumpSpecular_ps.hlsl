#include "BumpSpecular.hlsli"

float4 main(VertexOut input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 tex = colorTex.Sample(samplerState, input.uv);
	float4 color = tex * diffuse;

	// calculate normal per pixel
	float3 bump = 2.0f * bumpMap.Sample(samplerState, input.uv) - 1.0f;
	input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);
	float3 binormal = cross(input.normal, input.tangent);

	float3x3 texSpace = float3x3(input.tangent, binormal, input.normal);
	input.normal = normalize(mul(bump, texSpace));

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