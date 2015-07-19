#include "Background.h"

Background::Background(Shader * s, const std::wstring& objFileName, const std::wstring& texFileName)
{
	m_transform.reset(new DefaultTransform(this));

	MeshVertexBuffer * mvb;
	IndexBuffer * ib;

	if (loadMeshFromObjFile(objFileName, &mvb, &ib)) {
		m_vbuffer.reset(mvb);
		m_ibuffer.reset(ib);
		m_renderer.reset(new MeshRenderer(this, s, mvb, ib));

		m_texture.reset(new Texture2D(texFileName));

		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.Filter = D3D11_FILTER_ANISOTROPIC;
		sd.MaxAnisotropy = 8;
		sd.MinLOD = -FLT_MAX;
		sd.MaxLOD = FLT_MAX;
		dev->CreateSamplerState(&sd, &m_samplerState);

		m_material = {
			{ 1.f, 1.f, 1.f, 1.0f },
			{ 1.f, 1.f, 1.f, 1.0f },
			0.0f,
			0.0f
		};

		m_renderer->addTexture(m_texture.get());
		m_renderer->addSampler(m_samplerState.Get());

		setRenderer(m_renderer.get());
	}

	setTransform(m_transform.get());
}