#include "Camera.h"
#include "Engine.h"

Camera::Camera() : m_transform(new DefaultTransform(this)), m_nearPlane(0.1f), m_farPlane(300.f), m_aspectRatio(1.f), m_fov(75.f), m_backColor(0,0,0,0)
{
	setTransform(m_transform.get());
}

float Camera::getNearPlane() const
{
	return m_nearPlane;
}

float Camera::getFarPlane() const
{
	return m_farPlane;
}

float Camera::getAspectRatio() const
{
	return m_aspectRatio;
}

float Camera::getFOV() const
{
	return m_fov;
}

void Camera::setNearPlane(float v)
{
	m_nearPlane = v;
}

void Camera::setFarPlane(float v)
{
	m_farPlane = v;
}

void Camera::setAspectRatio(float v)
{
	m_aspectRatio = v;
}

void Camera::setFOV(float v)
{
	m_fov = v;
}

const XMFLOAT4& Camera::getBackColor() const
{
	return m_backColor;
}

void Camera::setBackColor(const XMFLOAT4& c)
{
	m_backColor = c;
}

XMMATRIX Camera::getProjectionMatrix() const
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::getPickingRay(int x, int y, PxVec3& pos, PxVec3& dir) const
{
	const Transform * t = getTransform();
	XMMATRIX proj = getProjectionMatrix();

	PxMat44 inverseView(t->getTransform()), inverseProj;
	XMStoreFloat4x4((XMFLOAT4X4*)&inverseProj, XMMatrixInverse(&XMMatrixDeterminant(proj), proj));

	float dx = ((float)x / (float)SCREEN_WIDTH - 0.5f) * 2.0f;
	float dy = -((float)y / (float)SCREEN_HEIGHT - 0.5f) * 2.0f;

	PxVec4 start(dx, dy, -1.0f, 1.0f), end(dx, dy, 0.0f, 1.0f);

	start = inverseProj.transform(start); start /= start.w;
	start = inverseView.transform(start); start /= start.w;

	end = inverseProj.transform(end); end /= end.w;
	end = inverseView.transform(end); end /= end.w;

	pos = t->getPosition();
	dir = (end - start).getXYZ().getNormalized();
}

void Camera::worldToScreen(const PxVec3& pos, int& x, int& y) const
{
	PxMat44 view(getTransform()->getTransform().getInverse()), proj;
	XMStoreFloat4x4((XMFLOAT4X4*)&proj, getProjectionMatrix());
	PxVec4 p(view.transform(pos), 1.0f);
	p = proj.transform(p);
	
	x = (int)(((p.x/p.w + 1.0f) / 2.0f) * SCREEN_WIDTH);
	y = (int)(((1.0f - p.y/p.w) / 2.0f) * SCREEN_HEIGHT);
}
