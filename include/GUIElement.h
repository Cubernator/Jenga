#pragma once

#include "GUI.h"

class GUIElement
{
private:
	D2D_RECT_F m_rect;

	virtual void draw() const = 0;

	friend class GUIInterface;

public:
	GUIElement(const D2D_RECT_F& rect);
	virtual ~GUIElement();

	GUIElement(const GUIElement& other) = delete;
	GUIElement& operator=(const GUIElement& other) = delete;

	GUIElement(GUIElement&& other) = delete;
	GUIElement& operator=(GUIElement&& other) = delete;

	const D2D_RECT_F& getRect() const;
	void setRect(const D2D_RECT_F& r);
};

class GUILabel : public GUIElement
{
private:
	std::wstring m_text;
	IDWriteTextFormat * m_format;
	ID2D1SolidColorBrush * m_textBrush;

	void draw() const override;

public:
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const D2D_COLOR_F& textColor);
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format);

	~GUILabel();
};