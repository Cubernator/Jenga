Texture2D shadowMap : register(t4);
SamplerComparisonState shadowSampler : register(s4);

float computeShadow(float4 pos, float NdotL)
{
	float2 shadowTexCoords;
	shadowTexCoords.x = 0.5f + (pos.x / pos.w * 0.5f);
	shadowTexCoords.y = 0.5f - (pos.y / pos.w * 0.5f);
	float pixelDepth = pos.z / pos.w;

	if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) && (saturate(shadowTexCoords.y) == shadowTexCoords.y) && (pixelDepth > 0)) {
		float margin = acos(saturate(NdotL));
		float epsilon = clamp(0.001 / margin, 0, 0.1);

		return float(shadowMap.SampleCmp(shadowSampler, shadowTexCoords, pixelDepth + epsilon));
	}

	return 1.0f;
}