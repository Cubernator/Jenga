#include "Ground.h"
#include "Brick.h"
#include "utility.h"

Ground::Ground(Shader * s, IndexBuffer * ib, const PxVec3& halfSize, const PxTransform& t, PxMaterial * m)
{
	m_vbuffer.reset(createCuboidBuffer(halfSize.x, halfSize.y, halfSize.z));
	m_renderer.reset(new MeshRenderer(this, s, m_vbuffer.get(), ib));
	m_renderer->createConstantBuffer<XMFLOAT4>();
	m_renderer->updateConstantBuffer(XMFLOAT4(.6f, .6f, .6f, 1.f));

	PxRigidActor * actor;

	actor = PxCreateStatic(*physics, t, PxBoxGeometry(halfSize), *m);

	setActor(actor);
	m_transform.reset(new PhysicsTransform(this, actor));

	setRenderer(m_renderer.get());
	setTransform(m_transform.get());

	setCollisionCallbackFlags(ENTER);
}

Ground::~Ground()
{
	getActor()->release();
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
