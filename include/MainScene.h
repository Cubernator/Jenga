#pragma once
#include "PhysicsScene.h"

#include "Block.h"

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

public:
	MainScene(PxSceneDesc desc);
	~MainScene();
};

