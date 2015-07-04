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

	CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)m_wicFactory.GetAddressOf());

	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)m_dwFactory.GetAddressOf());

	auto l = GetUserDefaultLocaleName(NULL, 0);
	if (l) {
		wchar_t * buf = new wchar_t[l];
		GetUserDefaultLocaleName(buf, l);
		m_localeName = buf;
		delete[] buf;
	}

	m_defaultFormat = createFormat(L"verdana", 18);
}

IDWriteTextFormat * GUIInterface::createFormat(const std::wstring& family, float size,
	DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch)
{
	IDWriteTextFormat * newFormat = nullptr;
	m_dwFactory->CreateTextFormat(family.c_str(), nullptr, weight, style, stretch, size, m_localeName.c_str(), &newFormat);
	return newFormat;
}

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

ID2D1RenderTarget * GUIInterface::getD2DRenderTarget()
{
	return m_renderTarget.Get();
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

void GUIInterface::update()
{
	for (GUIElement * e : m_elements) {
		e->update();
	}
}