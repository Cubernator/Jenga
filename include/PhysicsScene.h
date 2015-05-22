#pragma once

#include "Scene.h"
#include "PhysX.h"
#include "GameObject.h"

class PhysicsScene : public Scene
{
private:
	PxScene * m_physXScene;

public:
	PhysicsScene(PxSceneDesc sceneDesc);
	~PhysicsScene();

	PxScene * getPhysXObj();
	const PxScene * getPhysXObj() const;

	void addActor(PxActor * a);
	void removeActor(PxActor * a);

	void addObject(GameObject * obj);
	void removeObject(GameObject * obj);
};

