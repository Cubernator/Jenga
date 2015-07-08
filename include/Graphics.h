#pragma once

#include "DirectX.h"
#include "Light.h"

struct cbPerApplication
{
	XMFLOAT4X4 projection;
};

struct cbPerFrame
{
	XMFLOAT4X4 view;
	XMFLOAT4X4 lightVP;
	XMFLOAT4 cameraPosition;
	Light light;
};

struct cbPerObject
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 mvp;
	bool receiveShadow;
	XMFLOAT3 pad;
};

class Camera;
class Shader;

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

	float m_shadowMapDimension;
	ID3D11Texture2D *m_shadowMap;
	ID3D11DepthStencilView *m_shadowDepthView;
	ID3D11ShaderResourceView *m_shadowResourceView;
	ID3D11SamplerState *m_shadowSampler;
	ID3D11RasterizerState *m_shadowRasterizerState;
	D3D11_VIEWPORT m_shadowViewport;

	Shader * m_shadowPassShader;

	Camera * m_cam;

	cbPerApplication m_app;
	cbPerFrame m_frame;
	cbPerObject m_object;

	XMFLOAT4X4 m_shadowMVP;

	bool m_vSync;

	ID3D11Buffer *m_constantBuffers[4];
	ID3D11Buffer *m_shadowCB;

	void createShadowResources();

	void setConstantBuffers();

	XMMATRIX calcLightMatrix();

	void drawObjects();
	void shadowPass();
	void mainPass();

public:
	GraphicsInterface(HWND hWnd);
	~GraphicsInterface();

	IDXGISwapChain * getSwapChain();

	Shader * getShadowShader();

	void render(float alpha);
	void present();

	void setCamera(Camera * cam);
	void setLight(const Light& l);

	Camera * getCamera();
};

extern GraphicsInterface * graphics;