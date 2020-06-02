#include "PlayScreen.h"
#include "Actions.h"
#include "AssetDefs.h"
#include "Config.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "GameSettings.h"
#include "Levels.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "TileSelector.h"
#include "UIDefs.h"
#include <engine/ActionMgr.h>
#include <engine/Audio.h>
#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/Font.h>
#include <engine/Input.h>
#include <engine/SdlMusic.h>
#include <engine/SdlSound.h>
#include <engine/TextRender.h>

using namespace Wind;

namespace {

const UIButtonDesc optionButtonDesc {
	UIAbsolutePos(-90, 160),
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
                       MatchStats& matchStats)
    : mEngine(engine)
    , mGameConfig(gameConfig)
    , mGameSettings(gameSettings)
    , mRenderActionMgr(renderActionMgr)
    , mMatchStats(matchStats)
    , mBoard { NumCols, NumRows }
    , mTileSelector { std::make_unique<TileSelector>(mBoard, gameConfig, engine.GetInput()) }
    , mBoostInfoPanel(engine)
    , mPanel(UIDefaultPanelDesc)
    , mOptionsButton(MakeButton(optionButtonDesc, optionButtonBitmapDesc, engine))
    , mMatch3 { mBoard, gameConfig, *mTileSelector }
    , mBoardFillCounter { 0 }
    , mAnimCounter { 0 }
    , mTime { 0 } {
	mTileSelector->AddCallback([this](const TileSelectionEvent& event) { OnCellSelectionEvent(event); });
	mMatch3.SetCallback([this](const Match3Event& event) { OnMatch3Event(event); });
}

PlayScreen::~PlayScreen() = default;

void PlayScreen::LoadAssets() {
	mSelection = mEngine.LoadBitmap("outline.png");
	Audio& audio = mEngine.GetAudio();
	mMusic = audio.LoadMusic("audio/music.ogg");
	mSounds[0] = audio.LoadSound("audio/match.wav");
	for (int i = 0; i < NumSprites; ++i) {
		sprites[i] = mEngine.LoadBitmap(spriteDefs[i].bitmap);
	}
}

void PlayScreen::BuildUI(UICanvas& canvas) {
	mBoostInfoPanel.BuildUI(canvas);
	mFonts[0] = mEngine.GetTextRenderer().AddFont("mediumFont");
	mFonts[1] = mEngine.GetTextRenderer().AddFont("tiny");
	mFonts[2] = mEngine.GetTextRenderer().AddFont("smallFont");
	mPanel.AddButton(mOptionsButton);
	canvas.GetPanel().AddPanel(mPanel);
}

GameScreenId PlayScreen::Tick(float dt) {
	const Input& input = mEngine.GetInput();
	if (mOptionsButton.IsPressed(input)) {
		// TODO
	}
#if _WIN32
	if (input.GetKeyPressed(SDLK_ESCAPE)) {
#elif defined(__ANDROID__)
	if (input.GetKeyPressed(SDLK_AC_BACK)) {
#endif
		return ScreenId::leaveGame;
		//		mMusic->Pause();
	}
	if (mMatchStats.gameComplete) {
		return ScreenId::gameComplete;
	}
	else if (mMatchStats.levelComplete) {
		return ScreenId::levelComplete;
	}

	if (mBoostInfoPanel.Wait(input)) {
		return ScreenId::play;
	}
	mMusic->Resume();

	mTime = std::max(0.f, mTime - dt);
	if (mTime > 0.f) {
		if (mAnimCounter == 0) { // do not update match while animations are still running
			mMatch3.Update();
			// TODO speed up music when running out of time (missing in SDL_mixer)
		}
	}
	else {
		return ScreenId::gameOver;
	}
	return ScreenId::play;
}

void PlayScreen::Draw(GameScreenId topScreen) const {
	if (topScreen != ScreenId::play && topScreen != ScreenId::leaveGame) {
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
	else if (prevScreen == ScreenId::leaveGame) {
		// resume game
	}
	else if (prevScreen == ScreenId::gameOver) {
		ReplayLevel();
	}
	else {
		NewGame();
	}
	PlayMusic();
	mPanel.SetVisible(true);
}

void PlayScreen::Exit() {
	mBoostInfoPanel.Hide();
	mPanel.SetVisible(false);
	StopMusic();
}

void PlayScreen::NewGame() {
	mMatchStats.level = 0;
	mMatchStats.score = 0;
	mMatchStats.gameComplete = false;
	StartLevel();
}

void PlayScreen::NextLevel() {
	if (mMatchStats.level < numLevels) {
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
	const Level& level = levels[mMatchStats.level];
	mMatch3.NewBoard(level.seed, level.gemIds, level.numGemIds);
	mTime = level.time;
	for (int& c : mMatchStats.targetGemCount) {
		c = 0;
	}
	mMatchStats.levelComplete = false;
	mAnimCounter = 0;
	SetupNewBoardAnimation();
	mMatch3.Run();
}

void PlayScreen::OnCellSelectionEvent(const TileSelectionEvent& event) {
	Cell& cell = mBoard.GetCell(event.cellIdx);
	if (event.id == TileSelectionEvent::Id::undoDrag) {
		ActionMgr& actionMgr = mEngine.GetTickActionMgr();
		actionMgr.AddAction(&mAnimCounter, 0.f, actionUndefDuration, ReturnTile(cell, mGameConfig.tileMoveBackSpeed));
	}
}

void PlayScreen::OnTileRemoved(const Cell& cell) {
	if (cell.category != TileCategory::gem) {
		return;
	}
	const Level& level = levels[mMatchStats.level];
	for (int i = 0; i < 3; ++i) {
		if (level.gemIds[i] == cell.tileId) {
			++mMatchStats.targetGemCount[i];
			CheckLevelCompletion();
			break;
		}
	}
}

void PlayScreen::OnMatch3Event(const Match3Event& event) {
	ActionMgr& actionMgr = mEngine.GetTickActionMgr();

	switch (event.id) {
	case Match3Event::Id::match: {
		int         inc = IncreaseScore(event.match);
		const Cell& cell = mBoard.GetCell(event.match.cellIdx);
		mRenderActionMgr.AddAction(nullptr, 0.f, mGameConfig.scoreTextDuration, DrawMatchScore(inc, cell, mEngine, mGameConfig, *mFonts[1]));
		PlaySound(0);
		break;
	}
	case Match3Event::Id::removeTile: {
		Cell& cell = mBoard.GetCell(event.cellIdx);
		actionMgr.AddAction(&mAnimCounter, 0.f, mGameConfig.tileScaleDuration, ScaleTile(cell, cell.tileAnim.scale, 0.f));
		OnTileRemoved(cell);
		break;
	}
	case Match3Event::Id::newGem: {
		Cell& cell = mBoard.GetCell(event.newGem.cellIdx);
		cell.tileAnim.spriteIdx = gemDefs[event.newGem.targetGemId].sprite;
		cell.tileAnim.coords = { cell.coords.x, mGameConfig.tileFallYCoord };
		cell.tileAnim.scale = 1.f;
		cell.tileAnim.rotation = 0.f;
		// Drop new tiles from the top
		actionMgr.AddAction(&mAnimCounter, 0.f, actionUndefDuration, ReturnTile(cell, mGameConfig.tileFallSpeed));
		break;
	}
	case Match3Event::Id::swap: {
		Cell& firstTile = mBoard.GetCell(event.pair.first);
		Cell& secondTile = mBoard.GetCell(event.pair.second);
		actionMgr.AddAction(&mAnimCounter, 0.f, actionUndefDuration, MoveTile(firstTile, secondTile.coords, mGameConfig.tileSwapSpeed));
		actionMgr.AddAction(&mAnimCounter, 0.f, actionUndefDuration, MoveTile(secondTile, firstTile.coords, mGameConfig.tileSwapSpeed));
		break;
	}
	case Match3Event::Id::dropTile: {
		Cell&       firstTile = mBoard.GetCell(event.pair.first);
		const Cell& secondTile = mBoard.GetCell(event.pair.second);
		actionMgr.AddAction(&mAnimCounter, 0.f, actionUndefDuration, MoveTile(firstTile, secondTile.coords, mGameConfig.tileFallSpeed));
		break;
	}
	case Match3Event::Id::newBooster: {
		const int typeIdx = static_cast<int>(event.booster.type);
		mBoostInfoPanel.ShowHelp(event.booster.type);
		Cell& cell = mBoard.GetCell(event.booster.cellIdx);
		cell.tileAnim.spriteIdx = boosterDefs[typeIdx].sprite;
		cell.tileAnim.rotation = boosterDefs[typeIdx].rotation;
		cell.tileAnim.scale = 1.f;
		// TODO some animation ?
		break;
	}
	case Match3Event::Id::boosterTriggered: {
		Cell& cell = mBoard.GetCell(event.booster.cellIdx);
		if (event.booster.type == BoosterType::miniBomb) {
			mRenderActionMgr.AddAction(&mAnimCounter, 0.f, mGameConfig.bombExplosionTime, DrawExplosion(cell, mEngine, mGameConfig));
		}
		/*		else if (event.booster.type == BoosterType::yellowStar) {
		            mRenderActionMgr.AddAction(nullptr, 0.f, mGameConfig.starMoveTime, DrawMovingStar(cell, mEngine,
		   mGameConfig.yellowStarIconCoord));
		            ++mMatchStats.yellowStars;
		            CheckLevelCompletion();
		        }
		        else if (event.booster.type == BoosterType::redStar) {
		            mRenderActionMgr.AddAction(nullptr, 0.f, mGameConfig.starMoveTime, DrawMovingStar(cell, mEngine, mGameConfig.redStarIconCoord));
		            ++mMatchStats.redStars;
		            CheckLevelCompletion();
		        }*/
		break;
	}
	default:
		break;
	}
}

void PlayScreen::CheckLevelCompletion() {
	const Level& level = levels[mMatchStats.level];
	bool         res = true;
	for (int i = 0; i < 3; ++i) {
		res = (mMatchStats.targetGemCount[i] >= level.targetGemCount[i]) && res;
	}
	if (res) {
		if (mMatchStats.level + 1 == numLevels) {
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
	const Level&          level = levels[mMatchStats.level];
	char                  tmp[256];
	const float           y = 60.f;

	snprintf(tmp, sizeof(tmp), "LEVEL    %d", mMatchStats.level + 1);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { 40, y }, textStyle, DrawOrder::UI);

	snprintf(tmp, sizeof(tmp), "%s    %06d", GetLocalizedString(GameStringId::score), mMatchStats.score);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { 300, y }, textStyle, DrawOrder::UI);

	const int time = static_cast<int>(mTime);
	snprintf(tmp, sizeof(tmp), "%d:%02d", time / 60, time % 60);
	textRenderer.Write(*mFonts[2], tmp, Vec2 { RefWindowWidth - 140, y }, mTime < 10.f ? textStyle1 : textStyle, DrawOrder::UI);

	BitmapExtParams prm;
	prm.pivot = BitmapPivot::center;
	prm.blending = true;
	prm.orientation = 0.f;

	Vec2 pos = mGameConfig.targetGemCoord;
	for (int i = 0; i < 3; ++i) {
		const auto& def = gemDefs[level.gemIds[i]];
		bitmapRender.DrawBitmapEx(*sprites[def.sprite], pos, prm);
		snprintf(tmp, sizeof(tmp), "%d/%d", mMatchStats.targetGemCount[i], level.targetGemCount[i]);
		textRenderer.Write(*mFonts[2], tmp, pos + Vec2 { 40.f, -20.f }, textStyle, DrawOrder::UI);
		pos.x += 160.f;
	}
}

void PlayScreen::DrawBoard(const BitmapRenderer& bitmapRender) const {
	const float cellWidth = mGameConfig.cellWidth;
	const float cellHeight = mGameConfig.cellHeight;
	const float cellSpacing = mGameConfig.cellSpacing;
	{
		// Draw board tiles
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
	for (const Cell& cell : mBoard.GetCells()) {
		if (cell.tileAnim.spriteIdx >= 0) {
			BitmapExtParams prm;
			prm.width = cellWidth;
			prm.height = cellHeight;
			prm.pivot = BitmapPivot::center;
			prm.orientation = mTime * cell.tileAnim.rotation;
			prm.scale = cell.tileAnim.scale;
			prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardTile);
			prm.blending = true;
			Vec2 pos = cell.tileAnim.coords + Vec2 { cellWidth, cellHeight } * 0.5f;
			bitmapRender.DrawBitmapEx(*sprites[cell.tileAnim.spriteIdx], pos, prm);
		}
	}
	const int selected = mTileSelector->GetSelectedCell();
	if (selected >= 0) {
		const Cell&     cell = mBoard.GetCell(selected);
		BitmapExtParams prm;
		prm.width = cellWidth + 2.f * cellSpacing;
		prm.height = cellHeight + 2.f * cellSpacing;
		prm.pivot = BitmapPivot::topLeft;
		prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardTile);
		prm.blending = true;
		bitmapRender.DrawBitmapEx(*mSelection, cell.tileAnim.coords - Vec2 { cellSpacing, cellSpacing }, prm);
	}
}

void PlayScreen::SetupNewBoardAnimation() {
	ActionMgr& actionMgr = mEngine.GetTickActionMgr();
	mBoardFillCounter = 0;
	for (Cell& cell : mBoard.GetCells()) {
		cell.tileAnim.coords = cell.coords;
		if (cell.category == TileCategory::gem) {
			cell.tileAnim.spriteIdx = gemDefs[cell.tileId].sprite;
		}
		else if (cell.category == TileCategory::obstacle) {
			cell.tileAnim.spriteIdx = obstacleDefs[cell.tileId].sprite;
		}
		else {
			cell.tileAnim.spriteIdx = -1;
		}
		cell.tileAnim.scale = 0.f;
		cell.tileAnim.rotation = 0.f;
		// tile.currCoords = { tile.idleCoords.x, mGameConfig.tileFallYCoord };
		float delay = 0.f; //(mBoard.GetRows() - 1 - cell.row + cell.col) * 0.05f;
		actionMgr.AddAction(&mBoardFillCounter, delay, mGameConfig.tileScaleDuration, ScaleTile(cell, 0.f, 1.f));
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
		inc = 40;
		break;
	case ComboType::C5:
		mMatch3.AddBooster(BoosterType::yellowStar, match.cellIdx);
		inc = 80;
		break;
	case ComboType::T3:
		mMatch3.AddBooster(BoosterType::bomb, match.cellIdx);
		inc = 160;
		break;
	case ComboType::T4:
		mMatch3.AddBooster(BoosterType::bomb, match.cellIdx);
		inc = 320;
		break;
	case ComboType::T5:
		inc = 640;
		break;
	case ComboType::L:
		mMatch3.AddBooster(BoosterType::miniBomb, match.cellIdx);
		inc = 160;
		break;
	case ComboType::X:
		inc = 160;
		break;
	}
	const int mul = (match.cascadeCount + 1);
	inc *= mul;
	mMatchStats.score += inc;
	return inc;
}
