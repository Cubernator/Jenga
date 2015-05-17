#pragma once
#include "Transform.h"
class PhysicsTransform : public Transform
{
private:
	PxRigidActor * const m_actor;

public:
	PhysicsTransform(GameObject * parent, PxRigidActor * actor);

	PxMat44 getMatrix() const final;

	PxTransform getTransform() const final;

	PxVec3 getPosition() const final;
	PxQuat getRotation() const final;

	void setPosition(const PxVec3& v) final;
	void setRotation(const PxQuat& q) final;

	void setTransform(const PxTransform& t) final;
};

