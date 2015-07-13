#pragma once

#include "GUI.h"

#include <array>
#include <memory>

class Powerup
{
public:
	virtual ~Powerup() { }

	virtual bool isApplicable() const { return true; };

	virtual void apply() = 0;

	virtual void update() { }
};

class MainScene;
class Tower;

class QuickPlacePowerup : public Powerup
{
private:
	MainScene * m_scene;

public:
	QuickPlacePowerup(MainScene * scene);

	bool isApplicable() const final;
	void apply() final;
};

class StabilizePowerup : public Powerup
{
private:
	Tower * m_tower;
	float m_timer;

public:
	StabilizePowerup(MainScene * scene);

	void apply() final;
	void update() final;
};

class HighlightPowerup : public Powerup
{
private:
	MainScene * m_scene;

public:
	HighlightPowerup(MainScene * scene);

	void apply() final;
};

#define NUM_POWERUPS 3

class MainScene;

class PowerupManager
{
private:
	std::array<std::unique_ptr<Powerup>, NUM_POWERUPS> m_powerups;

	MainScene * m_scene;

	std::vector<unsigned int> m_collectedPowerups;
	std::vector<std::unique_ptr<GUIButton>> m_powerupButtons;
	unsigned int m_usedPowerup;

	GUIButtonStyle m_buttonStyle;

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