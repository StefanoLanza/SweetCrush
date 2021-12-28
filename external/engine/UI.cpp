#include "UI.h"
#include <cassert>
#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/Font.h>
#include <engine/Graphics.h>
#include <engine/Input.h>
#include <engine/SdlSurface.h>
#include <engine/StringTable.h>
#include <engine/TextRender.h>

namespace Wind {

namespace {

UIRect AlignRect(const UIPos& pos, const UISize& size, const UIRect& parentRect, UIHorizAlignment horizAlignment, UIVertAlignment vertAlignment) {
	UIRect alignedRect;
	alignedRect.width = size.aWidth + parentRect.width * size.rWidth;
	alignedRect.height = size.aHeight + parentRect.height * size.rHeight;

	alignedRect.pos = parentRect.pos + Vec2 { pos.ax + parentRect.width * pos.rx, pos.ay + parentRect.height * pos.ry };
	if (horizAlignment == UIHorizAlignment::center) {
		alignedRect.pos.x += (parentRect.width - alignedRect.width) * 0.5f;
	}
	else if (horizAlignment == UIHorizAlignment::right) {
		alignedRect.pos.x += (parentRect.width - alignedRect.width);
	}
	if (vertAlignment == UIVertAlignment::center) {
		alignedRect.pos.y += (parentRect.height - alignedRect.height) * 0.5f;
	}
	else if (vertAlignment == UIVertAlignment::bottom) {
		alignedRect.pos.y += (parentRect.height - alignedRect.height);
	}
	return alignedRect;
}

} // namespace

UIButton::UIButton(const UIButtonDesc& desc, std::unique_ptr<UIBitmap> bitmap, std::unique_ptr<UIText> text)
    : mDesc(desc)
    , mBitmap(std::move(bitmap))
    , mText(std::move(text))
    , mAlignedRect {} {
}

bool UIButton::IsPressed(const Input& input) {
	bool res = false;
	if (input.GetMouseButtonPressed() || input.GetFingerPressed()) {
		Rect r;
		r.left = mAlignedRect.pos.x;
		r.right = r.left + mAlignedRect.width;
		r.top = mAlignedRect.pos.y;
		r.bottom = r.top + mAlignedRect.height;
		res = RectContainsPoint(r, input.GetMappedMouseCoord());
	}
	return res;
}

void UIButton::Draw(const BitmapRenderer& renderer, const TextRenderer& textRender, DrawOrderType drawOrder) const {
	if (mBitmap) {
		mBitmap->Draw(renderer, drawOrder);
	}
	if (mText) {
		mText->Draw(textRender, drawOrder + 1); // text over bitmap
	}
}

void UIButton::UpdateRect(const UIRect& parentRect) {
	UISize size = mDesc.size;
	if (mBitmap) {
		if (size.rWidth <= 0.f) {
			size.aWidth = static_cast<float>(mBitmap->GetBitmap().Width());
			size.rWidth = 0.f;
		}
		if (size.rHeight <= 0.f) {
			size.aHeight = static_cast<float>(mBitmap->GetBitmap().Height());
			size.rHeight = 0.f;
		}
	}

	mAlignedRect = AlignRect(mDesc.pos, size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
	if (mBitmap) {
		mBitmap->UpdateRect(mAlignedRect);
	}
	if (mText) {
		mText->UpdateRect(mAlignedRect);
	}
}

UIBitmap* UIButton::GetBitmap() const {
	return mBitmap.get();
}

UIText* UIButton::GetText() const {
	return mText.get();
}

UIText::UIText(const UITextDesc& desc, Engine& engine)
    : mDesc(desc)
    , mFont(engine.GetTextRenderer().AddFont(desc.font))
    , mAlignedRect {} {
}

void UIText::Draw(const TextRenderer& textRender, DrawOrderType drawOrder) const {
	if (mFont) {
		const char* str = GetString(mDesc.stringId);
		if (str) {
			textRender.Write(*mFont, str, mAlignedRect.pos, mDesc.textStyle, drawOrder);
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

UIBitmap::UIBitmap(const UIBitmapDesc& desc, Engine& engine)
    : mDesc(desc)
    , mBitmap(engine.LoadBitmap(desc.fileName))
    , mAlignedRect {} {
	UISize size = mDesc.size;
	if (size.rWidth <= -1.f) {
		size.aWidth = static_cast<float>(mBitmap->Width());
		size.rWidth = 0.f;
	}
	if (size.rHeight <= -1.f) {
		size.aHeight = static_cast<float>(mBitmap->Height());
		size.rHeight = 0.f;
	}
	mDesc.size = size;
}

void UIBitmap::Draw(const BitmapRenderer& renderer, DrawOrderType drawOrder) const {
	BitmapExtParams prm;
	prm.width = mAlignedRect.width;
	prm.height = mAlignedRect.height;
	prm.color = mDesc.color;
	prm.blending = mDesc.blending == UIBlending::on ? true : false;
	prm.drawOrder = drawOrder + mDesc.relDrawOrder;
	renderer.DrawBitmapEx(*mBitmap, mAlignedRect.pos, prm);
}

void UIBitmap::UpdateRect(const UIRect& parentRect) {
	mAlignedRect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);
}

void UIBitmap::SetBitmap(const BitmapPtr& bitmap) {
	mBitmap = bitmap;
}

const SdlSurface& UIBitmap::GetBitmap() const {
	return *mBitmap;
}

UIPanel::UIPanel(const UIPanelDesc& desc)
    : mDesc(desc)
    , mVisible(false) {
}

void UIPanel::SetVisible(bool visible) {
	mVisible = visible;
}

bool UIPanel::IsVisible() const {
	return mVisible;
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

void UIPanel::LoadAssets(Engine& engine) {
	for (const auto& panel : mPanels) {
		panel->LoadAssets(engine);
	}
}

void UIPanel::AddText(UIText& text) {
	mTexts.push_back(&text);
}

void UIPanel::Draw(const BitmapRenderer& bitmapRenderer, const TextRenderer& textRender, DrawOrderType drawOrder) const {
	if (! mVisible) {
		return;
	}
	for (const auto& bitmap : mBitmaps) {
		bitmap->Draw(bitmapRenderer, drawOrder + 1);
	}
	for (const auto& panel : mPanels) {
		panel->Draw(bitmapRenderer, textRender, drawOrder + 2);
	}
	for (const auto& button : mButtons) {
		button->Draw(bitmapRenderer, textRender, drawOrder + 3);
	}
	for (const auto& text : mTexts) {
		text->Draw(textRender, drawOrder + 4);
	}
}

void UIPanel::UpdateRect(const UIRect& parentRect) {
	const UIRect rect = AlignRect(mDesc.pos, mDesc.size, parentRect, mDesc.horizontalAlignment, mDesc.verticalAlignment);

	for (auto& panel : mPanels) {
		panel->UpdateRect(rect);
	}
	for (auto& bitmap : mBitmaps) {
		bitmap->UpdateRect(rect);
	}
	for (auto& button : mButtons) {
		button->UpdateRect(rect);
	}
	for (auto& text : mTexts) {
		text->UpdateRect(rect);
	}
}

UICanvas::UICanvas()
    : mCanvasWidth(0.f)
    , mCanvasHeight(0.f)
    , mPanel(UIDefaultPanelDesc) {
	mPanel.SetVisible(true);
}

void UICanvas::SetBackground(const char* fileName, Engine& engine) {
	mBackground = engine.LoadBitmap(fileName);
}

void UICanvas::SetMousePointer(const char* fileName, Engine& engine) {
	mMousePointer = engine.LoadBitmap(fileName);
}

UIPanel& UICanvas::GetPanel() {
	return mPanel;
}

void UICanvas::LoadAssets(Engine& engine) {
	mPanel.LoadAssets(engine);
}

void UICanvas::UpdateWidgets(float canvasWidth, float canvasHeight) {
	const UIRect parentRect { { 0.f, 0.f }, canvasWidth, canvasHeight };
	mPanel.UpdateRect(parentRect);
}

void UICanvas::Draw(const BitmapRenderer& bitmapRender, const TextRenderer& textRender, const Vec2& mouseCoords) {
	if (mBackground) {
		BitmapExtParams prm;
		prm.width = mCanvasWidth;
		prm.height = mCanvasHeight;
		prm.blending = false;
		prm.drawOrder = DrawOrder::background;
		bitmapRender.DrawBitmapEx(*mBackground, Vec2 { 0.f, 0.f }, prm);
	}
	mPanel.Draw(bitmapRender, textRender, DrawOrder::UI);

	if (mMousePointer) {
		BitmapExtParams prm;
		prm.blending = true;
		prm.drawOrder = DrawOrder::mousePointer;
		bitmapRender.DrawBitmapEx(*mMousePointer, mouseCoords, prm);
	}
}

UIButton MakeButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc, const UITextDesc& textDesc, Engine& engine) {
	auto bitmap = std::make_unique<UIBitmap>(bitmapDesc, engine);
	auto text = std::make_unique<UIText>(textDesc, engine);
	return UIButton(desc, std::move(bitmap), std::move(text));
}

UIButton MakeButton(const UIButtonDesc& desc, const UIBitmapDesc& bitmapDesc, Engine& engine) {
	auto bitmap = std::make_unique<UIBitmap>(bitmapDesc, engine);
	return UIButton(desc, std::move(bitmap), nullptr);
}

} // namespace Wind
