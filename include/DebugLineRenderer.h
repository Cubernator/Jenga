#pragma once

#include "Renderer.h"
#include "VertexBuffer.h"

class DebugLineRenderer : public Renderer
{
public:
	typedef VertexBuffer<VertexPosColor> VertexBuffer;

private:
	VertexBuffer * m_vbuffer;

	void draw() const final;

public:
	DebugLineRenderer(GameObject * parent, Shader * s, VertexBuffer * vb);
};