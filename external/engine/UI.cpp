#include "UI.h"
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
		.scale = 1.f,
	},
	.bitmapStyle {},
	.buttonStyle {
		.idle {},
		.disabled {
			.grayScale = 100.f,
		},
		.hovered {},
		.pressed {
			.offset { 2.f, 2.f },
		},
	},
};

const UITheme* uiTheme = &defaultTheme;

UIRect AlignRect(const UIPos& pos, const UISize& size, const UIRect& parentRect, UIHorizAlignment horizAlignment, UIVertAlignment vertAlignment) {
	UIRect alignedRect;
	alignedRect.size.x = size.aWidth + parentRect.size.x * size.rWidth;
	alignedRect.size.y = size.aHeight + parentRect.size.y * size.rHeight;

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
	return { rect.pos.x + padding, rect.pos.y + padding, rect.size.x - padding * 2.f, rect.size.y - padding * 2.f };
}

UIRect ScaleRect(const UIRect& rect, Vec2 scale) {
	assert(scale.x > 0.f);
	assert(scale.y > 0.f);
	Vec2 center = rect.pos + rect.size * 0.5f;
	Vec2 newExtents = rect.size * scale * 0.5;
	return { center - newExtents, newExtents * 2.f };
}

} // namespace

void UIControl::SetEnabled(bool enabled) {
	mEnabled = enabled;
}

bool UIControl::IsEnabled() const {
	return mEnabled;
}

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

UIButton::UIButton(const UIButtonDesc& desc)
    : UIButton(desc, nullptr, nullptr) {
}

UIButton::UIButton(const UIButtonDesc& desc, std::unique_ptr<UIBitmap> bitmap, std::unique_ptr<UIText> text)
    : UIControl { true }
    , mDesc(desc)
    , mIcon(std::move(bitmap))
    , mLabel(std::move(text))
    , mState { UIButtonState::idle }
    , mClicked { false } {
}

UIButton::UIButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc, const UITextDesc& labelDesc)
    : UIButton(desc, std::make_unique<UIBitmap>(bitmapDesc), std::make_unique<UIText>(labelDesc)) {
}

UIButton::UIButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc)
    : UIButton(desc, std::make_unique<UIBitmap>(bitmapDesc), nullptr) {
}

UIButton::UIButton(const UIButtonDesc& desc, const UITextDesc& labelDesc)
    : UIButton(desc, nullptr, std::make_unique<UIText>(labelDesc)) {
}

const UIButtonSubStyle* UIButton::GetStyle() const {
	const UIButtonStyle*    style = &defaultTheme.buttonStyle;
	const UIButtonSubStyle* subStyle = &style->idle;
	if (IsEnabled()) {
		switch (mState) {
		case UIButtonState::idle:
			subStyle = &style->idle;
			break;
		case UIButtonState::pressed:
			subStyle = &style->pressed;
			break;
		case UIButtonState::hovered:
			subStyle = &style->hovered;
			break;
		};
	}
	else {
		subStyle = &style->disabled;
	}
	assert(subStyle);
	return subStyle;
}

UIButtonState UIButton::RefreshState(const Input& input) {
	const Rect r {
		.left = GetRect().pos.x,
		.top = GetRect().pos.y,
		.right = r.left + GetRect().size.x,
		.bottom = r.top + GetRect().size.y,
	};
	const bool mouseOver = RectContainsPoint(r, input.GetMappedMouseCoord());
	const bool mouseDown = input.GetMouseButtonDown(MouseButton::left) || input.GetFingerDown();
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
	return mState;
}

bool UIButton::IsClicked() const {
	return mClicked;
}

void UIButton::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	if (mDesc.background) {
		mBackground = graphics.LoadTexture(mDesc.background);
	}
	if (mIcon) {
		mIcon->LoadAssets(graphics, fontManager);
	}
	if (mLabel) {
		mLabel->LoadAssets(graphics, fontManager);
	}
}

void UIButton::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	auto style = GetStyle();
	if (mBackground) {
		const UIDrawParams prm {
			.color = mDesc.backgroundColor,
			.blendMode = UIBlendMode::Auto,
			.priority = drawOrder,
			._9patch = mDesc._9patch,
			.grayscale = style->grayScale,
		};
		renderer.DrawBitmap(GetRect(), *mBackground, prm);
	}
	if (mIcon) {
		mIcon->Draw(renderer, drawOrder + 1);
	}
	if (mLabel) {
		mLabel->Draw(renderer, drawOrder + 2); // text over bitmap
	}
}

void UIButton::ComputeRect(const UIRect& parentRect) {
	auto   style = GetStyle();
	UIRect rect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	rect = ScaleRect(rect, style->scale);
	rect.pos = rect.pos + style->offset;
	SetRect(rect);
	UIRect paddedRect = AddPadding(rect, mDesc.padding);

	if (mIcon) {
		mIcon->ComputeRect(paddedRect);
	}
	if (mLabel) {
		mLabel->ComputeRect(paddedRect);
	}
}

UIButtonState UIButton::GetState() const {
	return mState;
}

const UIButtonDesc& UIButton::GetDesc() const {
	return mDesc;
}

void UIButton::SetDesc(const UIButtonDesc& desc) {
	mDesc = desc;
}

bool UIButton::HandleInput(const Input& input) {
	if (! IsEnabled()) {
		return false;
	}

	UIButtonState currState = mState;
	UIButtonState newState = RefreshState(input);
	// TODO Make it configurable ?
	bool clicked = (currState == UIButtonState::pressed) && (newState == UIButtonState::hovered);
	mClicked = clicked;
	return (currState == UIButtonState::pressed);
}

UIText::UIText(const UITextDesc& desc)
    : UIControl { desc.visible }
    , mDesc(desc)
    , mText {}
    , mTextStyle { mDesc.style } {
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
			renderer.GetTextRenderer().Write(*mFont, str, GetRect().pos, mTextStyle, TextDirection::leftToRight, drawOrder);
		}
	}
}

void UIText::ComputeRect(const UIRect& parentRect) {
	UIRect rect {};
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
				.aWidth = static_cast<float>(mFont->CalculateStringWidth(str) * mTextStyle.scale.x),
				.aHeight = static_cast<float>(mFont->GetHeight() * mTextStyle.scale.y),
				.rWidth = 0.f,
				.rHeight = 0.f,
			};
			rect = AlignRect(UIAbsolutePos(mDesc.pos.x, mDesc.pos.y), textSize, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
		}
		else {
			rect = UIZeroRect;
		}
	}
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

void UIText::SetStyle(const TextStyle& style) {
	mTextStyle = style;
}

const TextStyle& UIText::GetStyle() const {
	return mTextStyle;
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

void UIBitmap::SetPosition(const Vec2& pos) {
	mDesc.pos = UIAbsolutePos(pos.x, pos.y);
}

void UIBitmap::SetColor(const Color& color) {
	mDesc.color = color;
}

const UIBitmapDesc& UIBitmap::GetDesc() const {
	return mDesc;
}

void UIBitmap::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	if (mBitmap) {
		const Color        tintedColor = Mul(mDesc.color, mStyle.color);
		const UIDrawParams prm {
			.color = tintedColor,
			.blendMode = UIBlendMode::Auto,
			.priority = drawOrder,
		};
		renderer.DrawBitmap(GetRect(), *mBitmap, prm);
	}
}

void UIBitmap::ComputeRect(const UIRect& parentRect) {
	UIRect rect {};
	if (mDesc.sizing == UIBitmapSizing::fit) {
		if (mBitmap) {
			UISize size;
			size.aWidth = static_cast<float>(mBitmap->Width()) * mStyle.scale.x;
			size.aHeight = static_cast<float>(mBitmap->Height()) * mStyle.scale.y;
			size.rWidth = 0.f;
			size.rHeight = 0.f;
			rect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
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
		size.aWidth *= mStyle.scale.x;
		size.aHeight *= mStyle.scale.y;
		rect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	}
	SetRect(rect);
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
		UIDrawParams prms;
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
#define Dispatch(Type)                                                   \
	{                                                                    \
		auto control = static_cast<Type*>(child.ptr);                    \
		if (control->IsEnabled() && control->IsVisible() && ! handled) { \
			handled = control->HandleInput(input);                       \
		}                                                                \
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

void UIPanel::ComputeRect(const UIRect& parentRect) {
	const UIRect rect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	SetRect(rect);
	UIRect paddedRect = AddPadding(rect, mDesc.padding);
	UpdateLayout(paddedRect);
}

void UIPanel::UpdateLayout(const UIRect& parentRect) const {
#define Dispatch(Type)                                \
	{                                                 \
		auto control = static_cast<Type*>(child.ptr); \
		if (control->IsVisible()) {                   \
			control->ComputeRect(parentRect);         \
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

UICanvas::UICanvas()
    : mPanel({ UIZeroPos, UIParentSize }) {
	mPanel.SetVisible(true);
}

UICanvas::UICanvas(const UICanvasDesc& desc)
    : mPanel(UIPanelDesc {
          .pos = UIZeroPos,
          .size = UIParentSize,
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
	const UIRect canvasRect { { 0.f, 0.f }, (float)canvasWidth, (float)canvasHeight };
	mPanel.ComputeRect(canvasRect);
	mPanel.Draw(renderer, drawOrder);
}

void UICanvas::HandleInput(const Input& input) const {
	mPanel.HandleInput(input);
}

UICheckBox::UICheckBox(const UICheckBoxDesc& desc, const UICheckBoxStyle& style)
    : UIControl { true }
    , mDesc(desc)
    , mStyle(style)
    , mCheckedIcon { desc.checkedIcon }
    , mUncheckedIcon { desc.uncheckedIcon }
    , mLabel { desc.label }
    , mToggled { mDesc.toggled } {
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
		const UIDrawParams prm {
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

void UICheckBox::ComputeRect(const UIRect& parentRect) {
	auto   style = mStyle; // GetStyle();
	UIRect rect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	rect.pos = rect.pos + style.offset;
	rect = ScaleRect(rect, style.scale);
	SetRect(rect);

	rect = AddPadding(rect, mDesc.padding); // TODO Scaled ?
	mCheckedIcon.ComputeRect(rect);
	mUncheckedIcon.ComputeRect(rect);
	mLabel.ComputeRect(rect);
}

bool UICheckBox::HandleInput(const Input& input) {
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

void UIMouseCursor::SetCursor(const char* fileName, Graphics& graphics) {
	mMousePointer = graphics.LoadTexture(fileName);
}

void UIMouseCursor::Draw(const UIRenderer& renderer, const Vec2& mouseCoords, unsigned drawOrder) {
	if (mMousePointer) {
		const UIDrawParams prm {
			.blendMode = UIBlendMode::On,
			.priority = drawOrder,
		};
		renderer.DrawBitmap({ mouseCoords.x, mouseCoords.y, (float)mMousePointer->Width(), (float)mMousePointer->Height() }, *mMousePointer, prm);
	}
}

void SetTheme(const UITheme* theme) {
	uiTheme = theme ? theme : &defaultTheme;
}

} // namespace Wind
