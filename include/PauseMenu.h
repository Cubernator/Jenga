#pragma once

#include "GUIElement.h"

class MainScene;

class PauseMenu
{
private:
	MainScene * m_scene;

	std::unique_ptr<GUIRectangle> m_tintRect;
	std::unique_ptr<GUIButton> m_resumeButton, m_restartButton, m_mainMenuButton, m_quitButton;

public:
	PauseMenu(MainScene * scene);
	~PauseMenu();

	void hideMenu();
};