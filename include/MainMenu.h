#pragma once

#include "Scene.h"
#include "GUIElement.h"
#include "AudioInterface.h"

class MainMenu : public Scene
{
private:
	ComPtr<IDWriteTextFormat> m_titleFormat, m_buttonFormat;

	std::unique_ptr<GUILabel> m_titleLabel;
	std::unique_ptr<GUIButton> m_classicButton, m_specialButton, m_scoreButton, m_quitButton;

	bool m_exit;

	void update() final;

public:
	MainMenu();
	~MainMenu();
};