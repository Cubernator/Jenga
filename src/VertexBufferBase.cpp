#include "VertexBufferBase.h"

VertexBufferBase::VertexBufferBase(UINT count, UINT stride, const D3D11_INPUT_ELEMENT_DESC *desc, UINT numElements)
	: m_vDesc(desc), m_count(count), m_stride(stride), m_numElements(numElements)
{
}

VertexBufferBase::~VertexBufferBase()
{
	m_buffer->Release();
}

ID3D11Buffer * VertexBufferBase::getBuffer()
{
	return m_buffer;
}

void VertexBufferBase::setBuffer(ID3D11Buffer * b)
{
	m_buffer = b;
}

const D3D11_INPUT_ELEMENT_DESC* VertexBufferBase::getVertexDesc() const
{
	return m_vDesc;
}

UINT VertexBufferBase::getNumVertexElements() const
{
	return m_numElements;
}

UINT VertexBufferBase::getCount() const
{
	return m_count;
}

UINT VertexBufferBase::getStride() const
{
	return m_stride;
}
