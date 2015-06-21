#pragma once
#include "PhysicsScene.h"

#include "Tower.h"
#include "Ground.h"
#include "Camera.h"
#include "DebugArrow.h"
#include "DebugPlane.h"

#include <vector>
#include <memory>

class MainScene : public PhysicsScene
{
private:
	std::unique_ptr<Ground> m_ground;
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Shader> m_shader, m_debugShader;
	std::unique_ptr<IndexBuffer> m_brickIndices;

	std::unique_ptr<Tower> m_tower;

	std::unique_ptr<DebugArrow> m_springVisualizer;
	std::unique_ptr<DebugPlane> m_planeVisualizer;

	px_ptr<PxD6Joint> m_spring;

	PxVec3 m_planeOrigin, m_springPos;
	float m_maxSpringDist;
	bool m_controlMode, m_showDebug;

	Brick * m_pickedBrick;

	float m_camX, m_camY, m_camDist, m_camYAngle;
	float m_xSens, m_ySens;

	void update() final;

	void tryPickBrick();
	void updateSpringPos();
	void updateCamPos();
	void releaseBrick();
	void setCamPos();

public:
	MainScene(PxSceneDesc desc);
	~MainScene();
};

