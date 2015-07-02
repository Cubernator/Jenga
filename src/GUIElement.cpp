#include "GUIElement.h"

GUIElement::GUIElement(const D2D_RECT_F& rect) : m_rect(rect)
{
	gui->add(this);
}

GUIElement::~GUIElement()
{
	gui->remove(this);
}

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

GUILabel::~GUILabel()
{
	m_textBrush->Release();
}

void GUILabel::draw() const
{
	gui->getD2DRenderTarget()->DrawTextW(m_text.c_str(), m_text.size(), m_format, getRect(), m_textBrush);
}