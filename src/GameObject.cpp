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

void GameObject::setActor(PxActor * a)
{
	m_actor = a;
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

PxActor * GameObject::getActor()
{
	return m_actor;
}

const PxActor * GameObject::getActor() const
{
	return m_actor;
}
