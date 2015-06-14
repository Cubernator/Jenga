#include "DebugLineRenderer.h"

DebugLineRenderer::DebugLineRenderer(GameObject * parent, Shader * s, VertexBuffer * vb) : Renderer(parent, s), m_vbuffer(vb)
{
	ID3DBlob * vsb = m_shader->getVertexByteCode();
	dev->CreateInputLayout(m_vbuffer->getVertexDesc(), m_vbuffer->getNumVertexElements(),
		vsb->GetBufferPointer(), vsb->GetBufferSize(), &m_layout);
}

DebugLineRenderer::~DebugLineRenderer()
{
	m_layout->Release();
}

void DebugLineRenderer::draw() const
{
	devcon->VSSetShader(m_shader->getVertexShader(), NULL, 0);
	devcon->PSSetShader(m_shader->getPixelShader(), NULL, 0);

	ID3D11Buffer * vb = m_vbuffer->getBuffer();
	const UINT stride = m_vbuffer->getStride(), offset = 0;
	devcon->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	devcon->IASetInputLayout(m_layout);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	devcon->Draw(m_vbuffer->getCount(), 0);
}
