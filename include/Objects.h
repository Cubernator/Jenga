#pragma once
#include "GameObject.h"
#include <vector>

class ObjectManager
{
private:
	std::vector<GameObject*> m_objects;

	XMFLOAT4 m_sceneAABBMin, m_sceneAABBMax;

	void computeAABB();

	friend class GraphicsInterface;
	friend class AudioInterface;

public:
	ObjectManager();

	void update();

	void add(GameObject * obj);
	void remove(GameObject * obj);

	void getSceneAABB(XMVECTOR& min, XMVECTOR& max) const;
};

extern ObjectManager * objects;

