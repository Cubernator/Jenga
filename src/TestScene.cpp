#include "TestScene.h"
#include "ObjectManager.h"
#include "Engine.h"

TestScene::TestScene(PxSceneDesc desc) : PhysicsScene(desc), m_camera(new Camera())
{
	m_shader.reset(new Shader(L"Diffuse_vs.cso", L"Diffuse_ps.cso"));

	UINT16 indices[] = {
		0, 1, 2,
		3, 2, 1,
		4, 6, 5,
		7, 5, 6,
		8, 9, 10,
		11, 10, 9,
		12, 13, 14,
		15, 14, 13,
		16, 17, 18,
		19, 18, 17,
		20, 21, 22,
		23, 22, 21
	};

	m_blockIndices.reset(new IndexBuffer(indices, 36));

	m_material = physics->createMaterial(0.6f, 0.6f, 0.1f);
	PxTransform objTrans(PxVec3(0.f, 5.f, 0.f), PxQuat(-40 * PxPi / 180.0f, PxVec3(0, 1, 0)));
	m_obj.reset(new Block(m_shader.get(), m_blockIndices.get(), PxVec3(0.075f, 0.015f, 0.025f), objTrans, m_material));
	m_obj->setColor(XMFLOAT4(0.f, 1.f, .5f, 1.f));
	m_ground.reset(new Block(m_shader.get(), m_blockIndices.get(), PxVec3(10.f, 0.1f, 10.f), PxTransform(PxVec3(0, 0, 0)), m_material, false));

	m_camera->getTransform()->setPosition(PxVec3(0, 0.2f, -0.3f));

	Light l;
	XMStoreFloat3(&l.direction, XMVector3Normalize(XMVectorSet(-0.8f, -1.0f, 0.4f, 0.0f)));
	l.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	l.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	objects->setLight(l);
	objects->setCamera(m_camera.get());
	objects->addObject(m_obj.get());
	objects->addObject(m_ground.get());

	getPhysXObj()->addActor(*m_obj->getActor());
	getPhysXObj()->addActor(*m_ground->getActor());
}

TestScene::~TestScene()
{
	getPhysXObj()->removeActor(*m_obj->getActor());
	getPhysXObj()->removeActor(*m_ground->getActor());
	objects->removeObject(m_obj.get());
	objects->removeObject(m_ground.get());
	m_material->release();
}
