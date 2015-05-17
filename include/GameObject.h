#pragma once

#include "Transform.h"
#include "Renderer.h"

class GameObject
{
private:
	Transform * m_transform;
	Renderer * m_renderer;

	virtual void update() { }

	friend class ObjectManager;

protected:
	void setTransform(Transform * t);
	void setRenderer(Renderer * r);

public:
	virtual ~GameObject() = 0;

	Transform * getTransform();
	const Transform * getTransform() const;

	Renderer * getRenderer();
	const Renderer * getRenderer() const;
};

