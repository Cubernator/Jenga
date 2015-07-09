#include "Tower.h"

#include "Objects.h"
#include "utility.h"
#include "MainScene.h"
#include "AudioInterface.h"

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

Tower::Tower(MainScene * scene, Shader * s, IndexBuffer * ib, unsigned int seed) : m_scene(scene), m_positionTolerance(0.3f), m_rotationTolerance(sinf(toRadf(1.0f))), m_brickSize(7.5f, 1.5f, 2.5f)
{
	m_brickTex.reset(new Texture2D(L"assets\\images\\brick.jpg"));

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = 8;
	sd.MinLOD = -FLT_MAX;
	sd.MaxLOD = FLT_MAX;
	dev->CreateSamplerState(&sd, &m_sampler);

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

				Brick * b = new Brick(this, s, m_brickTex.get(), m_sampler.Get(), ib, size, brickTrans, m_brickMat.get());

				m_bricks.emplace_back(b);
				row.setBrickAt(b, k);
			}

			y += rowSize * 2.0f + vgap;
		}
	}

	m_rows.front().setState(BASE);
	m_rows.back().setState(TOP);

	for (auto& b : m_bricks) {
		objects->add(b.get());
		m_scene->addObject(b.get());
	}

	m_brickSounds1.resize(10);
	m_brickSounds2.resize(10);

	for (std::size_t i = 0; i < 10; ++i) {
		std::wstring n = std::to_wstring(i + 1);
		if (i < 9) n = L"0" + n;

		m_brickSounds1[i].reset(audio->loadSoundEffect(L"assets\\audio\\wood_00_" + n + L".wav"));
		m_brickSounds2[i].reset(audio->loadSoundEffect(L"assets\\audio\\wood_01_" + n + L".wav"));

		for (auto& b : m_bricks) {
			b->allocateSoundInstance(m_brickSounds1[i].get());
			b->allocateSoundInstance(m_brickSounds2[i].get());
		}
	}
}

Tower::~Tower()
{
	for (auto& b : m_bricks) {
		m_scene->removeObject(b.get());
		objects->remove(b.get());
	}
}

SoundEffect * Tower::getRandomBrickSound(float force)
{
	std::default_random_engine gen((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
	std::discrete_distribution<int> dist{1, 10, 1};
	std::bernoulli_distribution dist2;

	std::vector<std::unique_ptr<SoundEffect>>& sv = dist2(gen) ? m_brickSounds1 : m_brickSounds2;

	float minForce = 1000.0f, maxForce = 50000.0f;
	int i = (int)floorf((force - minForce) / (maxForce - minForce) * 10.0f);

	if (i >= 0) {
		//OutputDebugString((std::to_wstring(i) + L"\n").c_str());

		i += dist(gen) - 1;
		i = max(min(i, 9), 0);

		return sv[i].get();
	}

	return nullptr;
}

unsigned int Tower::getHeight() const
{
	return m_bricks.size();
}

bool Tower::testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex) const
{
	return testBrickValidSpot(brick, rowBelow, newBrickIndex, m_positionTolerance, m_rotationTolerance);
}

bool Tower::testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex, float posTolerance, float rotTolerance) const
{
	PxVec3 topRowCenter(PxIdentity), topRowDir(PxIdentity), topRowUp(PxIdentity);
	PxVec3 f(1, 0, 0), u(0, 1, 0);

	if (rowBelow < 0) {
		topRowCenter = PxVec3(0, -m_brickSize.y / 2.0f, 0);
		topRowDir = PxVec3(0, 0, 1);
		topRowUp = u;
	} else {
		const Row& topRow = m_rows[rowBelow];

		std::vector<PxVec3> dirs;

		int n = 0;
		for (Brick* b : topRow.bricks) {
			if (b) {
				Transform * t = b->getTransform();
				PxQuat q = t->getRotation();
				PxVec3 dir = q.rotate(f);

				if ((n > 0) && (dir.dot(dirs[0]) < 0.0f)) dir = -dir;
				topRowDir += dir;
				dirs.push_back(dir);

				PxVec3 updir = q.rotate(u);

				if (updir.dot(u) < 0.0f) updir = -updir;
				topRowUp += updir;

				topRowCenter += t->getPosition();
				++n;
			}
		}

		if (n != 0) {
			topRowDir /= (float)n;
			topRowDir.normalize();

			topRowUp /= (float)n;
			topRowUp.normalize();

			Brick * b = topRow.bricks[1];
			if (n < 3 && b) {
				topRowCenter = b->getTransform()->getPosition();
			} else {
				topRowCenter /= (float)n;
			}
		} else {
			// freak out and run around in circles
		}
	}

	topRowCenter += m_brickSize.y * topRowUp;

	Transform * bt = brick->getTransform();
	PxVec3 bdir = bt->getRotation().rotate(f);

	if ((bdir.dot(topRowDir) <= rotTolerance) && (bdir.dot(topRowUp) <= rotTolerance)) {
		for (int i = -1; i < 2; ++i) {
			PxVec3 bc = bt->getPosition();
			PxVec3 diff = (topRowCenter + ((float)i * topRowDir * m_brickSize.z)) - bc;
			if (diff.magnitude() <= posTolerance) {
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
	bool a = testBrickValidSpot(brick, ri-1, bi, 1.0f, sinf(toRadf(10.0f)));
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