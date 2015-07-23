#include "Tower.h"

#include "Objects.h"
#include "utility.h"
#include "MainScene.h"
#include "AudioInterface.h"

#include <random>
#include <algorithm>
#include <numeric>

#define NUM_SPECIAL_BRICKS 16

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

Tower::Tower(MainScene * scene, Shader * s, IndexBuffer * ib) : m_scene(scene), m_positionTolerance(0.3f), m_rotationTolerance(sinf(toRadf(10.0f))), m_brickSize(7.5f, 1.5f, 2.5f)
{
	m_brickTex.reset(new Texture2D(L"assets\\images\\brick.png"));

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

	std::default_random_engine gen(m_scene->getSeed()); // initialize RNG with seed

	std::uniform_real_distribution<float> heightDist(-1.0f, 0.0f); // height variance distribution
	std::bernoulli_distribution rowTypeDist(0.5f); // row type distribution
	std::uniform_int_distribution<int> powerupDist(0, NUM_POWERUPS - 1); // powerup type distribution

	m_rows.resize(18);

	float minBrickSize = brickHSize.y - maxVariance;

	std::array<float, 3> sizes;
	float y = 0.0f;

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 2; j++) {
			int rowIndex = i * 2 + j;

			Row& row = m_rows[rowIndex];
			row.index = rowIndex;

			for (int k = 0; k < 3; k++) sizes[k] = brickHSize.y;

			bool rowType = rowTypeDist(gen);

			if (rowType) {
				sizes[1] += heightDist(gen) * maxVariance;
			} else {
				sizes[0] += heightDist(gen) * maxVariance;
				sizes[2] += heightDist(gen) * maxVariance;

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

				float difficulty = (size.y - minBrickSize) / (rowSize - minBrickSize);
				difficulty = min(max(difficulty, 0.0f), 1.0f);

				Brick * b = new Brick(this, s, ib, size, brickTrans, m_brickMat.get(), difficulty);
				Renderer * r = b->getRenderer();
				r->addTexture(m_brickTex.get());
				r->addSampler(m_sampler.Get());

				m_bricks.emplace_back(b);
				row.setBrickAt(b, k);
			}

			y += rowSize * 2.0f + vgap;
		}
	}

	if (m_scene->getSpecialMode()) {
		std::vector<std::size_t> v(m_bricks.size());
		std::iota(v.begin(), v.end(), 0);
		std::shuffle(v.begin(), v.end(), gen);

		for (int i = 0; i < NUM_SPECIAL_BRICKS; ++i) {
			m_bricks[v[i]]->setPowerup(true, powerupDist(gen));
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

void Tower::update()
{
	m_activeSounds = 0;
	
	for (auto& b : m_bricks) {
		AudioSource * s = b->getAudioSource();
		if (s && s->getSoundState()) ++m_activeSounds;
	}
	
}

int Tower::getActiveSounds() const
{
	return m_activeSounds;
}

void Tower::soundPlayed()
{
	++m_activeSounds;
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

void Tower::getRowProperties(int row, PxVec3& center, PxVec3& dir, PxVec3& up) const
{
	PxVec3 f(1, 0, 0), u(0, 1, 0);

	if (row < 0) {
		center = PxVec3(0, -m_brickSize.y / 2.0f, 0);
		dir = PxVec3(0, 0, 1);
		up = u;
	} else {
		const Row& topRow = m_rows[row];

		std::vector<PxVec3> dirs;

		int n = 0;
		for (Brick* b : topRow.bricks) {
			if (b) {
				Transform * t = b->getTransform();
				PxQuat q = t->getRotation();
				PxVec3 dir2 = q.rotate(f);

				if ((n > 0) && (dir2.dot(dirs[0]) < 0.0f)) dir2 = -dir2;
				dir += dir2;
				dirs.push_back(dir2);

				PxVec3 updir = q.rotate(u);

				if (updir.dot(u) < 0.0f) updir = -updir;
				up += updir;

				center += t->getPosition();
				++n;
			}
		}

		if (n != 0) {
			dir /= (float)n;
			dir.normalize();

			up /= (float)n;
			up.normalize();

			Brick * b = topRow.bricks[1];
			if (n < 3 && b) {
				center = b->getTransform()->getPosition();
			} else {
				center /= (float)n;
			}
		} else {
			// freak out and run around in circles
		}
	}
}

bool Tower::testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex, float& accuracy) const
{
	return testBrickValidSpot(brick, rowBelow, newBrickIndex, accuracy, m_positionTolerance, m_rotationTolerance);
}

bool Tower::testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex, float& accuracy, float posTolerance, float rotTolerance) const
{
	PxVec3 f(1, 0, 0), u(0, 1, 0);
	PxVec3 topRowCenter(PxIdentity), topRowDir(PxIdentity), topRowUp(PxIdentity);

	getRowProperties(rowBelow, topRowCenter, topRowDir, topRowUp);

	topRowCenter += m_brickSize.y * topRowUp;

	Transform * bt = brick->getTransform();
	PxVec3 bdir = bt->getRotation().rotate(f);

	float bdd = fabsf(bdir.dot(topRowDir)), bdu = fabsf(bdir.dot(topRowUp));
	if ((bdd <= rotTolerance) && (bdu <= rotTolerance)) {
		float rotAcc = 1.0f - (bdd / rotTolerance);

		for (int i = -1; i < 2; ++i) {
			PxVec3 bc = bt->getPosition();
			PxVec3 diff = (topRowCenter + ((float)i * topRowDir * m_brickSize.z)) - bc;
			float dm = diff.magnitude();
			if (dm <= posTolerance) {
				float posAcc = 1.0f - (dm / posTolerance);
				accuracy = (rotAcc + posAcc) * 0.5f;
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
	float acc;
	bool a = testBrickValidSpot(brick, ri-1, bi, acc, 1.0f, sinf(toRadf(10.0f)));
	return a && (bi == brick->getBrickIndex());
}

bool Tower::attemptPutBrickOnTop(Brick * brick, float& accuracy, bool dryRun)
{
	Row& topRow = m_rows.back();
	bool topFull = topRow.getBrickCount() == 3;
	int ri = topRow.index - ((topFull) ? 0 : 1);
	int bi;
	if (testBrickValidSpot(brick, ri, bi, accuracy)) {
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

PxTransform Tower::getFreeSpot() const
{
	PxVec3 f(1, 0, 0), u(0, 1, 0);
	PxVec3 topRowCenter(PxIdentity), topRowDir(PxIdentity), topRowUp(PxIdentity);

	const Row& topRow = m_rows.back();
	bool topFull = topRow.getBrickCount() == 3;
	int ri = topRow.index - ((topFull) ? 0 : 1);

	getRowProperties(ri, topRowCenter, topRowDir, topRowUp);

	topRowCenter += (m_brickSize.y + 0.01f) * topRowUp;

	if (!topFull) {
		for (int i = 0; i < 3; ++i) {
			if (!topRow.bricks[i]) {
				topRowCenter += ((float)(i - 1) * topRowDir * m_brickSize.z);
				break;
			}
		}
	}

	PxQuat r = fromToRotation(f, topRowDir);
	r *= PxQuat(toRadf(90), topRowUp);

	return PxTransform(topRowCenter, r);
}

void Tower::setDamping(bool d)
{
	float damping = d ? 5.0f : 0.0f;

	for (auto& b : m_bricks) {
		PxRigidDynamic * a = (PxRigidDynamic*)b->getActor();
		a->setLinearDamping(damping);
	}
}

void Tower::setHighlight(bool h)
{
	for (auto& b : m_bricks) b->setHighlight(h);
}
