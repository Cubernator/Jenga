#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include <vector>

struct cbPerApplication
{
	XMMATRIX projection;
};

struct cbPerFrame
{
	PxMat44 view;
	Light light;
};

struct cbPerObject
{
	PxMat44 world;
};

class ObjectManager
{
private:
	std::vector<GameObject*> m_objects;
	Camera * m_cam;

	cbPerApplication m_app;
	cbPerFrame m_frame;
	cbPerObject m_object;

	ID3D11Buffer *m_constantBuffers[4];

	void setConstantBuffers();

public:
	ObjectManager();
	~ObjectManager();

	void draw(float alpha);
	void update();

	void addObject(GameObject * obj);
	void removeObject(GameObject * obj);

	void setCamera(Camera * cam);
	void setLight(const Light& l);
};

extern ObjectManager * objects;

