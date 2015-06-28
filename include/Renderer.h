#pragma once
#include "Component.h"
#include "Shader.h"
#include "Texture.h"

#include <vector>

class Renderer : public Component
{
private:
	Shader * m_shader;
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

	void setStates() const
	{
		devcon->VSSetShader(m_shader->getVertexShader(), NULL, 0);
		devcon->PSSetShader(m_shader->getPixelShader(), NULL, 0);

		devcon->VSSetShaderResources(0, m_resources.size(), m_resources.data());
		devcon->PSSetShaderResources(0, m_resources.size(), m_resources.data());

		devcon->VSSetSamplers(0, m_samplers.size(), m_samplers.data());
		devcon->PSSetSamplers(0, m_samplers.size(), m_samplers.data());
	}

	friend class GraphicsInterface;

public:
	Renderer(GameObject * parent, Shader * s) : Component(parent), m_cbuffer(nullptr), m_shader(s) { };
	~Renderer() { if (m_cbuffer) m_cbuffer->Release(); }

	Shader * getShader() { return m_shader; }
	const Shader * getShader() const { return m_shader; }

	ID3D11Buffer * getConstantBuffer() const { return m_cbuffer; }

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

	void addTexture(Texture2D * texture)
	{
		m_resources.push_back(texture->getResourceView());
	}

	void clearTextures()
	{
		m_resources.clear();
	}

	void addSampler(ID3D11SamplerState* sampler)
	{
		m_samplers.push_back(sampler);
	}

	void clearSamplers()
	{
		m_samplers.clear();
	}
};

