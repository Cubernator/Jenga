#include "Scoreboard.h"
#include "Content.h"
#include "constants.h"
#include "Engine.h"
#include "MainMenu.h"
#include "MainScene.h"
#include "utility.h"

Scoreboard::Scoreboard() : m_back(false), m_switch(false), m_special(false), m_start(false)
{
	IDWriteTextFormat *titleFormat;
	content->get(L"menuTitleFormat", titleFormat);
	content->get(L"menuButtonFormat", m_buttonFormat);
	content->get(L"menuButtonStyle", m_buttonStyle);

	std::wstring fontFamily;
	content->get(L"fontFamily", fontFamily);

	gui->createFormat(fontFamily, 20, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, &m_headerFormat);
	m_headerFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_headerFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	gui->createFormat(fontFamily, 18, &m_tableFormat);
	m_tableFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_tableFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	float hw = SCREEN_WIDTH * 0.5f, hh = SCREEN_HEIGHT * 0.5f;

	m_title.reset(new GUILabel({ 0, 0, SCREEN_WIDTH, 200 }, L"", titleFormat));
	gui->add(m_title.get());

	m_scoreHeader.reset(new GUILabel({ 250, 190, 450, 240 }, L"SCORE", m_headerFormat.Get()));
	gui->add(m_scoreHeader.get());

	m_nameHeader.reset(new GUILabel({ 450, 190, 800, 240 }, L"NAME", m_headerFormat.Get()));
	gui->add(m_nameHeader.get());

	m_seedHeader.reset(new GUILabel({ 800, 190, SCREEN_WIDTH - 200, 240 }, L"SEED", m_headerFormat.Get()));
	gui->add(m_seedHeader.get());

	m_leftButton.reset(new GUIButton({ hw - 300, 75, hw - 250, 125 }, L"<", m_buttonFormat, m_buttonStyle));
	m_leftButton->setCallback([this] { m_switch = true; });
	gui->add(m_leftButton.get());

	m_rightButton.reset(new GUIButton({ hw + 250, 75, hw + 300, 125 }, L">", m_buttonFormat, m_buttonStyle));
	m_rightButton->setCallback([this] { m_switch = true; });
	gui->add(m_rightButton.get());

	m_backButton.reset(new GUIButton({ hw - 200, SCREEN_HEIGHT - 100, hw + 200, SCREEN_HEIGHT - 50 }, L"Back to main menu", m_buttonFormat, m_buttonStyle));
	m_backButton->setCallback([this] { m_back = true; });
	gui->add(m_backButton.get());

	loadEntries();
}

Scoreboard::~Scoreboard()
{
	clearBoard();

	gui->remove(m_scoreHeader.get());
	gui->remove(m_nameHeader.get());
	gui->remove(m_seedHeader.get());
	gui->remove(m_title.get());
	gui->remove(m_leftButton.get());
	gui->remove(m_rightButton.get());
	gui->remove(m_backButton.get());
}

void Scoreboard::update()
{
	if (m_back) {
		engine->enterScene<MainMenu>();
	} else if (m_start) {
		unsigned int seed = m_seed;
		bool special = m_special;
		engine->enterScene<MainScene>(special, seed);
	} else if (m_switch) {
		m_switch = false;
		switchBoards();
	}
}

void Scoreboard::switchBoards()
{
	m_special = !m_special;
	loadEntries();
}

void Scoreboard::loadEntries()
{
	ScoreFile file(m_special ? L"score1" : L"score0");
	m_entries.clear();
	file.readEntries(std::back_inserter(m_entries));
	std::stable_sort(m_entries.begin(), m_entries.end(), std::greater<ScoreEntry>());

	m_title->setText(m_special ? L"Special Mode" : L"Classic Mode");

	clearBoard();

	float y = 250, h = 40;

	int imax = min(m_entries.size(), 9);

	for (int i = 0; i < imax; ++i) {
		const ScoreEntry& e = m_entries[i];
		float y2 = y + h;

		GUILabel * posLabel = new GUILabel({ 200, y, 250, y2 }, std::to_wstring(i + 1) + L".", m_tableFormat.Get());
		gui->add(posLabel);
		m_posLabels.emplace_back(posLabel);

		GUILabel * scoreLabel = new GUILabel({ 250, y, 450, y2 }, formatScore(e.points), m_tableFormat.Get());
		gui->add(scoreLabel);
		m_scoreLabels.emplace_back(scoreLabel);

		GUILabel * nameLabel = new GUILabel({ 450, y, 800, y2 }, e.name, m_tableFormat.Get());
		gui->add(nameLabel);
		m_nameLabels.emplace_back(nameLabel);

		GUIButton * seedButton = new GUIButton({ 800, y, SCREEN_WIDTH - 200, y2 }, std::to_wstring(e.seed), m_tableFormat.Get(), m_buttonStyle);
		unsigned int s = e.seed;
		seedButton->setCallback([this, s] {
			m_start = true;
			m_seed = s;
		});
		gui->add(seedButton);
		m_seedButtons.emplace_back(seedButton);

		y += h;
	}
}

void Scoreboard::clearBoard()
{
	for (auto& p : m_posLabels) gui->remove(p.get());
	for (auto& p : m_scoreLabels) gui->remove(p.get());
	for (auto& p : m_nameLabels) gui->remove(p.get());
	for (auto& p : m_seedButtons) gui->remove(p.get());

	m_posLabels.clear();
	m_scoreLabels.clear();
	m_nameLabels.clear();
	m_seedButtons.clear();
}