#include "Camera.h"

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
