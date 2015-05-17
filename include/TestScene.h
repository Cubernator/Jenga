#pragma once
#include "PhysicsScene.h"
#include "Block.h"
#include "Camera.h"
#include <memory>

class TestScene : public PhysicsScene
{
private:
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Block> m_obj, m_ground;
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<IndexBuffer> m_blockIndices;

	PxMaterial * m_material;

public:
	TestScene(PxSceneDesc desc);
	~TestScene();
};



