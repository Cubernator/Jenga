#pragma once

#include "DirectX.h"
#include <chrono>
#include <memory>

#include "constants.h"
#include "Physics.h"

typedef std::chrono::duration<float> fsec;

class Input;
class ObjectManager;
class GraphicsInterface;
class GUIInterface;
class AudioInterface;
class Scene;
class PhysicsScene;

class Engine
{
private:
	const fsec m_realDelta;
	fsec m_realTime, m_time, m_delta;

	float m_timeScale;

	bool m_running;

	ObjectManager * m_objectManager;
	Input * m_input;
	PhysicsInterface * m_physics;
	GraphicsInterface * m_graphics;
	GUIInterface * m_gui;
	AudioInterface * m_audio;

	std::unique_ptr<Scene> m_activeScene;

	HWND m_hWnd;

	void update();
	void render(float alpha);

	template<class SceneType, bool, class... Args>
	struct enterSceneImpl
	{
		static SceneType * construct(Engine * e, Args&&... args)
		{
			e->m_physics->setScene(nullptr);
			return new SceneType(std::forward<Args>(args)...);
		}
	};

	template<class SceneType, class... Args>
	struct enterSceneImpl<SceneType, true, Args...>
	{
		static SceneType * construct(Engine * e, Args&&... args)
		{
			SceneType * newScene = e->m_physics->constructScene<SceneType>(std::forward<Args>(args)...);
			e->m_physics->setScene(newScene);
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

	float getRealTime() const;
	float getRealDelta() const;

	void setTimeScale(float scale);

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

