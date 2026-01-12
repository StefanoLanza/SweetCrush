#include "PlayScreen.h"
#include "Actions.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameConfig.h"
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
#include <engine/Input.h>
#include <engine/SdlMusic.h>
#include <engine/SdlSound.h>
#include <engine/TextRender.h>

#include <cassert>
#include <cmath>

using namespace Wind;

namespace {

const UIButtonDesc pauseButtonDesc {
	UIAbsolutePos(-60, -60),
	UIAutoSize,
	UIHorizAlignment::right,
	UIVertAlignment::bottom,
};

const UIBitmapDesc optionButtonBitmapDesc {
	"menuButton.png",
	UIZeroPos,
	UIAutoSize,
};

} // namespace

PlayScreen::PlayScreen(Engine& engine, const GameRenderer& gameRenderer, const GameConfig& gameConfig, const GameSettings& gameSettings,
                       ActionMgr& renderActionMgr, MatchStats& matchStats, const GameDataModule& gameDataModule)
    : mEngine(engine)
    , mGameRenderer(gameRenderer)
    , mGameConfig(gameConfig)
    , mGameSettings(gameSettings)
    , mRenderActionMgr(renderActionMgr)
    , mMatchStats(matchStats)
    , mGameDataModule(gameDataModule)
    , mBoard { NumCols, NumRows }
    , mTileSelector { std::make_unique<TileSelector>(mBoard, gameConfig) }
    , mBoostInfoPanel(engine)
    , mPanel(UIDefaultPanelDesc)
    , mPauseButton(MakeButton(pauseButtonDesc, optionButtonBitmapDesc, engine))
    , mMatch3 { mBoard, mBoardGenerator, *mTileSelector }
    , mTime { 0 } {
	mTileSelector->AddCallback([this](const TileSelectionEvent& event) { OnCellSelectionEvent(event); });
	mMatch3.SetCallback([this](const Match3Event& event) { OnMatch3Event(event); });
}

PlayScreen::~PlayScreen() = default;

const char* PlayScreen::GetName() const {
	return "PlayScreen";
}

void PlayScreen::LoadAssets() {
	Audio& audio = mEngine.GetAudio();
	mMusic = audio.LoadMusic("audio/music.ogg");
	mSounds[0] = audio.LoadSound("audio/match.wav");
	for (int i = 0; i < NumSprites; ++i) {
		sprites[i] = mEngine.LoadTexture(spriteDefs[i].bitmap);
	}
}

void PlayScreen::BuildUI(UICanvas& canvas) {
	mBoostInfoPanel.BuildUI(canvas);
	mFonts[0] = mEngine.GetTextRenderer().AddFont("mediumFont");
	mFonts[1] = mEngine.GetTextRenderer().AddFont("tiny");
	mFonts[2] = mEngine.GetTextRenderer().AddFont("smallFont");
	mPanel.AddButton(mPauseButton);
	canvas.GetPanel().AddPanel(mPanel);
}

GameScreenId PlayScreen::Tick(float dt, const Input& input) {
	if (mPauseButton.IsPressed(input)) {
		return ScreenId::pauseGame;
	}
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyPressed(SDLK_AC_BACK)) {
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyPressed(SDLK_ESCAPE)) {
#endif
		return ScreenId::pauseGame;
	}
	if (mMatchStats.gameComplete) {
		if (mRenderActionMgr.AnyRunning()) {
			return ScreenId::play; // wait until all animations are over
		}
		return ScreenId::gameComplete;
	}
	else if (mMatchStats.levelComplete) {
		if (mRenderActionMgr.AnyRunning()) {
			return ScreenId::play; // wait until all animations are over
		}
		return ScreenId::levelComplete;
	}

	if (mBoostInfoPanel.Wait(input)) {
		return ScreenId::play;
	}

	if (mMatch3.IsWaitingForUser()) {
		// Decrease time only when waiting for user selection
		mTime = std::max(0.f, mTime - dt);
	}

	if (mTime > 0.f) {
		if (! mActionMgr.AnyRunning()) { // do not update match while animations are still running
			mMatch3.Update(input);
			// TODO speed up music when running out of time (missing in SDL_mixer)
		}
	}
	else {
		return ScreenId::gameOver;
	}
	mActionMgr.RunActions(dt);

	return ScreenId::play;
}

void PlayScreen::Draw(GameScreenId topScreen) const {
	if (topScreen != ScreenId::play) {
		return;
	}
	mGameRenderer.DrawBoard(mBoard, mTileSelector->GetSelectedCell(), mGameConfig);
	DrawUI();
}

void PlayScreen::Enter(GameScreenId prevScreen) {
	if (prevScreen == levelComplete) {
		NextLevel();
	}
	else if (prevScreen == ScreenId::pauseGame) {
		// resume game
		if (mMatchStats.restartLevel) {
			ReplayLevel();
		}
	}
	else if (prevScreen == ScreenId::gameOver) {
		ReplayLevel();
	}
	else {
		NewGame();
	}
	if (prevScreen == ScreenId::pauseGame && ! mMatchStats.restartLevel) {
		ResumeMusic();
	}
	else {
		PlayMusic();
	}
	mPanel.SetVisible(true);
}

void PlayScreen::Exit() {
	mRenderActionMgr.Clear(); // stop showing score and other effects
	mBoostInfoPanel.Hide();
	mPanel.SetVisible(false);
	PauseMusic();
}

void PlayScreen::NewGame() {
	mMatchStats.level = 0;
	mMatchStats.score = 0;
	mMatchStats.gameComplete = false;
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
	mMatchStats.gameComplete = false;
	StartLevel();
}

void PlayScreen::StartLevel() {
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.level);
	if (level.boardDef) {
		mBoardGenerator.InitBoard(mBoard, *level.boardDef, level.seed, level.pieceIds, 5, mGameConfig.board);
	}
	else {
		mBoardGenerator.GenRandomBoard(mBoard, level.seed, level.mask, level.pieceIds, 5, mGameConfig.board);
	}
	mTime = level.time;
	for (int& c : mMatchStats.targetPieceCount) {
		c = 0;
	}
	mMatchStats.layerCount = mBoard.TotalLayerCount();
	mMatchStats.levelComplete = false;
	mActionMgr.Clear();
	SetupNewBoardAnimation();
	mMatch3.Run();
}

void PlayScreen::OnCellSelectionEvent(const TileSelectionEvent& event) {
	Cell& cell = mBoard.GetCell(event.cellIdx);
	if (event.id == TileSelectionEvent::Id::undoDrag) {
		mActionMgr.AddTimedAction(MoveBackPiece(cell), mGameConfig.moveBackPieceDuration);
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
		mRenderActionMgr.AddTimedAction(DrawMatchScore(inc, cell, mEngine.GetTextRenderer(), mGameConfig, *mFonts[1]), mGameConfig.scoreTextDuration,
		                                0.f, ActionFlags::nonBlocking);
		PlaySound(0);
		break;
	}
	case Match3Event::Id::removePiece: {
		Cell& cell = mBoard.GetCell(event.removePiece.cellIdx);
		assert(cell.category == CellCategory::piece);
		assert(! cell.hasBooster); // boosters are handled in Match3Event::Id::triggerBooster
		if (event.removePiece.boosterCellIdx != -1) {
			const Cell& dstCell = mBoard.GetCell(event.removePiece.boosterCellIdx);
			mActionMgr.AddTimedAction(MovePieceTo(cell, dstCell.coords), mGameConfig.suckPieceDuration);
		}
		else {
			mActionMgr.AddTimedAction(ScaleCellPiece(cell, 1.f, 0.f), mGameConfig.removePieceDuration);
		}
		OnPieceRemoved(cell);
		break;
	}
	case Match3Event::Id::newPiece: {
		Cell& cell = mBoard.GetCell(event.newPiece.cellIdx);
		assert(cell.category == CellCategory::piece);
		cell.pieceGraphics.bitmapIdx = pieceDefs[event.newPiece.pieceId].sprite;
		cell.pieceGraphics.scale = 1.f;
		cell.pieceGraphics.rotation = 0.f;
		// Drop new tiles from the top
		mActionMgr.AddTimedAction(FallPieceFromTo(cell, mGameConfig.pieceFallYCoord, cell.coords.y), mGameConfig.pieceFallDuration);
		mMatchStats.layerCount += cell.layers;
		break;
	}
	case Match3Event::Id::swap: {
		Cell& firstCell = mBoard.GetCell(event.pair.first);
		Cell& secondCell = mBoard.GetCell(event.pair.second);
		// Note: cells have been swapped already
		mActionMgr.AddTimedAction(MovePieceFromTo(firstCell, secondCell.coords, firstCell.coords), mGameConfig.swapSpeed);
		mActionMgr.AddTimedAction(MovePieceFromTo(secondCell, firstCell.coords, secondCell.coords), mGameConfig.swapSpeed);
		break;
	}
	case Match3Event::Id::dropPiece: {
		const Cell& firstCell = mBoard.GetCell(event.pair.first);
		Cell&       secondCell = mBoard.GetCell(event.pair.second);
		// Already swapped
		mActionMgr.AddTimedAction(FallPieceFromTo(secondCell, firstCell.coords.y, secondCell.coords.y), mGameConfig.pieceFallDuration);
		break;
	}
	case Match3Event::Id::newBooster: {
		if (mGameConfig.settings.infoOn) {
			mBoostInfoPanel.ShowHelp(event.booster.type);
		}
		Cell& cell = mBoard.GetCell(event.booster.cellIdx);
		assert(cell.category == CellCategory::piece);
		assert(cell.hasBooster);
		cell.pieceGraphics.bitmapIdx = pieceDefs[event.booster.pieceId].sprite;
		cell.pieceGraphics.scale = 1.f;
		cell.pieceGraphics.rotation = 0.f;
		break;
	}
	case Match3Event::Id::triggerBooster: {
		Cell& cell = mBoard.GetCell(event.booster.cellIdx);
		assert(cell.category == CellCategory::piece);
		assert(cell.hasBooster);
		// TODO Scale up
		Vec2 centralCoords = cell.coords + Vec2 { mGameConfig.board.cellWidth, mGameConfig.board.cellHeight } * 0.5f;
		;
		if (event.booster.type == BoosterType::hrocket) {
			mRenderActionMgr.AddTimedAction(DrawGlow(centralCoords, true, mEngine.GetBitmapRenderer()), mGameConfig.bombExplosionTime);
		}
		else if (event.booster.type == BoosterType::vrocket) {
			mRenderActionMgr.AddTimedAction(DrawGlow(centralCoords, false, mEngine.GetBitmapRenderer()), mGameConfig.bombExplosionTime);
		}
		//		mRenderActionMgr.AddTimedAction(DrawExplosion(cell, mEngine.GetBitmapRenderer(), mGameConfig), mGameConfig.bombExplosionTime, 0.f,
		//	                                ActionFlags::nonBlocking);
		mActionMgr.AddTimedAction(ScaleCellPiece(cell, 1.f, 0.f), mGameConfig.removePieceDuration);
		OnPieceRemoved(cell);
		break;
	}
	case Match3Event::Id::removeLayer: {
		const Cell& cell = mBoard.GetCell(event.removeLayer.cellIdx);
		mRenderActionMgr.AddTimedAction(DrawBrokenIce(cell, mEngine.GetBitmapRenderer(), mGameConfig), mGameConfig.brokenIceDuration, 0.f,
		                                ActionFlags::nonBlocking);
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
	case GoalId::breakAllIce:
		res = (mMatchStats.layerCount == 0);
		break;
	case GoalId::collectMatches:
		for (int i = 0; i < 3; ++i) {
			res = (mMatchStats.targetPieceCount[i] >= level.goal.collectMatches.count[i]) && res;
		}
		break;
	default:
		break;
	}
	if (res) {
		if (mMatchStats.level + 1 == mGameDataModule.GetNumLevels()) {
			mMatchStats.gameComplete = true;
		}
		else {
			mMatchStats.levelComplete = true;
		}
	}
}

void PlayScreen::DrawUI() const {
	const auto&           textRenderer = mEngine.GetTextRenderer();
	const BitmapRenderer& bitmapRender = mEngine.GetBitmapRenderer();
	const TextStyle       textStyle { whiteColor, blackColor };
	const TextStyle       textStyle1 { redColor, blackColor };
	const Level&          level = *mGameDataModule.GetLevel(mMatchStats.level);
	char                  tmp[256];
	const float           y = 60.f;

	snprintf(tmp, sizeof(tmp), "LEVEL    %d", mMatchStats.level + 1);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { 60, y }, textStyle, DrawOrder::UI);

	snprintf(tmp, sizeof(tmp), "%s    %04d", GetLocalizedString(GameStringId::score), mMatchStats.score);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { 340, y }, textStyle, DrawOrder::UI);

	const int time = static_cast<int>(mTime);
	snprintf(tmp, sizeof(tmp), "%d:%02d", time / 60, time % 60);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { 60, RefWindowHeight - 120 }, mTime < 10.f ? textStyle1 : textStyle, DrawOrder::UI);

	BitmapExtParams prm;
	prm.pivot = BitmapPivot::center;
	prm.blending = true;
	prm.orientation = 0.f;
	prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::overBackground);
	prm.blending = true;

	if (level.goal.id == GoalId::collectMatches) {
		Vec2 pos = mGameConfig.ui.goalStartCoord;
		for (int i = 0; i < 3; ++i) {
			const auto& def = pieceDefs[level.pieceIds[i]];
			bitmapRender.DrawBitmapEx(*sprites[def.sprite], pos, prm);
			snprintf(tmp, sizeof(tmp), "%d/%d", mMatchStats.targetPieceCount[i], level.goal.collectMatches.count[i]);
			textRenderer.Write(*mFonts[2], tmp, pos + Vec2 { 40.f, -20.f }, textStyle, DrawOrder::UI);
			pos.x += 180.f;
		}
	}
	else if (level.goal.id == GoalId::breakAllIce) {
		Vec2 pos = mGameConfig.ui.goalStartCoord;
		for (int i = 0; i < mMatchStats.layerCount; ++i) {
			bitmapRender.DrawBitmapEx(*sprites[iceSprite], pos, prm);
			pos.x += sprites[iceSprite]->Width() + 12;
		}
	}
}

void PlayScreen::SetupNewBoardAnimation() {
	for (Cell& cell : mBoard.GetCells()) {
		cell.pieceGraphics.coords = cell.coords;
		if (cell.category == CellCategory::piece) {
			cell.pieceGraphics.bitmapIdx = pieceDefs[cell.pieceId].sprite;
		}
		else if (cell.category == CellCategory::obstacle) {
			cell.pieceGraphics.bitmapIdx = obstacleDefs[cell.pieceId].sprite;
		}
		else if (cell.category == CellCategory::star) {
			cell.pieceGraphics.bitmapIdx = starSprite;
		}
		else {
			cell.pieceGraphics.bitmapIdx = -1;
		}
		cell.pieceGraphics.scale = 0.f;
		cell.pieceGraphics.rotation = 0.f;
		float delay = 0.f; //(mBoard.GetRows() - 1 - cell.row + cell.col) * 0.05f;
		mActionMgr.AddTimedAction(ScaleCellPiece(cell, 0.f, 1.f), mGameConfig.newPieceDuration, delay);
	}
}

void PlayScreen::PlayMusic() const {
	if (mMusic && mGameSettings.musicOn) {
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
