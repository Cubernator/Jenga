#include "GameObject.h"

GameObject::~GameObject() { }

void GameObject::setTransform(Transform * t)
{
	m_transform = t;
}

void GameObject::setRenderer(Renderer * r)
{
	m_renderer = r;
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
