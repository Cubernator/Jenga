#pragma once
#include "Component.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBufferBase.h"

#include <vector>

class Renderer : public Component
{
private:
	Shader *m_shader, *m_shadowShader;
	ID3D11InputLayout *m_inputLayout, *m_shadowInputLayout;
	ID3D11Buffer *m_cbuffer;
	std::vector<ID3D11ShaderResourceView*> m_resources;
	std::vector<ID3D11SamplerState*> m_samplers;

	virtual void draw() const = 0;

	template<typename T>
	void createConstantBufferInternal(const T* data)
	{
		D3D11_BUFFER_DESC cbd;
		ZeroMemory(&cbd, sizeof(D3D11_BUFFER_DESC));

		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = 0;
		cbd.Usage = D3D11_USAGE_DEFAULT;
		cbd.ByteWidth = sizeof(T);

		D3D11_SUBRESOURCE_DATA srd;
		ZeroMemory(&srd, sizeof(D3D11_SUBRESOURCE_DATA));
		srd.pSysMem = data;

		D3D11_SUBRESOURCE_DATA* srdp = &srd;
		if (!data) srdp = NULL;

		dev->CreateBuffer(&cbd, srdp, &m_cbuffer);
	}

	void setStates() const;
	void setStatesShadow() const;

	friend class GraphicsInterface;

public:
	Renderer(GameObject * parent, Shader * s, VertexBufferBase * vb);
	~Renderer();

	Shader * getShader();
	const Shader * getShader() const;

	ID3D11Buffer * getConstantBuffer() const;

	void addTexture(Texture2D * texture);
	void clearTextures();
	void addSampler(ID3D11SamplerState* sampler);
	void clearSamplers();

	template<typename T>
	void createConstantBuffer(const T& data)
	{
		createConstantBufferInternal(&data);
	}

	template<typename T>
	void createConstantBuffer()
	{
		createConstantBufferInternal<T>(NULL);
	}

	template<typename T>
	void updateConstantBuffer(const T& content)
	{
		devcon->UpdateSubresource(m_cbuffer, 0, NULL, &content, 0, 0);
	}
};

