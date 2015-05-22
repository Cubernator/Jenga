#pragma once

#include "GameObject.h"
#include "MeshRenderer.h"
#include "VertexBuffer.h"
#include "PhysicsTransform.h"

#include <memory>

class Block : public GameObject
{
private:
	std::unique_ptr<PhysicsTransform> m_transform;
	std::unique_ptr<MeshRenderer> m_renderer;
	std::unique_ptr<VertexBuffer<VertexPosNormal>> m_vbuffer;

	XMFLOAT4 m_color;

public:
	Block(Shader * s, IndexBuffer * ib, const PxVec3& halfSize, const PxTransform& t, PxMaterial * m, bool dynamic = true);
	~Block();

	void setColor(const XMFLOAT4& c);
};

