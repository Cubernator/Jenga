#pragma once

#include "Scene.h"
#include "PhysX.h"

class PhysicsScene : public Scene
{
private:
	PxScene * m_physXScene;

public:
	PhysicsScene(PxSceneDesc sceneDesc);
	~PhysicsScene();

	PxScene * getPhysXObj();
	const PxScene * getPhysXObj() const;
};

