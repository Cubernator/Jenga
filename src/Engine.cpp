#include "Engine.h"
#include <windows.h>
#include <windowsx.h>

#include "Objects.h"
#include "Scene.h"
#include "Physics.h"
#include "Graphics.h"
#include "GUI.h"
#include "Input.h"
#include "AudioInterface.h"
#include "Content.h"

Engine * engine;

Engine::Engine(HWND hWnd) : m_realDelta(1.f / 60.f), m_realTime(0.f), m_time(0.f), m_timeScale(1.0f), m_running(true), m_hWnd(hWnd)
{
	engine = this; // set singleton instance

	m_graphics = new GraphicsInterface(m_hWnd);
	m_gui = new GUIInterface(m_graphics->getSwapChain());
	m_audio = new AudioInterface();
	m_physics = new PhysicsInterface();
	m_input = new Input(m_hWnd);
	m_objectManager = new ObjectManager();
	m_content = new Content();
}

Engine::~Engine()
{
	m_activeScene.reset();

	delete m_content;
	delete m_objectManager;
	delete m_input;
	delete m_physics;
	delete m_audio;
	delete m_gui;
	delete m_graphics;
}

float Engine::getTime() const
{
	return m_time.count();
}

float Engine::getDelta() const
{
	return m_delta.count();
}

float Engine::getRealTime() const
{
	return m_realTime.count();
}

float Engine::getRealDelta() const
{
	return m_realDelta.count();
}

void Engine::setTimeScale(float scale)
{
	m_timeScale = max(min(scale, 5.0f), 0.0f);
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

		while (accumulator >= m_realDelta) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT)
				m_running = false;

			m_realTime += m_realDelta;

			m_delta = m_realDelta * m_timeScale;
			m_time += m_delta;

			update();

			accumulator -= m_realDelta;
		}

		// we have to explicitly call the appropriate division operator due to a bug in Microsoft's implementation of the C++ standard...
		// alpha = std::chrono::operator/<fsec::rep, fsec::period, fsec::rep, fsec::period>(accumulator, m_delta);

		// or just do this, because it basically does the same thing!
		render(accumulator.count() / m_realDelta.count());
	}

	return msg.wParam;
}

void Engine::update()
{
	m_input->preUpdate();

	m_physics->simulate(m_delta.count());

	if (m_activeScene) m_activeScene->update(); // update active scene object
	m_objectManager->update(); // update all registered game objects

	m_gui->update();

	m_input->update();

	m_audio->update();
}

void Engine::render(float alpha)
{
	graphics->render(alpha);
	gui->render();
	graphics->present();
}

LRESULT Engine::processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		stop();
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
		m_input->handle((RAWINPUT*)lpb);

		// free buffer
		delete[] lpb;
	} break;

	case WM_CHAR:
	{
		input->characterInput((TCHAR)wParam);
	} break;

	default:
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}

	return 0;
}
