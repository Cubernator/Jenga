#include "MainScene.h"
#include "Input.h"
#include "ObjectManager.h"

#include <random>
#include <chrono>

MainScene::MainScene(PxSceneDesc desc) : PhysicsScene(desc), m_camera(new Camera()),
m_camX(30), m_camY(13.5f), m_camDist(30), m_camYAngle(20.0f), m_xSens(5), m_ySens(1.f), m_pulledBlock(nullptr), m_spring(nullptr)
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

	m_blockMat = physics->createMaterial(0.6f, 0.6f, 0.1f);
	m_groundMat = physics->createMaterial(0.3f, 0.35f, 0.0f);

	XMFLOAT4 blockColor(0.f, 1.f, .5f, 1.f);
	PxVec3 blockHSize(3.75f, 0.75f, 1.25f);
	PxQuat r90(3.14159265f * 0.5f, PxVec3(0, 1, 0));
	float gap = 0.02f, maxVariance = 0.01f;

	std::default_random_engine generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 2; j++) {
			float y = ((i*2 + j)*2 + 1) * (blockHSize.y + gap);
			for (int k = 0; k < 3; k++) {
				float x = 0.0f, z = (k-1)*2 * (blockHSize.z + gap);

				PxTransform blockTrans(PxVec3(x, y, z));
				if (j > 0) {
					blockTrans.q = r90;
					blockTrans.p.x = z;
					blockTrans.p.z = 0.0f;
				}

				PxVec3 s = blockHSize;
				s.x += distribution(generator) * maxVariance;
				s.y += distribution(generator) * maxVariance;
				s.z += distribution(generator) * maxVariance;

				Block * b = new Block(m_shader.get(), m_blockIndices.get(), s, blockTrans, m_blockMat);
				b->setColor(blockColor);
				m_blocks.emplace_back(b);
				objects->addObject(b);
				addObject(b);
			}
		}
	}
	m_ground.reset(new Block(m_shader.get(), m_blockIndices.get(), PxVec3(100.f, 1.0f, 100.f), PxTransform(PxVec3(0, -1, 0)), m_groundMat, false));

	m_camera->setFOV(60.0f);

	Light l;
	XMStoreFloat3(&l.direction, XMVector3Normalize(XMVectorSet(-0.8f, -1.0f, 0.4f, 0.0f)));
	l.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	l.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	getPhysXObj()->setGravity(PxVec3(0, -9.81f, 0));

	objects->setLight(l);
	objects->setCamera(m_camera.get());
	objects->addObject(m_ground.get());

	addObject(m_ground.get());
}

void MainScene::update()
{
	if (input->getKeyPressed('R')) {
		engine->enterScene<MainScene>(); // restart
		return;
	}

	if (input->getMouseButtonPressed(MBUTTON1)) {
		// cast ray and pick block, create spring joint
		PxVec3 pos, dir;
		m_camera->getPickingRay(input->getMouseX(), input->getMouseY(), pos, dir);
		PxQueryFilterData fd;
		fd.flags |= PxQueryFlag::eDYNAMIC;
		fd.flags &= ~PxQueryFlag::eSTATIC;
		PxRaycastBuffer hit;
		if (getPhysXObj()->raycast(pos, dir, 100.0f, hit, PxHitFlag::eDEFAULT, fd)) {
			PxRigidActor * a = hit.block.actor;
			if (Block * b = (Block*)a->userData) {
				m_pulledBlock = b;
				m_springOrigin = hit.block.position;

				m_spring = PxD6JointCreate(*physics, nullptr, PxTransform(PxIdentity), a, PxTransform(m_springOrigin - a->getGlobalPose().p));

				m_spring->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
				m_spring->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
				m_spring->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
				m_spring->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
				m_spring->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
				m_spring->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

				PxD6JointDrive drive = PxD6JointDrive(1000.0f, 200.0f, PX_MAX_F32);
				m_spring->setDrive(PxD6Drive::eX, drive);
				m_spring->setDrive(PxD6Drive::eY, drive);
				m_spring->setDrive(PxD6Drive::eZ, drive);

				//m_spring->setDrivePosition(PxTransform(PxVec3(0, 50, 0)));
			}
		}
	}

	if (input->getMouseButtonDown(MBUTTON1) && m_spring) {
		// update spring joint
		PxVec3 pos, dir, n;
		m_camera->getPickingRay(input->getMouseX(), input->getMouseY(), pos, dir);
		n = (pos - m_springOrigin);
		n.normalize();
		PxVec3 up(0, 1, 0);
		n = n.cross(up).cross(up);
		float b = dir.dot(n);
		assert(b != 0.0f);
		PxVec3 springPos = pos + dir * (n.dot(m_springOrigin - pos) / b);
		m_spring->setDrivePosition(PxTransform(springPos));
		PxRigidDynamic * rd = (PxRigidDynamic*)m_pulledBlock->getActor();
		rd->wakeUp();
	}
	else if (input->getMouseButtonDown(MBUTTON2)) {
		// control camera
		m_camX += input->getMouseDeltaX() * engine->getDelta() * m_xSens;
		m_camY += input->getMouseDeltaY() * engine->getDelta() * m_ySens;

		if (m_camX < 0.0f) m_camX += 360.0f;
		else if (m_camX >= 360.0f) m_camX -= 360.0f;
		m_camY = max(m_camY, 0.0f);
	}

	if (input->getMouseButtonReleased(MBUTTON1)) {
		m_pulledBlock = nullptr;
		if (m_spring) m_spring->release();
		m_spring = nullptr;
	}

	float toRad = 3.14159265f / 180.0f;

	PxQuat rot(m_camX * toRad, PxVec3(0, 1, 0));
	PxVec3 d = rot.rotate(PxVec3(0, 0, -m_camDist));
	rot *= PxQuat(m_camYAngle * toRad, PxVec3(1, 0, 0));
	d.y = m_camY + m_camDist / tanf(m_camYAngle);

	Transform * t = m_camera->getTransform();
	t->setPosition(d);
	t->setRotation(rot);
}

MainScene::~MainScene()
{
	removeObject(m_ground.get());
	objects->removeObject(m_ground.get());

	for (std::unique_ptr<Block>& b : m_blocks) {
		removeObject(b.get());
		objects->removeObject(b.get());
	}

	if (m_spring) m_spring->release();

	m_groundMat->release();
	m_blockMat->release();
}
