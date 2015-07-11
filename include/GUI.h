#pragma once

#include <vector>

#include "DirectX.h"
#include "GUIElement.h"

class GUIInterface
{
private:
	ComPtr<ID2D1Factory> m_d2dFactory;
	ComPtr<ID2D1RenderTarget> m_renderTarget;
	ComPtr<IWICImagingFactory> m_wicFactory;
	ComPtr<IDWriteFactory> m_dwFactory;
	ComPtr<IDWriteTextFormat> m_defaultFormat;
	ComPtr<ID2D1SolidColorBrush> m_textBrush;

	std::wstring m_localeName;
	std::vector<GUIElement*> m_elements;

	bool m_elementsNotSorted;

public:
	GUIInterface(IDXGISwapChain * swapChain);

	HRESULT loadBitmap(const std::wstring& fileName, ID2D1Bitmap **bitmap);
	ID2D1Bitmap * loadBitmap(const std::wstring& fileName);

	IDWriteTextFormat * getDefaultFormat();
	HRESULT createFormat(const std::wstring& family, float size, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch, IDWriteTextFormat **format);
	HRESULT createFormat(const std::wstring& family, float size, IDWriteTextFormat **format);

	void add(GUIElement * e);
	void remove(GUIElement * e);

	void render();
	void update();

	void drawRectangle(const D2D_RECT_F& rect, const D2D_COLOR_F& color);
	void drawText(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const D2D_COLOR_F& color, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE);
	void drawImage(const D2D_RECT_F& rect, ID2D1Bitmap * image);
	void drawControlImage(const D2D_RECT_F& rect, ID2D1Bitmap * image);

	void sortElements();
};

extern GUIInterface * gui;
