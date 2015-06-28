#pragma once

#include "DirectX.h"
#include "Camera.h"
#include "Light.h"

struct cbPerApplication
{
	XMFLOAT4X4 projection;
};

struct cbPerFrame
{
	XMFLOAT4X4 view;
	Light light;
};

struct cbPerObject
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 mvp;
};

class GraphicsInterface
{
private:
	HWND m_hWnd;

	ID3D11Debug * m_debug;

	IDXGISwapChain *m_swapchain;
	ID3D11RenderTargetView *m_backbuffer;

	ID3D11Texture2D *m_depthStencilBuffer;
	ID3D11DepthStencilView *m_depthStencilView;
	ID3D11DepthStencilState *m_depthStencilState;

	ID3D11RasterizerState *m_rasterizerState;

	D3D11_VIEWPORT m_viewport;

	ID3D11Texture2D *m_shadowMap;

	Camera * m_cam;

	cbPerApplication m_app;
	cbPerFrame m_frame;
	cbPerObject m_object;

	ID3D11Buffer *m_constantBuffers[4];

	void setConstantBuffers();

	void drawObjects(float alpha);

public:
	GraphicsInterface(HWND hWnd);
	~GraphicsInterface();

	void render(float alpha);

	void setCamera(Camera * cam);
	void setLight(const Light& l);
};

extern GraphicsInterface * graphics;