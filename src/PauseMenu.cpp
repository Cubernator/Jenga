#include "PauseMenu.h"
#include "MainScene.h"
#include "constants.h"
#include "Engine.h"
#include "AudioInterface.h"
#include "Content.h"

PauseMenu::PauseMenu(MainScene * scene) : m_scene(scene)
{
	m_tintRect.reset(new GUIRectangle({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, D2D1::ColorF(0, 0.7f)));
	gui->add(m_tintRect.get());

	IDWriteTextFormat *buttonFormat;
	content->get(L"menuButtonFormat", buttonFormat);

	GUIButtonStyle bs;
	content->get(L"menuButtonStyle", bs);

	float hw = SCREEN_WIDTH * 0.5f, hh = SCREEN_HEIGHT * 0.5f;

	D2D_RECT_F br = { hw - 200, 350, hw + 200, 400 };
	int h = 70;

	m_resumeButton.reset(new GUIButton(br, L"Resume", buttonFormat, bs));
	m_resumeButton->setCallback([this] { m_scene->togglePause(); });
	gui->add(m_resumeButton.get());

	br.top += h;
	br.bottom += h;
	m_restartButton.reset(new GUIButton(br, L"Restart", buttonFormat, bs));
	m_restartButton->setCallback([this] { m_scene->restart(); });
	gui->add(m_restartButton.get());

	br.top += h;
	br.bottom += h;
	m_mainMenuButton.reset(new GUIButton(br, L"Back to Main Menu", buttonFormat, bs));
	m_mainMenuButton->setCallback([this] { m_scene->backToMainMenu(); });
	gui->add(m_mainMenuButton.get());

	br.top += h;
	br.bottom += h;
	m_quitButton.reset(new GUIButton(br, L"Quit Game", buttonFormat, bs));
	m_quitButton->setCallback([this] { engine->stop(); });
	gui->add(m_quitButton.get());

	engine->setTimeScale(0.0f);
}

PauseMenu::~PauseMenu()
{
	hideMenu();

	gui->remove(m_tintRect.get());

	engine->setTimeScale(1.0f);
}

void PauseMenu::hideMenu()
{
	gui->remove(m_quitButton.get());
	gui->remove(m_mainMenuButton.get());
	gui->remove(m_restartButton.get());
	gui->remove(m_resumeButton.get());
}