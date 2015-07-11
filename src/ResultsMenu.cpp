#include "ResultsMenu.h"
#include "MainScene.h"
#include "ScoreFile.h"

ResultsMenu::ResultsMenu(MainScene * scene) : m_scene(scene), m_finishedCounting(false), m_scoreSaved(false)
{
	m_tintRect.reset(new GUIRectangle({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, D2D1::ColorF(0, 0.2f)));
	gui->add(m_tintRect.get());

	gui->createFormat(L"verdana", 50, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, &m_titleFormat);
	m_titleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_titleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	m_title.reset(new GUILabel({0, 0, SCREEN_WIDTH, 200}, L"GAME OVER!", m_titleFormat.Get()));
	gui->add(m_title.get());
	
	gui->createFormat(L"verdana", 30, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, &m_buttonFormat);
	m_buttonFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_buttonFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	gui->createFormat(L"verdana", 20, &m_fieldFormat);
	m_fieldFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	gui->loadBitmap(L"assets\\images\\textfield_normal.png", &m_tfNormal);
	gui->loadBitmap(L"assets\\images\\textfield_hover.png", &m_tfHover);
	gui->loadBitmap(L"assets\\images\\textfield_focus.png", &m_tfFocus);

	PowerupManager * pwpm = m_scene->getPowerupManager();
	ScoreCounter * sc = m_scene->getScoreCounter();

	unsigned int numPwps = pwpm->getNumCollectedPowerups();
	if (numPwps) {
		pwpm->clearCollectedPowerups();

		sc->addPowerupScore(numPwps);
	}

	float shw = SCREEN_WIDTH / 2.0f;

	m_scoreLabel.reset(new GUILabel({0, 300, SCREEN_WIDTH, 350}, L"Final Score:", m_buttonFormat.Get()));
	gui->add(m_scoreLabel.get());

	GUILabel * display = sc->getDisplay();
	display->setRect({0, 380, SCREEN_WIDTH, 420});
	display->setDepth(-100);
}

ResultsMenu::~ResultsMenu()
{
	gui->remove(m_title.get());
	gui->remove(m_scoreLabel.get());
	gui->remove(m_promptLabel.get());
	gui->remove(m_nameField.get());
	gui->remove(m_quitButton.get());
	gui->remove(m_mainMenuButton.get());
	gui->remove(m_retryButton.get());
	gui->remove(m_tintRect.get());
}

void ResultsMenu::update()
{
	if (m_scene->getScoreCounter()->finishedCounting() && !m_finishedCounting) {
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
	GUITextFieldStyle tfs{
		GUIStyleState(m_tfNormal.Get()),
		GUIStyleState(m_tfHover.Get()),
		GUIStyleState(m_tfFocus.Get())
	};

	float shw = SCREEN_WIDTH / 2.0f;

	m_promptLabel.reset(new GUILabel({0, SCREEN_HEIGHT - 200, SCREEN_WIDTH, SCREEN_HEIGHT - 160}, L"Please enter your name:", m_buttonFormat.Get()));
	gui->add(m_promptLabel.get());

	m_nameField.reset(new GUITextField({ shw - 200, SCREEN_HEIGHT - 140, shw + 200, SCREEN_HEIGHT - 100 }, m_fieldFormat.Get(), tfs));
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