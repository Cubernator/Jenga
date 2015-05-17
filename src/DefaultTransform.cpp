#include "DefaultTransform.h"


DefaultTransform::DefaultTransform(GameObject * parent) : Transform(parent), m_internal(PxIdentity) { }

PxMat44 DefaultTransform::getMatrix() const
{
	return PxMat44(m_internal);
}

PxTransform DefaultTransform::getTransform() const
{
	return m_internal;
}

PxVec3 DefaultTransform::getPosition() const
{
	return m_internal.p;
}
PxQuat DefaultTransform::getRotation() const
{
	return m_internal.q;
}

void DefaultTransform::setPosition(const PxVec3& v)
{
	m_internal.p = v;
}
void DefaultTransform::setRotation(const PxQuat& q)
{
	m_internal.q = q;
}

void DefaultTransform::setTransform(const PxTransform& t)
{
	m_internal = t;
}
