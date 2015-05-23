#include "Engine.h"
#include <windows.h>
#include <windowsx.h>

#include "ObjectManager.h"
#include "Scene.h"
#include "PhysicsScene.h"
#include "Input.h"

// global declarations
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context

PxFoundation *physicsFoundation;
PxPhysics *physics;

Engine * engine;

Engine::Engine(HWND hWnd) : m_time(0.f), m_delta(1.f / 60.f), m_running(true), m_hWnd(hWnd), m_physicsScene(nullptr)
{
	engine = this; // set singleton instance

	initDirect3D();
	initPhysX();

	m_input = new Input(m_hWnd);
	m_objectManager = new ObjectManager();
}

Engine::~Engine()
{
	m_activeScene.reset();
	delete m_objectManager;

	m_cudaContextManager->release();
	m_physicsCpuDispatcher->release();
	physics->release();
	physicsFoundation->release();

	// close and release all existing COM objects
	m_rasterizerState->Release();
	m_depthStencilState->Release();
	m_depthStencilView->Release();
	m_depthStencilBuffer->Release();
	m_swapchain->Release();
	m_backbuffer->Release();

	dev->Release();
	devcon->Release();
}

void Engine::initDirect3D()
{
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
}

void Engine::initPhysX()
{
	physicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_physicsAllocatorCallback, m_physicsErrorCallback);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *physicsFoundation, PxTolerancesScale());

	m_physicsCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	PxCudaContextManagerDesc cudaDesc;
	cudaDesc.graphicsDevice = dev;
	cudaDesc.interopMode = PxCudaInteropMode::D3D11_INTEROP;
	m_cudaContextManager = PxCreateCudaContextManager(*physicsFoundation, cudaDesc, nullptr);
}

void Engine::destroyScene()
{
}

void Engine::setScene(Scene * s)
{
	m_activeScene.reset(s);
}

float Engine::getTime() const
{
	return m_time.count();
}

float Engine::getDelta() const
{
	return m_delta.count();
}

void Engine::stop()
{
	PostQuitMessage(0);
}

WPARAM Engine::run()
{
	using namespace std::chrono;
	system_clock::time_point currentTime = system_clock::now(), newTime;
	fsec frameTime(0.0f), accumulator(0.0f);
	const fsec maxFrameTime(0.25);

	//float alpha;

	MSG msg;

	while (m_running) {
		newTime = system_clock::now();
		frameTime = newTime - currentTime;
		if (frameTime > maxFrameTime) frameTime = maxFrameTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= m_delta) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT)
				m_running = false;

			update();

			m_time += m_delta;
			accumulator -= m_delta;
		}

		// we have to explicitly call the appropriate division operator due to a bug in Microsoft's implementation of the C++ standard...
		// alpha = std::chrono::operator/<fsec::rep, fsec::period, fsec::rep, fsec::period>(accumulator, m_delta);

		// or just do this, because it basically does the same thing!
		render(accumulator.count() / m_delta.count());
	}

	return msg.wParam;
}

void Engine::update()
{
	input->preUpdate();

	if (m_physicsScene) {
		PxScene * pxs = m_physicsScene->getPhysXObj();
		pxs->simulate(m_delta.count());
		pxs->fetchResults(true);
	}

	if (m_activeScene) m_activeScene->update(); // update active scene object
	m_objectManager->update(); // update all registered game objects

	m_input->update();
}

void Engine::render(float alpha)
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
	objects->draw(alpha);

	// switch the back buffer and the front buffer
	m_swapchain->Present(0, 0);
}

LRESULT Engine::processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		stop();
		return 0;
	} break;
	case WM_INPUT:
	{
		UINT dwSize;

		// get required size of input buffer
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

		// create buffer
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL) return 0;

		// fill buffer with input data
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

		// pass data to input module
		input->handle((RAWINPUT*)lpb);

		// free buffer
		delete[] lpb;
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
