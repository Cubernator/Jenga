#pragma once

#include "GUI.h"

#include <array>
#include <memory>

class Powerup
{
public:
	virtual ~Powerup() { }

	virtual bool isApplicable() const = 0;
	virtual void apply() = 0;
};

class TestPowerup : public Powerup
{
public:
	bool isApplicable() const final { return true; }
	void apply() final;
};

#define NUM_POWERUPS 1

class MainScene;

class PowerupManager
{
private:
	std::array<std::unique_ptr<Powerup>, NUM_POWERUPS> m_powerups;

	MainScene * m_scene;

	std::vector<unsigned int> m_collectedPowerups;
	std::vector<std::unique_ptr<GUIButton>> m_powerupButtons;
	unsigned int m_usedPowerup;

	void makePowerupButtons();

public:
	PowerupManager(MainScene * scene);
	~PowerupManager();

	void update();

	void collectPowerup(unsigned int id);
	void usePowerup(unsigned int index);

	Powerup * getPowerup(unsigned int id);

	unsigned int getNumCollectedPowerups() const;
	void clearCollectedPowerups();
};