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

	m_vbuffer.reset(createCuboidBuffer(halfSize.x, halfSize.y, halfSize.z));
	m_renderer.reset(new MeshRenderer(this, s, m_vbuffer.get(), ib));
	m_renderer->createConstantBuffer(XMFLOAT4(.6f, .6f, .6f, 1.f));
	setRenderer(m_renderer.get());

	setCollisionCallbackFlags(ENTER);
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
