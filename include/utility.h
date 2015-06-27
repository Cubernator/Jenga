#pragma once

#include "VertexBuffer.h"

#define _USE_MATH_DEFINES
#include <math.h>

extern float PIF;

VertexBuffer<VertexPosNormalTex> * createCuboidBuffer(float xs, float ys, float zs, float texScale = 1.0f);
VertexBuffer<VertexPosNormalTex> * createBrickBuffer(float xs, float ys, float zs);

inline float toRadf(float deg)
{
	return PIF * deg / 180.0f;
}

inline PxQuat fromToRotation(const PxVec3& from, const PxVec3& to) // to and from must be normalized!
{
	return PxQuat(acosf(from.dot(to)), from.cross(to).getNormalized());
}