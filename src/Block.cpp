#include "Block.h"
#include "Engine.h"
#include "utility.h"


Block::Block(Shader * s, IndexBuffer * ib, const PxVec3& halfSize, const PxTransform& t, PxMaterial * m, bool dynamic)
{
	m_vbuffer.reset(createCuboidBuffer(halfSize.x, halfSize.y, halfSize.z));
	m_renderer.reset(new MeshRenderer(this, s, m_vbuffer.get(), ib));
	m_renderer->createConstantBuffer<XMFLOAT4>();

	setColor(XMFLOAT4(.6f, .6f, .6f, 1.f));

	if (dynamic)
		m_actor = PxCreateDynamic(*physics, t, PxBoxGeometry(halfSize), *m, 480.f);
	else
		m_actor = PxCreateStatic(*physics, t, PxBoxGeometry(halfSize), *m);

	m_transform.reset(new PhysicsTransform(this, m_actor));

	setRenderer(m_renderer.get());
	setTransform(m_transform.get());
}

Block::~Block()
{
	m_actor->release();
}

PxRigidActor * Block::getActor()
{
	return m_actor;
}

void Block::setColor(const XMFLOAT4& c)
{
	m_color = c;
	m_renderer->updateConstantBuffer(m_color);
}
