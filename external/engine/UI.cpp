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
	return { pivot - (size.x * pivot01.x) * axis - (size.y * pivot01.y) * Vec2 { -axis.y, axis.x } + transform.offset, size, axis };
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
	if (mState != currState) {
		mAnimTime = 0.f;
	}
	return mState;
}

bool UIButton::IsClicked() const {
	return mClicked;
}

UIBitmap& UIButton::Add(const UIBitmapDesc& bitmapDesc) {
	mBitmaps.emplace_back(bitmapDesc);
	return mBitmaps.back();
}

UIText& UIButton::Add(const UITextDesc& textDesc) {
	mTexts.emplace_back(textDesc);
	return mTexts.back();
}

UIBitmap& UIButton::GetBitmap(size_t idx) {
	return mBitmaps[idx];
}

UIText& UIButton::GetText(size_t idx) {
	return mTexts[idx];
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
		if (bitmap.IsVisible()) {
			bitmap.Draw(renderer, drawOrder + 1);
		}
	}
	for (auto& text : mTexts) {
		if (text.IsVisible()) {
			text.Draw(renderer, drawOrder + 2); // text over bitmap
		}
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
	case UIButtonState::disabled:
		break;
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

UIPanel::UIPanel(const UIPanelDesc& desc)
    : UIControl { true }
    , mDesc(desc)
    , mAutoCellIdx { 0 } {
}

UIPanel::~UIPanel() {
#define Dispatch(Type) delete static_cast<Type*>(child.ptr);
	for (auto& child : mChildren) {
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
		default:
			break;
		}
	}
#undef Dispatch
}

UIButton* UIPanel::Add(UIButton&& button, int cellIdx) {
	auto newButton = new UIButton { std::move(button) };
	mChildren.push_back({ newButton, UIControlType::Button, cellIdx >= 0 ? cellIdx : mAutoCellIdx++ });
	return newButton;
}

UIText* UIPanel::Add(UIText&& text, int cellIdx) {
	auto newText = new UIText { std::move(text) };
	mChildren.push_back({ newText, UIControlType::Text, cellIdx >= 0 ? cellIdx : mAutoCellIdx++ });
	return newText;
}

UIBitmap* UIPanel::Add(const UIBitmapDesc& bitmapDesc, int cellIdx) {
	auto bitmap = new UIBitmap { bitmapDesc };
	mChildren.push_back({ bitmap, UIControlType::Bitmap, cellIdx >= 0 ? cellIdx : mAutoCellIdx++ });
	return bitmap;
}

UIText* UIPanel::Add(const UITextDesc& textDesc, int cellIdx) {
	auto text = new UIText { textDesc };
	mChildren.push_back({ text, UIControlType::Text, cellIdx >= 0 ? cellIdx : mAutoCellIdx++ });
	return text;
}

UIPanel* UIPanel::Add(const UIPanelDesc& panelDesc, int cellIdx) {
	auto panel = new UIPanel { panelDesc };
	mChildren.push_back({ panel, UIControlType::Panel, cellIdx >= 0 ? cellIdx : mAutoCellIdx++ });
	return panel;
}

UIControl& UIPanel::GetControl(int idx) const {
	return *static_cast<UIControl*>(mChildren[idx].ptr);
}

UIBitmap& UIPanel::GetBitmap(int idx) const {
	assert(mChildren[idx].type == UIControlType::Bitmap);
	return *static_cast<UIBitmap*>(mChildren[idx].ptr);
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
		default:
			break;
		}
	}
#undef Dispatch
}

void UIPanel::Draw(const UIRenderer& renderer, unsigned drawOrder) const {
	assert(IsVisible());
	if (mBackground) {
		const UIDrawBitmapArgs prms {
			.color = mDesc.backgroundColor,
			.blendMode = UIBlendMode::Auto,
			.priority = drawOrder,
			._9patch = mDesc._9patch,
		};
		renderer.DrawBitmap(GetRect(), *mBackground, prms);
	}
	else {
		if (drawOrder == 0 || mDesc.backgroundColor.a > 0.f) {
			renderer.DrawSolidRect(GetRect(), mDesc.backgroundColor, (drawOrder == 0) ? UIBlendMode::Off : UIBlendMode::On, drawOrder);
		}
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
		default:
			break;
		}
	}
#undef Dispatch
	return handled;
}

void UIPanel::Tick(float dt) {
#define Dispatch(Type) static_cast<Type*>(child.ptr)->Tick(dt);
	for (const auto& child : mChildren) {
		switch (child.type) {
		case UIControlType::Panel:
			Dispatch(UIPanel);
			break;
		case UIControlType::Button:
			Dispatch(UIButton);
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
	const UIRect paddedRect = AddPadding(rect, mDesc.padding);

	float stretchedRowHeight = 0.f;
	float rowHeight[16] {};
	float colWidth[16] {};
	int   numRows = 0;
	if (mDesc.grid.cols > 0) {
		// Compute number of rows
		for (const auto& child : mChildren) {
			int childRow = child.cellIdx / mDesc.grid.cols;
			numRows = std::max(childRow + 1, numRows);
		}

		// TODO Varying Col heights
		colWidth[0] = paddedRect.size.x / (float)mDesc.grid.cols - mDesc.grid.colSpacing * (mDesc.grid.cols - 1);

		if (mDesc.grid.rowHeight != nullptr) {
			float totalFixedRowHeight = 0.f;
			int   numStretchedRows = 0;
			for (int i = 0; i < numRows; ++i) {
				if (mDesc.grid.rowHeight[i] > 0.f) {
					totalFixedRowHeight += mDesc.grid.rowHeight[i];
				}
				else {
					++numStretchedRows; // TODO relative height e.g. 1fr, 2fr in CSS
				}
			}
			stretchedRowHeight = (paddedRect.size.y - totalFixedRowHeight) / numStretchedRows;
			for (int i = 0; i < numRows; ++i) {
				rowHeight[i] = mDesc.grid.rowHeight[i] > 0.f ? mDesc.grid.rowHeight[i] : stretchedRowHeight;
			}
		}
		else {
			// Equally spaced
			float uniformRowHeight = paddedRect.size.y / (float)numRows - mDesc.grid.rowSpacing * (numRows - 1);
			for (int i = 0; i < numRows; ++i) {
				rowHeight[i] = uniformRowHeight;
			}
		}
	}
	const Vec2 firstColPos = paddedRect.pos;

#define Dispatch(Type)                                \
	{                                                 \
		auto control = static_cast<Type*>(child.ptr); \
		if (control->IsVisible()) {                   \
			control->ComputeRect(subRect, transform); \
		}                                             \
	}

	for (const auto& child : mChildren) {
		UIRect subRect = paddedRect;
		if (mDesc.grid.cols > 0) {
			const int col = child.cellIdx % mDesc.grid.cols;
			const int row = child.cellIdx / mDesc.grid.cols;
			for (int c = 0; c < col; ++c) {
				subRect.pos.x += colWidth[0] + mDesc.grid.colSpacing; // FIXME colWidth[c]
			}
			for (int r = 0; r < row; ++r) {
				subRect.pos.y += rowHeight[r] + mDesc.grid.rowSpacing;
			}
			subRect.size.x = colWidth[0]; // FIXME
			subRect.size.y = rowHeight[row];
		}

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

UIButton* UICanvas::Add(UIButton&& button) {
	return mPanel.Add(std::move(button));
}

UIText* UICanvas::Add(const UITextDesc& textDesc) {
	return mPanel.Add(textDesc);
}

UIBitmap* UICanvas::Add(const UIBitmapDesc& bitmapDesc) {
	return mPanel.Add(bitmapDesc);
}

UIPanel* UICanvas::Add(const UIPanelDesc& panelDesc) {
	return mPanel.Add(panelDesc);
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
