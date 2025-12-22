#include "PlayScreen.h"
#include "Actions.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameConfig.h"
#include "GameDataModule.h"
#include "GameDrawOrder.h"
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

const UIButtonDesc optionButtonDesc {
	UIAbsolutePos(-70, 160),
	UIAutoSize,
	UIHorizAlignment::right,
	UIVertAlignment::top,
};

const UIBitmapDesc optionButtonBitmapDesc {
	"menuButton.png",
	UIZeroPos,
	UIAutoSize,
};

} // namespace

PlayScreen::PlayScreen(Engine& engine, const GameConfig& gameConfig, const GameSettings& gameSettings, ActionMgr& renderActionMgr,
                       MatchStats& matchStats, const GameDataModule& gameDataModule)
    : mEngine(engine)
    , mGameConfig(gameConfig)
    , mGameSettings(gameSettings)
    , mRenderActionMgr(renderActionMgr)
    , mMatchStats(matchStats)
    , mGameDataModule(gameDataModule)
    , mBoard { NumCols, NumRows }
    , mTileSelector { std::make_unique<TileSelector>(mBoard, gameConfig) }
    , mBoostInfoPanel(engine)
    , mPanel(UIDefaultPanelDesc)
    , mPauseButton(MakeButton(optionButtonDesc, optionButtonBitmapDesc, engine))
    , mMatch3 { mBoard, gameConfig, *mTileSelector }
    , mBoardFillCounter { 0 }
    , mAnimCounter { 0 }
    , mTime { 0 } {
	mTileSelector->AddCallback([this](const TileSelectionEvent& event) { OnCellSelectionEvent(event); });
	mMatch3.SetCallback([this](const Match3Event& event) { OnMatch3Event(event); });
}

PlayScreen::~PlayScreen() = default;

void PlayScreen::LoadAssets() {
	mSelectionBitmap = mEngine.LoadTexture("outline.png");
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
#if defined(__ANDROID__)
	if (input.GetKeyPressed(SDLK_AC_BACK)) {
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyPressed(SDLK_ESCAPE)) {
#endif
		return ScreenId::pauseGame;
	}
	if (mMatchStats.gameComplete) {
		return ScreenId::gameComplete;
	}
	else if (mMatchStats.levelComplete) {
		mRenderActionMgr.Clear(); // stop showing score and other effects
		return ScreenId::levelComplete;
	}

	if (mBoostInfoPanel.Wait(input)) {
		return ScreenId::play;
	}

	mTime = std::max(0.f, mTime - dt);
	if (mTime > 0.f) {
		if (mAnimCounter == 0) { // do not update match while animations are still running
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
	const BitmapRenderer& bitmapRender = mEngine.GetBitmapRenderer();
	DrawBoard(bitmapRender);
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
	mMatch3.NewBoard(level.seed, *level.boardDef, level.gemIds, 5);
	mTime = level.time;
	for (int& c : mMatchStats.targetGemCount) {
		c = 0;
	}
	mMatchStats.levelComplete = false;
	mAnimCounter = 0;
	mActionMgr.Clear();
	SetupNewBoardAnimation();
	mMatch3.Run();
}

void PlayScreen::OnCellSelectionEvent(const TileSelectionEvent& event) {
	Cell& cell = mBoard.GetCell(event.cellIdx);
	if (event.id == TileSelectionEvent::Id::undoDrag) {
		mActionMgr.AddAction(&mAnimCounter, 0.f, ReturnTile(cell, mGameConfig.tileMoveBackSpeed));
	}
}

void PlayScreen::OnTileRemoved(const Cell& cell) {
	if (cell.category != CellCategory::piece) {
		return;
	}
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.level);
	for (int i = 0; i < 3; ++i) {
		if (level.gemIds[i] == cell.pieceId) {
			++mMatchStats.targetGemCount[i];
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
		mRenderActionMgr.AddTimedAction(nullptr, 0.f, mGameConfig.scoreTextDuration, DrawMatchScore(inc, cell, mEngine, mGameConfig, *mFonts[1]));
		PlaySound(0);
		break;
	}
	case Match3Event::Id::removeTile: {
		Cell& cell = mBoard.GetCell(event.cellIdx);
		mActionMgr.AddTimedAction(&mAnimCounter, 0.f, mGameConfig.tileScaleDuration, ScaleTile(cell, 1.f, 0.f));
		OnTileRemoved(cell);
		break;
	}
	case Match3Event::Id::newPiece: {
		Cell& cell = mBoard.GetCell(event.newPiece.cellIdx);
		assert(cell.category == CellCategory::piece);
		cell.pieceAnim.spriteIdx = gemDefs[event.newPiece.targetPieceId].sprite;
		cell.pieceAnim.coords = { cell.coords.x, mGameConfig.tileFallYCoord };
		cell.pieceAnim.scale = 1.f;
		cell.pieceAnim.scaleDev = 0.f;
		cell.pieceAnim.rotation = 0.f;
		// Drop new tiles from the top
		mActionMgr.AddAction(&mAnimCounter, 0.f, ReturnTile(cell, mGameConfig.tileFallSpeed));
		break;
	}
	case Match3Event::Id::swap: {
		Cell& firstTile = mBoard.GetCell(event.pair.first);
		Cell& secondTile = mBoard.GetCell(event.pair.second);
		mActionMgr.AddAction(&mAnimCounter, 0.f, MoveTile(firstTile, secondTile.coords, mGameConfig.tileSwapSpeed));
		mActionMgr.AddAction(&mAnimCounter, 0.f, MoveTile(secondTile, firstTile.coords, mGameConfig.tileSwapSpeed));
		break;
	}
	case Match3Event::Id::dropTile: {
		Cell&       firstTile = mBoard.GetCell(event.pair.first);
		const Cell& secondTile = mBoard.GetCell(event.pair.second);
		mActionMgr.AddAction(&mAnimCounter, 0.f, MoveTile(firstTile, secondTile.coords, mGameConfig.tileFallSpeed));
		break;
	}
	case Match3Event::Id::newBooster: {
		const int typeIdx = static_cast<int>(event.booster.type);
		mBoostInfoPanel.ShowHelp(event.booster.type);
		Cell& cell = mBoard.GetCell(event.booster.cellIdx);
		assert(cell.category == CellCategory::booster);
		cell.pieceAnim.spriteIdx = boosterDefs[typeIdx].sprite;
		cell.pieceAnim.rotation = 0.f; // boosterDefs[typeIdx].rotation;
		cell.pieceAnim.scale = 1.f;
		cell.pieceAnim.scaleDev = boosterDefs[typeIdx].scaleDev;
		break;
	}
	case Match3Event::Id::boosterTriggered: {
		TriggerBooster(event.booster);
		break;
	}
	default:
		break;
	}
}

void PlayScreen::TriggerBooster(const Booster& booster) {
	const Cell& cell = mBoard.GetCell(booster.cellIdx);
	switch (booster.type) {
	case BoosterType::hrocket: {
		mMatch3.HorizontalRocket(cell.col, cell.row);
	} break;
	case BoosterType::vrocket: {
		mMatch3.VerticalRocket(cell.col, cell.row);
	} break;
	case BoosterType::miniBomb:
		mMatch3.Bomb(cell.col, cell.row, 1);
		break;
	case BoosterType::bomb:
		mMatch3.Bomb(cell.col, cell.row, 2);
		break;
	default:
		break;
	}
	mRenderActionMgr.AddTimedAction(&mAnimCounter, 0.f, mGameConfig.bombExplosionTime, DrawExplosion(cell, mEngine, mGameConfig));
}

void PlayScreen::CheckLevelCompletion() {
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.level);
	bool         res = true;
	for (int i = 0; i < 3; ++i) {
		res = (mMatchStats.targetGemCount[i] >= level.objective.gemCount[i]) && res;
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
	textRenderer.Write(*mFonts[2], tmp, Vec2 { 40, y }, textStyle, DrawOrder::UI);

	snprintf(tmp, sizeof(tmp), "%s    %04d", GetLocalizedString(GameStringId::score), mMatchStats.score);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { 300, y }, textStyle, DrawOrder::UI);

	const int time = static_cast<int>(mTime);
	snprintf(tmp, sizeof(tmp), "%d:%02d", time / 60, time % 60);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { RefWindowWidth - 140, y }, mTime < 10.f ? textStyle1 : textStyle, DrawOrder::UI);

	BitmapExtParams prm;
	prm.pivot = BitmapPivot::center;
	prm.blending = true;
	prm.orientation = 0.f;
	prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::overBackground);
	prm.blending = true;

	Vec2 pos = mGameConfig.targetGemCoord;
	for (int i = 0; i < 3; ++i) {
		const auto& def = gemDefs[level.gemIds[i]];
		bitmapRender.DrawBitmapEx(*sprites[def.sprite], pos, prm);
		snprintf(tmp, sizeof(tmp), "%d/%d", mMatchStats.targetGemCount[i], level.objective.gemCount[i]);
		textRenderer.Write(*mFonts[2], tmp, pos + Vec2 { 40.f, -20.f }, textStyle, DrawOrder::UI);
		pos.x += 180.f;
	}
}

void PlayScreen::DrawBoard(const BitmapRenderer& bitmapRender) const {
	const float cellWidth = mGameConfig.cellWidth;
	const float cellHeight = mGameConfig.cellHeight;
	const float cellSpacing = mGameConfig.cellSpacing;
	const float scaleFactor = std::cos(mTime * 4.f);
	{
		// Draw board background tiles
		BitmapExtParams prm;
		prm.width = cellWidth + 2.f * cellSpacing;
		prm.height = cellHeight + 2.f * cellSpacing;
		prm.color = { 255, 255, 255, 140 };
		prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::backgroundTile);
		prm.blending = true;
		for (const Cell& cell : mBoard.GetCells()) {
			if (cell.backgroundTileIdx) {
				const BoardTileDef& def = boardTileDefs[cell.backgroundTileIdx - 1];
				bitmapRender.DrawBitmapEx(*sprites[def.sprite], cell.coords - Vec2 { cellSpacing, cellSpacing }, prm);
			}
		}
	}
	// Draw pieces, obstacles and boosters
	for (const Cell& cell : mBoard.GetCells()) {
		if (cell.pieceAnim.spriteIdx >= 0) {
			BitmapExtParams prm;
			prm.width = cellWidth;
			prm.height = cellHeight;
			prm.pivot = BitmapPivot::center;
			prm.orientation = mTime * cell.pieceAnim.rotation;
			prm.scale = cell.pieceAnim.scale + cell.pieceAnim.scaleDev * scaleFactor;
			prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardTile);
			prm.blending = true;
			assert(cell.category == CellCategory::piece || cell.category == CellCategory::booster);
			Vec2 pos = cell.pieceAnim.coords + Vec2 { cellWidth, cellHeight } * 0.5f;
			bitmapRender.DrawBitmapEx(*sprites[cell.pieceAnim.spriteIdx], pos, prm);
		}
	}
	// Highlight selected cell
	const int selected = mTileSelector->GetSelectedCell();
	if (selected >= 0) {
		const Cell&     cell = mBoard.GetCell(selected);
		BitmapExtParams prm;
		prm.width = cellWidth + 2.f * cellSpacing;
		prm.height = cellHeight + 2.f * cellSpacing;
		prm.pivot = BitmapPivot::topLeft;
		prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardTile);
		prm.blending = true;
		bitmapRender.DrawBitmapEx(*mSelectionBitmap, cell.pieceAnim.coords - Vec2 { cellSpacing, cellSpacing }, prm);
	}
}

void PlayScreen::SetupNewBoardAnimation() {
	mBoardFillCounter = 0;
	for (Cell& cell : mBoard.GetCells()) {
		cell.pieceAnim.coords = cell.coords;
		if (cell.category == CellCategory::piece) {
			cell.pieceAnim.spriteIdx = gemDefs[cell.pieceId].sprite;
		}
		else if (cell.category == CellCategory::obstacle) {
			cell.pieceAnim.spriteIdx = obstacleDefs[cell.pieceId].sprite;
		}
		else {
			cell.pieceAnim.spriteIdx = -1; // empty cell or hole
		}
		if (cell.pieceAnim.spriteIdx != -1) {
			cell.pieceAnim.scale = 0.f;
			cell.pieceAnim.scaleDev = 0.f;
			cell.pieceAnim.rotation = 0.f;
			// tile.currCoords = { tile.idleCoords.x, mGameConfig.tileFallYCoord };
			float delay = 0.f; //(mBoard.GetRows() - 1 - cell.row + cell.col) * 0.05f;
			mActionMgr.AddTimedAction(&mBoardFillCounter, delay, mGameConfig.tileScaleDuration, ScaleTile(cell, 0.f, 1.f));
		}
	}
}

void PlayScreen::PlayMusic() const {
	if (mMusic && mGameSettings.audioOn) {
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
	if (mGameSettings.audioOn && mSounds[idx]) {
		mSounds[idx]->Play();
	}
}

int PlayScreen::IncreaseScore(const Match& match) {
	int inc = 0;
	switch (match.comboType) {
	case ComboType::C3:
		inc = 10;
		break;
	case ComboType::C4:
		mMatch3.AddBooster(BoosterType::miniBomb, match.cellIdx);
		inc = 40;
		break;
	case ComboType::C5:
		mMatch3.AddBooster(BoosterType::hrocket, match.cellIdx);
		inc = 80;
		break;
	case ComboType::T3:
		mMatch3.AddBooster(BoosterType::vrocket, match.cellIdx);
		inc = 160;
		break;
	case ComboType::L:
		mMatch3.AddBooster(BoosterType::bomb, match.cellIdx);
		inc = 160;
		break;
	case ComboType::T4:
		mMatch3.AddBooster(BoosterType::bomb, match.cellIdx);
		inc = 320;
		break;
	case ComboType::T5:
		mMatch3.AddBooster(BoosterType::bomb, match.cellIdx);
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
