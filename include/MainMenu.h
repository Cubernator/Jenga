#pragma once

#include "Scene.h"
#include "GUIElement.h"
#include "AudioInterface.h"
#include "SeedPrompt.h"

class MainMenu : public Scene
{
private:
	std::unique_ptr<GUILabel> m_titleLabel;
	std::unique_ptr<GUIButton> m_classicButton, m_specialButton, m_scoreButton, m_quitButton;

	std::unique_ptr<SeedPrompt> m_seedPrompt;

	bool m_start, m_specialMode, m_viewScoreboard;

	void update() final;

	void showSeedPrompt();
	void hideMenu();

	void loadResources();

public:
	MainMenu();
	~MainMenu();
};