#pragma once
#include "PhysicsScene.h"

#include "Block.h"
#include "Camera.h"

#include <vector>
#include <memory>

class MainScene : public PhysicsScene
{
private:
	std::vector<std::unique_ptr<Block>> m_blocks;
	std::unique_ptr<Block> m_ground;
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<IndexBuffer> m_blockIndices;

	PxMaterial *m_blockMat, *m_groundMat;

	float m_camX, m_camY, m_camDist, m_camYAngle;
	float m_xSens, m_ySens;

	void update() final;

public:
	MainScene(PxSceneDesc desc);
	~MainScene();
};

