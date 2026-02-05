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
#include <engine/FontManager.h>
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
	.pos = UIAbsolutePos(32, -32),
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::left,
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

const UIBitmapDesc pauseButtonBitmapDesc {
	.fileName = "backButton.png",
	.pos = UIZeroPos,
	.size = UIAutoSize,
	.color = yellowColor,
};

constexpr UIPanelDesc boosterPanelDesc {
	.pos = { 0.f, -20.f, 0.f, 0.f },
	.size = { 340.f, 100.f, 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::bottom,
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

constexpr float criticalTime = 10.f;

CellVisual& GetVisual(const Cell& cell) {
	return *static_cast<CellVisual*>(cell.ud);
}

} // namespace

PlayScreen::PlayScreen(Engine& engine, const GameRenderer& gameRenderer, const AppConfig& gameConfig, const GameSettings& gameSettings,
                       MatchStats& matchStats, const GameDataModule& gameDataModule)
    : mEngine(engine)
    , mGameRenderer(gameRenderer)
    , mGameConfig(gameConfig)
    , mGameSettings(gameSettings)
    , mMatchStats(matchStats)
    , mGameDataModule(gameDataModule)
    , mBoard { NumCols, NumRows, mGameConfig.board }
    , mCellSelector { std::make_unique<TileSelector>(mBoard, gameConfig) }
    , mCanvas(canvasDesc)
    , mBoostersPanel(boosterPanelDesc)
    , mPauseButton(pauseButtonDesc, pauseButtonBitmapDesc)
    , mBoosterButtons { { booster0ButtonDesc, boosterButtonBitmapDesc },
	                    { booster1ButtonDesc, boosterButtonBitmapDesc },
	                    { booster2ButtonDesc, boosterButtonBitmapDesc } }
    , mMatch3 { mBoard, mBoardGenerator, *mCellSelector }
    , mTime { 0.f }
    , mMatchTime { 0 } {
	mCellSelector->SetCallback([this](const TileSelectionEvent& event) { OnTileSelectionEvent(event); });
	mMatch3.SetCallback([this](const Match3Event& event) { OnMatch3Event(event); });
	mCellGraphics.resize(NumCols * NumRows);
	// Build UI
	mCanvas.AddPanel(mBoostersPanel);
	mCanvas.AddButton(mPauseButton);
	mBoostersPanel.AddButton(mBoosterButtons[0]);
	mBoostersPanel.AddButton(mBoosterButtons[1]);
	mBoostersPanel.AddButton(mBoosterButtons[2]);
}

PlayScreen::~PlayScreen() = default;

const char* PlayScreen::GetName() const {
	return "PlayScreen";
}

void PlayScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
	Audio& audio = engine.GetAudio();
	mMusic = audio.LoadMusic("audio/music.ogg");
	mSounds[0] = audio.LoadSound("audio/match.wav");
	mFonts[0] = mEngine.GetFontManager().AddFont("mediumFont");
	mFonts[1] = mEngine.GetFontManager().AddFont("tiny");
	mFonts[2] = mEngine.GetFontManager().AddFont("smallFont");
}

ScreenEvent PlayScreen::Tick(float dt, const Input& input) {
	if (mGameComplete) {
		if (mBlockingActionCounter != 0) {
			// wait for blocking animations
			return Continue();
		}
		else if (! mMatch3.IsWaitingForUser()) {
			// wait completion of all cascades
			mMatch3.Update(input);
			return Continue();
		}
		else {
			return GoTo(GameScreenIds::gameComplete);
		}
	}

	if (mLevelComplete) {
		if (mBlockingActionCounter != 0) {
			// wait for blocking animations
			return Continue();
		}
		else if (! mMatch3.IsWaitingForUser()) {
			// wait completion of all cascades
			mMatch3.Update(input);
			return Continue();
		}
		else {
			return GoTo(GameScreenIds::levelComplete);
		}
	}

	// Handle pause
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK)
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE)
#endif
	    || mPauseButton.IsClicked(input)) {
		return GoTo(GameScreenIds::pauseGame, ScreenTransition::slideIn);
	}

	if (mMatch3.IsWaitingForUser()) {
		SelectBooster(input);
		// Decrease time only when waiting for user selection
		mMatchTime = std::max(0.f, mMatchTime - dt);
	}

	if (mMatchTime <= 0.f) {
		return GoTo(GameScreenIds::gameOver);
	}

	if (mMatchTime < criticalTime) {
		if (mMusic) {
			mMusic->SetVolume(0.2f);
		}
		// TODO play clock sound
	}

	mTime += dt;
	if (mBlockingActionCounter == 0) {
		mMatch3.Update(input);
	}
	// else, do not update match while animations are still running

	return Continue();
}

void PlayScreen::SelectBooster(const Input& input) {
	bool handled = false;

	if (input.GetMouseButtonPressed(MouseButton::right)) {
		mSelectedBooster = -1; // release
		handled = true;
		return;
	}

	// Check buttons
	for (int i = 0; i < MaxBoosterTypesPerLevel; ++i) {
		if (mBoosterCount[i] > 0 && mBoosterButtons[i].IsClicked(input)) {
			// Unselect if pressing again on same button
			mSelectedBooster = mSelectedBooster == i ? -1 : i;
			mMatch3.ClearSelection();
			handled = true;
			break;
		}
	}

	if (! handled && mSelectedBooster >= 0) {
		// Check click on board
		int cellIdx = mBoard.GetCellAtCoords(input.GetMappedMouseCoord());
		if (input.GetMouseButtonPressed(MouseButton::left)) {
			if (cellIdx >= 0) {
				mMatch3.UseBooster(cellIdx);
				assert(mBoosterCount[mSelectedBooster] > 0);
				--mBoosterCount[mSelectedBooster];
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
		mSelectedBoosterCoord = input.GetMappedMouseCoord();
	}
}

void PlayScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mActionMgr.Run(dt);

	mGameRenderer.DrawBoard(mBoard, mCellSelector->GetSelectedTile(), mGameConfig, mTime);
	DrawUI(uiRenderer);

	// float t01 = (0.5f + 0.5f * sinf(mMatchTime * 5.0f));
	// Color c = whiteColor;
	// c.a = 255.f * t01;
	//  mGameRenderer.DrawBlast({ 300.f, 300.f }, 256.f * t01, 64, c);
	//  mGameRenderer.DrawLaser({ 0.f, 300.f }, { RefWindowWidth, 300.f }, 64);
	//   mGameRenderer.DrawLaser({ 100.f, 0.f }, { 100.f, mAppConfig.board.bottomRightCoord.y }, 64, 0.5f + 0.5f * sinf(mMatchTime * 5.0f));
}

void PlayScreen::Enter(const ScreenNavArgs& args) {
	bool restartLevel = false;
	if (args.mPrev == GameScreenIds::levelComplete) {
		NextLevel();
	}
	else if (args.mPrev == GameScreenIds::pauseGame) {
		// resume game
		std::memcpy(&restartLevel, args.mParams, sizeof restartLevel);
		if (restartLevel) {
			ReplayLevel();
		}
	}
	else if (args.mPrev == GameScreenIds::gameOver) {
		ReplayLevel();
	}
	else if (args.mPrev == GameScreenIds::effectInfo) {
		// continue playing
	}
	else {
		NewGame();
	}
	if (args.mPrev == GameScreenIds::pauseGame && ! restartLevel) {
		ResumeMusic();
	}
	else {
		PlayMusic();
	}
	mTime = 0.f;
}

void PlayScreen::Exit() {
	mActionMgr.Clear(); // stop showing score and other effects
	mBlockingActionCounter = 0;
	PauseMusic();
}

void PlayScreen::NewGame() {
	mMatchStats.levelIndex = 0;
	mMatchStats.score = 0;
	mLevelComplete = false;
	mGameComplete = false;
	StartLevel();
}

void PlayScreen::NextLevel() {
	if (mMatchStats.levelIndex < mGameDataModule.GetNumLevels()) {
		++mMatchStats.levelIndex;
		StartLevel();
	}
}

void PlayScreen::ReplayLevel() {
	mMatchStats.score = 0;
	mGameComplete = false;
	StartLevel();
}

void PlayScreen::StartLevel() {
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
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
	mMatch3.Restart();
	for (int i = 0; i < MaxBoosterTypesPerLevel; ++i) {
		mBoosterCount[i] = level.boosterCount[i];
	}
	mSelectedBooster = -1;
	mSelectedBoosterCoord = { 0.f, 0.f };
}

void PlayScreen::OnTileSelectionEvent(const TileSelectionEvent& event) {
	if (event.id == TileSelectionEvent::Id::drag) {
		static_cast<CellVisual*>(event.cell->ud)->coords = event.draggedCoord;
	}
	else if (event.id == TileSelectionEvent::Id::undoDrag) {
		mActionMgr.AddAction(MovePieceTo(*static_cast<CellVisual*>(event.cell->ud), event.cell->coords),
		                     { .duration = mGameConfig.moveBackPieceDuration, .counter = &mBlockingActionCounter });
	}
}

void PlayScreen::OnPieceRemoved(const Cell& cell) {
	if (cell.category != CellCategory::piece) {
		return;
	}
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
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
		int inc = IncreaseScore(event.match);
		mActionMgr.AddAction(DrawMatchScore(inc, *event.match.cell, mEngine.GetTextRenderer(), mGameConfig, *mFonts[1]),
		                     { .duration = mGameConfig.scoreTextDuration });
		PlaySound(0);
		break;
	}
	case Match3Event::Id::removePiece: {
		assert(event.removePiece.cell->category == CellCategory::piece);
		assert(! event.removePiece.cell->hasEffect); // effects are handled in Match3Event::Id::triggerEffect
		if (event.removePiece.targetCell) {
			mActionMgr.AddAction(MovePieceTo(*static_cast<CellVisual*>(event.removePiece.cell->ud), event.removePiece.targetCell->coords),
			                     { .duration = mGameConfig.suckPieceDuration, .counter = &mBlockingActionCounter });
		}
		else {
			mActionMgr.AddAction(ScalePiece(*static_cast<CellVisual*>(event.removePiece.cell->ud), 1.f, 0.f),
			                     { .duration = mGameConfig.removePieceDuration, .counter = &mBlockingActionCounter });
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
		mActionMgr.AddAction(FallPieceFromTo(visual, event.newPiece.cell->coords.x, mGameConfig.pieceFallYCoord, event.newPiece.cell->coords.y),
		                     { .duration = mGameConfig.pieceFallDuration, .counter = &mBlockingActionCounter });
		mMatchStats.layerCount += event.newPiece.cell->layers;
		break;
	}
	case Match3Event::Id::swap: {
		const Cell& firstCell = mBoard.GetCell(event.pair.first);
		const Cell& secondCell = mBoard.GetCell(event.pair.second);
		std::swap(GetVisual(firstCell), GetVisual(secondCell));
		// Note: cells have been swapped already
		mActionMgr.AddAction(MovePieceFromTo(*static_cast<CellVisual*>(firstCell.ud), secondCell.coords, firstCell.coords),
		                     { .duration = mGameConfig.swapSpeed, .counter = &mBlockingActionCounter });
		mActionMgr.AddAction(MovePieceFromTo(*static_cast<CellVisual*>(secondCell.ud), firstCell.coords, secondCell.coords),
		                     { .duration = mGameConfig.swapSpeed, .counter = &mBlockingActionCounter });
		break;
	}
	case Match3Event::Id::dropPiece: {
		const Cell& firstCell = mBoard.GetCell(event.pair.first);
		const Cell& secondCell = mBoard.GetCell(event.pair.second);
		std::swap(GetVisual(firstCell), GetVisual(secondCell));
		// Already swapped
		mActionMgr.AddAction(FallPieceFromTo(*static_cast<CellVisual*>(secondCell.ud), firstCell.coords.x, firstCell.coords.y, secondCell.coords.y),
		                     { .duration = mGameConfig.pieceFallDuration, .counter = &mBlockingActionCounter });
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
			mActionMgr.AddAction(DrawLaser(startCoords, { 0.f, startCoords.y }, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
			mActionMgr.AddAction(DrawLaser(startCoords, { RefWindowWidth, startCoords.y }, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
		}
		else if (event.effect.type == EffectType::vrocket) {
			mActionMgr.AddAction(DrawLaser(startCoords, { startCoords.x, 0.f }, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
			mActionMgr.AddAction(DrawLaser(startCoords, { startCoords.x, RefWindowHeight }, mGameRenderer),
			                     { .duration = mGameConfig.glowTrailTime });
		}
		else if (event.effect.type == EffectType::miniBomb) {
			mActionMgr.AddAction(DrawBlast(startCoords, 8.f, 128.f, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
		}
		else if (event.effect.type == EffectType::bomb) {
			mActionMgr.AddAction(DrawBlast(startCoords, 8.f, 256.f, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
		}
		mActionMgr.AddAction(ScalePiece(GetVisual(*event.effect.mainCell), 1.f, 0.f), { .duration = mGameConfig.removePieceDuration });
		OnPieceRemoved(*event.effect.mainCell);
		break;
	}
	case Match3Event::Id::removeLayer: {
		const Cell& cell = *event.removeLayer.cell;
		mActionMgr.AddAction(DrawBrokenIce(cell, mEngine.GetBitmapRenderer(), mGameConfig), { .duration = mGameConfig.brokenIceDuration });
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
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
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
		if (mMatchStats.levelIndex + 1 == mGameDataModule.GetNumLevels()) {
			mGameComplete = true;
		}
		else {
			mLevelComplete = true;
		}
	}
}

void PlayScreen::DrawUI(UIRenderer& uiRenderer) {
	const TextRenderer& textRenderer = mEngine.GetTextRenderer();
	const TextStyle     textStyle { whiteColor, blackColor };
	const TextStyle     textStyle1 { redColor, blackColor };
	const Level&        level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
	char                tmp[256];
	const float         y = 60.f;

	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);

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
			if (int diff = level.goal.collectMatches.count[i] - mMatchStats.targetPieceCount[i]; diff > 0) {
				SDL_snprintf(tmp, sizeof(tmp), "%d", diff);
				textRenderer.Write(*mFonts[2], tmp, pos + Vec2 { 40.f, -20.f }, textStyle, GameDrawOrder::overlays);
			}
			// TODO Draw tick icon
			pos.x += 140.f;
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
			snprintf(tmp, sizeof(tmp), "%d", mBoosterCount[i]);
			textRenderer.Write(*mFonts[1], tmp, mBoosterButtons[i].GetRect().pos + Vec2 { 12.f, 12.f }, defaultTextStyle, GameDrawOrder::overUI);
			mBoosterButtons[i].SetVisible(true);
		}
		else {
			mBoosterButtons[i].SetVisible(false);
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
		mActionMgr.AddAction(FadeInAlpha(visual), { .duration = mGameConfig.newPieceDuration, .delay = delay });
		mActionMgr.AddAction(ScalePiece(visual, 0.f, 1.f), { .duration = mGameConfig.newPieceDuration, .delay = delay });
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
