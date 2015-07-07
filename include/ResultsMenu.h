#pragma once

#include "GUIElement.h"

class MainScene;

class ResultsMenu
{
private:
	MainScene * m_scene;

	std::unique_ptr<GUIRectangle> m_tintRect;
	std::unique_ptr<GUIButton> m_retryButton, m_mainMenuButton, m_quitButton;

	ComPtr<IDWriteTextFormat> m_buttonFormat;

public:
	ResultsMenu(MainScene * scene);
	~ResultsMenu();
};