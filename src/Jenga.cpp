// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include "Engine.h"
#include "MainScene.h"

#include "constants.h"
#include "GUI.h"
#include "Content.h"

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return engine->processMessages(hWnd, message, wParam, lParam);
}

struct Resources
{
	ComPtr<IDWriteTextFormat> menuTitleFormat, menuButtonFormat, menuTextFieldFormat;
	ComPtr<ID2D1Bitmap> buttonNormal, buttonHover, buttonActive, textfieldNormal, textfieldHover, textfieldFocus;

	Resources()
	{
		std::wstring fontFamily = L"couture";
		content->put(L"fontFamily", fontFamily);

		HRESULT hr = gui->createFormat(fontFamily, 50, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, &menuTitleFormat);
		menuTitleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		menuTitleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		hr = gui->createFormat(fontFamily, 28, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, &menuButtonFormat);
		menuButtonFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		menuButtonFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		hr = gui->createFormat(fontFamily, 20, &menuTextFieldFormat);
		menuTextFieldFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		content->put(L"menuTitleFormat", menuTitleFormat.Get());
		content->put(L"menuButtonFormat", menuButtonFormat.Get());
		content->put(L"menuTextFieldFormat", menuTextFieldFormat.Get());

		//gui->loadBitmap(L"assets\\images\\button_normal.png", &buttonNormal);
		gui->loadBitmap(L"assets\\images\\button_hover.png", &buttonHover);
		gui->loadBitmap(L"assets\\images\\button_active.png", &buttonActive);

		gui->loadBitmap(L"assets\\images\\textfield_normal.png", &textfieldNormal);
		gui->loadBitmap(L"assets\\images\\textfield_hover.png", &textfieldHover);
		gui->loadBitmap(L"assets\\images\\textfield_focus.png", &textfieldFocus);

		content->put(L"buttonNormal", buttonNormal.Get());
		content->put(L"buttonHover", buttonHover.Get());
		content->put(L"buttonActive", buttonActive.Get());

		content->put(L"textfieldNormal", textfieldNormal.Get());
		content->put(L"textfieldHover", textfieldHover.Get());
		content->put(L"textfieldFocus", textfieldFocus.Get());

		GUIButtonStyle menuButtonStyle{
			GUIStyleState(buttonNormal.Get()),
			GUIStyleState(D2D1::ColorF(1.0f, 0.3f, 0.0f), buttonHover.Get()),
			GUIStyleState(D2D1::ColorF(1.0f, 0.6f, 0.6f), buttonActive.Get())
		};

		GUITextFieldStyle menuTextFieldStyle{
			GUIStyleState(textfieldNormal.Get()),
			GUIStyleState(textfieldHover.Get()),
			GUIStyleState(textfieldFocus.Get())
		};

		content->put(L"menuButtonStyle", menuButtonStyle);
		content->put(L"menuTextFieldStyle", menuTextFieldStyle);
	}
};

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

	CoInitializeEx(0, COINIT_MULTITHREADED);

	WPARAM result = -1;

	try {
		Engine e(hWnd);

		Resources r;

		e.enterScene<MainScene>();

		result = e.run();
	}
	catch (std::exception&) {

	}

	CoUninitialize();

	return result;
}