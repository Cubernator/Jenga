#include "Graphics.h"
#include "constants.h"
#include "Objects.h"

GraphicsInterface * graphics;

#define CB_Application 0
#define CB_Frame 1
#define CB_Object 2
#define CB_Material 3

GraphicsInterface::GraphicsInterface(HWND hWnd) : m_hWnd(hWnd), m_cam(nullptr)
{
	graphics = this;

	UINT flags = 0;
#if _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// create D3D device
	D3D11CreateDevice(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&dev,
		NULL,
		&devcon);

	// acquire debug interface
	dev->QueryInterface(IID_PPV_ARGS(&m_debug));

	// set multisampling to 4x
	UINT samples = 4, quality;

	// check supported multisampling quality level
	dev->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, samples, &quality);
	if (quality > 0) quality -= 1;
	else samples = 1;

	// create swap chain
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = m_hWnd;
	scd.SampleDesc.Count = samples;
	scd.SampleDesc.Quality = quality;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = TRUE;

	IDXGIFactory * pFactory;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
	pFactory->CreateSwapChain(dev, &scd, &m_swapchain);
	pFactory->Release();

	m_debug->SetSwapChain(m_swapchain);

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &m_backbuffer);
	pBackBuffer->Release();

	// create depth stencil buffer
	D3D11_TEXTURE2D_DESC dsbd;
	ZeroMemory(&dsbd, sizeof(D3D11_TEXTURE2D_DESC));

	dsbd.ArraySize = 1;
	dsbd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsbd.CPUAccessFlags = 0;
	dsbd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsbd.Width = SCREEN_WIDTH;
	dsbd.Height = SCREEN_HEIGHT;
	dsbd.MipLevels = 1;
	dsbd.SampleDesc.Count = samples;
	dsbd.SampleDesc.Quality = quality;
	dsbd.Usage = D3D11_USAGE_DEFAULT;

	dev->CreateTexture2D(&dsbd, nullptr, &m_depthStencilBuffer);

	// create depth stencil view from depth stencil buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	dev->CreateDepthStencilView(m_depthStencilBuffer, &dsvd, &m_depthStencilView);

	// create depth stencil state
	D3D11_DEPTH_STENCIL_DESC dssd;
	ZeroMemory(&dssd, sizeof(D3D11_DEPTH_STENCIL_DESC));

	dssd.DepthEnable = TRUE;
	dssd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssd.DepthFunc = D3D11_COMPARISON_LESS;
	dssd.StencilEnable = FALSE;

	dev->CreateDepthStencilState(&dssd, &m_depthStencilState);

	// create rasterizer state
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));

	rd.AntialiasedLineEnable = TRUE;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = TRUE;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.FrontCounterClockwise = FALSE;
	rd.MultisampleEnable = TRUE;
	rd.ScissorEnable = FALSE;
	rd.SlopeScaledDepthBias = 0.0f;

	dev->CreateRasterizerState(&rd, &m_rasterizerState);

	// Set the viewport
	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = SCREEN_WIDTH;
	m_viewport.Height = SCREEN_HEIGHT;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(D3D11_BUFFER_DESC));

	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	cbd.Usage = D3D11_USAGE_DEFAULT;

	cbd.ByteWidth = sizeof(cbPerApplication);
	dev->CreateBuffer(&cbd, NULL, &m_constantBuffers[CB_Application]);

	cbd.ByteWidth = sizeof(cbPerFrame);
	dev->CreateBuffer(&cbd, NULL, &m_constantBuffers[CB_Frame]);

	cbd.ByteWidth = sizeof(cbPerObject);
	dev->CreateBuffer(&cbd, NULL, &m_constantBuffers[CB_Object]);

	m_constantBuffers[CB_Material] = nullptr;

	setConstantBuffers();
}

GraphicsInterface::~GraphicsInterface()
{
	m_constantBuffers[CB_Application]->Release();
	m_constantBuffers[CB_Frame]->Release();
	m_constantBuffers[CB_Object]->Release();

	m_rasterizerState->Release();
	m_depthStencilState->Release();
	m_depthStencilView->Release();
	m_depthStencilBuffer->Release();
	m_swapchain->Release();
	m_backbuffer->Release();

	dev->Release();
	devcon->Release();

	m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	m_debug->Release();
}

void GraphicsInterface::setConstantBuffers()
{
	UINT cbc = 3;
	if (m_constantBuffers[CB_Material]) cbc = 4;
	devcon->VSSetConstantBuffers(0, cbc, m_constantBuffers);
	devcon->PSSetConstantBuffers(0, cbc, m_constantBuffers);
}

void GraphicsInterface::render(float alpha)
{
	// clear the back buffer
	XMFLOAT4 clearColor(0.f, 0.2f, 0.4f, 1.f);
	devcon->ClearRenderTargetView(m_backbuffer, (float*)&clearColor);
	// clear depth stencil
	devcon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// set rasterizer state
	devcon->RSSetState(m_rasterizerState);
	// set viewport
	devcon->RSSetViewports(1, &m_viewport);

	// render to backbuffer
	devcon->OMSetRenderTargets(1, &m_backbuffer, m_depthStencilView);
	// set depth stencil state
	devcon->OMSetDepthStencilState(m_depthStencilState, 1);

	// draw all objects
	drawObjects(alpha);

	// switch the back buffer and the front buffer
	m_swapchain->Present(0, 0);
}

void GraphicsInterface::drawObjects(float alpha)
{
	if (m_cam) {
		m_cam->setAspectRatio(16.f / 9.f);
		XMMATRIX proj = m_cam->getProjectionMatrix();
		XMStoreFloat4x4(&m_app.projection, proj);

		PxMat44 vm(m_cam->getTransform()->getTransform().getInverse());
		XMMATRIX view = XMLoadFloat4x4((XMFLOAT4X4*)&vm);
		XMStoreFloat4x4(&m_frame.view, view);

		devcon->UpdateSubresource(m_constantBuffers[CB_Application], 0, NULL, &m_app, 0, 0);
		devcon->UpdateSubresource(m_constantBuffers[CB_Frame], 0, NULL, &m_frame, 0, 0);

		XMMATRIX world;
		PxMat44 wm;

		for (GameObject * obj : objects->m_objects) {
			const Renderer * r = obj->getRenderer();
			if (r && r->getEnabled()) {
				const Transform * t = obj->getTransform();
				if (t && t->getEnabled()) {
					wm = t->getMatrix();
					world = XMLoadFloat4x4((XMFLOAT4X4*)&wm);
					XMStoreFloat4x4(&m_object.world, world);
					XMStoreFloat4x4(&m_object.mvp, world * view * proj);
					devcon->UpdateSubresource(m_constantBuffers[CB_Object], 0, NULL, &m_object, 0, 0);

					ID3D11Buffer * matBuffer = r->getConstantBuffer();
					if (matBuffer != m_constantBuffers[CB_Material]) {
						m_constantBuffers[CB_Material] = matBuffer;
					}
				}

				setConstantBuffers();
				r->setStates();

				r->draw();
			}
		}
	}
}

void GraphicsInterface::setCamera(Camera * cam)
{
	m_cam = cam;
}

void GraphicsInterface::setLight(const Light& l)
{
	m_frame.light = l;
}
