#include "LevelStartScreen.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameDataModule.h"
#include "GameDrawOrder.h"
#include "GameRenderer.h"
#include "GameUI.h"
#include "Level.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"

#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/Texture.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

using namespace Wind;

namespace {

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

const UIPanelDesc panelDesc {
	.pos = UIAbsolutePos(0.f, 0.f),
	.size = UIAbsoluteSize(560.f, 400.f),
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.background = "UI/button.png",
	.backgroundColor = panel0_color,
	._9patch = 16.f,
};

} // namespace

LevelStartScreen::LevelStartScreen(const MatchStats& matchStats, const GameDataModule& gameDataModule, const GameRenderer& gameRenderer)
    : mMatchStats(matchStats)
    , mGameDataModule(gameDataModule)
    , mGameRenderer(gameRenderer)
    , mTitle { MakeTitleText(GameStringId::level) }
    , mPlayButton { MakeMenuButton(button3_y, GameStringId::play) }
    , mCanvas(canvasDesc)
	, mPanel(panelDesc)
    , mGoalText { MakeScreenText(GameStringId::goal, 40.f) }
    , mGoalDesc { MakeDynScreenText(100.f) } {
	// Setup UI
	mCanvas.Add(mTitle);
	mCanvas.Add(mPlayButton);
	mCanvas.Add(mPanel);
	mPanel.Add(mGoalText);
	mPanel.Add(mGoalDesc);
}

const char* LevelStartScreen::GetName() const {
	return "LevelStartScreen";
}

void LevelStartScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent LevelStartScreen::Tick(float dt, const Input& input) {
	mCanvas.HandleInput(input);
	mAccumTime += dt;
	if (mAccumTime > 6.f || mPlayButton.IsClicked()) {
		return GoTo(GameScreenIds::play, ScreenTransition::slideTop);
	}
	return Continue();
}

void LevelStartScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);

	float y = mPanel.Rect().pos.y + 220.f;
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
	switch (level.goal.id) {
	case GoalId::breakIce:
		DrawIceBlocks(level, y);
		break;
	case GoalId::collectMatches: {
		DrawPieces(level, y);
		break;
	}
	case GoalId::removeJellies:
		// TODO
		break;
	case GoalId::collectAllStars:
		// TODO
		break;
	default:
		assert(false);
		break;
	}
}

void LevelStartScreen::Enter(const ScreenNavArgs& args) {
	char tmp[256];
	mAccumTime = 0.f;

	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::level), mMatchStats.levelIndex + 1);
	mTitle.SetText(tmp);

	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
	switch (level.goal.id) {
	case GoalId::breakIce:
		snprintf(tmp, sizeof(tmp), "%s", "Break all ice blocks");
		break;
	case GoalId::collectMatches: {
		snprintf(tmp, sizeof(tmp), "%s", "Match and remove these pieces");
		break;
	}
	case GoalId::removeJellies:
		snprintf(tmp, sizeof(tmp), "%s", "Remove all jellies");
		// TODO
		break;
	case GoalId::collectAllStars:
		snprintf(tmp, sizeof(tmp), "%s", "Collect all stars");
		// TODO
		break;
	default:
		assert(false);
		break;
	}
	mGoalDesc.SetText(tmp);
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
