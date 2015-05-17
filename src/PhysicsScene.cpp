#include "PhysicsScene.h"


PhysicsScene::PhysicsScene(PxSceneDesc sceneDesc)
{
	m_physXScene = physics->createScene(sceneDesc);
}


PhysicsScene::~PhysicsScene()
{
	m_physXScene->release();
}

PxScene * PhysicsScene::getPhysXObj()
{
	return m_physXScene;
}

const PxScene * PhysicsScene::getPhysXObj() const
{
	return m_physXScene;
}
