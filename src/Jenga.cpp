// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include "Engine.h"
#include "MainMenu.h"

#include "constants.h"

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return engine->processMessages(hWnd, message, wParam, lParam);
}


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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

	DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;

	RECT r = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	AdjustWindowRectEx(&r, style, FALSE, NULL);

	hWnd = CreateWindowEx(NULL,
		L"WindowClass",
		L"Jenga",
		style,
		CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	WPARAM result = -1;

	try {
		Engine e(hWnd);

		e.enterScene<MainMenu>();
		result = e.run();
	}
	catch (std::exception&) {

	}

	return result;
}