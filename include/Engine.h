#pragma once

#include "DirectX.h"
#include <chrono>
#include <memory>

#include "Physics.h"

#include "constants.h"

typedef std::chrono::duration<float> fsec;

class Input;
class ObjectManager;
class Scene;

class Engine
{
private:
	fsec m_time, m_delta;

	bool m_running;

	ObjectManager * m_objectManager;
	Input * m_input;
	PhysicsInterface * m_physics;

	std::unique_ptr<Scene> m_activeScene;

	HWND m_hWnd;

	IDXGISwapChain *m_swapchain;
	ID3D11RenderTargetView *m_backbuffer;

	ID3D11Texture2D *m_depthStencilBuffer;
	ID3D11DepthStencilView *m_depthStencilView;
	ID3D11DepthStencilState *m_depthStencilState;

	ID3D11RasterizerState *m_rasterizerState;

	D3D11_VIEWPORT m_viewport;

	void initDirect3D();

	void update();
	void render(float alpha);

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
			return e->m_physics->constructScene<SceneType>(std::forward<Args>(args)...);
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

		// the wonders of C++ template programming at work right here... *sigh*
		SceneType * s = enterSceneImpl<SceneType, std::is_base_of<PhysicsScene, SceneType>::value, Args...>::construct(this, std::forward<Args>(args)...);
		m_activeScene.reset(s); // set new scene
	}
};

extern Engine * engine; // engine singleton

