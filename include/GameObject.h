#pragma once
#include "DirectX.h"
#include "Collision.h"

class Transform;
class Renderer;
class AudioSource;

class GameObject
{
public:
	enum CollisionCallbackFlags
	{
		NONE = 0,
		ENTER = 1<<0,
		STAY = 1<<1,
		EXIT = 1<<2
	};

private:
	Transform * m_transform;
	Renderer * m_renderer;
	AudioSource * m_audioSource;
	PxRigidActor * m_actor;


	CollisionCallbackFlags m_ccFlags;

	bool m_castShadow, m_isGeometry;

	virtual void update() { }

	virtual void onCollisionEnter(const Collision& collision) { }
	virtual void onCollisionStay(const Collision& collision) { }
	virtual void onCollisionExit(const Collision& collision) { }

	friend class ObjectManager;
	friend class PhysicsInterface;

protected:
	void setTransform(Transform * t);
	void setRenderer(Renderer * r);
	void setAudioSource(AudioSource * s);
	void setActor(PxRigidActor * a);

public:
	GameObject();
	virtual ~GameObject() = 0;

	GameObject(const GameObject& other) = delete;
	GameObject& operator=(const GameObject& other) = delete;

	GameObject(GameObject&& other) = delete;
	GameObject& operator=(GameObject&& other) = delete;

	virtual void getLocalAABB(XMVECTOR& min, XMVECTOR& max) const;

	Transform * getTransform();
	const Transform * getTransform() const;

	Renderer * getRenderer();
	const Renderer * getRenderer() const;

	AudioSource * getAudioSource();
	const AudioSource * getAudioSource() const;

	PxRigidActor * getActor();
	const PxRigidActor * getActor() const;

	CollisionCallbackFlags getCollisionCallbackFlags() const;
	void setCollisionCallbackFlags(CollisionCallbackFlags f);

	bool getCastShadow() const;
	void setCastShadow(bool v);

	bool getIsGeometry() const;
	void setIsGeometry(bool v);
};

