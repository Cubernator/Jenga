#include "MainMenu.h"
#include "MainScene.h"
#include "Input.h"
#include "Engine.h"
#include "Content.h"
#include "Scoreboard.h"

MainMenu::MainMenu() : m_start(false), m_viewScoreboard(false)
{
	IDWriteTextFormat *buttonFormat, *titleFormat;
	content->get(L"menuButtonFormat", buttonFormat);
	content->get(L"menuTitleFormat", titleFormat);

	GUIButtonStyle bs;
	content->get(L"menuButtonStyle", bs);

	m_titleLabel.reset(new GUILabel({ 0, 0, SCREEN_WIDTH, 200 }, L"Jenga 3D", titleFormat));
	gui->add(m_titleLabel.get());

	float hw = SCREEN_WIDTH * 0.5f, hh = SCREEN_HEIGHT * 0.5f;

	D2D_RECT_F br = { hw - 200, 350, hw + 200, 400 };
	int h = 70;

	m_classicButton.reset(new GUIButton(br, L"Play Classic Mode", buttonFormat, bs));
	m_classicButton->setCallback([this] {
		m_start = true;
		m_specialMode = false;
	});
	gui->add(m_classicButton.get());

	br.top += h;
	br.bottom += h;
	m_specialButton.reset(new GUIButton(br, L"Play Special Mode", buttonFormat, bs));
	m_specialButton->setCallback([this] {
		m_start = true;
		m_specialMode = true;
	});
	gui->add(m_specialButton.get());

	br.top += h;
	br.bottom += h;
	m_scoreButton.reset(new GUIButton(br, L"View Scoreboard", buttonFormat, bs));
	m_scoreButton->setCallback([this] { m_viewScoreboard = true; });
	gui->add(m_scoreButton.get());

	br.top += h;
	br.bottom += h;
	m_quitButton.reset(new GUIButton(br, L"Quit Game", buttonFormat, bs));
	m_quitButton->setCallback([this] { engine->stop(); });
	gui->add(m_quitButton.get());
}

MainMenu::~MainMenu()
{
	hideMenu();
}

void MainMenu::update()
{
	if (m_seedPrompt) {
		m_seedPrompt->update();

		if (m_seedPrompt->isDone()) {
			bool sm = m_specialMode;
			unsigned int seed = m_seedPrompt->getSeed();
			engine->enterScene<MainScene>(sm, seed);
		}
	} else if (m_start) {
		m_start = false;
		showSeedPrompt();
	} else if (m_viewScoreboard) {
		engine->enterScene<Scoreboard>();
	}
}

void MainMenu::showSeedPrompt()
{
	m_seedPrompt.reset(new SeedPrompt);
	hideMenu();
}

void MainMenu::hideMenu()
{
	gui->remove(m_quitButton.get());
	gui->remove(m_scoreButton.get());
	gui->remove(m_specialButton.get());
	gui->remove(m_classicButton.get());
	gui->remove(m_titleLabel.get());
}