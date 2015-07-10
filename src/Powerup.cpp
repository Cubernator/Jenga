#include "Powerup.h"
#include "utility.h"

void TestPowerup::apply()
{
	OutputDebugString(L"BOOM!\n");
}

PowerupManager::PowerupManager()
{
	m_powerups[0].reset(new TestPowerup());
}

Powerup * PowerupManager::getPowerup(unsigned int id)
{
	if (id < NUM_POWERUPS) {
		return m_powerups[id].get();
	}

	return nullptr;
}