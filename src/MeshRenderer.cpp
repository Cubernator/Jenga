#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(GameObject * parent, Shader * s, VertexBufferBase * vb, IndexBuffer * ib)
	: Renderer(parent, s, vb), m_vertexBuffer(vb), m_indexBuffer(ib) { }

void MeshRenderer::draw() const
{
	ID3D11Buffer * vb = m_vertexBuffer->getBuffer();
	const UINT stride = m_vertexBuffer->getStride(), offset = 0;

	devcon->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	devcon->IASetIndexBuffer(m_indexBuffer->getBuffer(), DXGI_FORMAT_R16_UINT, 0);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	devcon->DrawIndexed(m_indexBuffer->getCount(), 0, 0);
}

VertexBufferBase * MeshRenderer::getVertexBuffer()
{
	return m_vertexBuffer;
}
const VertexBufferBase * MeshRenderer::getVertexBuffer() const
{
	return m_vertexBuffer;
}

IndexBuffer * MeshRenderer::getIndexBuffer()
{
	return m_indexBuffer;
}
const IndexBuffer * MeshRenderer::getIndexBuffer() const
{
	return m_indexBuffer;
}
