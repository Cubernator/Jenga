#include "VertexColor.hlsli"

float4 main(VertexOut input) : SV_TARGET
{
	return input.color;
}