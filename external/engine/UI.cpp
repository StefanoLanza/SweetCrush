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
	.idleButtonStyle {},
	.pressedButtonStyle {
	    .mIconStyle { .scale = 1.0f },
	    .mLabelStyle { .scale = 1.0f },
		.offset { 2.f, 2.f },
	},
	.hoveredButtonStyle {},
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

UIRect ScaleRect(const UIRect& rect, float scale) {
	assert(scale > 0.f);
	Vec2 center = rect.pos + rect.size * 0.5f;
	Vec2 newExtents = rect.size * 0.5f * scale;
	return { center - newExtents, newExtents * 2.f };
}

} // namespace

UIButton::UIButton(const UIButtonDesc& desc)
    : UIButton(desc, nullptr, nullptr) {
}

UIButton::UIButton(const UIButtonDesc& desc, std::unique_ptr<UIBitmap> bitmap, std::unique_ptr<UIText> text)
    : mDesc(desc)
    , mIcon(std::move(bitmap))
    , mLabel(std::move(text))
    , mRect {}
    , mState { UIButtonState::idle }
    , mVisible { true }
    , mToggled { mDesc.toggled }
    , mClicked { false } {
}

UIButton::UIButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc, const UITextDesc& labelDesc)
    : UIButton(desc, std::make_unique<UIBitmap>(bitmapDesc), std::make_unique<UIText>(labelDesc, defaultTheme.textStyle)) { // FIXME Style
}

UIButton::UIButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc)
    : UIButton(desc, std::make_unique<UIBitmap>(bitmapDesc), nullptr) {
}

UIButton::UIButton(const UIButtonDesc& desc, const UITextDesc& labelDesc)
    : UIButton(desc, nullptr, std::make_unique<UIText>(labelDesc, defaultTheme.textStyle)) { // FIXME Style
}

void UIButton::SetEnabled(bool enabled) {
	if (enabled && mState == UIButtonState::disabled) {
		mState = UIButtonState::idle;
	}
	else if (! enabled) {
		mState = UIButtonState::disabled;
	}
}

void UIButton::SetVisible(bool visible) {
	mVisible = visible;
}

bool UIButton::IsVisible() const {
	return mVisible;
}

UIButtonState UIButton::RefreshState(const Input& input) {
	const Rect r {
		.left = mRect.pos.x,
		.top = mRect.pos.y,
		.right = r.left + mRect.size.x,
		.bottom = r.top + mRect.size.y,
	};
	const bool mouseOver = RectContainsPoint(r, input.GetMappedMouseCoord());
	const bool mouseDown = input.GetMouseButtonDown(MouseButton::left) || input.GetFingerDown();
	switch (mState) {
	case UIButtonState::disabled:
		break;
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
	assert(! mDesc.toggleMode);
	return mClicked;
}

bool UIButton::IsToggled() const {
	assert(mDesc.toggleMode);
	return mToggled;
}

void UIButton::SetToggled(bool value) {
	assert(mDesc.toggleMode);
	mToggled = value;
}

void UIButton::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	if (mDesc.background) {
		mBackground = graphics.LoadTexture(mDesc.background);
	}
	if (mIcon) {
		mIcon->LoadGraphics(graphics);
	}
	if (mLabel) {
		mLabel->Load(fontManager);
	}
}

void UIButton::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	if (! mVisible) {
		return;
	}
	if (mBackground) {
		const UIDrawParams prm {
			.color = mDesc.backgroundColor,
			.blending = mBackground->HasAlpha() || (mDesc.backgroundColor.a < 255.f),
			.priority = drawOrder,
			._9patch = mDesc._9patch,
		};
		renderer.DrawBitmap(mRect, *mBackground, prm);
	}
	if (mIcon) {
		if ((mDesc.toggleMode == false) || (mDesc.toggleMode && mToggled)) {
			mIcon->Draw(renderer, drawOrder + 1);
		}
	}
	if (mLabel) {
		mLabel->Draw(renderer.GetTextRenderer(), drawOrder + 2); // text over bitmap
	}
}

void UIButton::UpdateRect(const UIRect& parentRect) {
	const UIButtonStyle* style = nullptr;
	switch (mState) {
	case UIButtonState::disabled:
		style = &defaultTheme.disabledButtonStyle;
		break;
	case UIButtonState::idle:
		style = &defaultTheme.idleButtonStyle;
		break;
	case UIButtonState::pressed:
		style = &defaultTheme.pressedButtonStyle;
		break;
	case UIButtonState::hovered:
		style = &defaultTheme.hoveredButtonStyle;
		break;
	};
	if (mLabel) {
		mLabel->SetStyle(style->mLabelStyle);
	}
	if (mIcon) {
		mIcon->SetStyle(style->mIconStyle);
	}

	mRect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	mRect = ScaleRect(mRect, mDesc.scale);
	mRect.pos = mRect.pos + style->offset;

	UIRect paddedRect = AddPadding(mRect, mDesc.padding);

	if (mIcon) {
		mIcon->ComputeRect(paddedRect);
	}
	if (mLabel) {
		mLabel->ComputeRect(paddedRect);
	}
}

UIBitmap* UIButton::GetBitmap() const {
	return mIcon.get();
}

UIText* UIButton::GetText() const {
	return mLabel.get();
}

const UIRect& UIButton::GetRect() const {
	return mRect;
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
	UIButtonState currState = mState;
	UIButtonState newState = RefreshState(input);
	// TODO Make it configurable ?
	bool clicked = (currState == UIButtonState::pressed) && (newState == UIButtonState::hovered);
	mClicked = clicked;
	if (mDesc.toggleMode && clicked) {
		mToggled = ! mToggled;
	}
	return (currState == UIButtonState::pressed);
}

UIText::UIText(const UITextDesc& desc, const TextStyle& style)
    : mDesc(desc)
    , mRect {}
    , mText {}
    , mTextStyle { style } {
}

void UIText::Load(FontManager& fontManager) {
	mFont = fontManager.AddFont(mDesc.font);
}

void UIText::Draw(const UITextRenderer& textRenderer, unsigned drawOrder) const {
	if (mFont) {
		const char* str = Text();
		if (str) {
			textRenderer.Write(*mFont, str, mRect.pos, mTextStyle, TextDirection::leftToRight, drawOrder);
		}
	}
}

void UIText::ComputeRect(const UIRect& parentRect) {
	if (mDesc.sizing == UITextSizing::stretch) {
		mRect = parentRect;
	}
	else if (mDesc.sizing == UITextSizing::user) {
		mRect = AlignRect(UIAbsolutePos(mDesc.pos.x, mDesc.pos.y), mDesc.size, parentRect, mDesc.horizontalAlignment,
		                  mDesc.verticalAlignment);
	}
	else {
		if (! mFont) {
			mRect = UIZeroRect;
			return;
		}
		const char* str = Text();

		const UISize textSize {
			.aWidth = static_cast<float>(mFont->CalculateStringWidth(str) * mTextStyle.scale),
			.aHeight = static_cast<float>(mFont->GetHeight() * mTextStyle.scale),
			.rWidth = 0.f,
			.rHeight = 0.f,
		};
		mRect = AlignRect(UIAbsolutePos(mDesc.pos.x, mDesc.pos.y), textSize, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	}
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
    : mDesc { desc }
    , mStyle { style }
    , mRect {} {
}

void UIBitmap::LoadGraphics(Graphics& graphics) {
	mBitmap = graphics.LoadTexture(mDesc.fileName);
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
			.blending = mBitmap->HasAlpha() || tintedColor.a < 255.f,
			.priority = drawOrder,
		};
		renderer.DrawBitmap(mRect, *mBitmap, prm);
	}
}

void UIBitmap::ComputeRect(const UIRect& parentRect) {
	if (mDesc.sizing == UIBitmapSizing::fit) {
		if (mBitmap) {
			UISize size;
			size.aWidth = static_cast<float>(mBitmap->Width()) * mStyle.scale;
			size.aHeight = static_cast<float>(mBitmap->Height()) * mStyle.scale;
			size.rWidth = 0.f;
			size.rHeight = 0.f;
			mRect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
		}
		else {
			mRect = UIZeroRect;
		}
	}
	else if (mDesc.sizing == UIBitmapSizing::stretch) {
		mRect = parentRect;
	}
	else if (mDesc.sizing == UIBitmapSizing::user) {
		UISize size = mDesc.size;
		size.aWidth *= mStyle.scale;
		size.aHeight *= mStyle.scale;
		mRect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	}
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

void UIContainer::AddPanel(UIPanel& panel) {
	mPanels.push_back(&panel);
}

void UIContainer::AddButton(UIButton& button) {
	mButtons.push_back(&button);
}

void UIContainer::AddBitmap(UIBitmap& bitmap) {
	mBitmaps.push_back(&bitmap);
}

void UIContainer::AddText(UIText& text) {
	mTexts.push_back(&text);
}

void UIContainer::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	for (auto& panel : mPanels) {
		panel->LoadAssets(graphics, fontManager);
	}
	for (auto& bitmap : mBitmaps) {
		bitmap->LoadGraphics(graphics);
	}
	for (auto& button : mButtons) {
		button->LoadAssets(graphics, fontManager);
	}
	for (auto& text : mTexts) {
		text->Load(fontManager);
	}
}

UIPanel::UIPanel(const UIPanelDesc& desc)
    : mDesc(desc)
    , mRect {}
    , mVisible(true) {
}

void UIPanel::SetVisible(bool visible) {
	mVisible = visible;
}

bool UIPanel::IsVisible() const {
	return mVisible;
}

const UIRect& UIPanel::Rect() const {
	return mRect;
}

void UIPanel::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	if (mDesc.background) {
		mBackground = graphics.LoadTexture(mDesc.background);
	}
	UIContainer::LoadAssets(graphics, fontManager);
}

void UIPanel::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	if (! mVisible) {
		return;
	}
	if (mBackground) {
		UIDrawParams prms;
		prms.blending = (mDesc.backgroundColor.a < 255.f) || mBackground->HasAlpha();
		prms.color = mDesc.backgroundColor;
		prms.priority = drawOrder;
		renderer.DrawBitmap(mRect, *mBackground, prms);
	}
	for (const auto& bitmap : mBitmaps) {
		bitmap->Draw(renderer, drawOrder + 1);
	}
	for (const auto& panel : mPanels) {
		panel->Draw(renderer, drawOrder + 2);
	}
	for (const auto& button : mButtons) {
		button->Draw(renderer, drawOrder + 3);
	}
	for (const auto& text : mTexts) {
		text->Draw(renderer.GetTextRenderer(), drawOrder + 4);
	}
}

bool UIPanel::HandleInput(const Input& input) const {
	// TODO Priorities?
	bool handled = false;
	for (auto& panel : mPanels) {
		if (panel->IsVisible() && ! handled) {
			handled = panel->HandleInput(input);
		}
	}
	for (auto& button : mButtons) {
		if (button->IsVisible() && ! handled) {
			handled = button->HandleInput(input);
		}
	}
	return handled;
}

void UIPanel::UpdateRect(const UIRect& parentRect) {
	const UIRect rect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	mRect = rect;
	UIRect paddedRect = AddPadding(rect, mDesc.padding);

	// Update children
	for (auto& panel : mPanels) {
		if (panel->IsVisible()) {
			panel->UpdateRect(paddedRect);
		}
	}
	for (auto& bitmap : mBitmaps) {
		bitmap->ComputeRect(paddedRect);
	}
	for (auto& button : mButtons) {
		if (button->IsVisible()) {
			button->UpdateRect(paddedRect);
		}
	}
	for (auto& text : mTexts) {
		text->ComputeRect(paddedRect);
	}
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

void UICanvas::AddPanel(UIPanel& panel) {
	mPanel.AddPanel(panel);
}

void UICanvas::AddButton(UIButton& button) {
	mPanel.AddButton(button);
}

void UICanvas::AddBitmap(UIBitmap& bitmap) {
	mPanel.AddBitmap(bitmap);
}

void UICanvas::AddText(UIText& text) {
	mPanel.AddText(text);
}

void UICanvas::Draw(int canvasWidth, int canvasHeight, const UIRenderer& renderer, unsigned drawOrder) {
	const UIRect parentRect { { 0.f, 0.f }, (float)canvasWidth, (float)canvasHeight };
	mPanel.UpdateRect(parentRect);
	mPanel.Draw(renderer, drawOrder);
}

void UICanvas::HandleInput(const Input& input) const {
	mPanel.HandleInput(input);
}

void UIMouseCursor::SetCursor(const char* fileName, Graphics& graphics) {
	mMousePointer = graphics.LoadTexture(fileName);
}

void UIMouseCursor::Draw(const UIRenderer& renderer, const Vec2& mouseCoords, unsigned drawOrder) {
	if (mMousePointer) {
		const UIDrawParams prm {
			.blending = true,
			.priority = drawOrder,
		};
		renderer.DrawBitmap({ mouseCoords.x, mouseCoords.y, (float)mMousePointer->Width(), (float)mMousePointer->Height() }, *mMousePointer, prm);
	}
}

void SetTheme(const UITheme* theme) {
	uiTheme = theme ? theme : &defaultTheme;
}

} // namespace Wind
