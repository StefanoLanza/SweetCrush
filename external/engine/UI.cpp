#include "UI.h"
#include "Engine.h"
#include "Font.h"
#include "Graphics.h"
#include "Input.h"
#include "StringTable.h"
#include "FontManager.h"
#include "Texture.h"
#include "UIRenderer.h"
#include <cassert>

namespace Wind {

namespace {

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
	Vec2 center = rect.pos + rect.size * 0.5f;
	Vec2 newExtents = rect.size * 0.5f * scale;
	return { center - newExtents, newExtents * 2.f };
}

} // namespace

UIButton::UIButton(const UIButtonDesc& desc, std::unique_ptr<UIBitmap> bitmap, std::unique_ptr<UIText> text)
    : mDesc(desc)
    , mBitmap(std::move(bitmap))
    , mText(std::move(text))
    , mRect {}
    , mState { UIButtonState::idle }
    , mVisible { true } {
}

UIButton::UIButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc, const UITextDesc& labelDesc)
    : UIButton(desc, std::make_unique<UIBitmap>(bitmapDesc), std::make_unique<UIText>(labelDesc)) {
}

UIButton::UIButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc)
    : UIButton(desc, std::make_unique<UIBitmap>(bitmapDesc), nullptr) {
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
			mState = UIButtonState::idle;
		}
		else if (! mouseDown) {
			// TODO Action callback ?
			mState = UIButtonState::hovered;
		}
		break;
	}
	return mState;
}

bool UIButton::IsClicked(const Input& input) {
	UIButtonState state = mState;
	UIButtonState newState = RefreshState(input);
	return (state == UIButtonState::pressed) && (newState == UIButtonState::hovered);
}

void UIButton::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	if (mBitmap) {
		mBitmap->LoadGraphics(graphics);
	}
	if (mText) {
		mText->Load(fontManager);
	}
}

void UIButton::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	if (! mVisible) {
		return;
	}
	if (mBitmap) {
		mBitmap->Draw(renderer, drawOrder);
	}
	if (mText) {
		mText->Draw(renderer.GetTextRenderer(), drawOrder + 1); // text mouseOver bitmap
	}
}

void UIButton::UpdateRect(const UIRect& parentRect) {
	UISize size = mDesc.size;
	if (mBitmap && mBitmap->GetBitmap()) {
		if (size.rWidth < 0.f && size.aWidth < 0.0f) {
			size.aWidth = static_cast<float>(mBitmap->GetBitmap()->Width());
			size.rWidth = 0.f;
		}
		if (size.rHeight < 0.f && size.aHeight < 0.0f) {
			size.aHeight = static_cast<float>(mBitmap->GetBitmap()->Height());
			size.rHeight = 0.f;
		}
	}
	else {
		mRect = UIZeroRect;
	}

	mRect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	UIRect paddedRect = AddPadding(mRect, mDesc.padding);
	if (mState == UIButtonState::pressed) {
		paddedRect = ScaleRect(paddedRect, 1.025f);
	}
	if (mBitmap) {
		mBitmap->UpdateRect(paddedRect);
	}
	if (mText) {
		mText->UpdateRect(paddedRect);
	}
}

UIBitmap* UIButton::GetBitmap() const {
	return mBitmap.get();
}

UIText* UIButton::GetText() const {
	return mText.get();
}

const UIRect& UIButton::GetRect() const {
	return mRect;
}

UIButtonState UIButton::GetState() const {
	return mState;
}

UIText::UIText(const UITextDesc& desc)
    : mDesc(desc)
    , mAlignedRect {} {
}

void UIText::Load(FontManager& fontManager) {
	mFont = fontManager.AddFont(mDesc.font);
}

void UIText::Draw(const TextRenderer& textRenderer, unsigned drawOrder) const {
	if (mFont) {
		const char* str = Text();
		if (str) {
			textRenderer.Write(*mFont, str, mAlignedRect.pos, mDesc.textStyle, drawOrder);
		}
	}
}

void UIText::UpdateRect(const UIRect& parentRect) {
	if (! mFont) {
		mAlignedRect = UIZeroRect;
		return;
	}
	const char* str = Text();

	const UISize size {
		.aWidth = static_cast<float>(mFont->CalculateStringWidth(str)),
		.aHeight = static_cast<float>(mFont->GetHeight()),
		.rWidth = 0.f,
		.rHeight = 0.f,
	};
	mAlignedRect = AlignRect(UIAbsolutePos(mDesc.pos.x, mDesc.pos.y), size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
}

void UIText::SetText(StringId stringId) {
	mDesc.stringId = stringId;
}

const char* UIText::Text() const {
	// TODO Custom
	return mDesc.text ? mDesc.text : GetString(mDesc.stringId);
}

UIBitmap::UIBitmap(const UIBitmapDesc& desc)
    : mDesc(desc)
    , mAlignedRect {} {
}

void UIBitmap::LoadGraphics(Graphics& graphics) {
	mBitmap = graphics.LoadTexture(mDesc.fileName);
}

void UIBitmap::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	if (mBitmap) {
		const UIDrawParams prm {
			.color = mDesc.color,
			.blending = mDesc.blending == UIBlending::on,
			.priority = drawOrder,
			._9patch = mDesc._9patch,
		};
		renderer.DrawRect(mAlignedRect, *mBitmap, prm);
	}
}

void UIBitmap::UpdateRect(const UIRect& parentRect) {
	if (mBitmap) {
		UISize size = mDesc.size;
		if (size.rWidth <= -1.f) {
			size.aWidth = static_cast<float>(mBitmap->Width());
			size.rWidth = 0.f;
		}
		if (size.rHeight <= -1.f) {
			size.aHeight = static_cast<float>(mBitmap->Height());
			size.rHeight = 0.f;
		}
		mAlignedRect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	}
	else {
		mAlignedRect = UIZeroRect;
	}
}

void UIBitmap::SetBitmap(const TexturePtr& bitmap) {
	mBitmap = bitmap;
}

const Texture* UIBitmap::GetBitmap() const {
	return mBitmap.get();
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
		renderer.DrawRect(mRect, *mBackground, prms);
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
		bitmap->UpdateRect(paddedRect);
	}
	for (auto& button : mButtons) {
		if (button->IsVisible()) {
			button->UpdateRect(paddedRect);
		}
	}
	for (auto& text : mTexts) {
		text->UpdateRect(paddedRect);
	}
}

UIGrid::UIGrid(const UIGridDesc& desc)
    : mDesc { desc }
    , mVisible { true } {
}

void UIGrid::SetVisible(bool visible) {
	mVisible = visible;
}
bool UIGrid::IsVisible() const {
	return mVisible;
}

void UIGrid::LoadAssets(Graphics& graphics, FontManager& fontManager) {
	UIContainer::LoadAssets(graphics, fontManager);
}

void UIGrid::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	if (! mVisible) {
		return;
	}
	if (mBackground) {
		UIDrawParams prms;
		prms.blending = (mDesc.backgroundColor.a < 255.f) || mBackground->HasAlpha();
		prms.color = mDesc.backgroundColor;
		prms.priority = drawOrder;
		renderer.DrawRect(mRect, *mBackground, prms);
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

void UIGrid::UpdateRect(const UIRect& parentRect) {
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
		bitmap->UpdateRect(paddedRect);
	}
	for (auto& button : mButtons) {
		if (button->IsVisible()) {
			button->UpdateRect(paddedRect);
		}
	}
	for (auto& text : mTexts) {
		text->UpdateRect(paddedRect);
	}
}

UICanvas::UICanvas()
    : mPanel(UIDefaultPanelDesc) {
	mPanel.SetVisible(true);
}

UICanvas::UICanvas(const UICanvasDesc& desc)
    : mPanel(UIPanelDesc { .pos = UIZeroPos, .size = UIParentSize, .background = desc.background, .backgroundColor = desc.backgroundColor }) {
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

void UIMouseCursor::SetCursor(const char* fileName, Graphics& graphics) {
	mMousePointer = graphics.LoadTexture(fileName);
}

void UIMouseCursor::Draw(const UIRenderer& renderer, const Vec2& mouseCoords, unsigned drawOrder) {
	if (mMousePointer) {
		const UIDrawParams prm {
			.blending = true,
			.priority = drawOrder,
		};
		renderer.DrawRect({ mouseCoords.x, mouseCoords.y, (float)mMousePointer->Width(), (float)mMousePointer->Height() }, *mMousePointer, prm);
	}
}

} // namespace Wind
