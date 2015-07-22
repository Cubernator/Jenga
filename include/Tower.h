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

	std::unique_ptr<Texture2D> m_brickTex, m_brickNormal;
	ComPtr<ID3D11SamplerState> m_sampler;

	px_ptr<PxMaterial> m_brickMat;

	std::vector<std::unique_ptr<SoundEffect>> m_brickSounds1, m_brickSounds2;
	int m_activeSounds;

	std::vector<std::unique_ptr<Brick>> m_bricks;
	std::vector<Row> m_rows;

	float m_positionTolerance, m_rotationTolerance;

	const PxVec3 m_brickSize;

public:
	Tower(MainScene * scene, Shader * s, IndexBuffer * ib);
	~Tower();

	void update();

	unsigned int getHeight() const;

	int getActiveSounds() const;
	void soundPlayed();

	SoundEffect * getRandomBrickSound(float force);

	void getRowProperties(int row, PxVec3& center, PxVec3& dir, PxVec3& up) const;

	PxTransform getFreeSpot() const;

	void setDamping(bool d);
	void setHighlight(bool h);

	bool testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex, float& accuracy, float posTolerance, float rotTolerance) const;
	bool testBrickValidSpot(Brick * brick, int rowBelow, int& newBrickIndex, float& accuracy) const;

	bool attemptPutBrickBack(Brick * brick);
	bool attemptPutBrickOnTop(Brick * brick, float& accuracy, bool dryRun = false);
};