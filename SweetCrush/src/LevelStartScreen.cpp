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

//	"gameartguppy/pet_turtle_160x160.png",
//"gameartguppy/pet_fish_160x160.png",

const UIBitmapDesc petDesc {
	.fileName = "gameartguppy/pet_bird_160x160.png",
	.pos = UIZeroPos,
	.size = UIZeroSize,
	.pivot = { 0.45f, 1.0f },
	.horizontalAlignment = UIHorizAlignment::right,
	.verticalAlignment = UIVertAlignment::bottom,
	.sizing = UIBitmapSizing::fit,
};
#if 0
	{
		BitmapExtParams prm;
		prm.orientation = std::sin(0.f + (float)mTime * 2.f) * 0.05f;
		prm.drawOrder = GameDrawOrder::overUI;
		prm.pivot = { 0.4f, 1.f };
		prm.blending = true;
		mEngine.GetBitmapRenderer().DrawBitmapEx(*gameTextures[petSprites[0]], { RefWindowWidth - 100.f, RefWindowHeight - 0.f }, prm);
	}
#endif

LevelStartScreen::LevelStartScreen(const MatchStats& matchStats, const GameDataModule& gameDataModule, const GameRenderer& gameRenderer)
    : mMatchStats(matchStats)
    , mGameDataModule(gameDataModule)
    , mGameRenderer(gameRenderer)
    , mCanvas(MakeCanvas())
    , mGoalText { MakeScreenText(GameStringId::goal, 40.f) }
    , mGoalDesc { MakeDynScreenText(160.f) } {
	// Build UI
	mTitle = mCanvas.Add(GetTitleTextDesc(GameStringId::level));
	mPanel = mCanvas.Add(GetInfoPanelDesc());
	mPanel->Add(mGoalText);
	mPanel->Add(mGoalDesc);
	mPet = mPanel->Add(petDesc);
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

	float        y = mPanel->GetRect().pos.y + 280.f;
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
	mTitle->SetText(tmp);

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

void LevelStartScreen::AnimateUI() {
	mPet->GetTransform().rotation = 0.2f * (0.5f + 0.5f * std::sin(mAccumTime));
}