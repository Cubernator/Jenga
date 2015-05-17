#pragma once

#include "DirectX.h"
#include "PhysX.h"
#include <chrono>
#include <memory>

#include "ObjectManager.h"
#include "Scene.h"
#include "PhysicsScene.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef std::chrono::duration<float> fsec;

class Engine
{
private:
	fsec m_time, m_delta;

	bool m_running;

	ObjectManager * m_objectManager;

	std::unique_ptr<Scene> m_activeScene;
	PhysicsScene * m_physicsScene;

	HWND m_hWnd;

	IDXGISwapChain *m_swapchain;
	ID3D11RenderTargetView *m_backbuffer;

	ID3D11Texture2D *m_depthStencilBuffer;
	ID3D11DepthStencilView *m_depthStencilView;
	ID3D11DepthStencilState *m_depthStencilState;

	ID3D11RasterizerState *m_rasterizerState;

	D3D11_VIEWPORT m_viewport;

	PxDefaultErrorCallback m_physicsErrorCallback;
	PxDefaultAllocator m_physicsAllocatorCallback;
	PxDefaultCpuDispatcher *m_physicsCpuDispatcher;
	PxCudaContextManager *m_cudaContextManager;

	void initDirect3D();
	void initPhysX();

	void update();
	void render(float alpha);

	void destroyScene();
	void setScene(Scene * s);

	template<class SceneType, bool, class... Args>
	struct enterSceneImpl
	{
		static SceneType * construct(Engine * e, Args&&... args)
		{
			return new SceneType(std::forward<Args>(args)...);
		}
	};

	template<class SceneType, class... Args>
	struct enterSceneImpl<SceneType, true, Args...>
	{
		static SceneType * construct(Engine * e, Args&&... args)
		{
			PxSceneDesc desc(physics->getTolerancesScale());
			desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
			desc.cpuDispatcher = e->m_physicsCpuDispatcher;
			desc.gpuDispatcher = e->m_cudaContextManager->getGpuDispatcher();
			desc.filterShader = &PxDefaultSimulationFilterShader;
			SceneType * newScene = new SceneType(desc, std::forward<Args>(args)...);
			e->m_physicsScene = newScene;
			return newScene;
		}
	};

public:
	Engine(HWND hWnd);
	~Engine();

	WPARAM run();

	LRESULT processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	float getTime() const;
	float getDelta() const;

	void stop();

	template<typename SceneType, class... Args>
	void enterScene(Args&&... args)
	{
		m_activeScene.reset(); // destroy current scene
		if (m_physicsScene) m_physicsScene = nullptr;

		// the wonders of C++ template programming at work right here... *sigh*
		SceneType * s = enterSceneImpl<SceneType, std::is_base_of<PhysicsScene, SceneType>::value, Args...>::construct(this, std::forward<Args>(args)...);
		m_activeScene.reset(s); // set new scene
	}
};

extern Engine * engine; // engine singleton

