#include "Graphics.h"
#include "constants.h"
#include "utility.h"
#include "Objects.h"
#include "Shader.h"
#include "Camera.h"
#include "Renderer.h"

#include <limits>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

ID3D11Device *dev;
ID3D11DeviceContext *devcon;

GraphicsInterface * graphics;

#define CB_Application 0
#define CB_Frame 1
#define CB_Object 2
#define CB_Material 3

GraphicsInterface::GraphicsInterface(HWND hWnd) : m_hWnd(hWnd), m_cam(nullptr), m_shadowMapDimension(2048), m_vSync(true)
{
	graphics = this;

	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
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

#if _DEBUG
	// acquire debug interface
	dev->QueryInterface(IID_PPV_ARGS(&m_debug));

	ComPtr<ID3D11InfoQueue> d3dInfoQueue;
	HRESULT hr = m_debug->QueryInterface(IID_PPV_ARGS(&d3dInfoQueue));
	if (SUCCEEDED(hr)) {
		D3D11_MESSAGE_ID hide[] =
		{
			D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET,
		};
		D3D11_INFO_QUEUE_FILTER filter;
		memset(&filter, 0, sizeof(filter));
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif

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

#if _DEBUG
	m_debug->SetSwapChain(m_swapchain);
#endif

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	m_swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

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

	createShadowResources();

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

	cbd.ByteWidth = sizeof(XMFLOAT4X4);
	dev->CreateBuffer(&cbd, NULL, &m_shadowCB);

	setConstantBuffers();
}

GraphicsInterface::~GraphicsInterface()
{
	m_shadowCB->Release();
	m_constantBuffers[CB_Application]->Release();
	m_constantBuffers[CB_Frame]->Release();
	m_constantBuffers[CB_Object]->Release();

	delete m_shadowPassShader;

	m_shadowRasterizerState->Release();
	m_shadowSampler->Release();
	m_shadowResourceView->Release();
	m_shadowDepthView->Release();
	m_shadowMap->Release();

	m_rasterizerState->Release();
	m_depthStencilState->Release();
	m_depthStencilView->Release();
	m_depthStencilBuffer->Release();
	m_swapchain->Release();
	m_backbuffer->Release();

	dev->Release();
	devcon->Release();

#if _DEBUG
	m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	m_debug->Release();
#endif
}

void GraphicsInterface::createShadowResources()
{
	D3D11_TEXTURE2D_DESC smd;
	ZeroMemory(&smd, sizeof(D3D11_TEXTURE2D_DESC));
	smd.Format = DXGI_FORMAT_R24G8_TYPELESS;
	smd.MipLevels = 1;
	smd.ArraySize = 1;
	smd.SampleDesc.Count = 1;
	smd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	smd.Height = static_cast<UINT>(m_shadowMapDimension);
	smd.Width = static_cast<UINT>(m_shadowMapDimension);

	HRESULT hr = dev->CreateTexture2D(&smd, nullptr, &m_shadowMap);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	hr = dev->CreateDepthStencilView(m_shadowMap, &dsvd, &m_shadowDepthView);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.Texture2D.MipLevels = 1;

	hr = dev->CreateShaderResourceView(m_shadowMap, &srvd, &m_shadowResourceView);

	D3D11_SAMPLER_DESC csd;
	ZeroMemory(&csd, sizeof(D3D11_SAMPLER_DESC));
	csd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	csd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	csd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	csd.BorderColor[0] = 1.0f;
	csd.BorderColor[1] = 1.0f;
	csd.BorderColor[2] = 1.0f;
	csd.BorderColor[3] = 1.0f;
	csd.MinLOD = 0.f;
	csd.MaxLOD = D3D11_FLOAT32_MAX;
	csd.MipLODBias = 0.f;
	csd.MaxAnisotropy = 0;
	csd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	csd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

	dev->CreateSamplerState(&csd, &m_shadowSampler);

	D3D11_RASTERIZER_DESC srsd;
	ZeroMemory(&srsd, sizeof(D3D11_RASTERIZER_DESC));
	srsd.CullMode = D3D11_CULL_FRONT;
	srsd.FillMode = D3D11_FILL_SOLID;
	srsd.DepthClipEnable = true;
	srsd.DepthBias = 0;
	srsd.DepthBiasClamp = 0.09f;
	srsd.SlopeScaledDepthBias = 0.2f;

	dev->CreateRasterizerState(&srsd, &m_shadowRasterizerState);

	ZeroMemory(&m_shadowViewport, sizeof(D3D11_VIEWPORT));
	m_shadowViewport.Height = m_shadowMapDimension;
	m_shadowViewport.Width = m_shadowMapDimension;
	m_shadowViewport.MinDepth = 0.f;
	m_shadowViewport.MaxDepth = 1.f;

	m_shadowPassShader = new Shader(L"ShadowPass");
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
	shadowPass();
	mainPass();
}

void GraphicsInterface::present()
{
	m_swapchain->Present(m_vSync ? 1 : 0, 0);
}

XMMATRIX GraphicsInterface::calcLightMatrix()
{
	// NOTE: some of these calculations might be wrong or unnecessary, but since shadow maps are such a complex topic, I didn't want to spend too much time on this...

	// calculate light view matrix
	XMMATRIX lightView = XMMatrixLookToLH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMLoadFloat3(&m_frame.light.direction), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	XMVECTOR minv = XMVectorReplicate(FLT_MAX), maxv = XMVectorReplicate(-FLT_MAX);

	/*
	XMMATRIX camProj = m_cam->getProjectionMatrix();
	PxMat44 ivt(m_cam->getTransform()->getTransform());
	XMMATRIX m = XMMatrixInverse(&XMMatrixDeterminant(camProj), camProj) * XMLoadFloat4x4((XMFLOAT4X4*)&ivt) * lightView;

	XMVECTOR v;
	
	float c[] = {-1.0f, 1.0f};
	for (float x : c) {
		for (float y : c) {
			for (float z : c) {
				v = XMVector4Transform(XMVectorSet(x, y, z, 1.0f), m);
				v = XMVectorDivide(v, XMVectorSplatW(v));
				minv = XMVectorMin(v, minv);
				maxv = XMVectorMax(v, maxv);
			}
		}
	}
	*/

	// for now, we just calculate the light projection matrix in such a way, that the entire scene fits into the shadow map
	XMVECTOR sceneMin, sceneMax;
	objects->getSceneAABB(sceneMin, sceneMax);
	transformAABB(lightView, sceneMin, sceneMax, sceneMin, sceneMax);
	
	//minv = XMVectorMax(minv, sceneMin);
	//maxv = XMVectorMin(maxv, sceneMax);
	
	minv = sceneMin;
	maxv = sceneMax;

	// snap to shadow texels
	float bound = max(XMVectorGetX(maxv) - XMVectorGetX(minv), XMVectorGetY(maxv) - XMVectorGetY(minv));
	float up = bound / m_shadowMapDimension;
	XMVECTOR unitsPerTexel = XMVectorSet(up, up, 1.0f, 0.0f);

	minv = XMVectorFloor(minv / unitsPerTexel) * unitsPerTexel;
	maxv = XMVectorFloor(maxv / unitsPerTexel) * unitsPerTexel;

	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(XMVectorGetX(minv), XMVectorGetX(maxv), XMVectorGetY(minv), XMVectorGetY(maxv), XMVectorGetZ(minv), XMVectorGetZ(maxv));

	return lightView * lightProj;
}

void GraphicsInterface::shadowPass()
{
	if (m_cam && objects->m_objects.size() > 0) {
		devcon->OMSetRenderTargets(0, nullptr, m_shadowDepthView);
		devcon->ClearDepthStencilView(m_shadowDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		devcon->RSSetState(m_shadowRasterizerState);
		devcon->RSSetViewports(1, &m_shadowViewport);

		XMMATRIX vp = calcLightMatrix();
		XMStoreFloat4x4(&m_frame.lightVP, vp);
		PxMat44 wm;

		for (GameObject * obj : objects->m_objects) {
			if (obj->getCastShadow()) {
				const Renderer * r = obj->getRenderer();
				if (r && r->getEnabled()) {
					const Transform * t = obj->getTransform();
					if (t && t->getEnabled()) {
						wm = t->getMatrix();
						XMStoreFloat4x4(&m_shadowMVP, XMLoadFloat4x4((XMFLOAT4X4*)&wm) * vp);
						devcon->UpdateSubresource(m_shadowCB, 0, NULL, &m_shadowMVP, 0, 0);
						devcon->VSSetConstantBuffers(0, 1, &m_shadowCB);

						r->setStatesShadow();
						r->draw();
					}
				}
			}
		}
	}
}

void GraphicsInterface::mainPass()
{
	XMFLOAT4 clearColor = m_cam ? m_cam->getBackColor() : XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	devcon->ClearRenderTargetView(m_backbuffer, (float*)&clearColor);
	devcon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (m_cam && objects->m_objects.size() > 0) {
		devcon->RSSetState(m_rasterizerState);
		devcon->RSSetViewports(1, &m_viewport);

		devcon->OMSetRenderTargets(1, &m_backbuffer, m_depthStencilView);
		devcon->OMSetDepthStencilState(m_depthStencilState, 1);

		m_cam->setAspectRatio(16.f / 9.f);
		XMMATRIX proj = m_cam->getProjectionMatrix();
		XMStoreFloat4x4(&m_app.projection, proj);

		PxMat44 vm(m_cam->getTransform()->getTransform().getInverse());
		XMMATRIX view = XMLoadFloat4x4((XMFLOAT4X4*)&vm);
		XMStoreFloat4x4(&m_frame.view, view);
		XMStoreFloat4(&m_frame.cameraPosition, XMLoadFloat3((XMFLOAT3*)&m_cam->getTransform()->getPosition()));

		devcon->UpdateSubresource(m_constantBuffers[CB_Application], 0, NULL, &m_app, 0, 0);
		devcon->UpdateSubresource(m_constantBuffers[CB_Frame], 0, NULL, &m_frame, 0, 0);

		XMMATRIX world, vp = view * proj;
		PxMat44 wm;

		devcon->PSSetShaderResources(4, 1, &m_shadowResourceView);
		devcon->PSSetSamplers(4, 1, &m_shadowSampler);

		for (GameObject * obj : objects->m_objects) {
			const Renderer * r = obj->getRenderer();
			if (r && r->getEnabled()) {
				const Transform * t = obj->getTransform();
				if (t && t->getEnabled()) {
					wm = t->getMatrix();
					world = XMLoadFloat4x4((XMFLOAT4X4*)&wm);
					XMStoreFloat4x4(&m_object.world, world);
					XMStoreFloat4x4(&m_object.mvp, world * vp);
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

		ID3D11ShaderResourceView * nullview = nullptr;
		devcon->PSSetShaderResources(4, 1, &nullview);
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

Shader * GraphicsInterface::getShadowShader()
{
	return m_shadowPassShader;
}

IDXGISwapChain * GraphicsInterface::getSwapChain()
{
	return m_swapchain;
}

Camera * GraphicsInterface::getCamera()
{
	return m_cam;
}
