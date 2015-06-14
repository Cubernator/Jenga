#pragma once

#include "GameObject.h"
#include "DefaultTransform.h"
#include "DebugLineRenderer.h"

#include "utility.h"

class DebugPlane : public GameObject
{
private:
	std::unique_ptr<DefaultTransform> m_transform;
	std::unique_ptr<DebugLineRenderer> m_renderer;
	std::unique_ptr<DebugLineRenderer::VertexBuffer> m_vbuffer;

public:
	DebugPlane(float size, int numLines, Shader * s, const XMFLOAT4& color);
};