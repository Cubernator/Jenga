#include "Brick.h"
#include "Engine.h"
#include "utility.h"


Brick::Brick(Shader * s, IndexBuffer * ib, const PxVec3& halfSize, const PxTransform& t, PxMaterial * m) : m_rowIndex(0)
{
	m_vbuffer.reset(createCuboidBuffer(halfSize.x, halfSize.y, halfSize.z));
	m_renderer.reset(new MeshRenderer(this, s, m_vbuffer.get(), ib));
	m_renderer->createConstantBuffer<XMFLOAT4>();

	PxRigidActor * actor;

	actor = PxCreateDynamic(*physics, t, PxBoxGeometry(halfSize), *m, 4.8f);

	setActor(actor);
	m_transform.reset(new PhysicsTransform(this, actor));

	setRenderer(m_renderer.get());
	setTransform(m_transform.get());

	setState(TOWER);
}

Brick::~Brick()
{
	getActor()->release();
}

void Brick::setColor(const XMFLOAT4& c)
{
	m_color = c;
	m_renderer->updateConstantBuffer(m_color);
}

XMFLOAT4 Brick::getStateColor() const
{
	switch (m_state) {
	case TOWER:
		return XMFLOAT4(0.96f, 0.6f, 0.278f, 1.0f);
	case BASE:
		return XMFLOAT4(0.278f, 0.74f, 0.96f, 1.0f);
	case TOP:
		return XMFLOAT4(0.96f, 0.278f, 0.278f, 1.0f);
	case PULLING:
		return XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	case SELECTED:
		return XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	case ALIGNED:
		return XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	case FAULTED:
		return XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	default:
		return XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	}
}

void Brick::setState(BrickState state)
{
	m_state = state;
	setColor(getStateColor());
}

unsigned int Brick::getRowIndex() const
{
	return m_rowIndex;
}

void Brick::setRowIndex(unsigned int i)
{
	m_rowIndex = i;
}

unsigned int Brick::getBrickIndex() const
{
	return m_brickIndex;
}

void Brick::setBrickIndex(unsigned int i)
{
	m_brickIndex = i;
}

bool Brick::canTouchGround() const
{
	return m_state == BASE || m_state == PULLING || m_state == SELECTED;
}

bool Brick::canPickUp() const
{
	return m_state != TOP && m_state != FAULTED;
}

bool Brick::isAligned() const
{
	return m_state == ALIGNED;
}
