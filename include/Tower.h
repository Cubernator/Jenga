#pragma once

#include "Brick.h"

#include <array>

class Tower
{
private:
	struct Row
	{
		unsigned int index;
		std::array<Brick*, 3> bricks;

		Row(unsigned int i) : index(i), bricks({ { nullptr, nullptr, nullptr } }) {}
		Row() : Row(0) { }

		void setState(BrickState state);

		unsigned int getBrickCount() const;

		void setBrickAt(Brick * brick, unsigned int brickIndex);
		void removeBrickAt(unsigned int brickIndex);

		bool isValid() const;
	};

	px_ptr<PxMaterial> m_brickMat;

	std::vector<std::unique_ptr<Brick>> m_bricks;
	std::vector<Row> m_rows;

	float m_positionTolerance, m_rotationTolerance;

	const PxVec3 m_brickSize;

public:
	Tower(Shader * s, IndexBuffer * ib, unsigned int seed);

	unsigned int getHeight() const;
	std::vector<std::unique_ptr<Brick>>& getBricks();

	bool testBrickAlignment(Brick * brick, int rowIndex, int& newBrickIndex) const;
	bool testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex) const;

	bool attemptPutBrickBack(Brick * brick);
	bool attemptPutBrickOnTop(Brick * brick, bool dryRun = false);
};