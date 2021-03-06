#include "Input.h"
#include "constants.h"

#include <algorithm>

InputInterface * input;

TextEditor::TextEditor() { }
TextEditor::TextEditor(const std::wstring& text) : m_text(text) {}
TextEditor::TextEditor(std::wstring&& text) : m_text(std::move(text)) {}

std::wstring& TextEditor::getText()
{
	return m_text;
}

const std::wstring& TextEditor::getText() const
{
	return m_text;
}

void TextEditor::setText(const std::wstring& text)
{
	m_text = text;
}

void TextEditor::setText(std::wstring&& text)
{
	m_text = std::move(text);
}

bool TextEditor::isActive() const
{
	return m_active;
}

void TextEditor::setActive(bool a)
{
	m_active = a;
}

void TextEditor::deleteChar()
{
	if (m_text.size() > 0) m_text.pop_back();
}

void TextEditor::insertChar(wchar_t c)
{
	m_text.push_back(c);
}


InputInterface::InputInterface(HWND hWnd) : m_mx(0), m_my(0), m_mxDelta(0), m_myDelta(0), m_wheelDelta(0), m_hWnd(hWnd)
{
	input = this;

	RAWINPUTDEVICE Rid[2];

	// register mouse device
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget = 0;

	// register keyboard device
	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = 0;
	Rid[1].hwndTarget = 0;

	if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE) {
		// error
	}

	retrieveCursorPos();
}

void InputInterface::retrieveCursorPos()
{
	POINT p;
	if (GetCursorPos(&p)) {
		if (ScreenToClient(m_hWnd, &p)) {
			m_mx = p.x;
			m_my = p.y;
		}
	}
}

void InputInterface::preUpdate()
{
	int mx = m_mx, my = m_my;
	retrieveCursorPos();

	m_mxDelta = m_mx - mx;
	m_myDelta = m_my - my;
}

void InputInterface::update()
{
	for (auto& bp : m_keyboardState) {
		bp.second.pressed = false;
		bp.second.released = false;
	}

	for (auto& bp : m_mouseButtons) {
		bp.second.pressed = false;
		bp.second.released = false;
	}

	m_wheelDelta = 0;
}

void InputInterface::updateButton(Button& b, bool down)
{
	if (down && !b.down) {
		b.pressed = true;
	}
	else if (!down && b.down) {
		b.released = true;
	}
	b.down = down;
}

void InputInterface::handle(RAWINPUT * raw)
{
	if (raw->header.dwType == RIM_TYPEKEYBOARD) {
		USHORT f = raw->data.keyboard.Flags;
		updateButton(m_keyboardState[raw->data.keyboard.VKey], (f & RI_KEY_BREAK) != RI_KEY_BREAK);
	}
	else if (raw->header.dwType == RIM_TYPEMOUSE) {
		USHORT f = raw->data.mouse.usButtonFlags;
		if (USHORT mb = f & MBUTTON1) {
			updateButton(m_mouseButtons[MBUTTON1], mb == RI_MOUSE_BUTTON_1_DOWN);
		}
		if (USHORT mb = f & MBUTTON2) {
			updateButton(m_mouseButtons[MBUTTON2], mb == RI_MOUSE_BUTTON_2_DOWN);
		}
		if (USHORT mb = f & MBUTTON3) {
			updateButton(m_mouseButtons[MBUTTON3], mb == RI_MOUSE_BUTTON_3_DOWN);
		}
		if (USHORT mb = f & MBUTTON4) {
			updateButton(m_mouseButtons[MBUTTON4], mb == RI_MOUSE_BUTTON_4_DOWN);
		}
		if (USHORT mb = f & MBUTTON5) {
			updateButton(m_mouseButtons[MBUTTON5], mb == RI_MOUSE_BUTTON_5_DOWN);
		}

		if ((f & RI_MOUSE_WHEEL) == RI_MOUSE_WHEEL) {
			m_wheelDelta = (short)raw->data.mouse.usButtonData / 120;
		}

		/* we will do this differently for now, since the cursor will probably always be visible
		if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {
			m_mxDelta = raw->data.mouse.lLastX;
			m_myDelta = raw->data.mouse.lLastY;

			m_mx += m_mxDelta;
			m_my += m_myDelta;
		}
		*/
	}
}

void InputInterface::characterInput(TCHAR character)
{
	switch (character) {
	case 0x0A: // linefeed
	case 0x1B: // escape
	case 0x09: // tab
	case 0x0D: // carriage return
		break;

	case 0x08: // backspace
		for (auto e : m_textEditors) {
			if (e->isActive()) e->deleteChar();
		}
		break;

	default: // displayable character
		for (auto e : m_textEditors) {
			if (e->isActive()) e->insertChar(character); 
		}
		break;
	}
}

bool InputInterface::getKeyDown(USHORT key)
{
	return m_keyboardState[key].down;
}

bool InputInterface::getKeyPressed(USHORT key)
{
	return m_keyboardState[key].pressed;
}

bool InputInterface::getKeyReleased(USHORT key)
{
	return m_keyboardState[key].released;
}

bool InputInterface::getMouseButtonDown(MouseButton button)
{
	return m_mouseButtons[button].down;
}

bool InputInterface::getMouseButtonPressed(MouseButton button)
{
	return m_mouseButtons[button].pressed;
}

bool InputInterface::getMouseButtonReleased(MouseButton button)
{
	return m_mouseButtons[button].released;
}

int InputInterface::getMouseX() const
{
	return m_mx;
}

int InputInterface::getMouseY() const
{
	return m_my;
}

int InputInterface::getMouseDeltaX() const
{
	return m_mxDelta;
}

int InputInterface::getMouseDeltaY() const
{
	return m_myDelta;
}

int InputInterface::getMouseWheelDelta() const
{
	return m_wheelDelta;
}

void InputInterface::setMousePos(int x, int y)
{
	x = max(min(x, SCREEN_WIDTH), 0);
	y = max(min(y, SCREEN_HEIGHT), 0);

	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(m_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);

	m_mx = x;
	m_my = y;
}

void InputInterface::registerTextEditor(TextEditor * editor)
{
	m_textEditors.push_back(editor);
}

void InputInterface::unregisterTextEditor(TextEditor * editor)
{
	m_textEditors.erase(std::remove(m_textEditors.begin(), m_textEditors.end(), editor), m_textEditors.end());
}

