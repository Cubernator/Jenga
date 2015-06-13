#pragma once

#include "Transform.h"
#include "Renderer.h"
#include "Physics.h"

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
	PxRigidActor * m_actor;

	CollisionCallbackFlags m_ccFlags;

	virtual void update() { }

	virtual void onCollisionEnter(const Collision& collision) { }
	virtual void onCollisionStay(const Collision& collision) { }
	virtual void onCollisionExit(const Collision& collision) { }

	friend class ObjectManager;
	friend class PhysicsInterface;

protected:
	void setTransform(Transform * t);
	void setRenderer(Renderer * r);
	void setActor(PxRigidActor * a);

public:
	GameObject();
	virtual ~GameObject() = 0;

	Transform * getTransform();
	const Transform * getTransform() const;

	Renderer * getRenderer();
	const Renderer * getRenderer() const;

	PxRigidActor * getActor();
	const PxRigidActor * getActor() const;

	CollisionCallbackFlags getCollisionCallbackFlags() const;
	void setCollisionCallbackFlags(CollisionCallbackFlags f);
};

