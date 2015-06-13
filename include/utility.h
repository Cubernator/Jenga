#pragma once

#include "VertexBuffer.h"

#define _USE_MATH_DEFINES
#include <math.h>

extern float PIF;

VertexBuffer<VertexPosNormal> * createCuboidBuffer(float xs, float ys, float zs);

inline float toRadf(float deg)
{
	return PIF * deg / 180.0f;
}