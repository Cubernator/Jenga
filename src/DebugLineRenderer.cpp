#include "DebugLineRenderer.h"

DebugLineRenderer::DebugLineRenderer(GameObject * parent, Shader * s, VertexBuffer * vb) : Renderer(parent, s, vb), m_vbuffer(vb) { }

void DebugLineRenderer::draw() const
{
	ID3D11Buffer * vb = m_vbuffer->getBuffer();
	const UINT stride = m_vbuffer->getStride(), offset = 0;
	devcon->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	devcon->Draw(m_vbuffer->getCount(), 0);
}
