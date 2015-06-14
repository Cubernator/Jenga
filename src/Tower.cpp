#include "Tower.h"

#include "ObjectManager.h"
#include "utility.h"

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

Tower::Tower(Shader * s, IndexBuffer * ib, unsigned int seed) : m_positionTolerance(0.3f), m_rotationTolerance(sinf(toRadf(1.0f))), m_brickSize(7.5f, 1.5f, 2.5f)
{
	PxVec3 brickHSize = m_brickSize / 2.0f;
	PxQuat r90(toRadf(-90.0f), PxVec3(0, 1, 0));
	float hgap = 0.01f, vgap = 0.001f, maxVariance = 0.025f;

	m_brickMat.reset(physics->createMaterial(0.7f, 0.4f, 0.1f));

	std::default_random_engine gen(seed);
	std::uniform_real_distribution<float> dist1(-1.0f, 0.0f);
	std::uniform_int_distribution<int> dist2(0, 2);
	std::bernoulli_distribution dist3(0.2f);

	m_rows.resize(18);

	std::array<float, 3> sizes;
	float y = 0.0f;

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 2; j++) {
			int rowIndex = i * 2 + j;

			Row& row = m_rows[rowIndex];
			row.index = rowIndex;

			int d = dist2(gen);

			for (int k = 0; k < 3; k++) {
				sizes[k] = brickHSize.y;
				if (d == k) {
					float o = dist1(gen) * maxVariance;
					if (k == 1 && dist3(gen)) o = -o;
					sizes[k] += o;
				}
			}

			float rowSize = *std::max_element(sizes.begin(), sizes.end());

			
			for (int k = 0; k < 3; k++) {
				float x = 0.0f, z = (k - 1) * 2 * (brickHSize.z + hgap);

				PxTransform brickTrans(PxVec3(x, y + sizes[k], z));
				if (j > 0) {
					brickTrans.q = r90;
					brickTrans.p.x = z;
					brickTrans.p.z = 0.0f;
				}

				PxVec3 size = brickHSize;
				size.y = sizes[k];

				Brick * b = new Brick(s, ib, size, brickTrans, m_brickMat.get());

				m_bricks.emplace_back(b);
				row.setBrickAt(b, k);
			}

			y += rowSize * 2.0f + vgap;
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


bool Tower::testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex) const
{
	PxVec3 topRowCenter, topRowDir;
	PxVec3 f(1, 0, 0), u(0, 1, 0);

	if (rowBelow < 0) {
		topRowCenter = PxVec3(0, -m_brickSize.y / 2.0f, 0);
		topRowDir = PxVec3(0, 0, 1);
	} else {
		const Row& topRow = m_rows[rowBelow];
		Transform *t0 = topRow.bricks[0]->getTransform(), *t2 = topRow.bricks[2]->getTransform();
		PxVec3 d0 = t0->getRotation().rotate(f), d2 = t2->getRotation().rotate(f);
		if (d0.dot(d2) < 0.0f) d2 = -d2;

		topRowCenter = (t0->getPosition() + t2->getPosition()) / 2.0f;
		topRowDir = (d0 + d2) / 2.0f;
		topRowDir.normalize();
	}

	Transform * bt = brick->getTransform();
	PxVec3 bdir = bt->getRotation().rotate(f);

	if ((bdir.dot(topRowDir) <= m_rotationTolerance) && (bdir.dot(u) <= m_rotationTolerance)) {
		topRowCenter.y += m_brickSize.y;
		for (int i = -1; i < 2; ++i) {
			PxVec3 bc = bt->getPosition();
			PxVec3 diff = (topRowCenter + ((float)i * topRowDir * m_brickSize.z)) - bc;
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
	int bi, ri = brick->getRowIndex();
	bool a = testBrickValidSpot(brick, ri-1, bi);
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