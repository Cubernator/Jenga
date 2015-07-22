#pragma once

#include "VertexBuffer.h"
#include "PhysX.h"

#define _USE_MATH_DEFINES
#include <math.h>

extern float PIF;

std::wstring formatScore(unsigned int score);

VertexBuffer<VertexPosTexNormTan> * createCuboidBuffer(float xs, float ys, float zs, float texScale = 1.0f);
VertexBuffer<VertexPosTexNormTan> * createBrickBuffer(float xs, float ys, float zs);

inline float toRadf(float deg)
{
	return PIF * deg / 180.0f;
}

inline PxQuat fromToRotation(const PxVec3& from, const PxVec3& to) // to and from must be normalized!
{
	return PxQuat(acosf(from.dot(to)), from.cross(to).getNormalized());
}

void transformAABB(const XMMATRIX& mat, const XMVECTOR& localMin, const XMVECTOR& localMax, XMVECTOR& newMin, XMVECTOR& newMax);

XMMATRIX matrixAbs(const XMMATRIX& mat);

struct TexSpecular
{
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	float specPower;
	float specIntensity;
	XMFLOAT2 pad;
};
