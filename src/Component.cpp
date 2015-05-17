#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject * parent) : m_parent(parent), m_enabled(true) { }
Component::~Component() { }

GameObject * Component::getParent()
{
	return m_parent;
}
const GameObject * Component::getParent() const
{
	return m_parent;
}

bool Component::getEnabled() const
{
	return m_enabled;
}
void Component::setEnabled(bool e)
{
	m_enabled = e;
}
