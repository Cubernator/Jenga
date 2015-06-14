#pragma once

#include "utility.h"

#include "GameObject.h"
#include "DefaultTransform.h"
#include "DebugLineRenderer.h"

#include <array>

class DebugArrow : public GameObject
{
private:
	std::unique_ptr<DebugLineRenderer::VertexBuffer> m_vbuffer;
	std::unique_ptr<DebugLineRenderer> m_renderer;
	std::unique_ptr<DefaultTransform> m_transform;

	std::array<VertexPosColor, 6> m_points;

	void setPosRot(const PxVec3& origin, const PxVec3& dir);
	void setLength(float length);
	void setColorInternal(const XMFLOAT4& color);

	void updateBuffer();

public:
	DebugArrow(const PxVec3& origin, const PxVec3& dir, Shader * s, XMFLOAT4 color);
	DebugArrow(Shader * s, XMFLOAT4 color);

	void setColor(const XMFLOAT4& color);

	void setProps(const PxVec3& origin, const PxVec3& dir, float length)
	{
		setPosRot(origin, dir);
		setLength(length);
		updateBuffer();
	}

	void setProps(const PxVec3& start, const PxVec3& end)
	{
		PxVec3 dir = end - start;
		setProps(start, dir, dir.magnitude());
	}
};