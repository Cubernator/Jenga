#include "ResultsMenu.h"
#include "MainScene.h"

ResultsMenu::ResultsMenu(MainScene * scene) : m_scene(scene)
{
	m_tintRect.reset(new GUIRectangle({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, D2D1::ColorF(0, 0.4f)));
	gui->add(m_tintRect.get());

	gui->createFormat(L"verdana", 30, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, &m_buttonFormat);
	m_buttonFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_buttonFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	GUIButtonStyle bs = {
		GUIStyleState(),
		GUIStyleState(D2D1::ColorF(1.0f, 0.3f, 0.0f)),
		GUIStyleState(D2D1::ColorF(1.0f, 0.6f, 0.6f))
	};

	float shw = SCREEN_WIDTH / 2.0f, shh = SCREEN_HEIGHT / 2.0f;

	D2D_RECT_F br = { shw - 100, shh + 10, shw + 100, shh + 70 };
	int h = 70;

	m_retryButton.reset(new GUIButton(br, L"Retry", m_buttonFormat.Get(), bs));
	m_retryButton->setCallback([this] { m_scene->restart(); });
	gui->add(m_retryButton.get());

	br.top += h;
	br.bottom += h;
	m_mainMenuButton.reset(new GUIButton(br, L"Main Menu", m_buttonFormat.Get(), bs));
	m_mainMenuButton->setCallback([this] { m_scene->backToMainMenu(); });
	gui->add(m_mainMenuButton.get());

	br.top += h;
	br.bottom += h;
	m_quitButton.reset(new GUIButton(br, L"Quit Game", m_buttonFormat.Get(), bs));
	m_quitButton->setCallback([this] { engine->stop(); });
	gui->add(m_quitButton.get());
}

ResultsMenu::~ResultsMenu()
{
	gui->remove(m_quitButton.get());
	gui->remove(m_mainMenuButton.get());
	gui->remove(m_retryButton.get());
	gui->remove(m_tintRect.get());
}