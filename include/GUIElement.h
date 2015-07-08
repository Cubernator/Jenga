#pragma once

#include "DirectX.h"
#include <functional>

struct GUIStyleState
{
	D2D_COLOR_F textColor;
	ID2D1Bitmap * controlImage;

	GUIStyleState();
	GUIStyleState(const D2D_COLOR_F& textColor);
	GUIStyleState(ID2D1Bitmap * controlImage);
	GUIStyleState(const D2D_COLOR_F& textColor, ID2D1Bitmap * controlImage);
};

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

class GUIRectangle : public GUIElement
{
private:
	D2D_COLOR_F m_color;

protected:
	void draw() const override;

public:
	GUIRectangle(const D2D_RECT_F& rect, const D2D_COLOR_F& color);
};

class GUILabel : public GUIElement
{
private:
	std::wstring m_text;
	D2D_COLOR_F m_textColor;
	IDWriteTextFormat * m_format;

protected:
	void draw() const override;

public:
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const D2D_COLOR_F& textColor);
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format);
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text, const D2D_COLOR_F& textColor);
	GUILabel(const D2D_RECT_F& rect, const std::wstring& text);

	void setPadding(const D2D_RECT_F& p);
};

class GUIImage : public GUIElement
{
private:
	ID2D1Bitmap * m_image;

protected:
	void draw() const override;
	
public:
	GUIImage(const D2D_RECT_F& rect, ID2D1Bitmap * image);
};

struct GUIButtonStyle
{
	GUIStyleState normal, hover, active, disabled;
};

class GUIButton : public GUIElement
{
private:
	ID2D1Bitmap * m_icon;
	std::wstring m_text;

	D2D_RECT_F m_padding;

	IDWriteTextFormat * m_textFormat;
	GUIButtonStyle m_style;

	std::function<void()> m_callback;
	bool m_hover, m_down;

protected:
	void draw() const override;
	void update() override;

public:
	GUIButton(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format, const GUIButtonStyle& style);
	GUIButton(const D2D_RECT_F& rect, const std::wstring& text, IDWriteTextFormat * format);
	GUIButton(const D2D_RECT_F& rect, const std::wstring& text, const GUIButtonStyle& style);
	GUIButton(const D2D_RECT_F& rect, const std::wstring& text);
	GUIButton(const D2D_RECT_F& rect, ID2D1Bitmap * icon, const GUIButtonStyle& style);
	GUIButton(const D2D_RECT_F& rect, ID2D1Bitmap * icon);

	void setCallback(std::function<void()> callback);
	void setText(const std::wstring& text);
	void setStyle(const GUIButtonStyle& style);
};