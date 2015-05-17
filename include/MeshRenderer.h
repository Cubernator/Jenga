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

	ID3D11InputLayout * m_layout;

	void draw() const final;

public:
	MeshRenderer(GameObject * parent, Shader * s, VertexBufferBase * vb, IndexBuffer * ib);
	~MeshRenderer();

	VertexBufferBase * getVertexBuffer();
	const VertexBufferBase * getVertexBuffer() const;

	IndexBuffer * getIndexBuffer();
	const IndexBuffer * getIndexBuffer() const;
};

