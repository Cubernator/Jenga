#include "Input.h"
#include "constants.h"

Input * input;

Input::Input(HWND hWnd) : m_mx(0), m_my(0), m_mxDelta(0), m_myDelta(0), m_wheelDelta(0), m_hWnd(hWnd)
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

void Input::retrieveCursorPos()
{
	POINT p;
	if (GetCursorPos(&p)) {
		if (ScreenToClient(m_hWnd, &p)) {
			m_mx = p.x;
			m_my = p.y;
		}
	}
}

void Input::preUpdate()
{
	int mx = m_mx, my = m_my;
	retrieveCursorPos();

	m_mxDelta = m_mx - mx;
	m_myDelta = m_my - my;
}

void Input::update()
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

void Input::updateButton(Button& b, bool down)
{
	if (down && !b.down) {
		b.pressed = true;
	}
	else if (!down && b.down) {
		b.released = true;
	}
	b.down = down;
}

void Input::handle(RAWINPUT * raw)
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

bool Input::getKeyDown(USHORT key)
{
	return m_keyboardState[key].down;
}

bool Input::getKeyPressed(USHORT key)
{
	return m_keyboardState[key].pressed;
}

bool Input::getKeyReleased(USHORT key)
{
	return m_keyboardState[key].released;
}

bool Input::getMouseButtonDown(MouseButton button)
{
	return m_mouseButtons[button].down;
}

bool Input::getMouseButtonPressed(MouseButton button)
{
	return m_mouseButtons[button].pressed;
}

bool Input::getMouseButtonReleased(MouseButton button)
{
	return m_mouseButtons[button].released;
}

int Input::getMouseX() const
{
	return m_mx;
}

int Input::getMouseY() const
{
	return m_my;
}

int Input::getMouseDeltaX() const
{
	return m_mxDelta;
}

int Input::getMouseDeltaY() const
{
	return m_myDelta;
}

int Input::getMouseWheelDelta() const
{
	return m_wheelDelta;
}

void Input::setMousePos(int x, int y)
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
