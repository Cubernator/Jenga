#pragma once

#include "Transform.h"
#include "Renderer.h"

class GameObject
{
private:
	Transform * m_transform;
	Renderer * m_renderer;
	PxActor * m_actor;

	virtual void update() { }

	friend class ObjectManager;

protected:
	void setTransform(Transform * t);
	void setRenderer(Renderer * r);
	void setActor(PxActor * a);

public:
	GameObject();
	virtual ~GameObject() = 0;

	Transform * getTransform();
	const Transform * getTransform() const;

	Renderer * getRenderer();
	const Renderer * getRenderer() const;

	PxActor * getActor();
	const PxActor * getActor() const;
};

