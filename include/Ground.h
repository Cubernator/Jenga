#pragma once

#include "GameObject.h"
#include "MeshRenderer.h"
#include "VertexBuffer.h"
#include "PhysicsTransform.h"

#include <memory>

class Ground : public GameObject
{
private:
	px_ptr<PxMaterial> m_mat;
	px_ptr<PxRigidStatic> m_actor;
	std::unique_ptr<PhysicsTransform> m_transform;
	std::unique_ptr<MeshRenderer> m_renderer;
	std::unique_ptr<VertexBuffer<VertexPosNormalTex>> m_vbuffer;
	std::unique_ptr<Texture2D> m_texture;
	ID3D11SamplerState * m_samplerState;

	void onCollisionEnter(const Collision& collision) override;

public:
	Ground(Shader * s, IndexBuffer * ib);
	~Ground();
};
