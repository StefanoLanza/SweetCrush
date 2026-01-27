#include "UI.h"
#include "Engine.h"
#include "Font.h"
#include "Graphics.h"
#include "Input.h"
#include "StringTable.h"
#include "TextRender.h"
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

} // namespace

UIButton::UIButton(const UIButtonDesc& desc, std::unique_ptr<UIBitmap> bitmap, std::unique_ptr<UIText> text)
    : mDesc(desc)
    , mBitmap(std::move(bitmap))
    , mText(std::move(text))
    , mRect {}
    , mState { UIButtonState::released } {
}

bool UIButton::IsPressed(const Input& input)  {
	Rect r {
		.left = mRect.pos.x,
		.top = mRect.pos.y,
		.right = r.left + mRect.size.x,
		.bottom = r.top + mRect.size.y,
	};
	UIButtonState state = UIButtonState::released;
	if (RectContainsPoint(r, input.GetMappedMouseCoord())) {
		state = UIButtonState::hovered;
	}
	if ((input.GetMouseButtonPressed() || input.GetFingerPressed()) && state == UIButtonState::hovered) {
		state = UIButtonState::pressed;
	}
	mState = state;
	return state == UIButtonState::pressed;
}

void UIButton::LoadAssets(Graphics& graphics, TextRenderer& textRenderer) {
	if (mBitmap) {
		mBitmap->LoadGraphics(graphics);
	}
	if (mText) {
		mText->Load(textRenderer);
	}
}

void UIButton::Draw(const UIRenderer& renderer, DrawOrderType drawOrder) const {
	if (mBitmap) {
		mBitmap->Draw(renderer, drawOrder);
	}
	if (mText) {
		mText->Draw(renderer.GetTextRenderer(), drawOrder + 1); // text over bitmap
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

	mRect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	UIRect paddedRect = AddPadding(mRect, mDesc.padding);
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

UIText::UIText(const UITextDesc& desc)
    : mDesc(desc)
    , mAlignedRect {} {
}

void UIText::Load(TextRenderer& textRenderer) {
	mFont = textRenderer.AddFont(mDesc.font);
}

void UIText::Draw(const TextRenderer& textRenderer, DrawOrderType drawOrder) const {
	if (mFont) {
		const char* str = GetString(mDesc.stringId);
		if (str) {
			textRenderer.Write(*mFont, str, mAlignedRect.pos, mDesc.textStyle, drawOrder);
		}
	}
}

void UIText::UpdateRect(const UIRect& parentRect) {
	if (! mFont) {
		return;
	}
	const char* text = GetString(mDesc.stringId);
	UISize      size = mDesc.size;
	if (size.rWidth == UIAutoSize.rWidth) {
		size.aWidth = static_cast<float>(mFont->CalculateStringWidth(text));
		size.rWidth = 0.f;
	}
	if (size.rHeight <= 0.f) {
		size.aHeight = static_cast<float>(mFont->GetHeight());
		size.rHeight = 0.f;
	}
	mAlignedRect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
}

void UIText::SetText(StringId stringId) {
	mDesc.stringId = stringId;
}

UIBitmap::UIBitmap(const UIBitmapDesc& desc)
    : mDesc(desc)
    , mAlignedRect {} {
}

void UIBitmap::LoadGraphics(Graphics& graphics) {
	mBitmap = graphics.LoadTexture(mDesc.fileName);
}

void UIBitmap::Draw(const UIRenderer& renderer, DrawOrderType drawOrder) const {
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
		mAlignedRect = {};
	}
}

void UIBitmap::SetBitmap(const TexturePtr& bitmap) {
	mBitmap = bitmap;
}

const Texture* UIBitmap::GetBitmap() const {
	return mBitmap.get();
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

void UIPanel::AddPanel(UIPanel& panel) {
	mPanels.push_back(&panel);
}

void UIPanel::AddButton(UIButton& button) {
	mButtons.push_back(&button);
}

void UIPanel::AddBitmap(UIBitmap& bitmap) {
	mBitmaps.push_back(&bitmap);
}

void UIPanel::AddText(UIText& text) {
	mTexts.push_back(&text);
}

void UIPanel::LoadAssets(Graphics& graphics, TextRenderer& textRenderer) {
	if (mDesc.background) {
		mBackground = graphics.LoadTexture(mDesc.background);
	}
	for (auto& panel : mPanels) {
		panel->LoadAssets(graphics, textRenderer);
	}
	for (auto& bitmap : mBitmaps) {
		bitmap->LoadGraphics(graphics);
	}
	for (auto& button : mButtons) {
		button->LoadAssets(graphics, textRenderer);
	}
	for (auto& text : mTexts) {
		text->Load(textRenderer);
	}
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
		button->UpdateRect(paddedRect);
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

void UICanvas::LoadAssets(Graphics& graphics, TextRenderer& textRenderer) {
	mPanel.LoadAssets(graphics, textRenderer);
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

UIButton MakeButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc, const UITextDesc& textDesc) {
	return UIButton { desc, std::make_unique<UIBitmap>(bitmapDesc), std::make_unique<UIText>(textDesc) };
}

UIButton MakeButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc) {
	return UIButton { desc, std::make_unique<UIBitmap>(bitmapDesc), nullptr };
}

} // namespace Wind
