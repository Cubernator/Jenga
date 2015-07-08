#pragma once

#include "Brick.h"

#include <array>

class MainScene;

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
	};

	MainScene * m_scene;

	std::unique_ptr<Texture2D> m_brickTex;
	ComPtr<ID3D11SamplerState> m_sampler;

	px_ptr<PxMaterial> m_brickMat;

	std::unique_ptr<SoundEffect> m_brickSound;

	std::vector<std::unique_ptr<Brick>> m_bricks;
	std::vector<Row> m_rows;

	float m_positionTolerance, m_rotationTolerance;

	const PxVec3 m_brickSize;

public:
	Tower(MainScene * scene, Shader * s, IndexBuffer * ib, unsigned int seed);
	~Tower();

	unsigned int getHeight() const;

	SoundEffect * getRandomBrickSound(float impactStrength);

	bool testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex, float posTolerance, float rotTolerance) const;
	bool testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex) const;

	bool attemptPutBrickBack(Brick * brick);
	bool attemptPutBrickOnTop(Brick * brick, bool dryRun = false);
};