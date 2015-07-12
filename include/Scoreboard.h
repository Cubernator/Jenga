#include "Scene.h"
#include "GUI.h"
#include "ScoreFile.h"

class Scoreboard : public Scene
{
private:
	std::vector<ScoreEntry> m_entries;

	std::unique_ptr<GUIButton> m_backButton, m_leftButton, m_rightButton;
	std::unique_ptr<GUILabel> m_title, m_scoreHeader, m_nameHeader, m_seedHeader;

	std::vector<std::unique_ptr<GUILabel>> m_posLabels, m_scoreLabels, m_nameLabels;
	std::vector<std::unique_ptr<GUIButton>> m_seedButtons;

	ComPtr<IDWriteTextFormat> m_headerFormat, m_tableFormat;
	IDWriteTextFormat *m_buttonFormat;
	GUIButtonStyle m_buttonStyle;

	bool m_switch, m_special, m_start, m_back;
	unsigned int m_seed;

	void update() final;

	void loadEntries();
	void switchBoards();

	void clearBoard();

	void reuseSeed(unsigned int index);

public:
	Scoreboard();
	~Scoreboard();
};