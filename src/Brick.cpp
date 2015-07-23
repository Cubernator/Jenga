#include "Brick.h"
#include "Engine.h"
#include "utility.h"
#include "Tower.h"

#define MAX_NUMBER_SOUNDS 20

Brick::Brick(Tower * tower, Shader * s, IndexBuffer * ib, const PxVec3& halfSize, const PxTransform& t, PxMaterial * m, float difficulty)
	: m_tower(tower), m_halfSize(halfSize), m_rowIndex(0), m_hasPowerup(false), m_powerupId(-1), m_difficulty(difficulty), m_highlight(false)
{
	m_actor.reset(PxCreateDynamic(*physics, t, PxBoxGeometry(halfSize), *m, 4.8f));
	setActor(m_actor.get());

	m_transform.reset(new PhysicsTransform(this));
	setTransform(m_transform.get());

	m_material = {
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		7.0f,
		0.4f
	};

	m_vbuffer.reset(createBrickBuffer(halfSize.x, halfSize.y, halfSize.z));
	m_renderer.reset(new MeshRenderer(this, s, m_vbuffer.get(), ib));
	m_renderer->createConstantBuffer(m_material);
	setRenderer(m_renderer.get());

	m_audioSource.reset(new AudioSource(this));
	setAudioSource(m_audioSource.get());

	setCollisionCallbackFlags(ENTER);

	setState(TOWER);
}

void Brick::allocateSoundInstance(SoundEffect * effect)
{
	m_audioSource->setClip(effect);
}

void Brick::getLocalAABB(XMVECTOR& min, XMVECTOR& max) const
{
	XMVECTOR hs = XMLoadFloat3((XMFLOAT3*)&m_halfSize);
	XMVectorSetW(hs, 1.0f);
	min = -hs;
	max = hs;
}

void Brick::onCollisionEnter(const Collision& collision)
{
	float force = 0.0f;
	float d = engine->getDelta();
	if (d > 0.0f) {
		float f = 1.0f / d;
		for (const PxContactPairPoint& p : collision.contacts) {
			force += p.impulse.magnitude() * f;
		}

		force /= collision.contacts.size();

		if (m_tower->getActiveSounds() < MAX_NUMBER_SOUNDS && !m_audioSource->getSoundState()) {
			if (SoundEffect * eff = m_tower->getRandomBrickSound(force)) {
				m_audioSource->setClip(eff);
				m_audioSource->play();
				m_tower->soundPlayed();
			}
		}
	}
}

void Brick::setHighlight(bool h)
{
	m_highlight = h;
	setColor(getStateColor());
}

void Brick::setColor(const XMFLOAT4& c)
{
	m_material.diffuse = c;
	m_renderer->updateConstantBuffer(m_material);
}

XMFLOAT4 Brick::getStateColor() const
{
	XMFLOAT4 defCol(1.0f, 1.0f, 1.0f, 1.0f);
	if (m_highlight) {
		XMStoreFloat4(&defCol, XMVectorLerp(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), m_difficulty));
	} else if (m_hasPowerup) {
		defCol = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	}

	switch (m_state) {
	case TOWER:
		return defCol;
	case BASE:
		return defCol;
	case TOP:
		return defCol;
	case PULLING:
		return XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	case SELECTED:
		return XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f);
	case ALIGNED:
		return XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f);
	case FAULTED:
		return defCol;
	default:
		return defCol;
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

bool Brick::hasPowerup() const
{
	return m_hasPowerup;
}

unsigned int Brick::getPowerupId() const
{
	return m_powerupId;
}

void Brick::setPowerup(bool hasPowerup, unsigned int powerupId)
{
	m_hasPowerup = hasPowerup;
	m_powerupId = powerupId;

	setColor(getStateColor());
}

float Brick::getDifficulty() const
{
	return m_difficulty;
}
