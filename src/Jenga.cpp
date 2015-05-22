// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include "Engine.h"
#include "MainScene.h"

// include the Direct3D Library file
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

#if _DEBUG
#pragma comment(lib,"PhysX3DEBUG_x86.lib")
#pragma comment(lib,"PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib,"PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib,"PhysX3GpuDEBUG_x86.lib")
#pragma comment(lib,"PxTaskDEBUG.lib")
#else
#pragma comment(lib,"PhysX3_x86.lib")
#pragma comment(lib,"PhysX3Common_x86.lib")
#pragma comment(lib,"PhysX3Extensions.lib")
#pragma comment(lib,"PhysX3Gpu_x86.lib")
#pragma comment(lib,"PxTask.lib")
#endif

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return engine->processMessages(hWnd, message, wParam, lParam);
}


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		L"WindowClass",
		L"Jenga",
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		0, 0,
		1280, 720,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	WPARAM result = -1;

	try {
		Engine e(hWnd);

		e.enterScene<MainScene>();
		result = e.run();
	}
	catch (std::exception&) {

	}

	return result;
}