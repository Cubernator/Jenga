#include "DebugPlane.h"

DebugPlane::DebugPlane(float size, int numLines, Shader * s, const XMFLOAT4& color)
{
	m_transform.reset(new DefaultTransform(this));
	setTransform(m_transform.get());

	std::vector<VertexPosColor> points;
	points.resize(numLines * 4);

	float hs = size / 2.0f, cs = size / (numLines-1);

	for (int i = 0; i < numLines; ++i) {
		int x = i * 4;
		points[x + 0] = { { i*cs - hs, 0, -hs }, color };
		points[x + 1] = { { i*cs - hs, 0, hs }, color };
		points[x + 2] = { { -hs, 0, i*cs - hs }, color };
		points[x + 3] = { { hs, 0, i*cs - hs }, color };
	}

	m_vbuffer.reset(new DebugLineRenderer::VertexBuffer(points.data(), points.size()));

	m_renderer.reset(new DebugLineRenderer(this, s, m_vbuffer.get()));
	setRenderer(m_renderer.get());
}
