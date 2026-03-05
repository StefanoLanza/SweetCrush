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

LevelStartScreen::LevelStartScreen(const MatchStats& matchStats, const GameDataModule& gameDataModule, const GameRenderer& gameRenderer)
    : mMatchStats(matchStats)
    , mGameDataModule(gameDataModule)
    , mGameRenderer(gameRenderer)
    , mCanvas(MakeCanvas()) {
	// Build UI
	mTitle = mCanvas.Add(MakeTitle(GameStringId::level));
	mPanel = mCanvas.Add(MakeInfoPanel());
	mPanel->Add(MakeScreenText(GameStringId::goal, 40.f));
	mGoalDesc = mPanel->Add(MakeDynScreenText(160.f));
	mPlayButton = mCanvas.Add(MakeMenuButton(button3_y, GameStringId::play));
}

const char* LevelStartScreen::GetName() const {
	return "LevelStartScreen";
}

void LevelStartScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent LevelStartScreen::Tick(float dt, const Input& input) {
	mCanvas.Tick(dt);
	mCanvas.HandleInput(input);
	mAccumTime += dt;
	if (mAccumTime > 6.f || mPlayButton->IsClicked()) {
		return GoTo(GameScreenIds::play, ScreenTransition::slideTop);
	}
	AnimateUI();
	return Continue();
}

void LevelStartScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);

	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
	switch (level.goal.id) {
	case GoalId::breakIce:
		DrawIceBlocks(level);
		break;
	case GoalId::collectMatches:
		DrawPieces(level);
		break;
	case GoalId::removeJellies:
		// TODO
		break;
	case GoalId::collectAllStars:
		DrawStars(level);
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
	mTitle->SetText(tmp);

	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
	switch (level.goal.id) {
	case GoalId::breakIce:
		snprintf(tmp, sizeof(tmp), "%s", "Break all ice blocks");
		break;
	case GoalId::collectMatches:
		snprintf(tmp, sizeof(tmp), "%s", "Match and remove these pieces");
		break;
	case GoalId::removeJellies:
		snprintf(tmp, sizeof(tmp), "%s", "Remove all jellies");
		break;
	case GoalId::collectAllStars:
		snprintf(tmp, sizeof(tmp), "%s", "Collect all stars");
		break;
	default:
		assert(false);
		break;
	}
	mGoalDesc->SetText(tmp);
}

void LevelStartScreen::Exit() {
}

void LevelStartScreen::ParseConfig(const char* varName, const char* varValue) {
}

void LevelStartScreen::DrawPieces(const Level& level) const {
	const int      count = MaxMatchesPerLevel;
	const uint32_t icons[MaxMatchesPerLevel] {
		pieceIcons[level.pieceIds[0]],
		pieceIcons[level.pieceIds[1]],
		pieceIcons[level.pieceIds[2]],
	};
	DrawIcons(icons, count);
}

void LevelStartScreen::DrawIceBlocks(const Level& level) const {
	const int      count = 3;
	const uint32_t icons[count] {
		iceSprites[0],
		iceSprites[1],
		iceSprites[2],
	};
	DrawIcons(icons, count);
}

void LevelStartScreen::DrawStars(const Level& level) const {
	const int count = 3;
	uint32_t  icons[count] {
        starSprite,
        starSprite,
        starSprite,
	};
	DrawIcons(icons, count);
}

void LevelStartScreen::DrawIcons(const uint32_t icons[], int count) const {
	const float y = mPanel->GetRect().pos.y + 280.f;
	const float phase = mAccumTime * 4.f;
	const float dx = gameTextures[icons[0]]->Width() + 12 * 2.f + 2;
	float       x = (RefWindowWidth - (count - 1) * dx) * 0.5f;
	for (int i = 0; i < count; ++i) {
		float rotation = std::sin(phase * .25f + (float)i) * 0.5f;
		mGameRenderer.DrawIcon(icons[i], Vec2 { x, y + std::cos(phase) * 4.f }, rotation, whiteColor, GameDrawOrder::overlays);
		x += dx;
	}
}

void LevelStartScreen::AnimateUI() {
}