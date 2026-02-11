#pragma once

#include "FwdDecl.h"
#include "Maths.h"
#include "StringTable.h"
#include "TextRender.h"

#include <memory>
#include <optional>
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

constexpr UIPos  UIZeroPos = { 0.f, 0.f, 0.f, 0.f };
constexpr UISize UIZeroSize = {
	0.f,
	0.f,
	0.f,
	0.f,
};
constexpr UISize UIParentSize = { 0.f, 0.f, 1.f, 1.f };

constexpr inline UIPos UIAbsolutePos(float x, float y) {
	return { x, y, 0.f, 0.f };
}

constexpr inline UISize UIAbsoluteSize(float x, float y) {
	return { x, y, 0.f, 0.f };
}

constexpr UIRect UIZeroRect { 0.f, 0.f, 0.f, 0.f };

// Use a macro instead of inheritance, to allow designated initializers in C++ 20
#define UIBaseDesc                                                   \
	UIPos            pos = UIZeroPos;                                \
	UISize           size = UIZeroSize;                              \
	UIHorizAlignment horizontalAlignment = UIHorizAlignment::center; \
	UIVertAlignment  verticalAlignment = UIVertAlignment::center;    \
	float            padding = 0.f;                                  \
	float            margin = 0.f;                                   \
	float            borderWidth = 0.f;

#define UIBackgroundDesc                      \
	const char* background = nullptr;         \
	Color       backgroundColor = whiteColor; \
	float       _9patch = 0.f; // pixels

enum class UITextSizing {
	fit,
	stretch,
	user,
};

struct UITextDesc {
	Vec2             pos = { 0.f, 0.f };
	UISize           size = { 0.f, 0.f };
	UITextSizing     sizing = UITextSizing::fit;
	UIHorizAlignment horizontalAlignment = UIHorizAlignment::center;
	UIVertAlignment  verticalAlignment = UIVertAlignment::center;
	float            padding = 0.f;
	float            borderWidth = 0.f;
	const char*      font = nullptr;
	StringId         stringId = 0;
	const TextStyle& style = {};
};

enum class UIBitmapSizing {
	fit,
	stretch,
	user,
};

struct UIBitmapDesc {
	const char* fileName;
	UIBaseDesc;
	UIBitmapSizing sizing = UIBitmapSizing::fit;
	Color          color = whiteColor;
};

struct UIButtonDesc {
	UIBaseDesc;
	UIBackgroundDesc;
	bool keepPressedOutside = false;
};

struct UICheckBoxDesc {
	UIBaseDesc;
	UIBackgroundDesc;
	bool toggled = true;
};

struct UIPanelDesc {
	UIBaseDesc;
	UIBackgroundDesc;
};

struct UICanvasDesc {
	const char* background = nullptr;
	Color       backgroundColor = whiteColor;
};

#undef UIBaseDesc

struct UIBitmapStyle {
	Color color = whiteColor;
	Vec2  scale = { 1.f, 1.f };
};

class UIContainer {
public:
	void Add(UIPanel& panel);
	void Add(UIButton& button);
	void Add(UIBitmap& bitmap);
	void Add(UIText& text);
	void Add(UICheckBox& checkBox);
	void LoadAssets(Graphics& graphics, FontManager& fontManager);
	void UpdateLayout(const UIRect& parentRect) const;

protected:
	// TODO Virtual ?
	std::vector<UIPanel*>    mPanels;
	std::vector<UIBitmap*>   mBitmaps;
	std::vector<UIButton*>   mButtons;
	std::vector<UIText*>     mTexts;
	std::vector<UICheckBox*> mCheckboxes;
};

class UIText final {
public:
	explicit UIText(const UITextDesc& desc);

	void             SetVisible(bool visible);
	bool             IsVisible() const;
	void             Load(FontManager& fontManager);
	void             Draw(const UITextRenderer& textRenderer, unsigned drawOrder) const;
	void             ComputeRect(const UIRect& parentRect);
	void             SetText(StringId stringId);
	void             SetText(const char* str);
	void             SetStyle(const TextStyle& style);
	const TextStyle& GetStyle() const;

private:
	const char* Text() const;

private:
	UITextDesc mDesc;
	FontPtr    mFont;
	UIRect     mRect;
	char       mText[32];
	TextStyle  mTextStyle;
	bool       mVisible;
};

class UIBitmap {
public:
	explicit UIBitmap(const UIBitmapDesc& desc, const UIBitmapStyle& style = {});

	void                LoadGraphics(Graphics& graphics);
	void                SetPosition(const Vec2& pos);
	void                SetColor(const Color& color);
	const UIBitmapDesc& GetDesc() const;
	void                Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void                ComputeRect(const UIRect& parentRect);
	void                SetBitmap(const TexturePtr& bitmap);
	const Texture*      GetBitmap() const;
	void                SetStyle(const UIBitmapStyle& style);

private:
	UIBitmapDesc  mDesc;
	UIBitmapStyle mStyle;
	TexturePtr    mBitmap;
	UIRect        mRect;
};

enum class UIButtonState {
	disabled,
	idle,
	hovered,
	pressed,
};

struct UIButtonSubStyle {
	Vec2  offset { 0.f, 0.f };
	float scale = 1.f;
	float grayScale = 0.f;
};

struct UIButtonStyle {
	UIButtonSubStyle idle;
	UIButtonSubStyle disabled;
	UIButtonSubStyle hovered;
	UIButtonSubStyle pressed;
};

class UIButton final {
public:
	explicit UIButton(const UIButtonDesc& desc);
	UIButton(const UIButtonDesc& desc, const UIBitmapDesc& iconDesc, const UITextDesc& labelDesc);
	UIButton(const UIButtonDesc& desc, const UITextDesc& labelDesc);
	UIButton(const UIButtonDesc& desc, const UIBitmapDesc& iconDesc);

	void                SetEnabled(bool enabled);
	void                SetVisible(bool visible);
	bool                IsVisible() const;
	bool                IsClicked() const;
	void                LoadAssets(Graphics& graphics, FontManager& fontManager);
	void                Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void                ComputeRect(const UIRect& parentRect);
	const UIRect&       GetRect() const;
	UIButtonState       GetState() const;
	const UIButtonDesc& GetDesc() const;
	void                SetDesc(const UIButtonDesc&);
	bool                HandleInput(const Input& input);

private:
	UIButton(const UIButtonDesc& desc, std::unique_ptr<UIBitmap> bitmap, std::unique_ptr<UIText> text);
	UIButtonState           RefreshState(const Input& input);
	const UIButtonSubStyle* GetStyle() const;

private:
	UIButtonDesc              mDesc;
	TexturePtr                mBackground;
	std::unique_ptr<UIBitmap> mIcon;
	std::unique_ptr<UIText>   mLabel;
	UIRect                    mRect;
	UIButtonState             mState;
	bool                      mVisible;
	bool                      mClicked;
};

struct UICheckBoxStyle {
	Vec2  offset { 0.f, 0.f };
	float grayScale = 0.f;
};

class UICheckBox final : public UIContainer {
public:
	explicit UICheckBox(const UICheckBoxDesc& desc, const UICheckBoxStyle* style);

	void          SetEnabled(bool enabled);
	void          SetVisible(bool visible);
	bool          IsVisible() const;
	bool          IsChecked() const;
	void          SetChecked(bool value);
	void          LoadAssets(Graphics& graphics, FontManager& fontManager);
	void          Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void          ComputeRect(const UIRect& parentRect);
	const UIRect& GetRect() const;
	bool          HandleInput(const Input& input);

private:
	void RefreshState(const Input& input);

private:
	UICheckBoxDesc         mDesc;
	const UICheckBoxStyle* mStyle;
	TexturePtr             mBackground;
	UIRect                 mRect;
	bool                   mVisible;
	bool                   mToggled;
};

class UIPanel final : public UIContainer {
public:
	explicit UIPanel(const UIPanelDesc& desc);

	void          SetVisible(bool visible);
	bool          IsVisible() const;
	const UIRect& Rect() const;
	void          LoadAssets(Graphics& graphics, FontManager& fontManager);
	void          Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	bool          HandleInput(const Input& input) const;
	void          ComputeRect(const UIRect& parentRect);

private:
	UIPanelDesc mDesc;
	UIRect      mRect;
	TexturePtr  mBackground;
	bool        mVisible;
	// TODO Layout
};

class UICanvas final {
public:
	UICanvas();
	explicit UICanvas(const UICanvasDesc& desc);

	void Add(UIPanel& panel);
	void Add(UIButton& button);
	void Add(UIBitmap& bitmap);
	void Add(UIText& text);
	void Add(UICheckBox& checkBox);
	void LoadAssets(Graphics& graphics, FontManager& fontManager);
	void Draw(int canvasWidth, int canvasHeight, const UIRenderer& renderer, unsigned drawOrder);
	void HandleInput(const Input& input) const;

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

struct UITheme {
	TextStyle     textStyle;
	UIBitmapStyle bitmapStyle;
	UIButtonStyle buttonStyle;
};

void SetTheme(const UITheme* theme);

} // namespace Wind
