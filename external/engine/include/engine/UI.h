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
	float grayScale = 0.f;
	bool keepPressedOutside = false;
};

struct UISliderDesc {
	UIBaseDesc;
	UIBackgroundDesc;
	bool  horizontal = true;
	float min = 0.f;
	float max = 1.f;
	float step = 0.f;
};

struct UIGridDesc {
	int          cols = 0;
	float        colSpacing = 0.f;
	float        rowSpacing = 0.f;
	const float* colWidth = nullptr;  // 0 means stretch
	const float* rowHeight = nullptr; // 0 means stretch
};

struct UIPanelDesc {
	UIBaseDesc;
	UIBackgroundDesc;
	UIGridDesc grid;
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
	Slider,
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
	~UIPanel();

	UIBitmap*  Add(const UIBitmapDesc& bitmapDesc, int cellIdx = -1);
	UIText*    Add(const UITextDesc& textDesc, int cellIdx = -1);
	UIButton*  Add(const UIButtonDesc& buttonDesc, int cellIdx = -1);
	UIPanel*   Add(const UIPanelDesc& panelDesc, int cellIdx = -1);
	UISlider*  Add(const UISliderDesc& sliderDesc, int cellIdx = -1);
	UIControl& GetControl(int idx) const;
	UIBitmap&  GetBitmap(int idx) const;
	void       LoadAssets(Graphics& graphics, FontManager& fontManager);
	void       Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	bool       HandleInput(const Input& input) const;
	void       ComputeRect(const UIRect& parentRect, const UITransform& transform);
	void       Tick(float dt);

private:
	struct Child {
		void*         ptr;
		UIControlType type;
		int           cellIdx;
	};
	std::vector<Child> mChildren;
	UIPanelDesc        mDesc;
	TexturePtr         mBackground;
	int                mAutoCellIdx;
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

class UIBitmap final : public UIControl {
public:
	explicit UIBitmap(const UIBitmapDesc& desc, const UIBitmapStyle& style = {});

	void                LoadAssets(Graphics& graphics, FontManager& fontManager);
	void                SetColor(const Color& color);
	const UIBitmapDesc& GetDesc() const;
	UIBitmapDesc&       GetDesc();
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
using UIButtonAction = std::function<void(UIButton& button, float dt)>;

struct UIButtonStyle {
	UIButtonAction onDisabled;
	UIButtonAction onIdle;
	UIButtonAction onPressed;
	UIButtonAction onHovered;
};

class UIButton final : public UIControl {
public:
	explicit UIButton(const UIButtonDesc& desc, const UIButtonStyle* style = nullptr);

	void          SetEnabled(bool enabled);
	bool          IsEnabled() const;
	bool          IsClicked() const;
	UIBitmap&     Add(const UIBitmapDesc& bitmapDesc);
	UIText&       Add(const UITextDesc& textDesc);
	UIBitmap&     GetBitmap(size_t idx);
	UIText&       GetText(size_t idx);
	UIButtonDesc& GetDesc();
	void          LoadAssets(Graphics& graphics, FontManager& fontManager);
	void          Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void          ComputeRect(const UIRect& parentRect, const UITransform& transform);
	UIButtonState GetState() const;
	bool          HandleInput(const Input& input);
	void          Tick(float dt);
	void          SetLabel(const char* label);

private:
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

struct UISliderThumbDesc {
	Vec2        size = {};
	const char* fileName = nullptr;
	Color       color = whiteColor;
};

class UISlider final : public UIControl {
public:
	explicit UISlider(const UISliderDesc& desc);

	void      SetThumb(const UISliderThumbDesc& thumbDesc);
	UIBitmap& Add(const UIBitmapDesc& bitmapDesc);
	UIText&   Add(const UITextDesc& textDesc);
	UIBitmap& GetBitmap(size_t idx);
	UIText&   GetText(size_t idx);
	void      LoadAssets(Graphics& graphics, FontManager& fontManager);
	void      Draw(const UIRenderer& renderer, unsigned drawOrder) const;
	void      ComputeRect(const UIRect& parentRect, const UITransform& transform);
	bool      HandleInput(const Input& input);
	void      Tick(float dt);
	float     GetValue() const;
	void      SetValue(float v);

private:
	float Snap(float v) const;
	void  PositionThumb();

private:
	enum class State {
		idle,
		disabled,
		hovered,
		dragging,
	};

	UISliderDesc              mDesc;
	std::unique_ptr<UIBitmap> mThumb;
	UIPanel                   mContainer;
	State                     mState;
	Vec2                      mLastMouseCoord;
	float                     mValue = 0.f;
	bool                      mFocused;
	float                     mAnimTime;
};

class UICanvas final {
public:
	explicit UICanvas(const UICanvasDesc& desc);

	UIText*   Add(const UITextDesc& textDesc);
	UIBitmap* Add(const UIBitmapDesc& bitmapDesc);
	UIPanel*  Add(const UIPanelDesc& panelDesc);
	UIPanel&  Panel();
	void      LoadAssets(Graphics& graphics, FontManager& fontManager);
	void      Draw(int canvasWidth, int canvasHeight, const UIRenderer& renderer, unsigned drawOrder);
	void      HandleInput(const Input& input) const;
	void      Tick(float dt);

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
	UITextStyle   textStyle;
	UIButtonStyle buttonStyle;
};

void SetUITheme(const UITheme* theme);

} // namespace Wind
