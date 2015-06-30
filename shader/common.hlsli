struct Light
{
	float3 direction;
	float4 ambient;
	float4 diffuse;
};

cbuffer PerApplication : register(b0)
{
	matrix projection;
}

cbuffer PerFrame : register(b1)
{
	matrix view;
	matrix lightVP;
	Light light;
}

cbuffer PerObject : register(b2)
{
	matrix world;
	matrix mvp;
	bool receiveShadow;
}
