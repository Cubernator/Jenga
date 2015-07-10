#pragma once

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

class PowerupManager
{
private:
	std::array<std::unique_ptr<Powerup>, NUM_POWERUPS> m_powerups;

public:
	PowerupManager();

	Powerup * getPowerup(unsigned int id);
};