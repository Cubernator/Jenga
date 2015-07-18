#include "ResultsMenu.h"
#include "MainScene.h"
#include "ScoreFile.h"
#include "Content.h"

ResultsMenu::ResultsMenu(MainScene * scene) : m_scene(scene), m_finishedCounting(false), m_scoreSaved(false)
{
	IDWriteTextFormat *titleFormat;
	content->get(L"menuTitleFormat", titleFormat);
	content->get(L"menuButtonFormat", m_labelFormat);

	std::wstring fontFamily;
	content->get(L"fontFamily", fontFamily);

	HRESULT hr = gui->createFormat(fontFamily, 50, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, &m_bigDisplayFormat);
	m_bigDisplayFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	m_tintRect.reset(new GUIRectangle({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, D2D1::ColorF(0, 0.2f)));
	gui->add(m_tintRect.get());

	m_title.reset(new GUILabel({0, 0, SCREEN_WIDTH, 200}, L"GAME OVER!", titleFormat));
	gui->add(m_title.get());

	PowerupManager * pwpm = m_scene->getPowerupManager();
	ScoreCounter * sc = m_scene->getScoreCounter();

	unsigned int numPwps = pwpm->getNumCollectedPowerups();
	if (numPwps) {
		pwpm->clearCollectedPowerups();

		sc->addPowerupScore(numPwps);
	}

	float shw = SCREEN_WIDTH / 2.0f;

	m_scoreLabel.reset(new GUILabel({0, 250, SCREEN_WIDTH, 240}, L"Final Score:", m_labelFormat));
	gui->add(m_scoreLabel.get());

	GUILabel * display = sc->getDisplay();
	display->setRect({shw - 100, 300, shw + 100, 350});
	display->setFormat(m_bigDisplayFormat.Get());
	display->setDepth(-100);

	sc->hideLabel();
}

ResultsMenu::~ResultsMenu()
{
	gui->remove(m_title.get());
	gui->remove(m_tintRect.get());

	hideMenu();
}

void ResultsMenu::hideMenu()
{
	gui->remove(m_scoreLabel.get());
	gui->remove(m_promptLabel.get());
	gui->remove(m_nameField.get());
	gui->remove(m_quitButton.get());
	gui->remove(m_mainMenuButton.get());
	gui->remove(m_retryButton.get());
}

void ResultsMenu::update()
{
	if (!m_finishedCounting && m_scene->getScoreCounter()->finishedCounting()) {
		m_finishedCounting = true;
		promptName();
	}

	if (m_scoreSaved) {
		m_scoreSaved = false;
		showOptions();
	}
}

void ResultsMenu::promptName()
{
	if (m_scene->getScoreCounter()->getPoints() == 0) {
		m_scoreSaved = true;
		return;
	}

	IDWriteTextFormat *fieldFormat;
	GUITextFieldStyle tfs;

	content->get(L"menuTextFieldFormat", fieldFormat);
	content->get(L"menuTextFieldStyle", tfs);

	float shw = SCREEN_WIDTH / 2.0f;

	m_promptLabel.reset(new GUILabel({ 0, SCREEN_HEIGHT - 200, SCREEN_WIDTH, SCREEN_HEIGHT - 160 }, L"Please enter your name:", m_labelFormat));
	gui->add(m_promptLabel.get());

	m_nameField.reset(new GUITextField({ shw - 200, SCREEN_HEIGHT - 140, shw + 200, SCREEN_HEIGHT - 100 }, fieldFormat, tfs));
	m_nameField->setCallback([this] {
		saveScore();
	});

	gui->add(m_nameField.get());
}

void ResultsMenu::showOptions()
{
	gui->remove(m_nameField.get());
	gui->remove(m_promptLabel.get());
	gui->remove(m_scoreLabel.get());

	m_scene->getScoreCounter()->hideDisplay();

	IDWriteTextFormat *buttonFormat;
	content->get(L"menuButtonFormat", buttonFormat);

	GUIButtonStyle bs;
	content->get(L"menuButtonStyle", bs);

	float hw = SCREEN_WIDTH * 0.5f, hh = SCREEN_HEIGHT  * 0.5f;

	D2D_RECT_F br = { hw - 200, 350, hw + 200, 400 };
	int h = 70;

	m_retryButton.reset(new GUIButton(br, L"Retry", buttonFormat, bs));
	m_retryButton->setCallback([this] { m_scene->restart(); });
	gui->add(m_retryButton.get());

	br.top += h;
	br.bottom += h;
	m_mainMenuButton.reset(new GUIButton(br, L"Main Menu", buttonFormat, bs));
	m_mainMenuButton->setCallback([this] { m_scene->backToMainMenu(); });
	gui->add(m_mainMenuButton.get());

	br.top += h;
	br.bottom += h;
	m_quitButton.reset(new GUIButton(br, L"Quit Game", buttonFormat, bs));
	m_quitButton->setCallback([this] { engine->stop(); });
	gui->add(m_quitButton.get());
}

void ResultsMenu::saveScore()
{
	ScoreEntry entry;
	entry.points = m_scene->getScoreCounter()->getPoints();
	entry.seed = m_scene->getSeed();
	entry.name = m_nameField->getText();

	std::wstring fileName = m_scene->getSpecialMode() ? L"score1" : L"score0";
	ScoreFile file(fileName);
	file.saveEntry(entry);

	m_scoreSaved = true;
}