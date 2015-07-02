#pragma once

#include "Graphics.h"

#include <d2d1_1.h>
#include <dwrite_2.h>

#include <vector>

class GUIElement;

class GUIInterface
{
private:
	ID2D1Factory *m_d2dFactory;
	ID2D1RenderTarget *m_renderTarget;

	IDWriteFactory *m_dwFactory;

	std::vector<GUIElement*> m_elements;

	void add(GUIElement * e);
	void remove(GUIElement * e);

	friend class GUIElement;

public:
	GUIInterface(IDXGISwapChain * swapChain);
	~GUIInterface();

	IDWriteFactory * getDWFactory();
	ID2D1RenderTarget * getD2DRenderTarget();

	void render();
};

extern GUIInterface * gui;
