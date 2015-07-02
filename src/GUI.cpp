#include "GUI.h"
#include "GUIElement.h"

#include <algorithm>

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")

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
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, fo, &m_d2dFactory);

	IDXGISurface * surface;
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
	D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);
	hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(surface, rtp, &m_renderTarget);
	surface->Release();

	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_dwFactory));
}

GUIInterface::~GUIInterface()
{
	m_dwFactory->Release();
	m_renderTarget->Release();
	m_d2dFactory->Release();
}

void GUIInterface::add(GUIElement * e)
{
	m_elements.push_back(e);
}

void GUIInterface::remove(GUIElement * e)
{
	m_elements.erase(std::remove(m_elements.begin(), m_elements.end(), e), m_elements.end());
}

IDWriteFactory * GUIInterface::getDWFactory()
{
	return m_dwFactory;
}

ID2D1RenderTarget * GUIInterface::getD2DRenderTarget()
{
	return m_renderTarget;
}

void GUIInterface::render()
{
	m_renderTarget->BeginDraw();

	for (const GUIElement * e : m_elements) {
		e->draw();
	}

	m_renderTarget->EndDraw();
}