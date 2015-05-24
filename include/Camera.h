#pragma once

#include "GameObject.h"
#include "DefaultTransform.h"
#include <memory>
#include "DirectX.h"

class Camera : public GameObject
{
private:
	std::unique_ptr<DefaultTransform> m_transform;
	float m_nearPlane, m_farPlane, m_aspectRatio, m_fov;

public:
	Camera();

	float getNearPlane() const;
	float getFarPlane() const;
	float getAspectRatio() const;
	float getFOV() const;

	void setNearPlane(float v);
	void setFarPlane(float v);
	void setAspectRatio(float v);
	void setFOV(float v);

	XMMATRIX getProjectionMatrix() const;

	void getPickingRay(int x, int y, PxVec3& pos, PxVec3& dir) const;
};

