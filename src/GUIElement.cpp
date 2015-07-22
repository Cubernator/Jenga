#include "GUIElement.h"
#include "GUI.h"

#include "Input.h"

GUIStyleState::GUIStyleState(const D2D_COLOR_F& textColor, ID2D1Bitmap * controlImage) : textColor(textColor), controlImage(controlImage) {}
GUIStyleState::GUIStyleState(const D2D_COLOR_F& textColor) : GUIStyleState(textColor, nullptr) {}
GUIStyleState::GUIStyleState(ID2D1Bitmap * controlImage) : GUIStyleState(D2D1::ColorF(D2D1::ColorF::White), controlImage) {}
GUIStyleState::GUIStyleState() : GUIStyleState(nullptr) {}

GUIElement::GUIElement(const D2D_RECT_F& rect) : m_rect(rect), m_depth(0) { }
GUIElement::~GUIElement() { }

const D2D_RECT_F& GUIElement::getRect() const
{
	return m_rect;
}

void GUIElement::setRect(const D2D_RECT_F& r)
{
	m_rect = r;
}

int GUIElement::getDepth() const
{
	return m_depth;
}

void GUIElement::setDepth(int d)
{
	if (d != m_depth) {
		m_depth = d;
		gui->sortElements();
	}
}

GUIRectangle::GUIRectangle(const D2D_RECT_F& rect, const D2D_COLOR_F& color) : GUIElement(rect), m_color(color) { }

void GUIRectangle::draw() const
{
	gui->drawRectangle(getRect(), m_color);
}

GUILabel::GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const D2D_COLOR_F& textColor)
	: GUIElement(rect), m_text(text), m_format(format), m_textColor(textColor) { }

GUILabel::GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format)
	: GUILabel(rect, text, format, D2D1::ColorF(D2D1::ColorF::White)) { }

GUILabel::GUILabel(const D2D_RECT_F& rect, const std::wstring& text, const D2D_COLOR_F& textColor)
	: GUILabel(rect, text, gui->getDefaultFormat(), textColor) { }

GUILabel::GUILabel(const D2D_RECT_F& rect, const std::wstring& text)
	: GUILabel(rect, text, gui->getDefaultFormat()) { }

void GUILabel::setText(const std::wstring& text)
{
	m_text = text;
}

void GUILabel::setFormat(IDWriteTextFormat * format)
{
	m_format = format;
}

void GUILabel::draw() const
{
	gui->drawText(getRect(), m_text, m_format, m_textColor);
}

GUIImage::GUIImage(const D2D_RECT_F& rect, ID2D1Bitmap * image) : GUIElement(rect), m_image(image) { }

void GUIImage::draw() const
{
	gui->drawImage(getRect(), m_image);
}

GUIButton::GUIButton(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const GUIButtonStyle& style)
	: GUIElement(rect), m_text(text), m_icon(nullptr), m_textFormat(format), m_style(style), m_padding({ 6, 3, 6, 3 }), m_hover(false), m_down(false) { }
GUIButton::GUIButton(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format)
	: GUIButton(rect, text, format, GUIButtonStyle()) { }
GUIButton::GUIButton(const D2D_RECT_F& rect, const std::wstring& text, const GUIButtonStyle& style)
	: GUIButton(rect, text, gui->getDefaultFormat(), style) { }
GUIButton::GUIButton(const D2D_RECT_F& rect, const std::wstring& text)
	: GUIButton(rect, text, GUIButtonStyle()) { }

GUIButton::GUIButton(const D2D_RECT_F& rect, ID2D1Bitmap * icon, const GUIButtonStyle& style)
	: GUIElement(rect), m_icon(icon), m_textFormat(nullptr), m_style(style), m_padding({ 6, 3, 6, 3 }), m_hover(false), m_down(false) { }
GUIButton::GUIButton(const D2D_RECT_F& rect, ID2D1Bitmap * icon)
	: GUIButton(rect, icon, GUIButtonStyle()) { }

void GUIButton::draw() const
{
	const GUIStyleState& state = m_down ? m_style.active : (m_hover ? m_style.hover : m_style.normal);
	ID2D1Bitmap * image = state.controlImage;
	if (!image) image = m_style.normal.controlImage;
	if (image) gui->drawControlImage(getRect(), image);

	D2D_RECT_F r = getRect();
	r.left += m_padding.left;
	r.top += m_padding.top;
	r.right -= m_padding.right;
	r.bottom -= m_padding.bottom;

	if (m_icon) {
		gui->drawImage(r, m_icon);
	} else if (m_textFormat) {
		gui->drawText(r, m_text, m_textFormat, state.textColor);
	}
}

void GUIButton::update()
{
	float mx = (float)input->getMouseX(), my = (float)input->getMouseY();
	D2D_RECT_F r = getRect();
	m_hover = mx >= r.left && mx < r.right && my >= r.top && my < r.bottom;
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

void GUIButton::setText(const std::wstring& text)
{
	m_text = text;
}

void GUIButton::setStyle(const GUIButtonStyle& style)
{
	m_style = style;
}

void GUIButton::setPadding(const D2D_RECT_F& padding)
{
	m_padding = padding;
}

GUITextField::GUITextField(const D2D_RECT_F& rect, IDWriteTextFormat * format, const GUITextFieldStyle& style)
	: GUIElement(rect), m_textFormat(format), m_style(style), m_padding({ 6, 3, 6, 3 }), m_hover(false), m_focused(false)
{
	m_textEditor.setActive(false);
	input->registerTextEditor(&m_textEditor);
}

GUITextField::~GUITextField()
{
	input->unregisterTextEditor(&m_textEditor);
}

GUITextField::GUITextField(const D2D_RECT_F& rect, IDWriteTextFormat * format)
	: GUITextField(rect, format, GUITextFieldStyle()) {}
GUITextField::GUITextField(const D2D_RECT_F& rect, const GUITextFieldStyle& style)
	: GUITextField(rect, gui->getDefaultFormat(), style) {}
GUITextField::GUITextField(const D2D_RECT_F& rect)
	: GUITextField(rect, GUITextFieldStyle()) {}

const std::wstring& GUITextField::getText() const
{
	return m_textEditor.getText();
}

void GUITextField::setCallback(std::function<void()> callback)
{
	m_callback = callback;
}

void GUITextField::draw() const
{
	const GUIStyleState& state = m_focused ? m_style.focused : (m_hover ? m_style.hover : m_style.normal);
	ID2D1Bitmap * image = state.controlImage;
	if (!image) image = m_style.normal.controlImage;
	if (image) gui->drawControlImage(getRect(), image);

	D2D_RECT_F r = getRect();
	r.left += m_padding.left;
	r.top += m_padding.top;
	r.right -= m_padding.right;
	r.bottom -= m_padding.bottom;

	gui->drawText(r, m_textEditor.getText(), m_textFormat, state.textColor, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void GUITextField::update()
{
	float mx = (float)input->getMouseX(), my = (float)input->getMouseY();
	D2D_RECT_F r = getRect();
	m_hover = mx >= r.left && mx < r.right && my >= r.top && my < r.bottom;

	if (input->getMouseButtonPressed(MBUTTONLEFT))
		m_focused = m_hover;

	m_textEditor.setActive(m_focused);

	if (m_focused && input->getKeyPressed(VK_RETURN)) {
		if (m_callback) m_callback();
	}
}