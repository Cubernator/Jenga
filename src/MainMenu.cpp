#include "MainMenu.h"
#include "MainScene.h"
#include "Input.h"
#include "Engine.h"
#include "Content.h"
#include "Scoreboard.h"

MainMenu::MainMenu(MainScene * scene) : m_scene(scene), m_start(false), m_viewScoreboard(false), m_hidden(true)
{
	IDWriteTextFormat *buttonFormat, *titleFormat;
	content->get(L"menuButtonFormat", buttonFormat);
	content->get(L"menuTitleFormat", titleFormat);

	GUIButtonStyle bs;
	content->get(L"menuButtonStyle", bs);

	m_tintRect.reset(new GUIRectangle({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, D2D1::ColorF(0, 0.2f)));
	gui->add(m_tintRect.get());

	m_titleLabel.reset(new GUILabel({ 0, 0, SCREEN_WIDTH, 200 }, L"Jenga 3D", titleFormat));

	float hw = SCREEN_WIDTH * 0.5f, hh = SCREEN_HEIGHT * 0.5f;

	D2D_RECT_F br = { hw - 200, 350, hw + 200, 400 };
	int h = 70;

	m_classicButton.reset(new GUIButton(br, L"Play Classic Mode", buttonFormat, bs));
	m_classicButton->setCallback([this] {
		m_start = true;
		m_specialMode = false;
	});

	br.top += h;
	br.bottom += h;
	m_specialButton.reset(new GUIButton(br, L"Play Special Mode", buttonFormat, bs));
	m_specialButton->setCallback([this] {
		m_start = true;
		m_specialMode = true;
	});

	br.top += h;
	br.bottom += h;
	m_scoreButton.reset(new GUIButton(br, L"Highscores", buttonFormat, bs));
	m_scoreButton->setCallback([this] { m_viewScoreboard = true; });

	br.top += h;
	br.bottom += h;
	m_quitButton.reset(new GUIButton(br, L"Quit", buttonFormat, bs));
	m_quitButton->setCallback([this] { engine->stop(); });

	setHidden(false);
}

MainMenu::~MainMenu()
{
	setHidden(true);
	gui->remove(m_tintRect.get());
}

void MainMenu::startRound(bool specialMode, unsigned int seed)
{
	m_scene->startRound(specialMode, seed);
}

void MainMenu::reset()
{
	m_reset = true;
}

void MainMenu::update()
{
	if (m_seedPrompt) {
		m_seedPrompt->update();

		if (m_seedPrompt->isDone()) {
			startRound(m_specialMode, m_seedPrompt->getSeed());
			return;
		} else if (m_seedPrompt->isCanceled()) {
			reset();
		}
	} else if (m_start) {
		m_start = false;
		showSeedPrompt();
	} else if (m_viewScoreboard) {
		m_viewScoreboard = false;
		setHidden(true);
		m_scoreboard.reset(new Scoreboard(this));
	}

	if (m_reset) {
		m_reset = false;
		m_scoreboard.reset();
		m_seedPrompt.reset();
		setHidden(false);
	}

	if (m_scoreboard) {
		m_scoreboard->update();
	}
}

void MainMenu::showSeedPrompt()
{
	m_seedPrompt.reset(new SeedPrompt);
	setHidden(true);
}

void MainMenu::setHidden(bool hidden)
{
	if (hidden != m_hidden) {
		m_hidden = hidden;
		if (m_hidden) {
			gui->remove(m_quitButton.get());
			gui->remove(m_scoreButton.get());
			gui->remove(m_specialButton.get());
			gui->remove(m_classicButton.get());
			gui->remove(m_titleLabel.get());
		} else {
			gui->add(m_titleLabel.get());
			gui->add(m_classicButton.get());
			gui->add(m_specialButton.get());
			gui->add(m_scoreButton.get());
			gui->add(m_quitButton.get());
		}
	}
}
