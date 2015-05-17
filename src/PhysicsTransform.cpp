#include "PhysicsTransform.h"


PhysicsTransform::PhysicsTransform(GameObject * parent, PxRigidActor * actor) : Transform(parent), m_actor(actor) { }

PxMat44 PhysicsTransform::getMatrix() const
{
	return PxMat44(m_actor->getGlobalPose());
}

PxTransform PhysicsTransform::getTransform() const
{
	return m_actor->getGlobalPose();
}

PxVec3 PhysicsTransform::getPosition() const
{
	return m_actor->getGlobalPose().p;
}
PxQuat PhysicsTransform::getRotation() const
{
	return m_actor->getGlobalPose().q;
}

void PhysicsTransform::setPosition(const PxVec3& v)
{
	PxTransform t = m_actor->getGlobalPose();
	t.p = v;
	m_actor->setGlobalPose(t);
}
void PhysicsTransform::setRotation(const PxQuat& q)
{
	PxTransform t = m_actor->getGlobalPose();
	t.q = q;
	m_actor->setGlobalPose(t);
}

void PhysicsTransform::setTransform(const PxTransform& t)
{
	m_actor->setGlobalPose(t);
}