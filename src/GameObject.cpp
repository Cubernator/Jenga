#include "GameObject.h"

GameObject::GameObject() : m_transform(nullptr), m_renderer(nullptr), m_actor(nullptr), m_castShadow(true), m_isGeometry(true) { }
GameObject::~GameObject() { }

void GameObject::getLocalAABB(XMVECTOR& min, XMVECTOR& max) const
{
	min = XMVectorZero();
	max = XMVectorZero();
}

void GameObject::setTransform(Transform * t)
{
	m_transform = t;
}

void GameObject::setRenderer(Renderer * r)
{
	m_renderer = r;
}

void GameObject::setActor(PxRigidActor * a)
{
	m_actor = a;
	m_actor->userData = this;
}

Transform * GameObject::getTransform()
{
	return m_transform;
}

const Transform * GameObject::getTransform() const
{
	return m_transform;
}

Renderer * GameObject::getRenderer()
{
	return m_renderer;
}

const Renderer * GameObject::getRenderer() const
{
	return m_renderer;
}

PxRigidActor * GameObject::getActor()
{
	return m_actor;
}

const PxRigidActor * GameObject::getActor() const
{
	return m_actor;
}

GameObject::CollisionCallbackFlags GameObject::getCollisionCallbackFlags() const
{
	return m_ccFlags;
}

void GameObject::setCollisionCallbackFlags(CollisionCallbackFlags f)
{
	m_ccFlags = f;
}

bool GameObject::getCastShadow() const
{
	return m_castShadow;
}

void GameObject::setCastShadow(bool v)
{
	m_castShadow = v;
}

bool GameObject::getIsGeometry() const
{
	return m_isGeometry;
}

void GameObject::setIsGeometry(bool v)
{
	m_isGeometry = v;
}

