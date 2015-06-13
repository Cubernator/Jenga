#include "GameObject.h"

GameObject::GameObject() : m_transform(nullptr), m_renderer(nullptr), m_actor(nullptr) { }
GameObject::~GameObject() { }

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

