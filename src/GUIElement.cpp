#include "GUIElement.h"

#include "Input.h"

GUIElement::GUIElement(const D2D_RECT_F& rect) : m_rect(rect) { }
GUIElement::~GUIElement() { }

const D2D_RECT_F& GUIElement::getRect() const
{
	return m_rect;
}

void GUIElement::setRect(const D2D_RECT_F& r)
{
	m_rect = r;
}

GUILabel::GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const D2D_COLOR_F& textColor)
	: GUIElement(rect), m_text(text), m_format(format)
{
	gui->getD2DRenderTarget()->CreateSolidColorBrush(textColor, &m_textBrush);
}

GUILabel::GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format)
	: GUILabel(rect, text, format, D2D1::ColorF(D2D1::ColorF::White)) { }

GUILabel::GUILabel(const D2D_RECT_F& rect, const std::wstring& text, const D2D_COLOR_F& textColor)
	: GUILabel(rect, text, gui->getDefaultFormat(), textColor) { }

GUILabel::GUILabel(const D2D_RECT_F& rect, const std::wstring& text)
	: GUILabel(rect, text, gui->getDefaultFormat()) { }

void GUILabel::draw() const
{
	gui->getD2DRenderTarget()->DrawTextW(m_text.c_str(), m_text.size(), m_format, getRect(), m_textBrush.Get());
}

GUIImage::GUIImage(const D2D_RECT_F& rect, ID2D1Bitmap * image) : GUIElement(rect), m_image(image) { }

void GUIImage::draw() const
{
	gui->getD2DRenderTarget()->DrawBitmap(m_image, getRect());
}

GUIButton::GUIButton(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const D2D_COLOR_F& textColor)
	: GUILabel(rect, text, format, textColor), m_hover(false), m_down(false)
{
	gui->loadBitmap(L"assets\\images\\buttonNormal.png", &m_imageNormal);
	gui->loadBitmap(L"assets\\images\\buttonHover.png", &m_imageHover);
	gui->loadBitmap(L"assets\\images\\buttonActive.png", &m_imageActive);
}

GUIButton::GUIButton(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format)
	: GUIButton(rect, text, format, D2D1::ColorF(D2D1::ColorF::White)) { }
GUIButton::GUIButton(const D2D_RECT_F& rect, const std::wstring& text, const D2D_COLOR_F& textColor)
	: GUIButton(rect, text, gui->getDefaultFormat(), textColor) { }
GUIButton::GUIButton(const D2D_RECT_F& rect, const std::wstring& text)
	: GUIButton(rect, text, gui->getDefaultFormat()) { }

void GUIButton::draw() const
{
	GUILabel::draw();
	ID2D1Bitmap * image = m_down ? m_imageActive.Get() : (m_hover ? m_imageHover.Get() : m_imageNormal.Get());
	if (image) {
		gui->getD2DRenderTarget()->DrawBitmap(image, getRect());
	}
}

void GUIButton::update()
{
	float mx = (float)input->getMouseX(), my = (float)input->getMouseY();
	D2D_RECT_F r = getRect();
	m_hover = mx >= r.left && mx <= r.right && my >= r.top && my <= r.bottom;
	if (m_hover) {
		if (input->getMouseButtonPressed(MBUTTONLEFT)) m_down = true;
	} else {
		m_down = false;
	}

	if (m_down && input->getMouseButtonReleased(MBUTTONLEFT)) {
		if (m_callback) m_callback();
		m_down = false;
	}
}

void GUIButton::setCallback(std::function<void()> callback)
{
	m_callback = callback;
}


