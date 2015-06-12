#pragma once
#include "PhysicsScene.h"

#include "Tower.h"
#include "Ground.h"
#include "Camera.h"

#include <vector>
#include <memory>

class MainScene : public PhysicsScene
{
private:
	std::unique_ptr<Ground> m_ground;
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<IndexBuffer> m_brickIndices;

	std::unique_ptr<Tower> m_tower;

	PxVec3 m_springOrigin, m_springPos;
	bool m_controlMode;

	Brick * m_pulledBrick;

	PxD6Joint * m_spring;
	PxMaterial *m_brickMat, *m_groundMat;

	float m_camX, m_camY, m_camDist, m_camYAngle;
	float m_xSens, m_ySens;

	void update() final;

public:
	MainScene(PxSceneDesc desc);
	~MainScene();
};

