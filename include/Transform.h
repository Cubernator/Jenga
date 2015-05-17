#pragma once

#include "PhysX.h"
#include "Component.h"

class Transform : public Component
{
public:
	Transform(GameObject * parent) : Component(parent) { }
	virtual ~Transform() { }

	virtual PxMat44 getMatrix() const = 0;

	virtual PxTransform getTransform() const
	{
		return PxTransform(getPosition(), getRotation());
	}

	virtual PxVec3 getPosition() const = 0;
	virtual PxQuat getRotation() const = 0;

	virtual void setPosition(const PxVec3& v) = 0;
	virtual void setRotation(const PxQuat& q) = 0;

	virtual void setTransform(const PxTransform& t)
	{
		setPosition(t.p);
		setRotation(t.q);
	}
};

