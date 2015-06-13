#include "PhysicsScene.h"


PhysicsScene::PhysicsScene(PxSceneDesc sceneDesc)
{
	m_physXScene = physics->createScene(sceneDesc);
}


PhysicsScene::~PhysicsScene()
{
	m_physXScene->release();
}

PxScene * PhysicsScene::getPxScene()
{
	return m_physXScene;
}

const PxScene * PhysicsScene::getPxScene() const
{
	return m_physXScene;
}

void PhysicsScene::addActor(PxActor * a)
{
	m_physXScene->addActor(*a);
}

void PhysicsScene::removeActor(PxActor * a)
{
	m_physXScene->removeActor(*a);
}

void PhysicsScene::addObject(GameObject * obj)
{
	if (PxActor * a = obj->getActor()) addActor(a);
}

void PhysicsScene::removeObject(GameObject * obj)
{
	if (PxActor * a = obj->getActor()) removeActor(a);
}
