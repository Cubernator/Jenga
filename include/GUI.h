#pragma once

#include "Graphics.h"

#include <d2d1_1.h>
#include <dwrite_2.h>
#include <wincodec.h>

#include <wrl.h>

#include <vector>

using namespace Microsoft::WRL;

class GUIElement;

class GUIInterface
{
private:
	ComPtr<ID2D1Factory> m_d2dFactory;
	ComPtr<ID2D1RenderTarget> m_renderTarget;
	ComPtr<IWICImagingFactory> m_wicFactory;
	ComPtr<IDWriteFactory> m_dwFactory;
	ComPtr<IDWriteTextFormat> m_defaultFormat;

	std::wstring m_localeName;
	std::vector<GUIElement*> m_elements;

public:
	GUIInterface(IDXGISwapChain * swapChain);

	ID2D1RenderTarget * getD2DRenderTarget();

	ID2D1Bitmap * createSharedBitmap(Texture2D * texture);
	HRESULT loadBitmap(const std::wstring& fileName, ID2D1Bitmap **bitmap);
	ID2D1Bitmap * loadBitmap(const std::wstring& fileName);

	IDWriteTextFormat * getDefaultFormat();
	IDWriteTextFormat * createFormat(const std::wstring& family, float size, DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL);

	void add(GUIElement * e);
	void remove(GUIElement * e);

	void render();
	void update();
};

extern GUIInterface * gui;
