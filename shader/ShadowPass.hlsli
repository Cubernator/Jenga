cbuffer PerObject : register(b0)
{
	matrix mvp;
}

struct VertexIn
{
	float3 position : POSITION;
};

struct VertexOut
{
	float4 position : SV_POSITION;
};
