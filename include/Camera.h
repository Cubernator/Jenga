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
	XMFLOAT4 m_backColor;

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

	const XMFLOAT4& getBackColor() const;
	void setBackColor(const XMFLOAT4& c);

	XMMATRIX getProjectionMatrix() const;

	void getPickingRay(int x, int y, PxVec3& pos, PxVec3& dir) const;
	void worldToScreen(const PxVec3& pos, int& x, int& y) const;
};

