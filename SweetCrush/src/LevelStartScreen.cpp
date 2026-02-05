#include "LevelStartScreen.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameDataModule.h"
#include "GameDrawOrder.h"
#include "GameRenderer.h"
#include "Level.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "GameUI.h"

#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/FontManager.h>
#include <engine/TextRender.h>
#include <engine/Texture.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

using namespace Wind;

namespace {

const UIButtonDesc playButtonDesc {
	UIAbsolutePos(0, button2_y),
	UIAutoSize,
	UIHorizAlignment::center,
	UIVertAlignment::top,
};

const UITextDesc textDescs[] {
	{
	    .pos = { 0.f, titleY },
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,

	    .font = "screenTitle",
	    .stringId = GameStringId::level,
	},
	{
	    .pos = { 0.f, 0.f },
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::play,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

LevelStartScreen::LevelStartScreen(const MatchStats& matchStats, const GameDataModule& gameDataModule, const GameRenderer& gameRenderer)
    : mMatchStats(matchStats)
    , mGameDataModule(gameDataModule)
    , mGameRenderer(gameRenderer)
    , mTitle(textDescs[0], titleTextStyle)
    , mPlayButton(playButtonDesc, buttonBitmapDesc, textDescs[1])
    , mCanvas(canvasDesc) {
	// Setup UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mPlayButton);
}

const char* LevelStartScreen::GetName() const {
	return "LevelStartScreen";
}

void LevelStartScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
	mFont = engine.GetFontManager().AddFont("smallFont");
}

ScreenEvent LevelStartScreen::Tick(float dt, const Input& input) {
	mAccumTime += dt;
	if (mAccumTime > 6.f || mPlayButton.IsClicked(input)) {
		return GoTo(GameScreenIds::play, ScreenTransition::slideTop);
	}
	return Continue();
}

void LevelStartScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);

	if (! mFont) {
		return;
	}
	const auto&     textRenderer = uiRenderer.GetTextRenderer();
	char            tmp[256];
	const TextStyle textStyle {
		.color = whiteColor,
		.outlineColor = blackColor,
	};
	snprintf(tmp, sizeof(tmp), "%s", GetLocalizedString(GameStringId::goal));
	// textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 400 }, TextAlignment::center, textStyle, GameDrawOrder::overUI);
	//  snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourFinalScoreIs), mMatchStats.score);
	//  textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 460 }, TextAlignment::center, textStyle, GameDrawOrder::overUI);

	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
	switch (level.goal.id) {
	case GoalId::breakIce:
		snprintf(tmp, sizeof(tmp), "%s", "Break all ice blocks");
		textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, text0_y }, TextAlignment::center, TextDirection::leftToRight, textStyle,
		                          GameDrawOrder::overUI);
		DrawIceBlocks(level, text0_y + 120.f);
		break;
	case GoalId::collectMatches: {
		snprintf(tmp, sizeof(tmp), "%s", "Match and remove these pieces");
		textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, text0_y }, TextAlignment::center, TextDirection::leftToRight, textStyle,
		                          GameDrawOrder::overUI);
		DrawPieces(level, text0_y + 120.f);
		break;
	}
	case GoalId::removeJellies:
		// TODO
		break;
	case GoalId::collectAllStars:
		// TODO
		break;
	default:
		break;
	}
}

void LevelStartScreen::Enter(const ScreenNavArgs& args) {
	mAccumTime = 0.f;
}

void LevelStartScreen::Exit() {
}

void LevelStartScreen::ParseConfig(const char* varName, const char* varValue) {
}

void LevelStartScreen::DrawPieces(const Level& level, float yCoord) const {
	constexpr float dx = TileWidth * 2.f + 2;
	float           phase = mAccumTime * 4.f;
	float           x = (RefWindowWidth - (MaxMatchesPerLevel - 1) * dx) * 0.5f;
	for (int i = 0; i < MaxMatchesPerLevel; ++i) {
		float rotation = std::sin(phase * .25f + (float)i) * 0.5f;
		mGameRenderer.DrawIcon(pieceIcons[level.pieceIds[i]], Vec2 { x, yCoord + std::cos(phase) * 4.f }, rotation, whiteColor,
		                       GameDrawOrder::overlays);
		x += dx;
		phase += 6.28f / static_cast<float>(MaxMatchesPerLevel);
	}
}

void LevelStartScreen::DrawIceBlocks(const Level& level, float yCoord) const {
	// FIXME Count
	const int   count = 4;
	const float dx = gameTextures[iceSprites[0]]->Width() + 12 * 2.f + 2;
	float       x = (RefWindowWidth - (count - 1) * dx) * 0.5f;
	for (int i = 0; i < count; ++i) {
		mGameRenderer.DrawIcon(iceSprites[0], Vec2 { x, yCoord }, 0.f, whiteColor, GameDrawOrder::overlays);
		x += dx;
	}
}
