#include "Objects.h"
#include <algorithm>

ObjectManager * objects;

ObjectManager::ObjectManager()
{
	objects = this;
}

void ObjectManager::update()
{
	for (GameObject * obj : m_objects) {
		obj->update();
	}
}

void ObjectManager::add(GameObject * obj)
{
	m_objects.push_back(obj);
}

void ObjectManager::remove(GameObject * obj)
{
	m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), obj), m_objects.end());
}
