#include "GUI.h"
#include "GUIElement.h"

#include <wincodec.h>

#include <algorithm>

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"Windowscodecs.lib")

GUIInterface * gui;

GUIInterface::GUIInterface(IDXGISwapChain * swapChain)
{
	gui = this;

	D2D1_FACTORY_OPTIONS fo;
#if _DEBUG
	fo.debugLevel = D2D1_DEBUG_LEVEL::D2D1_DEBUG_LEVEL_INFORMATION;
#else
	fo.debugLevel = D2D1_DEBUG_LEVEL::D2D1_DEBUG_LEVEL_NONE;
#endif
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, fo, m_d2dFactory.GetAddressOf());

	IDXGISurface * surface;
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
	D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);
	hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(surface, rtp, m_renderTarget.GetAddressOf());
	surface->Release();

	CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, &m_wicFactory);

	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)m_dwFactory.GetAddressOf());

	auto l = GetUserDefaultLocaleName(NULL, 0);
	if (l) {
		wchar_t * buf = new wchar_t[l];
		GetUserDefaultLocaleName(buf, l);
		m_localeName = buf;
		delete[] buf;
	}

	createFormat(L"verdana", 18, &m_defaultFormat);

	m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_textBrush);
}

HRESULT GUIInterface::createFormat(const std::wstring& family, float size, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch, IDWriteTextFormat **format)
{
	return m_dwFactory->CreateTextFormat(family.c_str(), nullptr, weight, style, stretch, size, m_localeName.c_str(), format);
}

HRESULT GUIInterface::createFormat(const std::wstring& family, float size, IDWriteTextFormat **format)
{
	return createFormat(family, size, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, format);
}
/*
ID2D1Bitmap * GUIInterface::createSharedBitmap(Texture2D * texture)
{
	// NOTE: this doesn't work.
	ID2D1Bitmap * newBitmap;
	IDXGISurface * surface;
	HRESULT hr = texture->getTexture()->QueryInterface(IID_PPV_ARGS(&surface));
	m_renderTarget->CreateSharedBitmap(__uuidof(*surface), surface, NULL, &newBitmap);
	surface->Release();
	return newBitmap;
}
*/
HRESULT GUIInterface::loadBitmap(const std::wstring& fileName, ID2D1Bitmap **bitmap)
{
	ComPtr<IWICBitmapDecoder> decoder;
	ComPtr<IWICBitmapFrameDecode> source;
	ComPtr<IWICFormatConverter> converter;

	HRESULT hr = m_wicFactory->CreateDecoderFromFilename(fileName.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf());

	if (SUCCEEDED(hr))
		hr = decoder->GetFrame(0, &source);
	if (SUCCEEDED(hr))
		hr = m_wicFactory->CreateFormatConverter(&converter);
	if (SUCCEEDED(hr))
		hr = converter->Initialize(source.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	if (SUCCEEDED(hr))
		hr = m_renderTarget->CreateBitmapFromWicBitmap(converter.Get(), NULL, bitmap);

	return hr;
}

ID2D1Bitmap * GUIInterface::loadBitmap(const std::wstring& fileName)
{
	ID2D1Bitmap * newBitmap;
	loadBitmap(fileName, &newBitmap);
	return newBitmap;
}

void GUIInterface::add(GUIElement * e)
{
	m_elements.push_back(e);
}

void GUIInterface::remove(GUIElement * e)
{
	m_elements.erase(std::remove(m_elements.begin(), m_elements.end(), e), m_elements.end());
}

IDWriteTextFormat * GUIInterface::getDefaultFormat()
{
	return m_defaultFormat.Get();
}

void GUIInterface::render()
{
	m_renderTarget->BeginDraw();

	for (const GUIElement * e : m_elements) {
		e->draw();
	}

	m_renderTarget->EndDraw();
}

void GUIInterface::drawRectangle(const D2D_RECT_F& rect, const D2D_COLOR_F& color)
{
	m_textBrush->SetColor(color);
	m_renderTarget->FillRectangle(rect, m_textBrush.Get());
}

void GUIInterface::drawText(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const D2D_COLOR_F& color)
{
	m_textBrush->SetColor(color);
	m_renderTarget->DrawTextW(text.c_str(), text.size(), format, rect, m_textBrush.Get());
}

void GUIInterface::drawImage(const D2D_RECT_F& rect, ID2D1Bitmap * image)
{
	m_renderTarget->DrawBitmap(image, rect);
}

void GUIInterface::drawControlImage(const D2D_RECT_F& rect, ID2D1Bitmap * image)
{
	D2D_SIZE_U s = image->GetPixelSize();
	float w = (float)s.width, h = (float)s.height;
	float hw = w / 2.0f, hh = h / 2.0f;

	// stretch the button image out
	m_renderTarget->DrawBitmap(image, { rect.left, rect.top, rect.left + hw, rect.top + hh }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { 0, 0, hw, hh }); // upper left
	m_renderTarget->DrawBitmap(image, { rect.left + hw, rect.top, rect.right - hw, rect.top + hh }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { hw, 0, hw + 1, hh }); // upper center
	m_renderTarget->DrawBitmap(image, { rect.right - hw, rect.top, rect.right, rect.top + hh }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { hw, 0, w, hh }); // upper right
	m_renderTarget->DrawBitmap(image, { rect.left, rect.top + hh, rect.left + hw, rect.bottom - hh }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { 0, hh, hw, hh + 1 }); // middle left
	m_renderTarget->DrawBitmap(image, { rect.left + hw, rect.top + hh, rect.right - hw, rect.bottom - hh }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { hw, hh, hw + 1, hh + 1 }); // middle center
	m_renderTarget->DrawBitmap(image, { rect.right - hw, rect.top + hh, rect.right, rect.bottom - hh }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { hw, hh, w, hh + 1 }); // middle right
	m_renderTarget->DrawBitmap(image, { rect.left, rect.bottom - hh, rect.left + hw, rect.bottom }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { 0, hh, hw, h }); // bottom left
	m_renderTarget->DrawBitmap(image, { rect.left + hw, rect.bottom - hh, rect.right - hw, rect.bottom }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { hw, hh, hw + 1, h }); // bottom center
	m_renderTarget->DrawBitmap(image, { rect.right - hw, rect.bottom - hh, rect.right, rect.bottom }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { hw, hh, w, h }); // bottom right
}

void GUIInterface::update()
{
	for (GUIElement * e : m_elements) {
		e->update();
	}
}