#pragma once

#include "GUI.h"
#include "SeedPrompt.h"

#include <memory>

class MainScene;

class ResultsMenu
{
private:
	MainScene * m_scene;

	std::unique_ptr<GUIRectangle> m_tintRect;
	std::unique_ptr<GUIButton> m_retryButton, m_mainMenuButton, m_quitButton;

	std::unique_ptr<GUILabel> m_title, m_scoreLabel, m_promptLabel;

	std::unique_ptr<GUITextField> m_nameField;

	IDWriteTextFormat *m_labelFormat;
	ComPtr<IDWriteTextFormat> m_bigDisplayFormat;

	bool m_finishedCounting, m_scoreSaved;

	void promptName();
	void saveScore();
	void showOptions();

public:
	ResultsMenu(MainScene * scene);
	~ResultsMenu();

	void hideMenu();

	void update();
};