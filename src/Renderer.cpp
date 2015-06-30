#include "Renderer.h"
#include "Graphics.h"

Renderer::Renderer(GameObject * parent, Shader * s, VertexBufferBase * vb) : Component(parent), m_cbuffer(nullptr), m_shader(s)
{
	m_inputLayout = m_shader->generateInputLayout(vb);
	m_shadowShader = graphics->getShadowShader();
	m_shadowInputLayout = m_shadowShader->generateInputLayout(vb);
}

Renderer::~Renderer()
{
	if (m_cbuffer) m_cbuffer->Release();
}

Shader * Renderer::getShader()
{
	return m_shader;
}

const Shader * Renderer::getShader() const
{
	return m_shader;
}

ID3D11Buffer * Renderer::getConstantBuffer() const
{
	return m_cbuffer;
}

void Renderer::addTexture(Texture2D * texture)
{
	m_resources.push_back(texture->getResourceView());
}

void Renderer::clearTextures()
{
	m_resources.clear();
}

void Renderer::addSampler(ID3D11SamplerState* sampler)
{
	m_samplers.push_back(sampler);
}

void Renderer::clearSamplers()
{
	m_samplers.clear();
}

void Renderer::setStates() const
{
	m_shader->apply();

	devcon->VSSetShaderResources(0, m_resources.size(), m_resources.data());
	devcon->PSSetShaderResources(0, m_resources.size(), m_resources.data());

	devcon->VSSetSamplers(0, m_samplers.size(), m_samplers.data());
	devcon->PSSetSamplers(0, m_samplers.size(), m_samplers.data());

	devcon->IASetInputLayout(m_inputLayout);
}

void Renderer::setStatesShadow() const
{
	m_shadowShader->apply();

	devcon->IASetInputLayout(m_inputLayout);
}
