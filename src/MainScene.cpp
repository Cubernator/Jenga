#include "MainScene.h"
#include "Input.h"
#include "ObjectManager.h"
#include "utility.h"

MainScene::MainScene(PxSceneDesc desc) : PhysicsScene(desc), 
m_camX(30), m_camY(13.5f), m_camDist(30), m_camYAngle(20.0f), m_xSens(5), m_ySens(1.f), m_pulledBrick(nullptr), m_spring(nullptr), m_controlMode(false)
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

	m_brickIndices.reset(new IndexBuffer(indices, 36));

	m_brickMat = physics->createMaterial(0.6f, 0.3f, 0.1f);
	m_groundMat = physics->createMaterial(0.4f, 0.8f, 0.5f);

	unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();

	m_tower.reset(new Tower(m_shader.get(), m_brickIndices.get(), m_brickMat, seed));
	for (std::unique_ptr<Brick>& b : m_tower->getBricks()) {
		objects->addObject(b.get());
		addObject(b.get());
	}

	m_ground.reset(new Ground(m_shader.get(), m_brickIndices.get(), PxVec3(100.f, 1.0f, 100.f), PxTransform(PxVec3(0, -1, 0)), m_groundMat));

	m_camera.reset(new Camera());
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
		// cast ray and pick brick, create spring joint
		PxVec3 pos, dir;
		m_camera->getPickingRay(input->getMouseX(), input->getMouseY(), pos, dir);
		PxQueryFilterData fd;
		fd.flags |= PxQueryFlag::eDYNAMIC;
		fd.flags &= ~PxQueryFlag::eSTATIC;
		PxRaycastBuffer hit;
		if (getPhysXObj()->raycast(pos, dir, 1000.0f, hit, PxHitFlag::eDEFAULT, fd)) {
			PxRigidActor * a = hit.block.actor;
			if (Brick * b = (Brick*)a->userData) {
				if (b->canPickUp() && ((!m_pulledBrick) || (b == m_pulledBrick) || m_pulledBrick->isAligned())) {
					if (m_pulledBrick) {
						if (m_pulledBrick->isAligned() && b != m_pulledBrick) {
							if (m_tower->attemptPutBrickOnTop(m_pulledBrick, false)) m_pulledBrick->setState(TOP);
						}
					}

					m_pulledBrick = b;
					m_pulledBrick->setState(PULLING);
					m_springOrigin = hit.block.position;
					m_controlMode = false;

					m_spring = PxD6JointCreate(*physics, nullptr, PxTransform(PxIdentity), a, PxTransform(a->getGlobalPose().transformInv(m_springOrigin)));

					m_spring->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
					m_spring->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
					m_spring->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
					m_spring->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
					m_spring->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
					m_spring->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

					PxD6JointDrive posDrive = PxD6JointDrive(2000.0f, 500.0f, PX_MAX_F32);

					m_spring->setDrive(PxD6Drive::eX, posDrive);
					m_spring->setDrive(PxD6Drive::eY, posDrive);
					m_spring->setDrive(PxD6Drive::eZ, posDrive);
					m_spring->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0.0f, 2000.0f, PX_MAX_F32));
				}
			}
		}
	}

	if (input->getMouseButtonDown(MBUTTON1) && m_spring) {

		if (input->getMouseButtonPressed(MBUTTON2)) {
			m_controlMode = !m_controlMode;
			m_springOrigin = m_springPos;
		}

		// update spring joint
		PxVec3 pos, dir, n;
		m_camera->getPickingRay(input->getMouseX(), input->getMouseY(), pos, dir);
		PxVec3 right(1, 0, 0), up(0, 1, 0), forward(0, 0, 1);
		PxQuat camRot = m_camera->getTransform()->getRotation();
		n = (camRot.rotate(forward));
		n = n.cross(up).cross(up);
		n.normalize();
		float b = dir.dot(n);
		assert(b != 0.0f);
		m_springPos = pos + dir * (n.dot(m_springOrigin - pos) / b);

		if (m_controlMode) {
			right = camRot.rotate(right);
			right.normalize();
			m_springPos = m_springOrigin + PxQuat(toRadf(90.0f), right).rotate(m_springPos - m_springOrigin);
		}

		PxRigidDynamic * rd = (PxRigidDynamic*)m_pulledBrick->getActor();
		m_spring->setDrivePosition(PxTransform(m_springPos));
		rd->wakeUp();
	} else {
		if (input->getMouseButtonDown(MBUTTON2)) {
			// control camera
			m_camX += input->getMouseDeltaX() * engine->getDelta() * m_xSens;
			m_camY += input->getMouseDeltaY() * engine->getDelta() * m_ySens;

			if (m_camX < 0.0f) m_camX += 360.0f;
			else if (m_camX >= 360.0f) m_camX -= 360.0f;
			m_camY = max(m_camY, 0.0f);
		}

		int md = input->getMouseWheelDelta();
		if (md != 0) {
			m_camDist = max(min(m_camDist - md * 2.0f, 100.0f), 6.0f);
		}

		if (m_pulledBrick && input->getKeyPressed('B')) {
			BrickState s = (m_pulledBrick->getRowIndex() == 0) ? BASE : TOWER;
			m_pulledBrick->setState(s);
			m_pulledBrick = nullptr;
		}
	}

	if (input->getMouseButtonReleased(MBUTTON1)) {
		if (m_pulledBrick) {
			if (m_tower->attemptPutBrickOnTop(m_pulledBrick, true)) {
				m_pulledBrick->setState(ALIGNED);
			} else if (m_tower->attemptPutBrickBack(m_pulledBrick)) {
				BrickState s = (m_pulledBrick->getRowIndex() == 0) ? BASE : TOWER;
				m_pulledBrick->setState(s);
				m_pulledBrick = nullptr;
			} else {
				m_pulledBrick->setState(SELECTED);
			}
		}
		if (m_spring) m_spring->release();
		m_spring = nullptr;
	}

	PxQuat rot(toRadf(m_camX), PxVec3(0, 1, 0));
	PxVec3 d = rot.rotate(PxVec3(0, 0, -m_camDist));
	rot *= PxQuat(toRadf(m_camYAngle), PxVec3(1, 0, 0));
	d.y = m_camY + m_camDist / tanf(m_camYAngle);

	Transform * t = m_camera->getTransform();
	t->setPosition(d);
	t->setRotation(rot);
}

MainScene::~MainScene()
{
	removeObject(m_ground.get());
	objects->removeObject(m_ground.get());

	for (std::unique_ptr<Brick>& b : m_tower->getBricks()) {
		removeObject(b.get());
		objects->removeObject(b.get());
	}

	if (m_spring) m_spring->release();

	m_groundMat->release();
	m_brickMat->release();
}
