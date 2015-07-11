#pragma once

#include "GUI.h"

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

	ComPtr<ID2D1Bitmap> m_tfNormal, m_tfHover, m_tfFocus;
	ComPtr<IDWriteTextFormat> m_buttonFormat, m_fieldFormat, m_titleFormat;

	bool m_finishedCounting, m_scoreSaved;

	void promptName();
	void saveScore();
	void showOptions();

public:
	ResultsMenu(MainScene * scene);
	~ResultsMenu();

	void update();
};