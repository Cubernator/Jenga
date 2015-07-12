#include "SeedPrompt.h"
#include "constants.h"
#include "Content.h"

#include <random>
#include <chrono>

SeedPrompt::SeedPrompt() : SeedPrompt(false, 0) {}
SeedPrompt::SeedPrompt(unsigned int lastSeed) : SeedPrompt(true, lastSeed) {}

SeedPrompt::SeedPrompt(bool hasLastSeed, unsigned int lastSeed) : m_hasLastSeed(hasLastSeed), m_done(false), m_lastSeed(lastSeed), m_promptInput(false)
{
	IDWriteTextFormat *buttonFormat;
	content->get(L"menuButtonFormat", buttonFormat);

	GUIButtonStyle bs;
	content->get(L"menuButtonStyle", bs);

	float shw = SCREEN_WIDTH / 2.0f, shh = SCREEN_HEIGHT / 2.0f;

	D2D_RECT_F br = { shw - 200, 350, shw + 200, 400 };
	int h = 70;

	if (m_hasLastSeed) {
		m_lastButton.reset(new GUIButton(br, L"Use last seed again", buttonFormat, bs));
		m_lastButton->setCallback([this] {
			useSeed(m_lastSeed);
		});
		gui->add(m_lastButton.get());
		br.top += h;
		br.bottom += h;
	}

	m_randomButton.reset(new GUIButton(br, L"Use random seed", buttonFormat, bs));
	m_randomButton->setCallback([this] {
		useSeed(getRandomSeed());
	});
	gui->add(m_randomButton.get());
	br.top += h;
	br.bottom += h;

	m_manualButton.reset(new GUIButton(br, L"Use custom seed", buttonFormat, bs));
	m_manualButton->setCallback([this] {
		m_promptInput = true;
	});
	gui->add(m_manualButton.get());
}

SeedPrompt::~SeedPrompt()
{
	hideMenu();
	gui->remove(m_field.get());
}

void SeedPrompt::hideMenu()
{
	gui->remove(m_lastButton.get());
	gui->remove(m_randomButton.get());
	gui->remove(m_manualButton.get());
}

void SeedPrompt::useSeed(unsigned int seed)
{
	m_seed = seed;
	m_done = true;
}

unsigned int SeedPrompt::getRandomSeed() const
{
	unsigned int s = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	s = (unsigned int)(s * 5238477503) ^ 322972321; // this is not necessary, but we do it anyway so the seed "looks more random"
	return s;
}

void SeedPrompt::showTextField()
{
	hideMenu();

	IDWriteTextFormat *format;
	GUITextFieldStyle style;
	content->get(L"menuTextFieldFormat", format);
	content->get(L"menuTextFieldStyle", style);

	float shw = SCREEN_WIDTH / 2.0f, shh = SCREEN_HEIGHT / 2.0f;

	m_field.reset(new GUITextField({ shw - 200, 350, shw + 200, 400 }, format, style));
	m_field->setCallback([this] {
		parseInput();
	});
	gui->add(m_field.get());
}

void SeedPrompt::update()
{
	if (m_promptInput) {
		m_promptInput = false;
		showTextField();
	}
}

void SeedPrompt::parseInput()
{
	std::wstring input = m_field->getText();
	try {
		int i = std::stoi(input);
		useSeed((unsigned int)i);
		return;
	} catch (std::exception&) { }

	std::hash<std::wstring> hasher;
	useSeed(hasher(input));
}

bool SeedPrompt::isDone() const
{
	return m_done;
}

unsigned int SeedPrompt::getSeed() const
{
	return m_seed;
}