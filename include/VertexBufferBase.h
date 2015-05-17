#pragma once
#include "DirectX.h"

class VertexBufferBase
{
private:
	ID3D11Buffer *m_buffer;
	const D3D11_INPUT_ELEMENT_DESC *m_vDesc;
	UINT m_count, m_stride, m_numElements;

protected:
	VertexBufferBase(UINT count, UINT stride, const D3D11_INPUT_ELEMENT_DESC *desc, UINT numElements);

	void setBuffer(ID3D11Buffer *b);

public:
	virtual ~VertexBufferBase();

	ID3D11Buffer * getBuffer();
	UINT getCount() const;
	UINT getStride() const;

	const D3D11_INPUT_ELEMENT_DESC* getVertexDesc() const;
	UINT getNumVertexElements() const;
};

