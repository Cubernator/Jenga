#pragma once

#include "Scene.h"
#include "GUIElement.h"

class MainMenu : public Scene
{
private:
	std::unique_ptr<GUILabel> m_titleLabel;
	IDWriteTextFormat * m_titleFormat;

	ID2D1Bitmap * m_testBitmap;
	std::unique_ptr<GUIImage> m_testImage;

	std::unique_ptr<GUIButton> m_button;

	bool m_exit;

	void update() final;

public:
	MainMenu();
	~MainMenu();
};