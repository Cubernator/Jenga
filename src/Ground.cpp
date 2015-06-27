#include "Ground.h"
#include "Brick.h"
#include "utility.h"

Ground::Ground(Shader * s, IndexBuffer * ib)
{
	PxVec3 halfSize(100.f, 1.0f, 100.f);

	m_mat.reset(physics->createMaterial(0.4f, 0.8f, 0.5f));

	m_actor.reset(PxCreateStatic(*physics, PxTransform(PxVec3(0, -1, 0)), PxBoxGeometry(halfSize), *m_mat.get()));
	setActor(m_actor.get());

	m_transform.reset(new PhysicsTransform(this));
	setTransform(m_transform.get());

	m_texture.reset(new Texture2D(L"assets\\images\\ground.jpg"));

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

	m_vbuffer.reset(createCuboidBuffer(halfSize.x, halfSize.y, halfSize.z, 30.0f));
	m_renderer.reset(new MeshRenderer(this, s, m_vbuffer.get(), ib));
	m_renderer->createConstantBuffer(XMFLOAT4(.6f, .6f, .6f, 1.f));
	m_renderer->addTexture(m_texture.get());
	m_renderer->addSampler(m_samplerState);
	setRenderer(m_renderer.get());

	setCollisionCallbackFlags(ENTER);
}

Ground::~Ground()
{
	m_samplerState->Release();
}

void Ground::onCollisionEnter(const Collision& collision)
{
	if (GameObject * obj = collision.getOtherObject()) {
		if (Brick * b = dynamic_cast<Brick*>(obj)) {
			if (!b->canTouchGround()) {
				m_renderer->updateConstantBuffer(XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f));
				b->setState(FAULTED);
			}
		}
	}
}
