#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(IndexType *pData, UINT count, D3D11_USAGE usage) : m_count(count)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = usage;
	bd.ByteWidth = count * sizeof(IndexType);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA srd;
	ZeroMemory(&srd, sizeof(srd));
	srd.pSysMem = pData;

	dev->CreateBuffer(&bd, &srd, &m_buffer);       // create the buffer
}

IndexBuffer::~IndexBuffer()
{
	m_buffer->Release();
}

ID3D11Buffer * IndexBuffer::getBuffer()
{
	return m_buffer;
}

UINT IndexBuffer::getCount() const
{
	return m_count;
}