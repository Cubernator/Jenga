#pragma once

#include "Engine.h"
#include <DirectXMath.h>

using namespace DirectX;

struct VertexPos
{
	XMFLOAT3 pos;

	static const D3D11_INPUT_ELEMENT_DESC ied[1];
	static const UINT num = 1;
};

struct VertexPosColor
{
	XMFLOAT3 pos;
	XMFLOAT4 color;

	static const D3D11_INPUT_ELEMENT_DESC ied[2];
	static const UINT num = 2;
};

struct VertexPosTex
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;

	static const D3D11_INPUT_ELEMENT_DESC ied[2];
	static const UINT num = 2;
};

struct VertexPosNormal
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;

	static const D3D11_INPUT_ELEMENT_DESC ied[2];
	static const UINT num = 2;
};

struct VertexPosNormalTex
{
	XMFLOAT3 pos, normal;
	XMFLOAT2 tex;

	static const D3D11_INPUT_ELEMENT_DESC ied[3];
	static const UINT num = 3;
};