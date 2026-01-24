#include "PlayScreen.h"
#include "Actions.h"
#include "AppConfig.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameDataModule.h"
#include "GameDrawOrder.h"
#include "GameRenderer.h"
#include "GameSettings.h"
#include "Level.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "TileSelector.h"
#include "UIDefs.h"

#include <engine/Audio.h>
#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/Font.h>
#include <engine/Graphics.h>
#include <engine/Input.h>
#include <engine/SdlMusic.h>
#include <engine/SdlSound.h>
#include <engine/TextRender.h>

#include <cassert>
#include <cmath>

using namespace Wind;

namespace {

const UIButtonDesc pauseButtonDesc {
	.pos = UIAbsolutePos(-60, -60),
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::right,
	.verticalAlignment = UIVertAlignment::bottom,
};

const UIButtonDesc booster0ButtonDesc {
	.pos = UIAbsolutePos(10, -10),
	.size = { 100.f, 100.f, 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::left,
	.verticalAlignment = UIVertAlignment::bottom,
};

const UIButtonDesc booster1ButtonDesc {
	.pos = UIAbsolutePos(0, -10),
	.size = { 100.f, 100.f, 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::bottom,
};

const UIButtonDesc booster2ButtonDesc {
	.pos = UIAbsolutePos(-10, -10),
	.size = { 100.f, 100.f, 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::right,
	.verticalAlignment = UIVertAlignment::bottom,
};

const UIBitmapDesc boosterButtonBitmapDesc {
	.fileName = "button.png",
	.pos = UIZeroPos,
	.size = UIParentSize,
	.color = Color { 255, 229, 102, 255 },
	._9patch = { 16, 0.f, 0.f, 0.f },
};

const UIBitmapDesc optionButtonBitmapDesc {
	"menuButton.png",
	UIZeroPos,
	UIAutoSize,
};

constexpr UIPanelDesc boosterPanelDesc {
	.pos = { 0.f, -20.f, 0.f, 0.f },
	.size = { 340.f, 100.f, 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::bottom,
};

constexpr float criticalTime = 10.f;

CellVisual& GetVisual(const Cell& cell) {
	return *static_cast<CellVisual*>(cell.ud);
}

} // namespace

PlayScreen::PlayScreen(Engine& engine, const GameRenderer& gameRenderer, const AppConfig& gameConfig, const GameSettings& gameSettings,
                       ActionMgr& renderActionMgr, MatchStats& matchStats, const GameDataModule& gameDataModule)
    : mEngine(engine)
    , mGameRenderer(gameRenderer)
    , mGameConfig(gameConfig)
    , mGameSettings(gameSettings)
    , mRenderActionMgr(renderActionMgr)
    , mMatchStats(matchStats)
    , mGameDataModule(gameDataModule)
    , mBoard { NumCols, NumRows, mGameConfig.board }
    , mCellSelector { std::make_unique<TileSelector>(mBoard, gameConfig) }
    , mPanel(UIDefaultPanelDesc)
    , mBoostersPanel(boosterPanelDesc)
    , mPauseButton(MakeButton(pauseButtonDesc, optionButtonBitmapDesc, engine))
    , mBoosterButtons { MakeButton(booster0ButtonDesc, boosterButtonBitmapDesc, engine),
	                    MakeButton(booster1ButtonDesc, boosterButtonBitmapDesc, engine),
	                    MakeButton(booster2ButtonDesc, boosterButtonBitmapDesc, engine) }
    , mMatch3 { mBoard, mBoardGenerator, *mCellSelector }
    , mTime { 0.f }
    , mMatchTime { 0 } {
	mCellSelector->SetCallback([this](const TileSelectionEvent& event) { OnTileSelectionEvent(event); });
	mMatch3.SetCallback([this](const Match3Event& event) { OnMatch3Event(event); });
	mCellGraphics.resize(NumCols * NumRows);
}

PlayScreen::~PlayScreen() = default;

const char* PlayScreen::GetName() const {
	return "PlayScreen";
}

void PlayScreen::LoadAssets() {
	Audio& audio = mEngine.GetAudio();
	mMusic = audio.LoadMusic("audio/music.ogg");
	mSounds[0] = audio.LoadSound("audio/match.wav");
}

void PlayScreen::BuildUI(UICanvas& canvas) {
	mFonts[0] = mEngine.GetTextRenderer().AddFont("mediumFont");
	mFonts[1] = mEngine.GetTextRenderer().AddFont("tiny");
	mFonts[2] = mEngine.GetTextRenderer().AddFont("smallFont");
	mPanel.AddPanel(mBoostersPanel);
	mBoostersPanel.SetVisible(true);
	mBoostersPanel.AddButton(mBoosterButtons[0]);
	mBoostersPanel.AddButton(mBoosterButtons[1]);
	mBoostersPanel.AddButton(mBoosterButtons[2]);
	// TODO ? mPanel.AddButton(mPauseButton);
	canvas.GetPanel().AddPanel(mPanel);
}

ScreenTransition PlayScreen::Tick(float dt, const Input& input) {
	{
#if defined(__ANDROID__) || defined(__OHOS__)
		if (input.GetKeyJustPressed(SDLK_AC_BACK)) {
#elif defined(_WIN32) || defined(__linux__)
		if (input.GetKeyJustPressed(SDLK_ESCAPE)) {
#endif
			return { ScreenOp::push, GameScreenIds::pauseGame };
		}
	}

	if (mPauseButton.IsPressed(input)) {
		return { ScreenOp::push, GameScreenIds::pauseGame };
	}

	if (mMatch3.IsWaitingForUser()) {
		SelectBooster(input);
	}

	if (mGameComplete) {
		if (mRenderActionMgr.AnyRunning()) {
			return { ScreenOp::keep }; // wait until all animations are over
		}
		return { ScreenOp::replace, GameScreenIds::gameComplete };
	}
	else if (mLevelComplete) {
		if (mRenderActionMgr.AnyRunning()) {
			return { ScreenOp::keep }; // wait until all animations are over
		}
		return { ScreenOp::replace, GameScreenIds::levelComplete };
	}

	if (mMatch3.IsWaitingForUser()) {
		// Decrease time only when waiting for user selection
		mMatchTime = std::max(0.f, mMatchTime - dt);
	}

	if (mMatchTime < criticalTime) {
		if (mMusic) {
			mMusic->SetVolume(0.2f);
		}
		// TODO play clock sound
	}

	mTime += dt;
	if (mMatchTime > 0.f) {
		if (! mActionMgr.AnyRunning()) { // do not update match while animations are still running
			mMatch3.Update(input);
		}
	}
	else {
		return { ScreenOp::replace, GameScreenIds::gameOver };
	}
	mActionMgr.RunActions(dt);

	return { ScreenOp::keep };
}

void PlayScreen::SelectBooster(const Input& input) {
	bool handled = false;

	// Check buttons
	for (int i = 0; i < MaxBoosterTypesPerLevel; ++i) {
		if (mBoosterButtons[i].IsPressed(input)) {
			// Unselect if pressing again on same button
			mSelectedBooster = mSelectedBooster == i ? -1 : i;
			handled = true;
			break;
		}
	}

	if (! handled && mSelectedBooster >= 0) {
		// Check click on board
		int cellIdx = mBoard.GetCellAtCoords(input.GetMouseCoord());
		if (input.GetMouseButtonPressed()) {
			if (cellIdx >= 0) {
				mMatch3.UseBooster(cellIdx);
			}
			mSelectedBooster = -1; // release
		}
		else {
			if (cellIdx >= 0) {
				// TODO GetVisual(mBoard.GetCell(cellIdx)).highlighted = true;
			}
			// TODO highlight cell
		}
	}

	if (mSelectedBooster >= 0) {
		mSelectedBoosterCoord = input.GetMouseCoord();
	}
}

void PlayScreen::Draw(ScreenId topScreen) const {
	mGameRenderer.DrawBoard(mBoard, mCellSelector->GetSelectedTile(), mGameConfig, mTime);
	DrawUI();

	float t01 = (0.5f + 0.5f * sinf(mMatchTime * 5.0f));
	Color c = whiteColor;
	c.a = 255.f * t01;
	//mGameRenderer.DrawBlast({ 300.f, 300.f }, 256.f * t01, 64, c);
	//mGameRenderer.DrawLaser({ 0.f, 300.f }, { RefWindowWidth, 300.f }, 64);
	// mGameRenderer.DrawLaser({ 100.f, 0.f }, { 100.f, mGameConfig.board.bottomRightCoord.y }, 64, 0.5f + 0.5f * sinf(mMatchTime * 5.0f));
}

void PlayScreen::Enter(ScreenId prevScreen, const void* payload) {
	bool restartLevel = false;
	if (prevScreen == GameScreenIds::levelComplete) {
		NextLevel();
	}
	else if (prevScreen == GameScreenIds::pauseGame) {
		// resume game
		// TODO Receive restartLevel as generic arg of Enter
		if (restartLevel) {
			ReplayLevel();
		}
	}
	else if (prevScreen == GameScreenIds::gameOver) {
		ReplayLevel();
	}
	else if (prevScreen == GameScreenIds::effectInfo) {
		// continue playing
	}
	else {
		NewGame();
	}
	if (prevScreen == GameScreenIds::pauseGame && ! restartLevel) {
		ResumeMusic();
	}
	else {
		PlayMusic();
	}
	mPanel.SetVisible(true);
	mTime = 0.f;
}

void PlayScreen::Exit() {
	mRenderActionMgr.Clear(); // stop showing score and other effects
	mPanel.SetVisible(false);
	PauseMusic();
}

void PlayScreen::NewGame() {
	mMatchStats.level = 0;
	mMatchStats.score = 0;
	mLevelComplete = false;
	mGameComplete = false;
	StartLevel();
}

void PlayScreen::NextLevel() {
	if (mMatchStats.level < mGameDataModule.GetNumLevels()) {
		++mMatchStats.level;
		StartLevel();
	}
}

void PlayScreen::ReplayLevel() {
	mMatchStats.score = 0;
	mGameComplete = false;
	StartLevel();
}

void PlayScreen::StartLevel() {
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.level);
	if (level.boardDef) {
		mBoardGenerator.InitBoard(mBoard, *level.boardDef, level.seed, level.pieceIds, 5);
	}
	else {
		mBoardGenerator.GenRandomBoard(mBoard, level.seed, level.boardMask, level.pieceIds, 5);
	}
	for (int i = 0; i < mBoard.GetCellCount(); ++i) {
		mBoard.GetCell(i).ud = &mCellGraphics[i];
	}
	mMatchTime = level.availableTime;
	for (int& c : mMatchStats.targetPieceCount) {
		c = 0;
	}
	mMatchStats.layerCount = mBoard.TotalLayerCount();
	mLevelComplete = false;
	mActionMgr.Clear();
	SetupNewBoardAnimation();
	mMatch3.Run();
}

void PlayScreen::OnTileSelectionEvent(const TileSelectionEvent& event) {
	// Cell& cell = mBoard.GetCell(event.cellIdx);
	if (event.id == TileSelectionEvent::Id::drag) {
		static_cast<CellVisual*>(event.cell->ud)->coords = event.draggedCoord;
	}
	else if (event.id == TileSelectionEvent::Id::undoDrag) {
		mActionMgr.AddTimedAction(MovePieceTo(*static_cast<CellVisual*>(event.cell->ud), event.cell->coords),
		                          mGameConfig.moveBackPieceDuration); // FIXME
	}
}

void PlayScreen::OnPieceRemoved(const Cell& cell) {
	if (cell.category != CellCategory::piece) {
		return;
	}
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.level);
	for (int i = 0; i < 3; ++i) {
		if (level.pieceIds[i] == cell.pieceId) {
			++mMatchStats.targetPieceCount[i];
			CheckLevelCompletion();
			break;
		}
	}
}

void PlayScreen::OnMatch3Event(const Match3Event& event) {
	switch (event.id) {
	case Match3Event::Id::match: {
		int         inc = IncreaseScore(event.match);
		const Cell& cell = mBoard.GetCell(event.match.cellIdx);
		mRenderActionMgr.AddTimedAction(DrawMatchScore(inc, cell, mEngine.GetTextRenderer(), mGameConfig, *mFonts[1]), mGameConfig.scoreTextDuration);
		PlaySound(0);
		break;
	}
	case Match3Event::Id::removePiece: {
		assert(event.removePiece.cell->category == CellCategory::piece);
		assert(! event.removePiece.cell->hasEffect); // effects are handled in Match3Event::Id::triggerEffect
		if (event.removePiece.targetCellIdx != -1) {
			const Cell& dstCell = mBoard.GetCell(event.removePiece.targetCellIdx);
			mActionMgr.AddTimedAction(MovePieceTo(*static_cast<CellVisual*>(event.removePiece.cell->ud), dstCell.coords),
			                          mGameConfig.suckPieceDuration);
		}
		else {
			mActionMgr.AddTimedAction(ScalePiece(*static_cast<CellVisual*>(event.removePiece.cell->ud), 1.f, 0.f), mGameConfig.removePieceDuration);
		}
		OnPieceRemoved(*event.removePiece.cell);
		break;
	}
	case Match3Event::Id::newPiece: {
		assert(event.newPiece.cell->category == CellCategory::piece);
		CellVisual& visual = GetVisual(*event.newPiece.cell);
		visual.bitmapIdx = pieceIcons[event.newPiece.pieceId];
		visual.scale = 1.f;
		visual.rotation = 0.f;
		// Drop new tiles from the top
		mActionMgr.AddTimedAction(FallPieceFromTo(visual, event.newPiece.cell->coords.x, mGameConfig.pieceFallYCoord, event.newPiece.cell->coords.y),
		                          mGameConfig.pieceFallDuration);
		mMatchStats.layerCount += event.newPiece.cell->layers;
		break;
	}
	case Match3Event::Id::swap: {
		const Cell& firstCell = mBoard.GetCell(event.pair.first);
		const Cell& secondCell = mBoard.GetCell(event.pair.second);
		std::swap(GetVisual(firstCell), GetVisual(secondCell));
		// Note: cells have been swapped already
		mActionMgr.AddTimedAction(MovePieceFromTo(*static_cast<CellVisual*>(firstCell.ud), secondCell.coords, firstCell.coords),
		                          mGameConfig.swapSpeed);
		mActionMgr.AddTimedAction(MovePieceFromTo(*static_cast<CellVisual*>(secondCell.ud), firstCell.coords, secondCell.coords),
		                          mGameConfig.swapSpeed);
		break;
	}
	case Match3Event::Id::dropPiece: {
		const Cell& firstCell = mBoard.GetCell(event.pair.first);
		const Cell& secondCell = mBoard.GetCell(event.pair.second);
		std::swap(GetVisual(firstCell), GetVisual(secondCell));
		// Already swapped
		mActionMgr.AddTimedAction(
		    FallPieceFromTo(*static_cast<CellVisual*>(secondCell.ud), firstCell.coords.x, firstCell.coords.y, secondCell.coords.y),
		    mGameConfig.pieceFallDuration);
		break;
	}
	case Match3Event::Id::newEffect: {
		if (mGameConfig.settings.infoOn) {
			// TODO mEffectInfoPanel.ShowHelp(event.specialPiece.type);
		}
		assert(event.specialPiece.cell->category == CellCategory::piece);
		assert(event.specialPiece.cell->hasEffect);
		CellVisual* visual = static_cast<CellVisual*>(event.specialPiece.cell->ud);
		visual->bitmapIdx = pieceIcons[event.specialPiece.pieceId];
		visual->scale = 1.f;
		visual->rotation = 0.f;
		break;
	}
	case Match3Event::Id::triggerEffect: {
		assert(event.effect.mainCell->category == CellCategory::piece);
		assert(event.effect.mainCell->hasEffect);
		// TODO Scale up
		Vec2 startCoords = event.effect.mainCell->coords + Vec2 { mGameConfig.board.cellWidth, mGameConfig.board.cellHeight } * 0.5f;
		if (event.effect.type == EffectType::hrocket) {
			mRenderActionMgr.AddTimedAction(DrawLaser(startCoords, { 0.f, startCoords.y }, mGameRenderer), mGameConfig.glowTrailTime);
			mRenderActionMgr.AddTimedAction(DrawLaser(startCoords, { RefWindowWidth, startCoords.y }, mGameRenderer), mGameConfig.glowTrailTime);
		}
		else if (event.effect.type == EffectType::vrocket) {
			mRenderActionMgr.AddTimedAction(DrawLaser(startCoords, { startCoords.x, 0.f }, mGameRenderer), mGameConfig.glowTrailTime);
			mRenderActionMgr.AddTimedAction(DrawLaser(startCoords, { startCoords.x, RefWindowHeight }, mGameRenderer), mGameConfig.glowTrailTime);
		}
		else if (event.effect.type == EffectType::miniBomb) {
			mRenderActionMgr.AddTimedAction(DrawBlast(startCoords, 32.f, 128.f, mGameRenderer), mGameConfig.glowTrailTime);
		}
		else if (event.effect.type == EffectType::bomb) {
			mRenderActionMgr.AddTimedAction(DrawBlast(startCoords, 32.f, 256.f, mGameRenderer), mGameConfig.glowTrailTime);
		}
		mActionMgr.AddTimedAction(ScalePiece(GetVisual(*event.effect.mainCell), 1.f, 0.f), mGameConfig.removePieceDuration);
		OnPieceRemoved(*event.effect.mainCell);
		break;
	}
	case Match3Event::Id::removeLayer: {
		const Cell& cell = *event.removeLayer.cell;
		mRenderActionMgr.AddTimedAction(DrawBrokenIce(cell, mEngine.GetBitmapRenderer(), mGameConfig), mGameConfig.brokenIceDuration);
		assert(mMatchStats.layerCount > 0);
		mMatchStats.layerCount--;
		// TODO PlaySound(0);
		break;
	}
	default:
		break;
	}
}

void PlayScreen::CheckLevelCompletion() {
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.level);
	bool         res = true;
	switch (level.goal.id) {
	case GoalId::breakIce:
		res = (mMatchStats.layerCount == 0);
		break;
	case GoalId::collectMatches:
		for (int i = 0; i < 3; ++i) {
			res = (mMatchStats.targetPieceCount[i] >= level.goal.collectMatches.count[i]) && res;
		}
		break;
	case GoalId::removeJellies:
		// TODO
		res = false;
		break;
	case GoalId::collectAllStars:
		// TODO
		res = false;
		break;
	default:
		break;
	}
	if (res) {
		if (mMatchStats.level + 1 == mGameDataModule.GetNumLevels()) {
			mGameComplete = true;
		}
		else {
			mLevelComplete = true;
		}
	}
}

void PlayScreen::DrawUI() const {
	const TextRenderer& textRenderer = mEngine.GetTextRenderer();
	const TextStyle     textStyle { whiteColor, blackColor };
	const TextStyle     textStyle1 { redColor, blackColor };
	const Level&        level = *mGameDataModule.GetLevel(mMatchStats.level);
	char                tmp[256];
	const float         y = 60.f;

	snprintf(tmp, sizeof(tmp), "%04d", mMatchStats.score);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { 60, y }, textStyle, GameDrawOrder::overlays);

	const int time = static_cast<int>(mMatchTime);
	snprintf(tmp, sizeof(tmp), "%d:%02d", time / 60, time % 60);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { 500, y }, mMatchTime < criticalTime ? textStyle1 : textStyle, GameDrawOrder::overlays);

	if (level.goal.id == GoalId::collectMatches) {
		Vec2 pos = mGameConfig.ui.goalStartCoord;
		for (int i = 0; i < 3; ++i) {
			const int icon = pieceIcons[level.pieceIds[i]];
			mGameRenderer.DrawIcon(icon, pos, 0.f, whiteColor, GameDrawOrder::overlays);
			snprintf(tmp, sizeof(tmp), "%d/%d", mMatchStats.targetPieceCount[i], level.goal.collectMatches.count[i]);
			textRenderer.Write(*mFonts[2], tmp, pos + Vec2 { 40.f, -20.f }, textStyle, GameDrawOrder::overlays);
			pos.x += 180.f;
		}
	}
	else if (level.goal.id == GoalId::breakIce) {
		Vec2 pos = mGameConfig.ui.goalStartCoord;
		for (int i = 0; i < mMatchStats.layerCount; ++i) {
			mGameRenderer.DrawIcon(iceSprites[0], pos, 0.f, whiteColor, GameDrawOrder::overlays);
			pos.x += gameTextures[iceSprites[0]]->Width() + 12;
		}
	}

	for (int i = 0; i < MaxBoosterTypesPerLevel; ++i) {
		if (level.boosterCount[i] > 0) {
			Vec2 coords;
			if (mSelectedBooster == i) {
				coords = mSelectedBoosterCoord;
			}
			else {
				coords = mBoosterButtons[i].GetRect().pos + mBoosterButtons[i].GetRect().size * 0.5f + Vec2 { 8.f, 0.f };
			}
			mGameRenderer.DrawIcon(boosterIcons[level.boosterIds[i]], coords, 0.f, whiteColor, GameDrawOrder::overUI);
			snprintf(tmp, sizeof(tmp), "%d", level.boosterCount[i]);
			textRenderer.Write(*mFonts[1], tmp, mBoosterButtons[i].GetRect().pos + Vec2 { 12.f, 12.f }, defaultTextStyle, GameDrawOrder::overUI);
		}
	}
}

void PlayScreen::SetupNewBoardAnimation() {
	Random rnd;
	for (Cell& cell : mBoard.GetCells()) {
		CellVisual& visual = GetVisual(cell);
		visual.coords = cell.coords;
		if (cell.category == CellCategory::piece) {
			visual.bitmapIdx = pieceIcons[cell.pieceId];
		}
		else if (cell.category == CellCategory::obstacle) {
			visual.bitmapIdx = obstacleIcons[cell.pieceId];
		}
		else if (cell.category == CellCategory::star) {
			visual.bitmapIdx = starSprite;
		}
		else {
			visual.bitmapIdx = -1;
		}
		visual.scale = 0.f;
		visual.rotation = 0.f;
		visual.bkgAlpha = 0.f;
		float delay = rnd.NextF(0.f, 0.5f);
		mActionMgr.AddTimedAction(FadeInAlpha(visual), mGameConfig.newPieceDuration, delay);
		mActionMgr.AddTimedAction(ScalePiece(visual, 0.f, 1.f), mGameConfig.newPieceDuration, delay);
	}
}

void PlayScreen::PlayMusic() const {
	if (mMusic && mGameSettings.musicOn) {
		mMusic->SetVolume(1.0f);
		mMusic->Play();
	}
}

void PlayScreen::StopMusic() const {
	if (mMusic) {
		mMusic->Stop();
	}
}

void PlayScreen::PauseMusic() const {
	if (mMusic) {
		mMusic->Pause();
	}
}

void PlayScreen::ResumeMusic() const {
	if (mMusic) {
		mMusic->Resume();
	}
}

void PlayScreen::PlaySound(int idx) const {
	if (mGameSettings.sfxOn && mSounds[idx]) {
		mSounds[idx]->Play();
	}
}

int PlayScreen::IncreaseScore(const MatchEvent& match) {
	int inc = 0;
	switch (match.comboType) {
	case ComboType::C3:
		inc = 10;
		break;
	case ComboType::C4:
		inc = 40;
		break;
	case ComboType::C5:
		inc = 80;
		break;
	case ComboType::T3:
		inc = 160;
		break;
	case ComboType::L:
		inc = 160;
		break;
	case ComboType::T4:
		inc = 320;
		break;
	case ComboType::T5:
		inc = 640;
		break;
	case ComboType::Unknown:
		break;
	}
	const int mul = (match.cascadeCount + 1);
	inc *= mul;
	mMatchStats.score += inc;
	mMatchStats.score = std::min(9999, mMatchStats.score);
	return inc;
}

void PlayScreen::ParseConfig(const char* varName, const char* varValue) {
}
