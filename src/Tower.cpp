#include "Tower.h"

#include "ObjectManager.h"

#include <random>
#include <algorithm>

void Tower::Row::setState(BrickState state)
{
	for (Brick * b : bricks) {
		if (b) b->setState(state);
	}
}

unsigned int Tower::Row::getBrickCount() const
{
	return std::count_if(bricks.begin(), bricks.end(), [](Brick * b) { return b != nullptr; });
}

void Tower::Row::setBrickAt(Brick * brick, unsigned int brickIndex)
{
	brick->setRowIndex(index);
	brick->setBrickIndex(brickIndex);

	bricks[brickIndex] = brick;
}

void Tower::Row::removeBrickAt(unsigned int brickIndex)
{
	bricks[brickIndex]->setRowIndex(-1);
	bricks[brickIndex]->setBrickIndex(-1);
	bricks[brickIndex] = nullptr;
}

bool Tower::Row::isValid() const
{
	// TODO
	return false;
}

Tower::Tower(Shader * s, IndexBuffer * ib, PxMaterial * m, unsigned int seed) : m_positionTolerance(0.3f), m_rotationTolerance(0.05f), m_brickSize(7.5f, 1.5f, 2.5f)
{
	PxVec3 brickHSize = m_brickSize / 2.0f;
	PxQuat r90(3.14159265f * 0.5f, PxVec3(0, 1, 0));
	float gap = 0.02f, maxVariance = 0.01f, maxVariance2 = 0.003f;

	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_int_distribution<int> dist2(0, 2);

	m_rows.resize(18);

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 2; j++) {
			int rowIndex = i * 2 + j;
			float y = ((rowIndex)* 2 + 1) * (brickHSize.y + gap);

			Row& row = m_rows[rowIndex];
			row.index = rowIndex;

			int d = dist2(generator);

			for (int k = 0; k < 3; k++) {
				float x = 0.0f, z = (k - 1) * 2 * (brickHSize.z + gap);

				PxTransform brickTrans(PxVec3(x, y, z));
				if (j > 0) {
					brickTrans.q = r90;
					brickTrans.p.x = z;
					brickTrans.p.z = 0.0f;
				}

				PxVec3 size = brickHSize;
				size.x += distribution(generator) * maxVariance;
				size.y += distribution(generator) * (d == k) ? maxVariance : maxVariance2;
				size.z += distribution(generator) * maxVariance;

				Brick * b = new Brick(s, ib, size, brickTrans, m);

				m_bricks.emplace_back(b);
				row.setBrickAt(b, k);
			}
		}
	}

	m_rows.front().setState(BASE);
	m_rows.back().setState(TOP);
}

unsigned int Tower::getHeight() const
{
	return m_bricks.size();
}

std::vector<std::unique_ptr<Brick>>& Tower::getBricks()
{
	return m_bricks;
}

bool Tower::testBrickAlignment(Brick * brick, int rowIndex, int& newBrickIndex) const
{
	Row row = m_rows[rowIndex];
	unsigned int bc = row.getBrickCount();
	if (bc == 1) {
		
	} else if (bc == 2) {

	}
	return false;
}

bool Tower::testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex) const
{
	const Row& topRow = m_rows[rowBelow];
	Transform *t0 = topRow.bricks[0]->getTransform(), *t2 = topRow.bricks[2]->getTransform();
	PxVec3 f(1, 0, 0), u(0, 1, 0);
	PxVec3 d0 = t0->getRotation().rotate(f), d2 = t2->getRotation().rotate(f);
	if (d0.dot(d2) < 0.0f) d2 = -d2;
	PxVec3 topRowDir = (d0 + d2) / 2.0f;
	topRowDir.normalize();

	Transform * bt = brick->getTransform();
	PxVec3 bdir = bt->getRotation().rotate(f);

	if ((bdir.dot(topRowDir) <= m_rotationTolerance) && (bdir.dot(u) <= m_rotationTolerance)) {
		PxVec3 topRowCenter = (t0->getPosition() + t2->getPosition()) / 2.0f;
		topRowCenter.y += m_brickSize.y;
		for (int i = -1; i < 2; ++i) {
			PxVec3 diff = (topRowCenter + (i * topRowDir * m_brickSize.z)) - bt->getPosition();
			if (diff.magnitude() <= m_positionTolerance) {
				newBrickIndex = i + 1;
				return true;
			}
		}
	}

	return false;
}

bool Tower::attemptPutBrickBack(Brick * brick)
{
	int bi;
	bool a = testBrickAlignment(brick, brick->getRowIndex(), bi);
	return a && (bi == brick->getBrickIndex());
}

bool Tower::attemptPutBrickOnTop(Brick * brick, bool dryRun)
{
	Row& topRow = m_rows.back();
	bool topFull = topRow.getBrickCount() == 3;
	int ri = topRow.index - ((topFull) ? 0 : 1);
	int bi;
	if (testBrickValidSpot(brick, ri, bi)) {
		if (!dryRun) {
			if (topFull) {
				m_rows.emplace_back(m_rows.size());
			}

			Row& newTop = m_rows.back();
			m_rows[brick->getRowIndex()].removeBrickAt(brick->getBrickIndex());
			newTop.setBrickAt(brick, bi);

			if (newTop.getBrickCount() == 3) { // the top row is complete
				m_rows[m_rows.size() - 2].setState(TOWER);
			}
		}

		return true;
	}

	return false;
}