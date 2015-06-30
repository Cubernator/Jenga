#include "DebugArrow.h"

DebugArrow::DebugArrow(const PxVec3& origin, const PxVec3& dir, Shader * s, XMFLOAT4 color)
{
	m_transform.reset(new DefaultTransform(this));
	setPosRot(origin, dir);
	setTransform(m_transform.get());

	setLength(dir.magnitude());
	setColorInternal(color);

	m_vbuffer.reset(new DebugLineRenderer::VertexBuffer(m_points.data(), m_points.size()));
	updateBuffer();

	m_renderer.reset(new DebugLineRenderer(this, s, m_vbuffer.get()));
	setRenderer(m_renderer.get());

	setCastShadow(false);
}

DebugArrow::DebugArrow(Shader * s, XMFLOAT4 color) : DebugArrow(PxVec3(0, 0, 0), PxVec3(0, 0, 1), s, color) {}

void DebugArrow::updateBuffer()
{
	m_vbuffer->updateData(m_points.data());
}

void DebugArrow::setLength(float l)
{
	m_points[0].pos = { 0, 0, 0 };
	m_points[1].pos = { 0, 0, l };
	m_points[2].pos = { 0, 0, l };
	m_points[3].pos = { -0.3f, 0, l-0.6f };
	m_points[4].pos = { 0, 0, l };
	m_points[5].pos = { 0.3f, 0, l-0.6f };
}

void DebugArrow::setColorInternal(const XMFLOAT4& color)
{
	for (VertexPosColor& v : m_points) v.color = color;
}

void DebugArrow::setColor(const XMFLOAT4& color)
{
	setColorInternal(color);
	updateBuffer();
}

void DebugArrow::setPosRot(const PxVec3& origin, const PxVec3& dir)
{
	m_transform->setPosition(origin);
	m_transform->setRotation(fromToRotation(PxVec3(0, 0, 1), dir.getNormalized()));
}