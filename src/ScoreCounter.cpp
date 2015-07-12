#include "ScoreCounter.h"

#include <sstream>
#include <iomanip>

ScoreCounter::ScoreCounter() : m_points(0), m_displayedPoints(0)
{
	gui->createFormat(L"verdana", 40, &m_displayFormat);
	m_displayFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	m_displayFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	gui->createFormat(L"verdana", 20, &m_labelFormat);
	m_labelFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	m_labelFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	m_label.reset(new GUILabel({ SCREEN_WIDTH - 300, 20, SCREEN_WIDTH - 40, 40 }, L"SCORE:", m_labelFormat.Get()));
	gui->add(m_label.get());

	m_display.reset(new GUILabel({ SCREEN_WIDTH - 300, 50, SCREEN_WIDTH - 30, 100 }, L"", m_displayFormat.Get()));
	gui->add(m_display.get());

	updateDisplay();
}

ScoreCounter::~ScoreCounter()
{
	gui->remove(m_display.get());
	gui->remove(m_label.get());
}

unsigned int ScoreCounter::getPoints() const
{
	return m_points;
}

GUILabel * ScoreCounter::getDisplay()
{
	return m_display.get();
}

void ScoreCounter::hideDisplay()
{
	gui->remove(m_display.get());
}

void ScoreCounter::hideLabel()
{
	gui->remove(m_label.get());
}

bool ScoreCounter::finishedCounting() const
{
	return m_displayedPoints == m_points;
}

void ScoreCounter::updateDisplay()
{
	m_display->setText(formatScore(m_displayedPoints));
}

void ScoreCounter::update()
{
	float pointsPerSecond = 1000.0f; // increase visual counter by this amount every second

	if (m_displayedPoints < m_points) {
		m_displayedPoints = min(m_displayedPoints + (unsigned int)(engine->getDelta() * pointsPerSecond), m_points);
		updateDisplay();
	}
}

void ScoreCounter::brickPlaced(Brick * brick, float accuracy)
{
	unsigned int basePoints = 100;

	// both of these values are mapped from [0, 1] to [1, 10]

	float difficultyFactor = brick->getDifficulty() * 9.0f + 1.0f;
	float accuracyFactor = accuracy * 9.0f + 1.0f;

	m_points += (unsigned int)(basePoints * difficultyFactor * accuracyFactor);
}

void ScoreCounter::addPowerupScore(unsigned int count)
{
	unsigned int basePoints = 5000;
	
	m_points += basePoints * count;
}