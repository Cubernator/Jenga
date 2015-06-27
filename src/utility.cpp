#include "utility.h"

float PIF = (float)M_PI;

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
