#pragma once

#include "Brick.h"
#include "GUI.h"

class ScoreCounter
{
private:
	unsigned int m_points, m_displayedPoints;

	std::unique_ptr<GUILabel> m_display, m_label;

	ComPtr<IDWriteTextFormat> m_displayFormat, m_labelFormat;

	void updateDisplay();

public:
	ScoreCounter();
	~ScoreCounter();

	unsigned int getPoints() const;

	GUILabel * getDisplay();

	void hideDisplay();
	void hideLabel();

	bool finishedCounting() const;

	void update();

	void brickPlaced(Brick * brick, float accuracy);
	void addPowerupScore(unsigned int count);
};