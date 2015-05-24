#include "Camera.h"
#include "Engine.h"

Camera::Camera() : m_transform(new DefaultTransform(this)), m_nearPlane(0.01f), m_farPlane(500.f), m_aspectRatio(1.f), m_fov(75.f)
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
