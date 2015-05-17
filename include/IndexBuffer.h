#pragma once

#include "DirectX.h"

class IndexBuffer
{
private:
	ID3D11Buffer *m_buffer;
	UINT m_count;

public:
	typedef UINT16 IndexType;

	IndexBuffer(IndexType *pData, UINT count, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);
	~IndexBuffer();

	UINT getCount() const;

	ID3D11Buffer * getBuffer();
};

