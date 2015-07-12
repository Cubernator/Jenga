#include "MainScene.h"
#include "MainMenu.h"
#include "Input.h"
#include "Graphics.h"
#include "Objects.h"
#include "utility.h"
#include "ScoreFile.h"

MainScene::MainScene(PxSceneDesc desc, bool specialMode, unsigned int seed) : PhysicsScene(desc), m_specialMode(specialMode), m_seed(seed),
m_camX(30), m_camY(13.5f), m_camDist(30), m_camYANormal(20.0f), m_camYASteep(40.0f), m_camYAngle(20.0f), m_xSens(5), m_ySens(1.f),
m_pickedBrick(nullptr), m_controlMode(false), m_showDebug(false), m_maxSpringDist(50.0f),
m_paused(false), m_roundOver(false), m_togglePause(false), m_restart(false), m_backToMain(false)
{
	m_groundShader.reset(new Shader(L"TexSpecular"));
	m_debugShader.reset(new Shader(L"VertexColor"));

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

	m_tower.reset(new Tower(this, m_groundShader.get(), m_brickIndices.get()));

	m_ground.reset(new Ground(this, m_groundShader.get(), m_brickIndices.get()));

	m_camera.reset(new Camera());
	m_camera->setFOV(60.0f);
	m_camera->setBackColor(XMFLOAT4(0.f, 0.2f, 0.4f, 1.f));

	Light l;
	XMStoreFloat3(&l.direction, XMVector3Normalize(XMVectorSet(-0.8f, -1.0f, 0.4f, 0.0f)));
	l.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	l.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_springVisualizer.reset(new DebugArrow(m_debugShader.get(), XMFLOAT4(1, 1, 1, 1)));
	m_springVisualizer->getRenderer()->setEnabled(false);

	m_planeVisualizer.reset(new DebugPlane(200.0f, 100, m_debugShader.get(), XMFLOAT4(1, 1, 1, 1)));
	m_planeVisualizer->getRenderer()->setEnabled(false);

	getPxScene()->setGravity(PxVec3(0, -9.81f, 0));

	graphics->setLight(l);
	graphics->setCamera(m_camera.get());
	objects->add(m_ground.get());
	objects->add(m_springVisualizer.get());
	objects->add(m_planeVisualizer.get());

	addObject(m_ground.get());

	gui->loadBitmap(L"assets\\images\\pause_white.png", &m_pauseSymbol);
	gui->loadBitmap(L"assets\\images\\play_white.png", &m_playSymbol);

	GUIButtonStyle pbs = { GUIStyleState(m_pauseSymbol.Get()) };
	m_pauseButton.reset(new GUIButton({ 10, 10, 42, 42 }, L"", pbs));
	m_pauseButton->setCallback([this] { togglePause(); });
	m_pauseButton->setDepth(-100);
	gui->add(m_pauseButton.get());

	m_powerupManager.reset(new PowerupManager(this));

	m_scoreCounter.reset(new ScoreCounter());

	setCamPos();
}

MainScene::~MainScene()
{
	graphics->setCamera(nullptr);

	gui->remove(m_pauseButton.get());

	removeObject(m_ground.get());
	objects->remove(m_ground.get());
	objects->remove(m_springVisualizer.get());
	objects->remove(m_planeVisualizer.get());
}

ScoreCounter * MainScene::getScoreCounter()
{
	return m_scoreCounter.get();
}

PowerupManager * MainScene::getPowerupManager()
{
	return m_powerupManager.get();
}

bool MainScene::getSpecialMode() const
{
	return m_specialMode;
}

unsigned int MainScene::getSeed() const
{
	return m_seed;
}

void MainScene::brickFaulted()
{
	if (!m_roundOver) {
		m_roundOver = true;
		m_resultsMenu.reset(new ResultsMenu(this));
		gui->remove(m_pauseButton.get());
	}
}

void MainScene::togglePause()
{
	m_togglePause = true;
}

void MainScene::restart()
{
	m_restart = true;
}

void MainScene::backToMainMenu()
{
	m_backToMain = true;
}

void MainScene::update()
{
	if (m_togglePause) {
		m_togglePause = false;

		m_paused = !m_paused;

		m_pauseButton->setStyle({ GUIStyleState(m_paused ? m_playSymbol.Get() : m_pauseSymbol.Get()) });
		m_pauseMenu.reset(m_paused ? new PauseMenu(this) : nullptr);
	}

	if (m_restart) {
		m_restart = false;
		m_seedPrompt.reset(new SeedPrompt(m_seed));

		if (m_resultsMenu) m_resultsMenu->hideMenu();
		if (m_pauseMenu) m_pauseMenu->hideMenu();
		gui->remove(m_pauseButton.get());
	}

	if (m_seedPrompt) {
		m_seedPrompt->update();

		if (m_seedPrompt->isDone()) {
			bool sm = m_specialMode;
			unsigned int seed = m_seedPrompt->getSeed();
			engine->enterScene<MainScene>(sm, seed); // restart
			return;
		}
	}

	if (m_backToMain) {
		engine->enterScene<MainMenu>();
		return;
	}

	if (input->getKeyPressed('D')) {
		m_showDebug = !m_showDebug;
		if (!m_showDebug) {
			m_springVisualizer->getRenderer()->setEnabled(false);
			m_planeVisualizer->getRenderer()->setEnabled(false);
		} else if (m_spring) {
			m_springVisualizer->getRenderer()->setEnabled(true);
			m_planeVisualizer->getRenderer()->setEnabled(true);
		}
	}

	if (!(m_paused || m_roundOver)) {
		if (input->getMouseButtonPressed(MBUTTON1)) {
			tryPickBrick();
		}

		if (input->getMouseButtonDown(MBUTTON1) && m_spring) {
			updateSpringPos();
		} else {
			updateCamPos();

			/* NOTE: this is a cheat! remove in release version! */
			if (m_pickedBrick && input->getKeyPressed('B')) {
				BrickState s = (m_pickedBrick->getRowIndex() == 0) ? BASE : TOWER;
				m_pickedBrick->setState(s);
				m_pickedBrick = nullptr;
			}
		}
	}

	if (input->getMouseButtonReleased(MBUTTON1)) {
		releaseBrick();
	}

	setCamPos();

	if (m_spring) {
		PxVec3 lp = m_spring->getLocalPose(PxJointActorIndex::eACTOR1).p;
		lp = m_pickedBrick->getTransform()->getTransform().transform(lp);
		m_springVisualizer->setProps(lp, m_springPos);
	}

	m_tower->update();
	m_powerupManager->update();
	m_scoreCounter->update();

	if (m_resultsMenu) m_resultsMenu->update();
}

void MainScene::tryPickBrick()
{
	// cast ray and pick brick, create spring joint
	PxVec3 pos, dir;
	m_camera->getPickingRay(input->getMouseX(), input->getMouseY(), pos, dir);
	PxQueryFilterData fd;
	fd.flags |= PxQueryFlag::eDYNAMIC;
	fd.flags &= ~PxQueryFlag::eSTATIC;
	PxRaycastBuffer hit;
	if (getPxScene()->raycast(pos, dir, 1000.0f, hit, PxHitFlag::eDEFAULT, fd)) {
		PxRigidActor * a = hit.block.actor;
		if (Brick * b = (Brick*)a->userData) {
			if (b->canPickUp()) {
				if (m_pickedBrick) {
					if (b != m_pickedBrick) {
						if (m_pickedBrick->isAligned()) {
							float accuracy;
							if (m_tower->attemptPutBrickOnTop(m_pickedBrick, accuracy, false)) {
								m_pickedBrick->setState(TOP);

								if (m_pickedBrick->hasPowerup()) {
									m_powerupManager->collectPowerup(m_pickedBrick->getPowerupId());
									m_pickedBrick->setPowerup(false, -1);
								}

								m_scoreCounter->brickPlaced(m_pickedBrick, accuracy);
							}
						} else {
							return;
						}
					}
				}

				m_pickedBrick = b;
				m_pickedBrick->setState(PULLING);
				m_planeOrigin = hit.block.position;
				m_controlMode = false;

				// set up spring joint
				m_spring.reset(PxD6JointCreate(*physics, nullptr, PxTransform(PxIdentity), a, PxTransform(a->getGlobalPose().transformInv(m_planeOrigin))));

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

				if (m_showDebug) {
					m_springVisualizer->getRenderer()->setEnabled(true);
					m_planeVisualizer->getRenderer()->setEnabled(true);
				}
			}
		}
	}
}

void MainScene::updateSpringPos()
{
	if (input->getMouseButtonPressed(MBUTTON2)) {
		// switch control modes
		m_controlMode = !m_controlMode;
		m_planeOrigin = m_springPos;

		m_camYAngle = m_controlMode ? m_camYASteep : m_camYANormal;

		if (m_controlMode) {
			float diff = m_camY + m_camDist * tanf(toRadf(m_camYAngle)) - m_planeOrigin.y - 15.0f;
			if (diff < 0.0f) {
				m_camY -= diff;
			}
		}

		setCamPos();

		int x, y;
		m_camera->worldToScreen(m_springPos, x, y);
		input->setMousePos(x, y);
	}

	// update spring joint
	PxVec3 pos, dir, n;
	m_camera->getPickingRay(input->getMouseX(), input->getMouseY(), pos, dir);
	PxVec3 right(1, 0, 0), up(0, 1, 0), forward(0, 0, 1);
	PxQuat camRot = m_camera->getTransform()->getRotation();

	if (m_controlMode) {
		n = up;
	} else {
		n = (camRot.rotate(forward));
		n = n.cross(up).cross(up);
		n.normalize();
	}

	// project cursor onto plane
	float nd = n.dot(m_planeOrigin - pos);
	float ddn = dir.dot(n);
	if (ddn < -0.01f) { // do this to prevent overflow and wrap-around
		m_springPos = pos + dir * (nd / ddn);
	}

	Transform * pt = m_planeVisualizer->getTransform();

	if (m_controlMode) {
		pt->setPosition(PxVec3(0, m_planeOrigin.y, 0));
		pt->setRotation(PxQuat(PxIdentity));
	} else {
		PxQuat pr = fromToRotation(forward, n) * PxQuat(toRadf(90.0f), right);
		PxVec3 cf = camRot.rotate(forward);
		pt->setPosition(pos + cf * (nd / cf.dot(n)));
		pt->setRotation(pr);
	}

	// limit the horizontal distance of the spring to the tower
	PxVec3 p = up.dot(m_springPos) * up;
	PxVec3 ps = m_springPos - p;
	float psm = ps.magnitude();
	if (psm > m_maxSpringDist) {
		m_springPos = p + (ps * (m_maxSpringDist / psm));
	}

	PxRigidDynamic * rd = (PxRigidDynamic*)m_pickedBrick->getActor();
	m_spring->setDrivePosition(PxTransform(m_springPos));
	rd->wakeUp();
}

void MainScene::updateCamPos()
{
	if (input->getMouseButtonDown(MBUTTON2)) {
		// control camera
		m_camX += input->getMouseDeltaX() * engine->getRealDelta() * m_xSens;
		m_camY += input->getMouseDeltaY() * engine->getRealDelta() * m_ySens;

		if (m_camX < 0.0f) m_camX += 360.0f;
		else if (m_camX >= 360.0f) m_camX -= 360.0f;
		m_camY = max(m_camY, 0.0f);
	}

	int md = input->getMouseWheelDelta();
	if (md != 0) {
		m_camDist = max(min(m_camDist - md * 2.0f, 100.0f), 6.0f);
	}
}

void MainScene::releaseBrick()
{
	if (m_pickedBrick) {
		float acc;
		if (m_tower->attemptPutBrickOnTop(m_pickedBrick, acc, true)) {
			m_pickedBrick->setState(ALIGNED);
		} else if (m_tower->attemptPutBrickBack(m_pickedBrick)) {
			BrickState s = (m_pickedBrick->getRowIndex() == 0) ? BASE : TOWER;
			m_pickedBrick->setState(s);
			m_pickedBrick = nullptr;
		} else {
			m_pickedBrick->setState(SELECTED);
		}
	}
	m_spring.reset();

	m_springVisualizer->getRenderer()->setEnabled(false);
	m_planeVisualizer->getRenderer()->setEnabled(false);

	m_camYAngle = m_camYANormal;
}

void MainScene::setCamPos()
{
	PxQuat rot(toRadf(m_camX), PxVec3(0, 1, 0));
	PxVec3 d = rot.rotate(PxVec3(0, 0, -m_camDist));
	rot *= PxQuat(toRadf(m_camYAngle), PxVec3(1, 0, 0));
	d.y = m_camY + m_camDist * tan(toRadf(m_camYAngle));

	Transform * t = m_camera->getTransform();
	t->setPosition(d);
	t->setRotation(rot);
}
