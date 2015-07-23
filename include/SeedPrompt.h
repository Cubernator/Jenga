#pragma once

#include "GUI.h"

class SeedPrompt
{
private:
	bool m_hasLastSeed, m_done, m_canceled;
	unsigned int m_lastSeed, m_seed;

	bool m_promptInput, m_hideInput;

	IDWriteTextFormat *m_buttonFormat;
	GUIButtonStyle m_bs;

	std::unique_ptr<GUIButton> m_lastButton, m_randomButton, m_manualButton, m_cancelButton, m_manualDoneButton, m_manualCancelButton;
	std::unique_ptr<GUITextField> m_field;

	SeedPrompt(bool hasLastSeed, unsigned int lastSeed);

	void useSeed(unsigned int seed);
	unsigned int getRandomSeed() const;

	void showTextField();
	void hideMenu();
	void hideInput();
	void showMenu();
	void parseInput();

public:
	SeedPrompt();
	SeedPrompt(unsigned int lastSeed);
	~SeedPrompt();

	void update();

	bool isDone() const;
	bool isCanceled() const;
	unsigned int getSeed() const;
};