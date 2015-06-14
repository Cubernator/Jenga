#pragma once

#include "Vertex.h"
#include "VertexBufferBase.h"

template<typename VertexType = VertexPos>
class VertexBuffer : public VertexBufferBase
{
public:
	VertexBuffer(VertexType *pData, UINT count, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) : VertexBufferBase(count, sizeof(VertexType), VertexType::ied, VertexType::num)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = usage;
		bd.ByteWidth = count * getStride();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA srd;
		ZeroMemory(&srd, sizeof(srd));
		srd.pSysMem = pData;

		ID3D11Buffer * b;

		HRESULT hr = dev->CreateBuffer(&bd, &srd, &b);       // create the buffer

		setBuffer(b);
	}

	void updateData(VertexType *pData)
	{
		devcon->UpdateSubresource(getBuffer(), 0, NULL, pData, 0, 0);
	}
};

