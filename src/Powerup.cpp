#include "Powerup.h"
#include "utility.h"
#include "Content.h"
#include "MainScene.h"

#include <string>

PowerupManager::PowerupManager(MainScene * scene) : m_scene(scene), m_usedPowerup(-1)
{
	content->get(L"menuButtonStyle", m_buttonStyle);

	m_powerups[0].reset(new QuickPlacePowerup(m_scene));
	m_powerups[1].reset(new StabilizePowerup(m_scene));
	m_powerups[2].reset(new HighlightPowerup(m_scene));
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

	for (auto& p : m_powerups) p->update();

	// NOTE: THIS IS A CHEAT!
	char c = '1';
	for (int i = 0; i < NUM_POWERUPS; ++i, ++c) {
		if (input->getKeyPressed(c)) {
			collectPowerup(i);
		}
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

	float bw = 74, bh = 74, o = 20, g = 10;

	D2D_RECT_F r{ o, SCREEN_HEIGHT - o - bh, o + bw, SCREEN_HEIGHT - o };

	for (std::size_t i = 0; i < m_collectedPowerups.size(); ++i) {
		unsigned int pwpId = m_collectedPowerups[i];
		GUIButton * b = new GUIButton(r, m_powerups[pwpId]->getIcon(), m_buttonStyle);
		b->setPadding({5, 5, 5, 5});
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

QuickPlacePowerup::QuickPlacePowerup(MainScene * scene) : m_scene(scene)
{
	gui->loadBitmap(L"assets\\images\\quickplace_icon.png", &m_icon);
}

bool QuickPlacePowerup::isApplicable() const
{
	return m_scene->getSelectedBrick() != nullptr;
}

void QuickPlacePowerup::apply()
{
	m_scene->quickPlace();
}

ID2D1Bitmap * QuickPlacePowerup::getIcon()
{
	return m_icon.Get();
}

StabilizePowerup::StabilizePowerup(MainScene * scene) : m_tower(scene->getTower()), m_timer(0.0f)
{
	gui->loadBitmap(L"assets\\images\\stabilize_icon.png", &m_icon);
}

void StabilizePowerup::apply()
{
	m_tower->setDamping(true);
	m_timer = 5.0f;
}

ID2D1Bitmap * StabilizePowerup::getIcon()
{
	return m_icon.Get();
}

void StabilizePowerup::update()
{
	if (m_timer > 0.0f) {
		m_timer -= engine->getRealDelta();
		if (m_timer <= 0.0f) {
			m_tower->setDamping(false);
		}
	}
}

HighlightPowerup::HighlightPowerup(MainScene * scene) : m_scene(scene)
{
	gui->loadBitmap(L"assets\\images\\highlight_icon.png", &m_icon);
}

void HighlightPowerup::apply()
{
	m_scene->activateHighlight();
}

ID2D1Bitmap * HighlightPowerup::getIcon()
{
	return m_icon.Get();
}
