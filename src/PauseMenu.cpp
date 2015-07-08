#include "PauseMenu.h"
#include "MainScene.h"
#include "constants.h"
#include "Engine.h"
#include "AudioInterface.h"

PauseMenu::PauseMenu(MainScene * scene) : m_scene(scene)
{
	m_tintRect.reset(new GUIRectangle({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, D2D1::ColorF(0, 0.7f)));
	gui->add(m_tintRect.get());

	gui->createFormat(L"verdana", 18, &m_buttonFormat);
	m_buttonFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_buttonFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	GUIButtonStyle bs = {
		GUIStyleState(),
		GUIStyleState(D2D1::ColorF(1.0f, 0.3f, 0.0f)),
		GUIStyleState(D2D1::ColorF(1.0f, 0.6f, 0.6f))
	};

	D2D_RECT_F br = { 150, 250, 450, 270 };
	int h = 40;

	m_resumeButton.reset(new GUIButton(br, L"Resume", m_buttonFormat.Get(), bs));
	m_resumeButton->setCallback([this] { m_scene->togglePause(); });
	gui->add(m_resumeButton.get());

	br.top += h;
	br.bottom += h;
	m_restartButton.reset(new GUIButton(br, L"Restart", m_buttonFormat.Get(), bs));
	m_restartButton->setCallback([this] { m_scene->restart(); });
	gui->add(m_restartButton.get());

	br.top += h;
	br.bottom += h;
	m_mainMenuButton.reset(new GUIButton(br, L"Back to Main Menu", m_buttonFormat.Get(), bs));
	m_mainMenuButton->setCallback([this] { m_scene->backToMainMenu(); });
	gui->add(m_mainMenuButton.get());

	br.top += h;
	br.bottom += h;
	m_quitButton.reset(new GUIButton(br, L"Quit Game", m_buttonFormat.Get(), bs));
	m_quitButton->setCallback([this] { engine->stop(); });
	gui->add(m_quitButton.get());

	engine->setTimeScale(0.0f);
}

PauseMenu::~PauseMenu()
{
	gui->remove(m_quitButton.get());
	gui->remove(m_mainMenuButton.get());
	gui->remove(m_restartButton.get());
	gui->remove(m_resumeButton.get());
	gui->remove(m_tintRect.get());

	engine->setTimeScale(1.0f);
}