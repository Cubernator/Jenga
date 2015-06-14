#include "ObjectManager.h"
#include <algorithm>

ObjectManager * objects;

#define CB_Application 0
#define CB_Frame 1
#define CB_Object 2
#define CB_Material 3

ObjectManager::ObjectManager() : m_cam(nullptr)
{
	objects = this;

	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(D3D11_BUFFER_DESC));

	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	cbd.Usage = D3D11_USAGE_DEFAULT;

	cbd.ByteWidth = sizeof(cbPerApplication);
	dev->CreateBuffer(&cbd, NULL, &m_constantBuffers[CB_Application]);

	cbd.ByteWidth = sizeof(cbPerFrame);
	dev->CreateBuffer(&cbd, NULL, &m_constantBuffers[CB_Frame]);

	cbd.ByteWidth = sizeof(cbPerObject);
	dev->CreateBuffer(&cbd, NULL, &m_constantBuffers[CB_Object]);

	m_constantBuffers[CB_Material] = nullptr;

	setConstantBuffers();
}

ObjectManager::~ObjectManager()
{
	m_constantBuffers[CB_Application]->Release();
	m_constantBuffers[CB_Frame]->Release();
	m_constantBuffers[CB_Object]->Release();
}

void ObjectManager::setConstantBuffers()
{
	UINT cbc = 3;
	if (m_constantBuffers[CB_Material]) cbc = 4;
	devcon->VSSetConstantBuffers(0, cbc, m_constantBuffers);
	devcon->PSSetConstantBuffers(0, cbc, m_constantBuffers);
}

void ObjectManager::draw(float alpha)
{
	if (m_cam) {
		m_cam->setAspectRatio(16.f / 9.f);
		XMMATRIX proj = m_cam->getProjectionMatrix();
		XMStoreFloat4x4(&m_app.projection, proj);

		PxMat44 vm(m_cam->getTransform()->getTransform().getInverse());
		XMMATRIX view = XMLoadFloat4x4((XMFLOAT4X4*)&vm);
		//XMVECTOR det = XMMatrixDeterminant(view);
		//view = XMMatrixInverse(&det, view);
		XMStoreFloat4x4(&m_frame.view, view);

		devcon->UpdateSubresource(m_constantBuffers[CB_Application], 0, NULL, &m_app, 0, 0);
		devcon->UpdateSubresource(m_constantBuffers[CB_Frame], 0, NULL, &m_frame, 0, 0);

		XMMATRIX world;
		PxMat44 wm;

		for (GameObject * obj : m_objects) {
			const Renderer * r = obj->getRenderer();
			if (r && r->getEnabled()) {
				const Transform * t = obj->getTransform();
				if (t && t->getEnabled()) {
					wm = t->getMatrix();
					world = XMLoadFloat4x4((XMFLOAT4X4*)&wm);
					XMStoreFloat4x4(&m_object.world, world);
					XMStoreFloat4x4(&m_object.mvp, world * view * proj);
					devcon->UpdateSubresource(m_constantBuffers[CB_Object], 0, NULL, &m_object, 0, 0);

					ID3D11Buffer * matBuffer = r->getConstantBuffer();
					if (matBuffer != m_constantBuffers[CB_Material]) {
						m_constantBuffers[CB_Material] = matBuffer;
					}
				}

				setConstantBuffers();

				r->draw();
			}
		}
	}
}

void ObjectManager::update()
{
	for (GameObject * obj : m_objects) {
		obj->update();
	}
}

void ObjectManager::addObject(GameObject * obj)
{
	m_objects.push_back(obj);
}

void ObjectManager::removeObject(GameObject * obj)
{
	m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), obj), m_objects.end());
}

void ObjectManager::setCamera(Camera * cam)
{
	m_cam = cam;
}

void ObjectManager::setLight(const Light& l)
{
	m_frame.light = l;
}
