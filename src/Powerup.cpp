#include "Powerup.h"
#include "utility.h"

#include <string>

void TestPowerup::apply()
{
	OutputDebugString(L"BOOM!\n");
}

PowerupManager::PowerupManager(MainScene * scene) : m_scene(scene), m_usedPowerup(-1)
{
	m_powerups[0].reset(new TestPowerup());
}

PowerupManager::~PowerupManager()
{
	clearCollectedPowerups();
}

void PowerupManager::update()
{
	if (m_usedPowerup < m_collectedPowerups.size()) {
		Powerup * pwp = getPowerup(m_collectedPowerups[m_usedPowerup]);

		if (pwp && pwp->isApplicable()) {
			pwp->apply();
			m_collectedPowerups.erase(m_collectedPowerups.begin() + m_usedPowerup);

			makePowerupButtons();
		}

		m_usedPowerup = -1;
	}
}

Powerup * PowerupManager::getPowerup(unsigned int id)
{
	if (id < NUM_POWERUPS) {
		return m_powerups[id].get();
	}

	return nullptr;
}

void PowerupManager::collectPowerup(unsigned int id)
{
	m_collectedPowerups.push_back(id);
	makePowerupButtons();
}

void PowerupManager::usePowerup(unsigned int index)
{
	m_usedPowerup = index;
}

void PowerupManager::makePowerupButtons()
{
	for (auto& b : m_powerupButtons) gui->remove(b.get());
	m_powerupButtons.resize(m_collectedPowerups.size());

	float bw = 64, bh = 64, o = 20, g = 10;

	D2D_RECT_F r{ o, SCREEN_HEIGHT - o - bh, o + bw, SCREEN_HEIGHT - o };

	for (std::size_t i = 0; i < m_collectedPowerups.size(); ++i) {
		unsigned int pwpId = m_collectedPowerups[i];
		GUIButton * b = new GUIButton(r, std::to_wstring(pwpId));
		b->setCallback([this, i] { usePowerup(i); });
		m_powerupButtons[i].reset(b);
		gui->add(b);

		r.left += bw + g;
		r.right += bw + g;
	}
}

unsigned int PowerupManager::getNumCollectedPowerups() const
{
	return m_collectedPowerups.size();
}

void PowerupManager::clearCollectedPowerups()
{
	for (auto& p : m_powerupButtons)
		gui->remove(p.get());

	m_powerupButtons.clear();
	m_collectedPowerups.clear();
}
