#include "utility.h"

#include <sstream>
#include <iomanip>

float PIF = (float)M_PI;

std::wstring formatScore(unsigned int score)
{
	std::wstringstream wss;
	wss << std::setw(7) << std::setfill(L'0') << score;
	return wss.str();
}

VertexBuffer<VertexPosNormalTex> * createCuboidBuffer(float xs, float ys, float zs, float texScale)
{
	float f = 2.0f / texScale;
	float xs2 = xs*f, ys2 = ys*f, zs2 = zs*f;
	VertexPosNormalTex vertices[] = {
		{ { -xs, -ys, -zs }, { -1.f, 0.f, 0.f }, { zs2, ys2 } }, // left
		{ { -xs, -ys, zs }, { -1.f, 0.f, 0.f }, { 0.f, ys2 } },
		{ { -xs, ys, -zs }, { -1.f, 0.f, 0.f }, { zs2, 0.f } },
		{ { -xs, ys, zs }, { -1.f, 0.f, 0.f }, { 0.f, 0.f } },

		{ { -xs, -ys, -zs }, { 0.f, -1.f, 0.f }, { zs2, 0.f } }, // bottom
		{ { -xs, -ys, zs }, { 0.f, -1.f, 0.f }, { 0.f, 0.f } },
		{ { xs, -ys, -zs }, { 0.f, -1.f, 0.f }, { zs2, xs2 } },
		{ { xs, -ys, zs }, { 0.f, -1.f, 0.f }, { 0.f, xs2 } },

		{ { -xs, -ys, -zs }, { 0.f, 0.f, -1.f }, { 0.f, 0.f } }, // front
		{ { -xs, ys, -zs }, { 0.f, 0.f, -1.f }, { ys2, 0.f } },
		{ { xs, -ys, -zs }, { 0.f, 0.f, -1.f }, { 0.f, xs2 } },
		{ { xs, ys, -zs }, { 0.f, 0.f, -1.f }, { ys2, xs2 } },

		{ { xs, -ys, -zs }, { 1.f, 0.f, 0.f }, { 0.f, ys2 } }, // right
		{ { xs, ys, -zs }, { 1.f, 0.f, 0.f }, { 0.f, 0.f } },
		{ { xs, -ys, zs }, { 1.f, 0.f, 0.f }, { zs2, ys2 } },
		{ { xs, ys, zs }, { 1.f, 0.f, 0.f }, { zs2, 0.f } },

		{ { -xs, ys, -zs }, { 0.f, 1.f, 0.f }, { 0.f, 0.f } }, // top
		{ { -xs, ys, zs }, { 0.f, 1.f, 0.f }, { zs2, 0.f } },
		{ { xs, ys, -zs }, { 0.f, 1.f, 0.f }, { 0.f, xs2 } },
		{ { xs, ys, zs }, { 0.f, 1.f, 0.f }, { zs2, xs2 } },

		{ { -xs, -ys, zs }, { 0.f, 0.f, 1.f }, { ys2, 0.f } }, // back
		{ { xs, -ys, zs }, { 0.f, 0.f, 1.f }, { ys2, xs2 } },
		{ { -xs, ys, zs }, { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
		{ { xs, ys, zs }, { 0.f, 0.f, 1.f }, { 0.f, xs2 } }
	};

	return new VertexBuffer<VertexPosNormalTex>(vertices, 24);
}


VertexBuffer<VertexPosNormalTex> * createBrickBuffer(float xs, float ys, float zs)
{
	float zw = zs*2.0f, yw = ys*2.0f;
	float w = zw + yw;
	zw /= w;
	yw /= w;

	float yo = zw + yw;
	VertexPosNormalTex vertices[] = {
		{ { -xs, -ys, -zs }, { -1.f, 0.f, 0.f }, { 1.f, 1.f } }, // left
		{ { -xs, -ys, zs }, { -1.f, 0.f, 0.f }, { 0.f, 1.f } },
		{ { -xs, ys, -zs }, { -1.f, 0.f, 0.f }, { 1.f, 0.f } },
		{ { -xs, ys, zs }, { -1.f, 0.f, 0.f }, { 0.f, 0.f } },

		{ { -xs, -ys, -zs }, { 0.f, -1.f, 0.f }, { zw, 0.f } }, // bottom
		{ { -xs, -ys, zs }, { 0.f, -1.f, 0.f }, { 0.f, 0.f } },
		{ { xs, -ys, -zs }, { 0.f, -1.f, 0.f }, { zw, 1.f } },
		{ { xs, -ys, zs }, { 0.f, -1.f, 0.f }, { 0.f, 1.f } },

		{ { -xs, -ys, -zs }, { 0.f, 0.f, -1.f }, { zw, 0.f } }, // front
		{ { -xs, ys, -zs }, { 0.f, 0.f, -1.f }, { yo, 0.f } },
		{ { xs, -ys, -zs }, { 0.f, 0.f, -1.f }, { zw, 1.f } },
		{ { xs, ys, -zs }, { 0.f, 0.f, -1.f }, { yo, 1.f } },

		{ { xs, -ys, -zs }, { 1.f, 0.f, 0.f }, { 0.f, 1.f } }, // right
		{ { xs, ys, -zs }, { 1.f, 0.f, 0.f }, { 0.f, 0.f } },
		{ { xs, -ys, zs }, { 1.f, 0.f, 0.f }, { 1.f, 1.f } },
		{ { xs, ys, zs }, { 1.f, 0.f, 0.f }, { 1.f, 0.f } },

		{ { -xs, ys, -zs }, { 0.f, 1.f, 0.f }, { 0.f, 0.f } }, // top
		{ { -xs, ys, zs }, { 0.f, 1.f, 0.f }, { zw, 0.f } },
		{ { xs, ys, -zs }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } },
		{ { xs, ys, zs }, { 0.f, 1.f, 0.f }, { zw, 1.f } },

		{ { -xs, -ys, zs }, { 0.f, 0.f, 1.f }, { yo, 0.f } }, // back
		{ { xs, -ys, zs }, { 0.f, 0.f, 1.f }, { yo, 1.f } },
		{ { -xs, ys, zs }, { 0.f, 0.f, 1.f }, { zw, 0.f } },
		{ { xs, ys, zs }, { 0.f, 0.f, 1.f }, { zw, 1.f } }
	};

	return new VertexBuffer<VertexPosNormalTex>(vertices, 24);
}

void transformAABB(const XMMATRIX& mat, const XMVECTOR& localMin, const XMVECTOR& localMax, XMVECTOR& newMin, XMVECTOR& newMax)
{
	XMVECTOR center = (localMin + localMax) / 2.0f;
	XMVECTOR extent = (localMax - localMin) / 2.0f;
	XMVectorSetW(extent, 0.0f);

	center = XMVector4Transform(center, mat);
	extent = XMVector4Transform(extent, matrixAbs(mat));

	newMin = center - extent;
	newMax = center + extent;
}

XMMATRIX matrixAbs(const XMMATRIX& mat)
{
	XMFLOAT4X4 m;
	XMStoreFloat4x4(&m, mat);

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			m(i, j) = fabsf(m(i, j));
		}
	}

	return XMLoadFloat4x4(&m);
}