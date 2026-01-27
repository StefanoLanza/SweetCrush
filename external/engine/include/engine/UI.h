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
	Vec2 pos;
	Vec2 size;
};

// Use a macro instead of inheritance, to allow designated initializers in C++ 20
#define UIBaseDesc                                                   \
	UIPos            pos;                                            \
	UISize           size;                                           \
	UIHorizAlignment horizontalAlignment = UIHorizAlignment::center; \
	UIVertAlignment  verticalAlignment = UIVertAlignment::center;    \
	float            padding = 0.f;                                  \
	float            borderWidth = 0.f;

struct UITextDesc {
	UIBaseDesc;
	const char* font = nullptr;
	StringId    stringId;
	TextStyle   textStyle = defaultTextStyle;
};

struct UIBitmapDesc {
	const char* fileName;
	UIBaseDesc;
	Color      color = whiteColor;
	UIBlending blending = UIBlending::on;
	Rect       _9patch = { 0.f, 0.f, 0.f, 0.f }; // pixels
};

struct UIButtonDesc {
	UIBaseDesc;
};

struct UIPanelDesc {
	UIBaseDesc;
	const char* background = nullptr;
	Color       backgroundColor = whiteColor;
};

struct UICanvasDesc {
	const char* background = nullptr;
	Color       backgroundColor = whiteColor;
};

#undef UIBaseDesc

class UIText final {
public:
	explicit UIText(const UITextDesc& desc);

	void Load(TextRenderer& textRenderer);
	void Draw(const TextRenderer& textRenderer, DrawOrderType drawOrder) const;
	void UpdateRect(const UIRect& parentRect);
	void SetText(StringId stringId);

private:
	UITextDesc mDesc;
	FontPtr    mFont;
	UIRect     mAlignedRect;
};

class UIBitmap {
public:
	explicit UIBitmap(const UIBitmapDesc& desc);

	void           LoadGraphics(Graphics& graphics);
	void           Draw(const UIRenderer& renderer, DrawOrderType drawOrder) const;
	void           UpdateRect(const UIRect& parentRect);
	void           SetBitmap(const TexturePtr& bitmap);
	const Texture* GetBitmap() const;

private:
	UIBitmapDesc mDesc;
	TexturePtr   mBitmap;
	UIRect       mAlignedRect;
};

class UIButton final {
public:
	UIButton(const UIButtonDesc& desc, std::unique_ptr<UIBitmap> bitmap, std::unique_ptr<UIText> text);

	bool          IsPressed(const Input& input) const;
	void          LoadAssets(Graphics& graphics, TextRenderer& textRenderer);
	void          Draw(const UIRenderer& renderer, DrawOrderType drawOrder) const;
	void          UpdateRect(const UIRect& parentRect);
	UIBitmap*     GetBitmap() const;
	UIText*       GetText() const;
	const UIRect& GetRect() const;

private:
	UIButtonDesc              mDesc;
	std::unique_ptr<UIBitmap> mBitmap;
	std::unique_ptr<UIText>   mText;
	UIRect                    mRect;
};

class UIPanel final {
public:
	explicit UIPanel(const UIPanelDesc& desc);

	void          SetVisible(bool visible);
	bool          IsVisible() const;
	const UIRect& Rect() const;
	void          AddPanel(UIPanel& panel);
	void          AddButton(UIButton& button);
	void          AddBitmap(UIBitmap& bitmap);
	void          AddText(UIText& text);
	void          LoadAssets(Graphics& graphics, TextRenderer& textRenderer);
	void          Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void          UpdateRect(const UIRect& parentRect);

private:
	UIPanelDesc            mDesc;
	std::vector<UIPanel*>  mPanels;
	std::vector<UIBitmap*> mBitmaps;
	std::vector<UIButton*> mButtons;
	std::vector<UIText*>   mTexts;
	UIRect                 mRect;
	TexturePtr             mBackground;
	bool                   mVisible;
};

class UICanvas final {
public:
	UICanvas();
	explicit UICanvas(const UICanvasDesc& desc);

	void AddPanel(UIPanel& panel);
	void AddButton(UIButton& button);
	void AddBitmap(UIBitmap& bitmap);
	void AddText(UIText& text);
	void LoadAssets(Graphics& graphics, TextRenderer& textRenderer);
	void Draw(int canvasWidth, int canvasHeight, const UIRenderer& renderer, unsigned drawOrder);

private:
	UIPanel mPanel;
};

class UIMouseCursor final {
public:
	void SetCursor(const char* fileName, Graphics& graphics);
	void Draw(const UIRenderer& renderer, const Vec2& mouseCoords, unsigned drawOrder);

private:
	TexturePtr mMousePointer;
};

UIButton MakeButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc, const UITextDesc& textDesc);
UIButton MakeButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc);

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
