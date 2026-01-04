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
    , mMatch3 { mBoard, mBoardGenerator, gameConfig, *mTileSelector }
    , mTime { 0 } {
	mTileSelector->AddCallback([this](const TileSelectionEvent& event) { OnCellSelectionEvent(event); });
	mMatch3.SetCallback([this](const Match3Event& event) { OnMatch3Event(event); });
}

PlayScreen::~PlayScreen() = default;

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
#if defined(__ANDROID__)
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

	mTime = std::max(0.f, mTime - dt);
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
	if (level.boardDef) {
		mBoardGenerator.InitBoard(mBoard, *level.boardDef, level.seed, level.pieceIds, 5, mGameConfig.board);
	}
	else {
		mBoardGenerator.GenRandomBoard(mBoard, level.seed, level.pieceIds, 5, mGameConfig.board);
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
		mActionMgr.AddAction(MoveBackPiece(cell, mGameConfig.moveBackPieceSpeed), 0.f);
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
		mRenderActionMgr.AddTimedAction(DrawMatchScore(inc, cell, mEngine, mGameConfig, *mFonts[1]), 0.f, mGameConfig.scoreTextDuration);
		PlaySound(0);
		break;
	}
	case Match3Event::Id::removePiece: {
		Cell& cell = mBoard.GetCell(event.removePiece.cellIdx);
		mActionMgr.AddTimedAction(ScaleCellSprite(cell, 1.f, 0.f), 0.f, mGameConfig.removePieceDuration);
		OnPieceRemoved(cell);
		break;
	}
	case Match3Event::Id::newPiece: {
		Cell& cell = mBoard.GetCell(event.newPiece.cellIdx);
		assert(cell.category == CellCategory::piece);
		// TODO fully separate cell logic from visuals
		cell.pieceGraphics.coords = { cell.coords.x, mGameConfig.pieceFallYCoord };
		cell.pieceGraphics.scale = 1.f;
		cell.pieceGraphics.rotation = 0.f;
		// Drop new tiles from the top
		mActionMgr.AddAction(MoveBackPiece(cell, mGameConfig.pieceFallSpeed), 0.f);
		mMatchStats.layerCount += cell.layers;
		break;
	}
	case Match3Event::Id::swap: {
		Cell& firstTile = mBoard.GetCell(event.pair.first);
		Cell& secondTile = mBoard.GetCell(event.pair.second);
		mActionMgr.AddAction(MovePieceTo(firstTile, secondTile.coords, mGameConfig.swapSpeed), 0.f);
		mActionMgr.AddAction(MovePieceTo(secondTile, firstTile.coords, mGameConfig.swapSpeed), 0.f);
		break;
	}
	case Match3Event::Id::dropPiece: {
		Cell&       firstTile = mBoard.GetCell(event.pair.first);
		const Cell& secondTile = mBoard.GetCell(event.pair.second);
		mActionMgr.AddAction(MovePieceTo(firstTile, secondTile.coords, mGameConfig.pieceFallSpeed), 0.f);
		break;
	}
	case Match3Event::Id::newBooster: {
		if (mGameConfig.settings.infoOn) {
			mBoostInfoPanel.ShowHelp(event.booster.type);
		}
		Cell& cell = mBoard.GetCell(event.booster.cellIdx);
		cell.pieceGraphics.scale = 1.f;
		cell.pieceGraphics.rotation = 0.f;
		break;
	}
	case Match3Event::Id::triggerBooster: {
		const Cell& cell = mBoard.GetCell(event.booster.cellIdx);
		mRenderActionMgr.AddTimedAction(DrawExplosion(cell, mEngine, mGameConfig), 0.f, mGameConfig.bombExplosionTime);
		break;
	}
	case Match3Event::Id::removeLayer: {
		const Cell& cell = mBoard.GetCell(event.removeLayer.cellIdx);
		mRenderActionMgr.AddTimedAction(DrawBrokenIce(cell, mEngine, mGameConfig), 0.f, mGameConfig.brokenIceDuration);
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

void PlayScreen::DrawBoard(const BitmapRenderer& bitmapRender) const {
	const float cellWidth = mGameConfig.board.cellWidth;
	const float cellHeight = mGameConfig.board.cellHeight;
	const float cellSpacing = mGameConfig.board.cellSpacing;
	const float dynScaleFactor = std::cos(mTime * 4.f);
	{
		// Draw board background tiles
		BitmapExtParams prm;
		prm.width = cellWidth + 2.f * cellSpacing;
		prm.height = cellHeight + 2.f * cellSpacing;
		prm.color = { 255, 255, 255, 140 };
		prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::backgroundTile);
		prm.blending = true;
		for (const Cell& cell : mBoard.GetCells()) {
			if (cell.category != CellCategory::hole) {
				const BoardTileDef& def = boardTileDefs[0];
				bitmapRender.DrawBitmapEx(*sprites[def.sprite], cell.coords - Vec2 { cellSpacing, cellSpacing }, prm);
			}
		}
	}
	// Draw pieces, obstacles and boosters
	for (const Cell& cell : mBoard.GetCells()) {
		Vec2            pos = cell.pieceGraphics.coords + Vec2 { cellWidth, cellHeight } * 0.5f;
		BitmapExtParams prm;
		prm.width = cellWidth;
		prm.height = cellHeight;
		prm.pivot = BitmapPivot::center;
		prm.blending = true;
		if (cell.category == CellCategory::piece || cell.category == CellCategory::obstacle) {
			prm.orientation = mTime * cell.pieceGraphics.rotation;
			prm.scale = cell.pieceGraphics.scale;
			prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardTile);
			int spriteIdx = cell.category == CellCategory::piece ? pieceDefs[cell.pieceId].sprite : obstacleDefs[0].sprite;
			bitmapRender.DrawBitmapEx(*sprites[spriteIdx], pos, prm);
			if (cell.layers > 0) {
				prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::ice);
				prm.orientation = 0.0f;
				prm.scale = 1.f;
				bitmapRender.DrawBitmapEx(*sprites[iceSprite], pos, prm);
			}
		}
		if (cell.hasBooster) {
			prm.scale = 0.5f + 0.1f * dynScaleFactor;
			prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::ice);
			prm.orientation = 0.f;
			pos.x += cellWidth * 0.25f;
			pos.y += cellHeight * 0.25f;
			bitmapRender.DrawBitmapEx(*sprites[boosterDefs[(int)cell.boosterType].sprite], pos, prm);
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
		bitmapRender.DrawBitmapEx(*sprites[selectionSprite], cell.pieceGraphics.coords - Vec2 { cellSpacing, cellSpacing }, prm);
	}
}

void PlayScreen::SetupNewBoardAnimation() {
	for (Cell& cell : mBoard.GetCells()) {
		cell.pieceGraphics.coords = cell.coords;
		if (cell.category == CellCategory::piece) {
		}
		else if (cell.category == CellCategory::obstacle) {
			//cell.pieceGraphics.spriteIdx = obstacleDefs[cell.pieceId].sprite;
		}
		cell.pieceGraphics.scale = 0.f;
		cell.pieceGraphics.rotation = 0.f;
		float delay = 0.f; //(mBoard.GetRows() - 1 - cell.row + cell.col) * 0.05f;
		mActionMgr.AddTimedAction(ScaleCellSprite(cell, 0.f, 1.f), delay, mGameConfig.newPieceDuration);
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
