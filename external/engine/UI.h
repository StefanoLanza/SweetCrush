#pragma once

#include "DrawOrder.h"
#include "FwdDecl.h"
#include "Maths.h"
#include "StringTable.h"
#include "TextRender.h"
#include <memory>
#include <vector>

namespace Wind {

enum class UIHorizAlignment {
	left,
	right,
	center,
};

enum class UIVertAlignment {
	top,
	bottom,
	center,
};

enum class UIBlending : bool {
	on,
	off,
};

struct UIPos {
	float ax;
	float ay;
	float rx;
	float ry;
};

struct UISize {
	float aWidth;
	float aHeight;
	float rWidth;
	float rHeight;
};

struct UIRect {
	Vec2  pos;
	float width;
	float height;
};

struct UITextDesc {
	const char*      font;
	StringId         stringId;
	UIPos            pos;
	UISize           size;
	UIHorizAlignment horizontalAlignment = UIHorizAlignment::center;
	UIVertAlignment  verticalAlignment = UIVertAlignment::center;
	TextStyle        textStyle = defaultTextStyle;
};

struct UIBitmapDesc {
	const char*      fileName;
	UIPos            pos;
	UISize           size;
	UIHorizAlignment horizontalAlignment = UIHorizAlignment::center;
	UIVertAlignment  verticalAlignment = UIVertAlignment::center;
	Color            color = whiteColor;
	UIBlending       blending = UIBlending::on;
	unsigned int     relDrawOrder = 0;
	Rect             _9patch = { 0.f, 0.f, 0.f, 0.f }; // pixels
};

struct UIButtonDesc {
	UIPos            pos;
	UISize           size;
	UIHorizAlignment horizontalAlignment = UIHorizAlignment::center;
	UIVertAlignment  verticalAlignment = UIVertAlignment::center;
};

struct UIPanelDesc {
	UIPos            pos;
	UISize           size;
	UIHorizAlignment horizontalAlignment = UIHorizAlignment::center;
	UIVertAlignment  verticalAlignment = UIVertAlignment::center;
};

class UIText final {
public:
	UIText(const UITextDesc& desc, Engine& engine);

	void Draw(const TextRenderer& textRender, DrawOrderType drawOrder) const;
	void UpdateRect(const UIRect& parentRect);
	void SetText(StringId stringId);

private:
	UITextDesc mDesc;
	FontPtr    mFont;
	UIRect     mAlignedRect;
};

class UIBitmap {
public:
	UIBitmap(const UIBitmapDesc& desc, Graphics& graphics);

	void           Draw(const UIRenderer& renderer, DrawOrderType drawOrder) const;
	void           UpdateRect(const UIRect& parentRect);
	void           SetBitmap(const TexturePtr& bitmap);
	const Texture& GetBitmap() const;

private:
	UIBitmapDesc mDesc;
	TexturePtr   mBitmap;
	UIRect       mAlignedRect;
};

class UIButton final {
public:
	UIButton(const UIButtonDesc& desc, std::unique_ptr<UIBitmap> bitmap, std::unique_ptr<UIText> text);

	bool      IsPressed(const Input& input);
	void      Draw(const UIRenderer& renderer, const TextRenderer& textRender, DrawOrderType drawOrder) const;
	void      UpdateRect(const UIRect& parentRect);
	UIBitmap* GetBitmap() const;
	UIText*   GetText() const;

private:
	UIButtonDesc              mDesc;
	std::unique_ptr<UIBitmap> mBitmap;
	std::unique_ptr<UIText>   mText;
	UIRect                    mAlignedRect;
};

class UIPanel final {
public:
	explicit UIPanel(const UIPanelDesc& desc);

	void          SetVisible(bool visible);
	bool          IsVisible() const;
	const UIRect& Rect() const;
	void          AddPanel(UIPanel& panel);
	void          AddButton(UIButton& button);
	void          AddText(UIText& text);
	void          AddBitmap(UIBitmap& bitmap);
	void          Draw(const UIRenderer& renderer, const TextRenderer& textRender, DrawOrderType drawOrder) const;
	void          UpdateRect(const UIRect& parentRect);

private:
	UIPanelDesc            mDesc;
	std::vector<UIPanel*>  mPanels;
	std::vector<UIBitmap*> mBitmaps;
	std::vector<UIButton*> mButtons;
	std::vector<UIText*>   mTexts;
	UIRect                 mRect;
	bool                   mVisible;
};

class UICanvas final {
public:
	UICanvas();

	void     SetBackground(const char* fileName, Graphics& graphics);
	void     SetBackground(TexturePtr background);
	void     SetMousePointer(const char* fileName, Graphics& graphics);
	UIPanel& GetPanel();
	void     UpdateWidgets(int canvasWidth, int canvasHeight);
	void     Draw(const UIRenderer& renderer, const TextRenderer& textRender, const Vec2& mouseCoords);

private:
	UIPanel    mPanel;
	TexturePtr mBackground;
	TexturePtr mMousePointer;
};

UIButton MakeButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc, const UITextDesc& textDesc, Engine& engine);
UIButton MakeButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc, Engine& engine);

constexpr UIPos       UIZeroPos = { 0.f, 0.f, 0.f, 0.f };
constexpr UISize      UIAutoSize = { -1.f, -1.f, -1.f, -1.f };
constexpr UISize      UIParentSize = { 0.f, 0.f, 1.f, 1.f };
constexpr UIPanelDesc UIDefaultPanelDesc { UIZeroPos, UIParentSize };

constexpr inline UIPos UIAbsolutePos(float x, float y) {
	return { x, y, 0.f, 0.f };
}

constexpr inline UISize UIAbsoluteSize(float x, float y) {
	return { x, y, 0.f, 0.f };
}

} // namespace Wind
