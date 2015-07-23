#pragma once

#include "GUIElement.h"
#include "AudioInterface.h"
#include "SeedPrompt.h"
#include "Scoreboard.h"

class MainScene;

class MainMenu
{
private:
	MainScene * m_scene;

	std::unique_ptr<GUIRectangle> m_tintRect;
	std::unique_ptr<GUILabel> m_titleLabel;
	std::unique_ptr<GUIButton> m_classicButton, m_specialButton, m_scoreButton, m_quitButton;

	std::unique_ptr<SeedPrompt> m_seedPrompt;

	std::unique_ptr<Scoreboard> m_scoreboard;

	bool m_start, m_specialMode, m_viewScoreboard, m_hidden, m_reset;

	void showSeedPrompt();

	void loadResources();

public:
	MainMenu(MainScene * scene);
	~MainMenu();

	void update();

	void startRound(bool specialMode, unsigned int seed);

	void setHidden(bool hidden);
	void reset();
};