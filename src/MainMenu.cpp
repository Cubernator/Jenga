#include "MainMenu.h"
#include "MainScene.h"
#include "Input.h"
#include "Engine.h"

#include <chrono>

MainMenu::MainMenu() : m_startClassic(false), m_startSpecial(false), m_viewScoreboard(false)
{
	gui->createFormat(L"verdana", 40, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, &m_titleFormat);
	m_titleLabel.reset(new GUILabel({ 100, 150, 500, 250 }, L"Jenga 3D", m_titleFormat.Get()));
	gui->add(m_titleLabel.get());

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

	m_classicButton.reset(new GUIButton(br, L"Play Classic Mode", m_buttonFormat.Get(), bs));
	m_classicButton->setCallback([this] { m_startClassic = true; });
	gui->add(m_classicButton.get());

	br.top += h;
	br.bottom += h;
	m_specialButton.reset(new GUIButton(br, L"Play Special Mode", m_buttonFormat.Get(), bs));
	m_specialButton->setCallback([this] { m_startSpecial = true; });
	gui->add(m_specialButton.get());

	br.top += h;
	br.bottom += h;
	m_scoreButton.reset(new GUIButton(br, L"View Scoreboard", m_buttonFormat.Get(), bs));
	m_scoreButton->setCallback([this] { m_viewScoreboard = true; });
	gui->add(m_scoreButton.get());

	br.top += h;
	br.bottom += h;
	m_quitButton.reset(new GUIButton(br, L"Quit Game", m_buttonFormat.Get(), bs));
	m_quitButton->setCallback([this] { engine->stop(); });
	gui->add(m_quitButton.get());
}

MainMenu::~MainMenu()
{
	gui->remove(m_quitButton.get());
	gui->remove(m_scoreButton.get());
	gui->remove(m_specialButton.get());
	gui->remove(m_classicButton.get());
	gui->remove(m_titleLabel.get());
}

void MainMenu::update()
{
	unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();

	if (m_startClassic) {
		engine->enterScene<MainScene>(false, seed);
	} else if (m_startSpecial) {
		engine->enterScene<MainScene>(true, seed);
	} else if (m_viewScoreboard) {
		// TODO
	}
}