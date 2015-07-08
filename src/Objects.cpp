#include "Objects.h"
#include "utility.h"
#include "Transform.h"
#include <algorithm>

ObjectManager * objects;

ObjectManager::ObjectManager()
{
	objects = this;
}

void ObjectManager::update()
{
	for (GameObject * obj : m_objects) {
		obj->update();
	}

	computeAABB();
}

void ObjectManager::add(GameObject * obj)
{
	m_objects.push_back(obj);
	computeAABB();
}

void ObjectManager::remove(GameObject * obj)
{
	m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), obj), m_objects.end());
	computeAABB();
}

void ObjectManager::computeAABB()
{
	XMVECTOR minv = XMVectorReplicate(FLT_MAX), maxv = XMVectorReplicate(-FLT_MAX);
	XMVECTOR objMin, objMax;
	XMMATRIX mat, absMat;
	PxMat44 m;
	for (GameObject * obj : m_objects) {
		if (obj->getIsGeometry()) {
			obj->getLocalAABB(objMin, objMax);
			mat = XMLoadFloat4x4((XMFLOAT4X4*)&obj->getTransform()->getMatrix());
			transformAABB(mat, objMin, objMax, objMin, objMax);
			minv = XMVectorMin(objMin, minv);
			maxv = XMVectorMax(objMax, maxv);
		}
	}

	if (XMComparisonAnyTrue(XMVector3GreaterR(minv, maxv))) {
		XMStoreFloat4(&m_sceneAABBMin, XMVectorZero());
		XMStoreFloat4(&m_sceneAABBMax, XMVectorZero());
	} else {
		XMStoreFloat4(&m_sceneAABBMin, minv);
		XMStoreFloat4(&m_sceneAABBMax, maxv);
	}

	m_sceneAABBMin.w = 1.0f;
	m_sceneAABBMax.w = 1.0f;
}

void ObjectManager::getSceneAABB(XMVECTOR& min, XMVECTOR& max) const
{
	min = XMLoadFloat4(&m_sceneAABBMin);
	max = XMLoadFloat4(&m_sceneAABBMax);
}
