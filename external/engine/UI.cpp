#include "UI.h"
#include "Easings.h"
#include "Engine.h"
#include "Font.h"
#include "FontManager.h"
#include "Graphics.h"
#include "Input.h"
#include "StringTable.h"
#include "Texture.h"
#include "UIRenderer.h"

#include <cassert>

namespace Wind {

namespace {

const UITheme defaultTheme {
	.textStyle = {
		.color = whiteColor,
		.outlineColor = blackColor,
	},
	.buttonStyle {},
};

const UITheme* uiTheme = &defaultTheme;

UIRect AlignRect(const UIPos& pos, const UISize& size, const UIRect& parentRect, UIHorizAlignment horizAlignment, UIVertAlignment vertAlignment) {
	UIRect alignedRect;
	alignedRect.size.x = size.aWidth + parentRect.size.x * size.rWidth;
	alignedRect.size.y = size.aHeight + parentRect.size.y * size.rHeight;
	alignedRect.axis = parentRect.axis;

	alignedRect.pos = parentRect.pos + Vec2 { pos.ax + parentRect.size.x * pos.rx, pos.ay + parentRect.size.y * pos.ry };
	if (horizAlignment == UIHorizAlignment::center) {
		alignedRect.pos.x += (parentRect.size.x - alignedRect.size.x) * 0.5f;
	}
	else if (horizAlignment == UIHorizAlignment::right) {
		alignedRect.pos.x += (parentRect.size.x - alignedRect.size.x);
	}
	if (vertAlignment == UIVertAlignment::center) {
		alignedRect.pos.y += (parentRect.size.y - alignedRect.size.y) * 0.5f;
	}
	else if (vertAlignment == UIVertAlignment::bottom) {
		alignedRect.pos.y += (parentRect.size.y - alignedRect.size.y);
	}
	return alignedRect;
}

UIRect AddPadding(const UIRect& rect, float padding) {
	return { rect.pos.x + padding, rect.pos.y + padding, rect.size.x - padding * 2.f, rect.size.y - padding * 2.f, rect.axis };
}

UITransform ConcatenateTransforms(const UITransform& parent, const UITransform& child) {
	return UITransform {
		.offset = parent.offset + parent.scale * child.offset,
		.scale = parent.scale * child.scale,
		.rotation = parent.rotation + child.rotation,
	};
}

UIRect TransformRect(const UIRect& rect, const Vec2& pivot01, const UITransform& transform) {
	Vec2 pivot = rect.pos + rect.size * pivot01;
	Vec2 size = rect.size * transform.scale;
	Vec2 axis = { std::cos(transform.rotation), std::sin(transform.rotation) };
	return { pivot - (size.x * pivot01.x) * axis - (size.y * pivot01.y) * Vec2{ -axis.y, axis.x } + transform.offset, size, axis };
}

} // namespace

void UIControl::SetVisible(bool visible) {
	mVisible = visible;
}

bool UIControl::IsVisible() const {
	return mVisible;
}

const UIRect& UIControl::GetRect() const {
	return mRect;
}

void UIControl::SetRect(const UIRect& rect) {
	mRect = rect;
}

UITransform& UIControl::GetTransform() {
	return mTransform;
}

const UITransform& UIControl::GetTransform() const {
	return mTransform;
}

UIButton::UIButton(const UIButtonDesc& desc, const UIButtonStyle* style)
    : UIControl { true }
    , mDesc(desc)
    , mStyle { style }
    , mState { UIButtonState::idle }
    , mAnimTime { 0.f }
    , mClicked { false } {
}

void UIButton::SetEnabled(bool enabled) {
	if (mState == UIButtonState::disabled) {
		mState = UIButtonState::idle;
	}
}

bool UIButton::IsEnabled() const {
	return mState != UIButtonState::disabled;
}

const UIButtonStyle* UIButton::GetStyle() const {
	return mStyle ? mStyle : &uiTheme->buttonStyle;
}

UIButtonState UIButton::RefreshState(const Input& input) {
	const UIRect transformedRect = mRect; // FIXME TransformRect(mRect, mFinalTransform);
	// TODO Handle rotation
	const Rect r {
		.left = transformedRect.pos.x,
		.top = transformedRect.pos.y,
		.right = r.left + transformedRect.size.x,
		.bottom = r.top + transformedRect.size.y,
	};
	const bool          mouseOver = RectContainsPoint(r, input.GetMappedMouseCoord());
	const bool          mouseDown = input.GetMouseButtonDown(MouseButton::left) || input.GetFingerDown();
	const UIButtonState currState = mState;
	switch (mState) {
	case UIButtonState::idle:
		if (mouseOver) {
			mState = UIButtonState::hovered;
		}
		break;
	case UIButtonState::hovered:
		if (! mouseOver) {
			mState = UIButtonState::idle;
		}
		else if (mouseDown) {
			mState = UIButtonState::pressed;
		}
		break;
	case UIButtonState::pressed:
		if (! mouseOver) {
			if (mDesc.keepPressedOutside == false) {
				mState = UIButtonState::idle;
			}
			// else keep button pressed even if mouse is not over it
		}
		else if (! mouseDown) {
			// TODO Action callback ?
			mState = UIButtonState::hovered;
		}
		break;
	}
	if (mState != currState) {
		mAnimTime = 0.f;
	}
	return mState;
}

bool UIButton::IsClicked() const {
	return mClicked;
}

void UIButton::Add(UIBitmap&& bitmap) {
	mBitmaps.emplace_back(std::move(bitmap));
}

void UIButton::Add(UIText&& text) {
	mTexts.emplace_back(std::move(text));
}

void UIButton::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	if (mDesc.background) {
		mBackground = graphics.LoadTexture(mDesc.background);
	}
	for (auto& bitmap : mBitmaps) {
		bitmap.LoadAssets(graphics, fontManager);
	}
	for (auto& text : mTexts) {
		text.LoadAssets(graphics, fontManager);
	}
}

void UIButton::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	auto style = GetStyle();
	if (mBackground) {
		UIRect                 rect = TransformRect(mRect, mDesc.pivot, mFinalTransform);
		const UIDrawBitmapArgs prm {
			.color = mDesc.backgroundColor,
			.blendMode = UIBlendMode::Auto,
			.priority = drawOrder,
			._9patch = mDesc._9patch,
			.grayscale = style->grayScale,
		};
		renderer.DrawBitmap(rect, *mBackground, prm);
	}
	for (auto& bitmap : mBitmaps) {
		bitmap.Draw(renderer, drawOrder + 1);
	}
	for (auto& text : mTexts) {
		text.Draw(renderer, drawOrder + 2); // text over bitmap
	}
}

void UIButton::ComputeRect(const UIRect& parentRect, const UITransform& parentTransform) {
	UITransform finalTransform = ConcatenateTransforms(parentTransform, mTransform);
	UIRect      rect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	SetRect(rect);

	UIRect paddedRect = AddPadding(rect, mDesc.padding);
	for (auto& bitmap : mBitmaps) {
		bitmap.ComputeRect(paddedRect, finalTransform);
	}
	for (auto& text : mTexts) {
		text.ComputeRect(paddedRect, finalTransform);
	}

	mFinalTransform = finalTransform;
}

UIButtonState UIButton::GetState() const {
	return mState;
}

bool UIButton::HandleInput(const Input& input) {
	if (mState == UIButtonState::disabled) {
		return false;
	}
	UIButtonState currState = mState;
	UIButtonState newState = RefreshState(input);
	// TODO Make it configurable ?
	bool clicked = (currState == UIButtonState::pressed) && (newState == UIButtonState::hovered);
	mClicked = clicked;
	return (currState == UIButtonState::pressed);
}

void UIButton::Tick(float dt) {
	if (mState == UIButtonState::disabled) {
		return;
	}
	auto style = GetStyle();
	switch (mState) {
	case UIButtonState::idle:
		if (style->onIdle) {
			style->onIdle(mTransform, dt);
		}
		break;
	case UIButtonState::hovered:
		if (style->onHovered) {
			style->onHovered(mTransform, dt);
		}
		break;
	case UIButtonState::pressed:
		if (style->onPressed) {
			style->onPressed(mTransform, dt);
		}
		break;
	}
	mAnimTime += dt;
}

void UIButton::SetLabel(const char* label) {
	assert(label);
	if (! mTexts.empty()) {
		mTexts[0].SetText(label);
	}
}

UIText::UIText(const UITextDesc& desc)
    : UIControl { desc.visible }
    , mDesc(desc)
    , mText {}
    , mStyle { mDesc.style } {
}

void UIText::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	if (mDesc.font) {
		mFont = fontManager.AddFont(mDesc.font);
	}
	else {
		mFont.reset();
	}
}

void UIText::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	if (mFont) {
		const char* str = Text();
		if (str) {
			const UIRect transformedRect = TransformRect(mRect, mDesc.pivot, mFinalTransform);
			renderer.GetTextRenderer().Write(*mFont, str, transformedRect, mStyle, drawOrder);
		}
	}
}

void UIText::ComputeRect(const UIRect& parentRect, const UITransform& parentTransform) {
	UITransform finalTransform = ConcatenateTransforms(parentTransform, mTransform);
	UIRect      rect {};
	if (mDesc.sizing == UITextSizing::stretch) {
		rect = parentRect;
	}
	else if (mDesc.sizing == UITextSizing::user) {
		rect = AlignRect(UIAbsolutePos(mDesc.pos.x, mDesc.pos.y), mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	}
	else {
		if (mFont) {
			const char*  str = Text();
			const UISize textSize {
				.aWidth = static_cast<float>(mFont->CalculateStringWidth(str)),
				.aHeight = static_cast<float>(mFont->GetHeight()),
				.rWidth = 0.f,
				.rHeight = 0.f,
			};
			rect = AlignRect(UIAbsolutePos(mDesc.pos.x, mDesc.pos.y), textSize, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
		}
		else {
			rect = UIZeroRect;
		}
	}
	mFinalTransform = finalTransform;
	SetRect(rect);
}

void UIText::SetText(StringId stringId) {
	mDesc.stringId = stringId;
}

void UIText::SetText(const char* str) {
	int count = SDL_snprintf(mText, sizeof mText, "%s", str);
	if (count >= sizeof mText) {
		SDL_LogWarn(0, "Truncating UI text %s", str);
	}
}

void UIText::SetStyle(const UITextStyle& style) {
	mStyle = style;
}

const UITextStyle& UIText::GetStyle() const {
	return mStyle;
}

const char* UIText::Text() const {
	return mText[0] ? mText : GetString(mDesc.stringId);
}

UIBitmap::UIBitmap(const UIBitmapDesc& desc, const UIBitmapStyle& style)
    : UIControl { desc.visible }
    , mDesc { desc }
    , mStyle { style } {
}

void UIBitmap::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	if (mDesc.fileName) {
		mBitmap = graphics.LoadTexture(mDesc.fileName);
	}
	else {
		mBitmap.reset();
	}
}

void UIBitmap::SetColor(const Color& color) {
	mDesc.color = color;
}

const UIBitmapDesc& UIBitmap::GetDesc() const {
	return mDesc;
}

void UIBitmap::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	if (mBitmap) {
		const UIDrawBitmapArgs prm {
			.color = Mul(mDesc.color, mStyle.color),
			.blendMode = UIBlendMode::Auto,
			.priority = drawOrder,
		};
		const UIRect transformedRect = TransformRect(mRect, mDesc.pivot, mFinalTransform);
		renderer.DrawBitmap(transformedRect, *mBitmap, prm);
	}
}

void UIBitmap::ComputeRect(const UIRect& parentRect, const UITransform& parentTransform) {
	UITransform finalTransform = ConcatenateTransforms(parentTransform, mTransform);
	UIRect      rect {};
	if (mDesc.sizing == UIBitmapSizing::fit) {
		if (mBitmap) {
			UISize size;
			size.aWidth = static_cast<float>(mBitmap->Width());   // * finalTransform.scale.x;
			size.aHeight = static_cast<float>(mBitmap->Height()); // * transform.scale.y;
			size.rWidth = 0.f;
			size.rHeight = 0.f;
			rect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
			// rect.pos = rect.pos + transform.offset;
		}
		else {
			rect = UIZeroRect;
		}
	}
	else if (mDesc.sizing == UIBitmapSizing::stretch) {
		rect = parentRect;
	}
	else if (mDesc.sizing == UIBitmapSizing::user) {
		UISize size = mDesc.size;
		// size.aWidth *= transform.scale.x;
		// size.aHeight *= transform.scale.y;
		rect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	}
	SetRect(rect);
	mFinalTransform = finalTransform;
}

void UIBitmap::SetBitmap(const TexturePtr& bitmap) {
	mBitmap = bitmap;
}

const Texture* UIBitmap::GetBitmap() const {
	return mBitmap.get();
}

void UIBitmap::SetStyle(const UIBitmapStyle& style) {
	mStyle = style;
}

void UIPanel::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	if (mDesc.background) {
		mBackground = graphics.LoadTexture(mDesc.background);
	}

#define Dispatch(Type)                                \
	{                                                 \
		auto control = static_cast<Type*>(child.ptr); \
		control->LoadAssets(graphics, fontManager);   \
	}

	for (const auto& child : mChildren) {
		switch (child.type) {
		case UIControlType::Panel:
			Dispatch(UIPanel);
			break;
		case UIControlType::Bitmap:
			Dispatch(UIBitmap);
			break;
		case UIControlType::Text:
			Dispatch(UIText);
			break;
		case UIControlType::Button:
			Dispatch(UIButton);
			break;
		case UIControlType::Checkbox:
			Dispatch(UICheckBox);
			break;
		default:
			break;
		}
	}
#undef Dispatch
}

UIPanel::UIPanel(const UIPanelDesc& desc)
    : UIControl { true }
    , mDesc(desc) {
}

void UIPanel::Add(UIPanel& panel) {
	mChildren.push_back({ &panel, UIControlType::Panel });
}

void UIPanel::Add(UIButton& button) {
	mChildren.push_back({ &button, UIControlType::Button });
}

void UIPanel::Add(UIBitmap& bitmap) {
	mChildren.push_back({ &bitmap, UIControlType::Bitmap });
}

void UIPanel::Add(UIText& text) {
	mChildren.push_back({ &text, UIControlType::Text });
}

void UIPanel::Add(UICheckBox& checkBox) {
	mChildren.push_back({ &checkBox, UIControlType::Checkbox });
}

void UIPanel::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	assert(IsVisible());
	if (mBackground) {
		UIDrawBitmapArgs prms;
		prms.blendMode = UIBlendMode::Auto;
		prms.color = mDesc.backgroundColor;
		prms.priority = drawOrder;
		prms._9patch = mDesc._9patch;
		renderer.DrawBitmap(GetRect(), *mBackground, prms);
	}
	else {
		renderer.DrawSolidRect(GetRect(), mDesc.backgroundColor, UIBlendMode::Off);
	}

#define Dispatch(Type)                                \
	{                                                 \
		auto control = static_cast<Type*>(child.ptr); \
		if (control->IsVisible()) {                   \
			control->Draw(renderer, drawOrder + 1);   \
		}                                             \
	}

	for (const auto& child : mChildren) {
		switch (child.type) {
		case UIControlType::Panel:
			Dispatch(UIPanel);
			break;
		case UIControlType::Bitmap:
			Dispatch(UIBitmap);
			break;
		case UIControlType::Text:
			Dispatch(UIText);
			break;
		case UIControlType::Button:
			Dispatch(UIButton);
			break;
		case UIControlType::Checkbox:
			Dispatch(UICheckBox);
			break;
		default:
			break;
		}
	}
#undef Dispatch
}

bool UIPanel::HandleInput(const Input& input) const {
#define Dispatch(Type)                                \
	{                                                 \
		auto control = static_cast<Type*>(child.ptr); \
		if (control->IsVisible() && ! handled) {      \
			handled = control->HandleInput(input);    \
		}                                             \
	}

	bool handled = false;
	for (const auto& child : mChildren) {
		switch (child.type) {
		case UIControlType::Panel:
			Dispatch(UIPanel);
			break;
		case UIControlType::Button:
			Dispatch(UIButton);
			break;
		case UIControlType::Checkbox:
			Dispatch(UICheckBox);
			break;
		default:
			break;
		}
	}
#undef Dispatch
	return handled;
}

void UIPanel::Tick(float dt) {
#define Dispatch(Type)                                \
	{                                                 \
		auto control = static_cast<Type*>(child.ptr); \
		control->Tick(dt);                            \
	}

	for (const auto& child : mChildren) {
		switch (child.type) {
		case UIControlType::Panel:
			Dispatch(UIPanel);
			break;
		case UIControlType::Button:
			Dispatch(UIButton);
			break;
		case UIControlType::Checkbox:
			Dispatch(UICheckBox);
			break;
		default:
			break;
		}
	}
#undef Dispatch
}

void UIPanel::ComputeRect(const UIRect& parentRect, const UITransform& transform) {
	const UIRect rect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	SetRect(rect);
	UIRect paddedRect = AddPadding(rect, mDesc.padding);

#define Dispatch(Type)                                   \
	{                                                    \
		auto control = static_cast<Type*>(child.ptr);    \
		if (control->IsVisible()) {                      \
			control->ComputeRect(paddedRect, transform); \
		}                                                \
	}

	for (const auto& child : mChildren) {
		switch (child.type) {
		case UIControlType::Panel:
			Dispatch(UIPanel);
			break;
		case UIControlType::Bitmap:
			Dispatch(UIBitmap);
			break;
		case UIControlType::Text:
			Dispatch(UIText);
			break;
		case UIControlType::Button:
			Dispatch(UIButton);
			break;
		case UIControlType::Checkbox:
			Dispatch(UICheckBox);
			break;
		default:
			break;
		}
	}
#undef Dispatch
}

UICanvas::UICanvas(const UICanvasDesc& desc)
    : mPanel(UIPanelDesc {
          .pos = UIZeroPos,
          .size = { 0.f, 0.f, 1.f, 1.f },
          .padding = desc.padding,
          .background = desc.background,
          .backgroundColor = desc.backgroundColor,
      }) {
}

void UICanvas::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	mPanel.LoadAssets(graphics, fontManager);
}

void UICanvas::Add(UIPanel& panel) {
	mPanel.Add(panel);
}

void UICanvas::Add(UIButton& button) {
	mPanel.Add(button);
}

void UICanvas::Add(UIBitmap& bitmap) {
	mPanel.Add(bitmap);
}

void UICanvas::Add(UIText& text) {
	mPanel.Add(text);
}

void UICanvas::Add(UICheckBox& checkBox) {
	mPanel.Add(checkBox);
}

void UICanvas::Draw(int canvasWidth, int canvasHeight, const UIRenderer& renderer, unsigned drawOrder) {
	const UIRect canvasRect {
		0.f, 0.f, (float)canvasWidth, (float)canvasHeight, 1.f, 0.f,
	};
	const UITransform nullTransform {};
	mPanel.ComputeRect(canvasRect, nullTransform);
	mPanel.Draw(renderer, drawOrder);
}

void UICanvas::HandleInput(const Input& input) const {
	mPanel.HandleInput(input);
}

void UICanvas::Tick(float dt) {
	mPanel.Tick(dt);
}

UICheckBox::UICheckBox(const UICheckBoxDesc& desc, const UICheckBoxStyle& style)
    : UIControl { true }
    , mDesc(desc)
    , mStyle(style)
    , mCheckedIcon { desc.checkedIcon }
    , mUncheckedIcon { desc.uncheckedIcon }
    , mLabel { desc.label }
    , mToggled { mDesc.toggled }
    , mEnabled { true } {
}

bool UICheckBox::IsChecked() const {
	return mToggled;
}

void UICheckBox::SetChecked(bool value) {
	mToggled = value;
}

void UICheckBox::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	if (mDesc.background) {
		mBackground = graphics.LoadTexture(mDesc.background);
	}
	mCheckedIcon.LoadAssets(graphics, fontManager);
	mUncheckedIcon.LoadAssets(graphics, fontManager);
	mLabel.LoadAssets(graphics, fontManager);
}

void UICheckBox::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	assert(IsVisible());
	if (mBackground) {
		const UIDrawBitmapArgs prm {
			.color = mDesc.backgroundColor, .blendMode = UIBlendMode::Auto, .priority = drawOrder, ._9patch = mDesc._9patch,
			//.grayscale = style.grayScale,
		};
		renderer.DrawBitmap(GetRect(), *mBackground, prm);
	}
	const UIBitmap* icon = mToggled ? &mCheckedIcon : &mUncheckedIcon;
	if (icon->IsVisible()) {
		icon->Draw(renderer, drawOrder + 1);
	}
	mLabel.Draw(renderer, drawOrder + 1);
}

void UICheckBox::ComputeRect(const UIRect& parentRect, const UITransform& transform) {
	auto   style = mStyle; // GetStyle();
	UIRect rect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	// rect.pos = rect.pos + style.offset;
	// rect = ScaleRect(rect, style.scale);
	SetRect(rect);

	rect = AddPadding(rect, mDesc.padding); // TODO Scaled ?
	mCheckedIcon.ComputeRect(rect, transform);
	mUncheckedIcon.ComputeRect(rect, transform);
	mLabel.ComputeRect(rect, transform);
}

bool UICheckBox::HandleInput(const Input& input) {
	if (! mEnabled) {
		return false;
	}
	const UIRect& rect = GetRect();
	const Rect    r {
		   .left = rect.pos.x,
		   .top = rect.pos.y,
		   .right = r.left + rect.size.x,
		   .bottom = r.top + rect.size.y,
	};
	const bool over = RectContainsPoint(r, input.GetMappedMouseCoord());
	const bool pressed = input.GetMouseButtonPressed(MouseButton::left) || input.GetFingerPressed();
	if (over && pressed) {
		mToggled = ! mToggled;
		return true;
	}
	return false;
}

void UICheckBox::Tick(float dt) {
	if (! mEnabled) {
		return;
	}
}

void UIMouseCursor::SetCursor(const char* fileName, Graphics& graphics) {
	mMousePointer = graphics.LoadTexture(fileName);
}

void UIMouseCursor::Draw(const UIRenderer& renderer, const Vec2& mouseCoords, unsigned drawOrder) {
	if (mMousePointer) {
		const UIDrawBitmapArgs prm {
			.blendMode = UIBlendMode::On,
			.priority = drawOrder,
		};
		UIRect mouseRect { mouseCoords.x, mouseCoords.y, (float)mMousePointer->Width(), (float)mMousePointer->Height(), 1.f, 0.f };
		renderer.DrawBitmap(mouseRect, *mMousePointer, prm);
	}
}

void SetUITheme(const UITheme* theme) {
	uiTheme = theme ? theme : &defaultTheme;
}

} // namespace Wind
