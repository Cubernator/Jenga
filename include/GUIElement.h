#pragma once

#include "GUI.h"

#include <functional>

class GUIElement
{
private:
	D2D_RECT_F m_rect;

	virtual void draw() const = 0;
	virtual void update() { }

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
	ComPtr<ID2D1SolidColorBrush> m_textBrush;

protected:
	void draw() const override;

public:
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const D2D_COLOR_F& textColor);
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format);
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text, const D2D_COLOR_F& textColor);
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text);
};

class GUIImage : public GUIElement
{
private:
	ID2D1Bitmap * m_image;

	void draw() const override;
	
public:
	GUIImage(const D2D_RECT_F& rect, ID2D1Bitmap * image);
};

class GUIButton : public GUILabel
{
private:
	ComPtr<ID2D1Bitmap> m_imageNormal, m_imageHover, m_imageActive;

	std::function<void()> m_callback;
	bool m_hover, m_down;

	void draw() const override;
	void update() override;

public:
	GUIButton(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const D2D_COLOR_F& textColor);
	GUIButton(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format);
	GUIButton(const D2D_RECT_F& rect, const std::wstring& text, const D2D_COLOR_F& textColor);
	GUIButton(const D2D_RECT_F& rect, const std::wstring& text);

	void setCallback(std::function<void()> callback);
};