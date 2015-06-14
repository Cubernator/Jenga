#include "utility.h"

float PIF = (float)M_PI;

VertexBuffer<VertexPosNormal> * createCuboidBuffer(float xs, float ys, float zs)
{
	VertexPosNormal vertices[] = {
		{ { -xs, -ys, -zs }, { -1.f, 0.f, 0.f } }, // left
		{ { -xs, -ys,  zs }, { -1.f, 0.f, 0.f } },
		{ { -xs,  ys, -zs }, { -1.f, 0.f, 0.f } },
		{ { -xs,  ys,  zs }, { -1.f, 0.f, 0.f } },

		{ { -xs, -ys, -zs }, { 0.f, -1.f, 0.f } }, // bottom
		{ { -xs, -ys,  zs }, { 0.f, -1.f, 0.f } },
		{ {  xs, -ys, -zs }, { 0.f, -1.f, 0.f } },
		{ {  xs, -ys,  zs }, { 0.f, -1.f, 0.f } },

		{ { -xs, -ys, -zs }, { 0.f, 0.f, -1.f } }, // front
		{ { -xs,  ys, -zs }, { 0.f, 0.f, -1.f } },
		{ {  xs, -ys, -zs }, { 0.f, 0.f, -1.f } },
		{ {  xs,  ys, -zs }, { 0.f, 0.f, -1.f } },

		{ {  xs, -ys, -zs }, { 1.f, 0.f, 0.f } }, // right
		{ {  xs,  ys, -zs }, { 1.f, 0.f, 0.f } },
		{ {  xs, -ys,  zs }, { 1.f, 0.f, 0.f } },
		{ {  xs,  ys,  zs }, { 1.f, 0.f, 0.f } },

		{ { -xs,  ys, -zs }, { 0.f, 1.f, 0.f } }, // top
		{ { -xs,  ys,  zs }, { 0.f, 1.f, 0.f } },
		{ {  xs,  ys, -zs }, { 0.f, 1.f, 0.f } },
		{ {  xs,  ys,  zs }, { 0.f, 1.f, 0.f } },

		{ { -xs, -ys,  zs }, { 0.f, 0.f, 1.f } }, // back
		{ {  xs, -ys,  zs }, { 0.f, 0.f, 1.f } },
		{ { -xs,  ys,  zs }, { 0.f, 0.f, 1.f } },
		{ {  xs,  ys,  zs }, { 0.f, 0.f, 1.f } }
	};

	return new VertexBuffer<VertexPosNormal>(vertices, 24);
}
