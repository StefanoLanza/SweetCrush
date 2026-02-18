#pragma once

#include "FwdDecl.h"
#include "Maths.h"
#include "StringTable.h"
#include "TextRender.h"

#include <functional>
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
	Vec2 pos; // top-left corner
	Vec2 size;
	Vec2 axis;
};

constexpr UIPos  UIZeroPos = { 0.f, 0.f, 0.f, 0.f };
constexpr UISize UIZeroSize = { 0.f, 0.f, 0.f, 0.f };

constexpr inline UIPos UIAbsolutePos(float x, float y) {
	return { x, y, 0.f, 0.f };
}

constexpr inline UISize UIAbsoluteSize(float x, float y) {
	return { x, y, 0.f, 0.f };
}

constexpr UIRect UIZeroRect { 0.f, 0.f, 0.f, 0.f };

struct UITransform {
	Vec2  offset = { 0.f, 0.f };
	Vec2  scale = { 1.f, 1.f };
	float rotation = 0.f;
};

// Use a macro instead of inheritance, to allow designated initializers in C++ 20
#define UIBaseDesc                                                   \
	UIPos            pos = UIZeroPos;                                \
	UISize           size = UIZeroSize;                              \
	Vec2             pivot = { 0.5f, 0.5f };                         \
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
	StringId           stringId = 0;
	Vec2               pos = { 0.f, 0.f };
	UISize             size = { 0.f, 0.f };
	Vec2               pivot = { 0.5f, 0.5f };
	UITextSizing       sizing = UITextSizing::fit;
	UIHorizAlignment   horizontalAlignment = UIHorizAlignment::center;
	UIVertAlignment    verticalAlignment = UIVertAlignment::center;
	float              padding = 0.f;
	const char*        font = nullptr;
	const UITextStyle& style = {};
	bool               visible = true;
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
	bool           visible = true;
};

struct UIButtonDesc {
	UIBaseDesc;
	UIBackgroundDesc;
	bool keepPressedOutside = false;
};

struct UICheckBoxDesc {
	UIBaseDesc;
	UIBackgroundDesc;
	bool         toggled = true;
	UITextDesc   label;
	UIBitmapDesc checkedIcon;
	UIBitmapDesc uncheckedIcon;
};

struct UIPanelDesc {
	UIBaseDesc;
	UIBackgroundDesc;
};

struct UICanvasDesc {
	const char* background = nullptr;
	Color       backgroundColor = whiteColor;
	float       padding = 0.f;
};

#undef UIBaseDesc

struct UIBitmapStyle {
	Color color = whiteColor;
};

enum class UIControlType {
	Panel,
	Bitmap,
	Text,
	Button,
	Checkbox,
};

class UIControl {
public:
	explicit UIControl(bool visible)
	    : mRect {}
	    , mVisible { visible } {
	}
	void               SetVisible(bool visible);
	bool               IsVisible() const;
	const UIRect&      GetRect() const;
	UITransform&       GetTransform();
	const UITransform& GetTransform() const;

protected:
	void SetRect(const UIRect& rect);

protected:
	UITransform mTransform; // TODO Compute on the fly based on styles ?
	UITransform mFinalTransform;
	UIRect      mRect;
	bool        mVisible;
};

class UIPanel final : public UIControl {
public:
	explicit UIPanel(const UIPanelDesc& desc);

	void Add(UIPanel& panel);
	void Add(UIButton& button);
	void Add(UIBitmap& bitmap);
	void Add(UIText& text);
	void Add(UICheckBox& checkBox);
	void LoadAssets(Graphics& graphics, FontManager& fontManager);
	void Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	bool HandleInput(const Input& input) const;
	void ComputeRect(const UIRect& parentRect, const UITransform& transform);
	void Tick(float dt);

private:
	struct Child {
		void*         ptr;
		UIControlType type;
	};
	std::vector<Child> mChildren;
	UIPanelDesc        mDesc;
	TexturePtr         mBackground;
	// TODO Layout
};

class UIText final : public UIControl {
public:
	explicit UIText(const UITextDesc& desc);

	void               LoadAssets(Graphics& graphics, FontManager& fontManager);
	void               Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void               ComputeRect(const UIRect& parentRect, const UITransform& transform);
	void               SetText(StringId stringId);
	void               SetText(const char* str);
	void               SetStyle(const UITextStyle& style);
	const UITextStyle& GetStyle() const;

private:
	const char* Text() const;

private:
	UITextDesc  mDesc;
	FontPtr     mFont;
	char        mText[32];
	UITextStyle mStyle;
};

class UIBitmap : public UIControl {
public:
	explicit UIBitmap(const UIBitmapDesc& desc, const UIBitmapStyle& style = {});

	void                LoadAssets(Graphics& graphics, FontManager& fontManager);
	void                SetColor(const Color& color);
	const UIBitmapDesc& GetDesc() const;
	void                Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void                ComputeRect(const UIRect& parentRect, const UITransform& transform);
	void                SetBitmap(const TexturePtr& bitmap);
	const Texture*      GetBitmap() const;
	void                SetStyle(const UIBitmapStyle& style);

private:
	UIBitmapDesc  mDesc;
	UIBitmapStyle mStyle;
	TexturePtr    mBitmap;
};

enum class UIButtonState {
	idle,
	hovered,
	pressed,
	disabled,
};

struct UIButtonStyle;
using UIButtonAction = std::function<void(UITransform& transform, float dt)>;

struct UIButtonStyle {
	float          grayScale = 0.f;
	UIButtonAction onIdle;
	UIButtonAction onPressed;
	UIButtonAction onHovered;
};

class UIButton final : public UIControl {
public:
	UIButton(const UIButtonDesc& desc, const UIButtonStyle* style = nullptr);

	void          SetEnabled(bool enabled);
	bool          IsEnabled() const;
	bool          IsClicked() const;
	void          Add(UIBitmap&& bitmap);
	void          Add(UIText&& text);
	void          LoadAssets(Graphics& graphics, FontManager& fontManager);
	void          Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void          ComputeRect(const UIRect& parentRect, const UITransform& transform);
	UIButtonState GetState() const;
	bool          HandleInput(const Input& input);
	void          Tick(float dt);
	void          SetLabel(const char* label);

private:
	UIButton(const UIButtonDesc& desc, std::unique_ptr<UIBitmap> bitmap, std::unique_ptr<UIText> text);
	UIButtonState        RefreshState(const Input& input);
	const UIButtonStyle* GetStyle() const;

private:
	UIButtonDesc          mDesc;
	const UIButtonStyle*  mStyle;
	TexturePtr            mBackground;
	std::vector<UIBitmap> mBitmaps;
	std::vector<UIText>   mTexts;
	UIButtonState         mState;
	float                 mAnimTime;
	bool                  mClicked;
};

class UICanvas final {
public:
	explicit UICanvas(const UICanvasDesc& desc);

	void Add(UIPanel& panel);
	void Add(UIButton& button);
	void Add(UIBitmap& bitmap);
	void Add(UIText& text);
	void Add(UICheckBox& checkBox);
	void LoadAssets(Graphics& graphics, FontManager& fontManager);
	void Draw(int canvasWidth, int canvasHeight, const UIRenderer& renderer, unsigned drawOrder);
	void HandleInput(const Input& input) const;
	void Tick(float dt);

private:
	UIPanel mPanel;
};

struct UICheckBoxStyle {
	Vec2  offset { 0.f, 0.f };
	Vec2  scale { 1.f, 1.f };
	float grayScale = 0.f;
};

class UICheckBox final : public UIControl {
public:
	explicit UICheckBox(const UICheckBoxDesc& desc, const UICheckBoxStyle& style);

	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	bool IsChecked() const;
	void SetChecked(bool value);
	void LoadAssets(Graphics& graphics, FontManager& fontManager);
	void Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void ComputeRect(const UIRect& parentRect, const UITransform& transform);
	bool HandleInput(const Input& input);
	void Tick(float dt);

private:
	void RefreshState(const Input& input);

private:
	UICheckBoxDesc  mDesc;
	UICheckBoxStyle mStyle;
	UIBitmap        mCheckedIcon;
	UIBitmap        mUncheckedIcon;
	UIText          mLabel;
	TexturePtr      mBackground;
	bool            mToggled;
	bool            mEnabled;
};

class UIMouseCursor final {
public:
	void SetCursor(const char* fileName, Graphics& graphics);
	void Draw(const UIRenderer& renderer, const Vec2& mouseCoords, unsigned drawOrder);

private:
	TexturePtr mMousePointer;
};

struct UITheme {
	UITextStyle   textStyle;
	UIButtonStyle buttonStyle;
};

void SetUITheme(const UITheme* theme);

} // namespace Wind
