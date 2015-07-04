#include "MainMenu.h"
#include "MainScene.h"
#include "Input.h"
#include "Engine.h"

MainMenu::MainMenu() : m_exit(false)
{
	m_titleFormat = gui->createFormat(L"verdana", 40, DWRITE_FONT_WEIGHT_BOLD);
	D2D_RECT_F r = {200, 200, 500, 250};
	m_titleLabel.reset(new GUILabel(r, L"Jenga 3D", m_titleFormat));
	gui->add(m_titleLabel.get());

	r = {200, 300, 500, 600};

	m_testBitmap = gui->loadBitmap(L"assets\\images\\UV.jpg");
	m_testImage.reset(new GUIImage(r, m_testBitmap));
	gui->add(m_testImage.get());

	r = {600, 400, 700, 450};
	m_button.reset(new GUIButton(r, L"click me!"));
	m_button->setCallback([this] { m_exit = true; });
	gui->add(m_button.get());
}

MainMenu::~MainMenu()
{
	gui->remove(m_button.get());
	gui->remove(m_testImage.get());
	gui->remove(m_titleLabel.get());
	m_titleFormat->Release();
	m_testBitmap->Release();
}

void MainMenu::update()
{
	if (input->getKeyPressed(VK_RETURN) || m_exit) {
		engine->enterScene<MainScene>();
	}
}