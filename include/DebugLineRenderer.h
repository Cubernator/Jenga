#pragma once

#include "Renderer.h"
#include "VertexBuffer.h"

class DebugLineRenderer : public Renderer
{
public:
	typedef VertexBuffer<VertexPosColor> VertexBuffer;
private:
	VertexBuffer * m_vbuffer;
	ID3D11InputLayout * m_layout;

	void draw() const final;

public:
	DebugLineRenderer(GameObject * parent, Shader * s, VertexBuffer * vb);
	~DebugLineRenderer();
};