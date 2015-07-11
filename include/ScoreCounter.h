#pragma once

#include "Brick.h"
#include "GUI.h"

class ScoreCounter
{
private:
	unsigned int m_points, m_displayedPoints;

	ComPtr<IDWriteTextFormat> m_displayFormat;
	std::unique_ptr<GUILabel> m_display;

	void updateDisplay();

public:
	ScoreCounter();
	~ScoreCounter();

	unsigned int getPoints() const;

	GUILabel * getDisplay();

	void hideDisplay();

	bool finishedCounting() const;

	void update();

	void brickPlaced(Brick * brick, float accuracy);
	void addPowerupScore(unsigned int count);
};