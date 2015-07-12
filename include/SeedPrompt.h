#pragma once

#include "GUI.h"

class SeedPrompt
{
private:
	bool m_hasLastSeed, m_done;
	unsigned int m_lastSeed, m_seed;

	bool m_promptInput;

	std::unique_ptr<GUIButton> m_lastButton, m_randomButton, m_manualButton;
	std::unique_ptr<GUITextField> m_field;

	SeedPrompt(bool hasLastSeed, unsigned int lastSeed);

	void useSeed(unsigned int seed);
	unsigned int getRandomSeed() const;

	void showTextField();
	void hideMenu();
	void parseInput();

public:
	SeedPrompt();
	SeedPrompt(unsigned int lastSeed);
	~SeedPrompt();

	void update();

	bool isDone() const;
	unsigned int getSeed() const;
};