#pragma once
#include "GameObject.h"
#include <vector>

class ObjectManager
{
private:
	std::vector<GameObject*> m_objects;

	friend class GraphicsInterface;

public:
	ObjectManager();

	void update();

	void add(GameObject * obj);
	void remove(GameObject * obj);
};

extern ObjectManager * objects;

