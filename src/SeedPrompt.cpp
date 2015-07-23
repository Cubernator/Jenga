#include "SeedPrompt.h"
#include "constants.h"
#include "Content.h"

#include <random>
#include <chrono>

SeedPrompt::SeedPrompt() : SeedPrompt(false, 0) {}
SeedPrompt::SeedPrompt(unsigned int lastSeed) : SeedPrompt(true, lastSeed) {}

SeedPrompt::SeedPrompt(bool hasLastSeed, unsigned int lastSeed) : m_hasLastSeed(hasLastSeed), m_done(false), m_canceled(false), m_lastSeed(lastSeed), m_promptInput(false)
{
	content->get(L"menuButtonFormat", m_buttonFormat);
	content->get(L"menuButtonStyle", m_bs);

	float shw = SCREEN_WIDTH / 2.0f, shh = SCREEN_HEIGHT / 2.0f;

	D2D_RECT_F br = { shw - 200, 350, shw + 200, 400 };
	int h = 70;

	if (m_hasLastSeed) {
		m_lastButton.reset(new GUIButton(br, L"Use last seed again", m_buttonFormat, m_bs));
		m_lastButton->setCallback([this] {
			useSeed(m_lastSeed);
		});
		br.top += h;
		br.bottom += h;
	}

	m_randomButton.reset(new GUIButton(br, L"Use random seed", m_buttonFormat, m_bs));
	m_randomButton->setCallback([this] {
		useSeed(getRandomSeed());
	});
	br.top += h;
	br.bottom += h;

	m_manualButton.reset(new GUIButton(br, L"Use custom seed", m_buttonFormat, m_bs));
	m_manualButton->setCallback([this] {
		m_promptInput = true;
	});
	br.top += h;
	br.bottom += h;

	m_cancelButton.reset(new GUIButton(br, L"Cancel", m_buttonFormat, m_bs));
	m_cancelButton->setCallback([this] {
		m_canceled = true;
	});

	showMenu();
}

SeedPrompt::~SeedPrompt()
{
	hideMenu();
	hideInput();
}

void SeedPrompt::hideMenu()
{
	gui->remove(m_lastButton.get());
	gui->remove(m_randomButton.get());
	gui->remove(m_manualButton.get());
	gui->remove(m_cancelButton.get());
}

void SeedPrompt::showMenu()
{
	if (m_lastButton) gui->add(m_lastButton.get());
	gui->add(m_randomButton.get());
	gui->add(m_manualButton.get());
	gui->add(m_cancelButton.get());
}

void SeedPrompt::hideInput()
{
	gui->remove(m_field.get());
	gui->remove(m_manualDoneButton.get());
	gui->remove(m_manualCancelButton.get());
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
	D2D_RECT_F br = { shw - 200, 350, shw + 200, 400 };
	int h = 70;

	std::function<void()> doneCallback = [this] {
		parseInput();
	};

	m_field.reset(new GUITextField(br, format, style));
	m_field->setCallback(doneCallback);
	gui->add(m_field.get());
	br.top += h;
	br.bottom += h;

	m_manualDoneButton.reset(new GUIButton(br, L"Done", m_buttonFormat, m_bs));
	m_manualDoneButton->setCallback(doneCallback);
	gui->add(m_manualDoneButton.get());
	br.top += h;
	br.bottom += h;

	m_manualCancelButton.reset(new GUIButton(br, L"Cancel", m_buttonFormat, m_bs));
	m_manualCancelButton->setCallback([this] {
		m_hideInput = true;
	});
	gui->add(m_manualCancelButton.get());
}

void SeedPrompt::update()
{
	if (m_promptInput) {
		m_promptInput = false;
		showTextField();
	}

	if (m_hideInput) {
		m_hideInput = false;
		hideInput();
		showMenu();
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

bool SeedPrompt::isCanceled() const
{
	return m_canceled;
}

unsigned int SeedPrompt::getSeed() const
{
	return m_seed;
}