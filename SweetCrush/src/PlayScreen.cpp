#include "PlayScreen.h"
#include "Actions.h"
#include "AppConfig.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameDataModule.h"
#include "GameDrawOrder.h"
#include "GameRenderer.h"
#include "GameSettings.h"
#include "GameUI.h"
#include "Level.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "TileSelector.h"

#include <engine/Audio.h>
#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/Font.h>
#include <engine/FontManager.h>
#include <engine/Graphics.h>
#include <engine/Input.h>
#include <engine/SdlMusic.h>
#include <engine/SdlSound.h>

#include <cassert>
#include <cmath>

using namespace Wind;

namespace {

UIButton MakeBoosterButton() {
	const UIButtonDesc desc {
		.pos = UIAbsolutePos(0, 0),
		.size = UIAbsoluteSize(120.f, 120.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::center,
		.padding = 20.f,
		//.background = "UI/button_round_flat.png",
		.backgroundColor = panel1_color,
		._9patch = 16.f,
	};
	const UITextDesc labelDesc {
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::top,
		.font = "tiny",
		.style = defaultTextStyle,
	};
	const UIBitmapDesc iconDesc {
		.fileName = nullptr,
		.pos = UIZeroPos,
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::center,
		.sizing = UIBitmapSizing::fit,
	};
	UIButton button { desc };
	button.Add(iconDesc);
	button.Add(labelDesc);
	return button;
}

const UIButtonDesc pauseButtonDesc {
	.pos = UIAbsolutePos(0, 0),
	.size = UIAbsoluteSize(64, 64),
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
};

const UIBitmapDesc pauseButtonBitmapDesc {
	.fileName = "icons/options.png",
	.sizing = UIBitmapSizing::stretch,
	.color = button0_color,
};

const UIBitmapDesc goalIconDesc {
	.size = UIAbsoluteSize(48, 48),
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.sizing = UIBitmapSizing::user,
};

const UITextDesc goalCounterDesc {
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::bottom,
	.font = "tiny",
	.style = defaultTextStyle,
};

const UITextDesc scoreHeaderDesc {
	.stringId = GameStringId::score,
	.pos = { 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::left,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.style = defaultTextStyle,
};

const UITextDesc scoreTextDesc {
	.pos = { 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::left,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.style = defaultTextStyle,
};

const UITextDesc goalTextDesc {
	.stringId = GameStringId::goal,
	.pos = { 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.style = defaultTextStyle,
};

const UITextDesc timeHeaderDesc {
	.stringId = GameStringId::moves,
	.pos = { 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::right,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.style = defaultTextStyle,
};

const UITextDesc timeTextDesc {
	.pos = { 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::right,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.style = defaultTextStyle,
};

const float       topPanelRowHeight[] = { 48.f, 0.f };
const UIPanelDesc topPanelDesc {
	.pos = UIAbsolutePos(0.f, 0.f),
	.size = { 0.f, 160.f, 1.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.padding = 16.f,
	.background = "UI/button.png",
	.backgroundColor = panel1_color,
	._9patch = 16.f,
	.grid {
	    .cols = 3,
	    .rowHeight = topPanelRowHeight,
	},
};

const UIPanelDesc goalPanelDesc {
	.size = { 0.f, 0.f, 1.f, 1.f },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.padding = 0.f,
	.backgroundColor = transparentColor,
	.grid {
	    .cols = 3,
	},
};

const UIPanelDesc bottomPanelDesc {
	.pos = { 0.f, -80.f, 0.f, 0.f },
	.size = { 0.f, 120.f, 1.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::bottom,
	.padding = 16,
	.background = "UI/button.png",
	.backgroundColor = panel1_color,
	._9patch = 16.f,
	.grid {
	    .cols = 4,
	},
};

//	"gameartguppy/pet_turtle_160x160.png",
//"gameartguppy/pet_fish_160x160.png",

const UIBitmapDesc petDesc {
	.fileName = "gameartguppy/pet_bird_160x160.png",
	.pos = UIAbsolutePos(0.f, 30.f),
	.size = UIAbsoluteSize(96, 96),
	.pivot = { 0.45f, 1.0f },
	.horizontalAlignment = UIHorizAlignment::right,
	.verticalAlignment = UIVertAlignment::bottom,
	.sizing = UIBitmapSizing::user,
};

const UIBitmapDesc speechBubbleDesc {
	.fileName = "UI/speechBubble.png",
	.pos = UIAbsolutePos(0.f, 30.f),
	.size = UIAbsoluteSize(96, 96),
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::bottom,
	.sizing = UIBitmapSizing::fit,
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
    , mCanvas(MakeCanvas())
    , mMatch3 { mBoard, mBoardGenerator, *mCellSelector }
    , mTime { 0.f } {
	mCellSelector->SetCallback([this](const TileSelectionEvent& event) { OnTileSelectionEvent(event); });
	mMatch3.SetClientCallback([this](const Match3Event& event) { OnMatch3Event(event); });
	mCellGraphics.resize(NumCols * NumRows);

	// Build UI
	auto topPanel = mCanvas.Add(topPanelDesc);
	topPanel->Add(scoreHeaderDesc);
	topPanel->Add(goalTextDesc);
	topPanel->Add(timeHeaderDesc);
	mScoreText = topPanel->Add(scoreTextDesc);
	auto goalPanel = topPanel->Add(goalPanelDesc);
	for (int i = 0; i < 3; ++i) {
		mGoalIcons[i] = goalPanel->Add(goalIconDesc);
	}
	for (int i = 0; i < 3; ++i) {
		mGoalCounters[i] = goalPanel->Add(goalCounterDesc);
	}
	mTimeText = topPanel->Add(timeTextDesc);
	mPet = mCanvas.Add(petDesc);
	mCanvas.Add(speechBubbleDesc);

	Wind::UIPanel* bottomPanel = mCanvas.Add(bottomPanelDesc);
	mBoosterButtons[0] = bottomPanel->Add(MakeBoosterButton());
	mBoosterButtons[1] = bottomPanel->Add(MakeBoosterButton());
	mBoosterButtons[2] = bottomPanel->Add(MakeBoosterButton());
	mPauseButton = bottomPanel->Add(pauseButtonDesc);
	mPauseButton->Add(pauseButtonBitmapDesc);
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
	mFonts[0] = mEngine.GetFontManager().AddFont("tiny");
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
	mCanvas.HandleInput(input);
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK)
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE)
#endif
	    || mPauseButton->IsClicked()) {
		return GoTo(GameScreenIds::pauseGame, ScreenTransition::blur);
	}

	SelectBooster(input);

	if (mMatch3.IsWaitingForUser()) {
		UseSelectedBooster(input);
	}

	// TODO Moves
	if (mMatchStats.moves == 0) {
		return GoTo(GameScreenIds::gameOver);
	}

	mTime += dt;
	if (mBlockingActionCounter == 0) {
		mMatch3.Update(input);
	}
	// else, do not update match while animations are still running

	mPet->GetTransform().rotation = 0.1f * (0.5f + 0.5f * std::sin(mTime));

	return Continue();
}

void PlayScreen::SelectBooster(const Input& input) {
	if (input.GetMouseButtonPressed(MouseButton::right)) {
		mSelectedBooster = -1; // release
		return;
	}

	// Check buttons
	for (int i = 0; i < MaxBoosterTypesPerLevel; ++i) {
		if (mBoosterCount[i] > 0 && mBoosterButtons[i]->IsClicked()) {
			// Unselect if pressing again on same button
			mSelectedBooster = mSelectedBooster == i ? -1 : i;
			mMatch3.ClearSelection();
			break;
		}
	}

	for (int i = 0; i < MaxBoosterTypesPerLevel; ++i) {
		mBoosterButtons[i]->SetEnabled(mBoosterCount[i] > 0);
	}
}

void PlayScreen::UseSelectedBooster(const Input& input) {
	if (mSelectedBooster >= 0) {
		// Check click on board
		int cellIdx = mBoard.GetCellAtCoords(input.GetMappedMouseCoord());
		if (input.GetMouseButtonPressed(MouseButton::left)) {
			if (cellIdx >= 0) {
				mMatch3.UseBooster(cellIdx);
				assert(mBoosterCount[mSelectedBooster] > 0);
				--mBoosterCount[mSelectedBooster];
				mSelectedBooster = -1; // release
			}
		}
		else {
			if (cellIdx >= 0) {
				// TODO GetVisual(mBoard.GetCell(cellIdx)).highlighted = true;
			}
			// TODO highlight cell
		}
	}
}

void PlayScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mActionMgr.Run(dt);

	mGameRenderer.DrawBoard(mBoard, mCellSelector->GetSelectedTile(), mGameConfig, mTime);
	DrawUI(uiRenderer);
}

void PlayScreen::Enter(const ScreenNavArgs& args) {
	bool restartLevel = false;
	if (args.mPrev == GameScreenIds::pauseGame) {
		// resume game
		std::memcpy(&restartLevel, args.mParams, sizeof restartLevel);
		if (restartLevel) {
			NewGame();
		}
	}
	else if (args.mPrev == GameScreenIds::gameOver) {
		NewGame();
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
	PauseMusic();
}

void PlayScreen::NewGame() {
	mLevelComplete = false;
	mGameComplete = false;

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
	for (int& c : mMatchStats.targetPieceCount) {
		c = 0;
	}
	mMatchStats.score = 0;
	mMatchStats.moves = level.availableMoves;
	mMatchStats.layerCount = mBoard.TotalLayerCount();
	mActionMgr.Clear();
	SetupNewBoardAnimation();
	mMatch3.Restart();
	for (int i = 0; i < MaxBoosterTypesPerLevel; ++i) {
		mBoosterCount[i] = level.boosterCount[i];
	}
	mSelectedBooster = -1;
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
	case Match3Event::Id::swap:
		mMatchStats.moves--;
		// Fallthrough
	case Match3Event::Id::undoSwap: {
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
	case Match3Event::Id::match: {
		int inc = IncreaseScore(event.match);
		mActionMgr.AddAction(DrawMatchScore(inc, *event.match.cell, mEngine.GetTextRenderer(), mGameConfig, *mFonts[0]),
		                     { .duration = mGameConfig.scoreTextDuration });
		PlaySound(0);
		break;
	}
	case Match3Event::Id::removePiece: {
		assert(event.removePiece.cell->category == CellCategory::piece);
		assert(event.removePiece.cell->effect == EffectType::none); // effects are handled in Match3Event::Id::triggerEffect
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
		assert(event.specialPiece.cell->effect != EffectType::none);
		CellVisual* visual = static_cast<CellVisual*>(event.specialPiece.cell->ud);
		if (event.specialPiece.type == EffectType::colorBomb) {
			visual->bitmapIdx = colorBombIcon;
		}
		else {
			visual->bitmapIdx = pieceIcons[event.specialPiece.pieceId];
		}
		visual->scale = 1.f;
		visual->rotation = 0.f;
		break;
	}
	case Match3Event::Id::triggerEffect: {
		assert(event.effect.mainCell->category == CellCategory::piece);
		assert(event.effect.mainCell->effect != EffectType::none);
		Vec2 startCoords = event.effect.mainCell->coords + Vec2 { mGameConfig.board.cellWidth, mGameConfig.board.cellHeight } * 0.5f;
		if (event.effect.type == EffectType::hStriped) {
			mActionMgr.AddAction(DrawLaser(startCoords, { 0.f, startCoords.y }, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
			mActionMgr.AddAction(DrawLaser(startCoords, { RefWindowWidth, startCoords.y }, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
		}
		else if (event.effect.type == EffectType::vStriped) {
			mActionMgr.AddAction(DrawLaser(startCoords, { startCoords.x, 0.f }, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
			mActionMgr.AddAction(DrawLaser(startCoords, { startCoords.x, RefWindowHeight }, mGameRenderer),
			                     { .duration = mGameConfig.glowTrailTime });
		}
		else if (event.effect.type == EffectType::wrapped) {
			mActionMgr.AddAction(DrawBlast(startCoords, 8.f, 128.f, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
		}
		else if (event.effect.type == EffectType::colorBomb) {
			// TODO mActionMgr.AddAction(DrawBlast(startCoords, 8.f, 128.f, mGameRenderer), { .duration = mGameConfig.glowTrailTime });
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
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
	char         tmp[256];

	snprintf(tmp, sizeof(tmp), "%04d", mMatchStats.score);
	mScoreText->SetText(tmp);

#if 0
	const int time = static_cast<int>(mMatchTime);
	snprintf(tmp, sizeof(tmp), "%d:%02d", time / 60, time % 60);
	UITextStyle textStyle = defaultTextStyle;
	if (mMatchTime < criticalTime) {
		textStyle.color = redColor;
	}
#else
	snprintf(tmp, sizeof(tmp), "%d", mMatchStats.moves);
	UITextStyle textStyle = defaultTextStyle;
#endif
	mTimeText->SetText(tmp);
	mTimeText->SetStyle(textStyle);

	if (level.goal.id == GoalId::collectMatches) {
		for (int i = 0; i < 3; ++i) {
			const int icon = pieceIcons[level.pieceIds[i]];
			mGoalIcons[i]->SetVisible(true);
			mGoalIcons[i]->SetBitmap(gameTextures[icon]);
			// mGameRenderer.DrawIcon(icon, pos, 0.f, whiteColor, GameDrawOrder::overlays);
			if (int diff = level.goal.collectMatches.count[i] - mMatchStats.targetPieceCount[i]; diff > 0) {
				SDL_snprintf(tmp, sizeof(tmp), "%d", diff);
				mGoalCounters[i]->SetText(tmp);
				mGoalCounters[i]->SetVisible(true);
			}
			else {
				mGoalCounters[i]->SetVisible(false);
				// TODO
				// mGameRenderer.DrawIcon(checkIcon, pos + Vec2 { 0.f, 50.f }, 0.f, whiteColor, GameDrawOrder::overlays + 1);
			}
		}
	}
	else if (level.goal.id == GoalId::breakIce) {
		/*
		Vec2 pos = mGoalPanel->GetRect().pos + Vec2 { 70.f, 50.f };
		for (int i = 0; i < mMatchStats.layerCount; ++i) {
		    mGameRenderer.DrawIcon(iceSprites[0], pos, 0.f, whiteColor, GameDrawOrder::overlays);
		    pos.x += gameTextures[iceSprites[0]]->Width() + 12;
		}*/
	}
	for (int i = 0; i < MaxBoosterTypesPerLevel; ++i) {
		constexpr Color selectedColor = { 512.f, 512.f, 512.f, 255.f };
		if (level.boosterCount[i] > 0) {
			Vec2 coords = mBoosterButtons[i]->GetRect().pos + mBoosterButtons[i]->GetRect().size * 0.5f + Vec2 { 8.f, 0.f };

			BitmapExtParams prm;
			prm.pivot = BitmapPivot::center;
			prm.drawOrder = GameDrawOrder::overUI;
			prm.blending = true;
			prm.grayscale = (mBoosterCount[i] == 0) ? 100.f : 0.f;
			prm.color = (mSelectedBooster == i) ? selectedColor : whiteColor;
			mEngine.GetBitmapRenderer().DrawBitmapEx(*gameTextures[boosterIcons[level.boosterIds[i]]], coords, prm);

			snprintf(tmp, sizeof(tmp), "%d", mBoosterCount[i]);
			mBoosterButtons[i]->SetLabel(tmp);
			mBoosterButtons[i]->SetVisible(true);
		}
		else {
			mBoosterButtons[i]->SetVisible(false);
		}
	}

	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void PlayScreen::SetupNewBoardAnimation() {
	mActionMgr.Clear();
	mBlockingActionCounter = 0;

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
