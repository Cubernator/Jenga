#pragma once

#include "DirectX.h"

struct Light
{
	XMFLOAT3 direction;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;

	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
};