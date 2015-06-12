#pragma once

#include "GameObject.h"
#include "MeshRenderer.h"
#include "VertexBuffer.h"
#include "PhysicsTransform.h"

#include <memory>

enum BrickState
{
	TOWER,		// in the "body" of the tower (everything except selected, top and base)
	BASE,		// bricks in the bottom row (can be taken out)
	TOP,		// bricks in the top row. if the top row isn't complete, also bricks in the row below that
	PULLING,	// the brick which is currently attached to the mouse, if there is
	SELECTED,	// the brick which was last attached to the mouse and has not left the tower or been aligned
	ALIGNED		// the brick which was last attached to the mouse and has successfully been aligned with the top row
};

class Brick : public GameObject
{
private:
	std::unique_ptr<PhysicsTransform> m_transform;
	std::unique_ptr<MeshRenderer> m_renderer;
	std::unique_ptr<VertexBuffer<VertexPosNormal>> m_vbuffer;

	XMFLOAT4 m_color;
	BrickState m_state;
	unsigned int m_rowIndex, m_brickIndex;

	XMFLOAT4 getStateColor() const;

public:
	Brick(Shader * s, IndexBuffer * ib, const PxVec3& halfSize, const PxTransform& t, PxMaterial * m);
	~Brick();

	void setColor(const XMFLOAT4& c);

	void setState(BrickState state);

	unsigned int getRowIndex() const;
	void setRowIndex(unsigned int i);

	unsigned int getBrickIndex() const;
	void setBrickIndex(unsigned int i);

	bool canTouchGround() const;
	bool canPickUp() const;
	bool isAligned() const;
};

