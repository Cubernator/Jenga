#pragma once
#include "Renderer.h"
#include "VertexBufferBase.h"
#include "IndexBuffer.h"

#include "DirectX.h"

class MeshRenderer : public Renderer
{
private:
	VertexBufferBase * m_vertexBuffer;
	IndexBuffer * m_indexBuffer;

	void draw() const final;

public:
	MeshRenderer(GameObject * parent, Shader * s, VertexBufferBase * vb, IndexBuffer * ib);

	VertexBufferBase * getVertexBuffer();
	const VertexBufferBase * getVertexBuffer() const;

	IndexBuffer * getIndexBuffer();
	const IndexBuffer * getIndexBuffer() const;
};

