#pragma once

#include "Board.h"
#include "BoardGenerator.h"
#include "Level.h"
#include "Match3.h"

#include <engine/ActionMgr.h>
#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

class TileSelector;
struct TileSelectionEvent;
struct GameSettings;
struct MatchStats;
class GameDataModule;
class GameRenderer;

class PlayScreen final : public Wind::Screen {
public:
	PlayScreen(Wind::Engine& engine, const GameRenderer& gameRenderer, const AppConfig& appConfig, const GameSettings& gameSettings,
	           MatchStats& matchStats, const GameDataModule& gameDataModule);
	~PlayScreen();

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	void SelectBooster(const Wind::Input& input);
	void NewGame();
	void SetupNewBoardAnimation();
	void OnTileSelectionEvent(const TileSelectionEvent& event);
	void OnPieceRemoved(const Cell& cell);
	void OnMatch3Event(const Match3Event& event);
	void CheckLevelCompletion();
	void DrawUI(Wind::UIRenderer& uiRenderer);
	int  IncreaseScore(const MatchEvent& match);
	void PlayMusic() const;
	void StopMusic() const;
	void PauseMusic() const;
	void ResumeMusic() const;
	void PlaySound(int idx) const;

private:
	Wind::Engine&                 mEngine;
	const GameRenderer&           mGameRenderer;
	const AppConfig&              mGameConfig;
	const GameSettings&           mGameSettings;
	Wind::ActionMgr               mActionMgr;
	MatchStats&                   mMatchStats;
	const GameDataModule&         mGameDataModule;
	Board                         mBoard;
	BoardGenerator                mBoardGenerator;
	std::unique_ptr<TileSelector> mCellSelector;
	Wind::UICanvas                mCanvas;
	Wind::UIButton*               mPauseButton;
	Wind::UIBitmap*               mGoalIcons[3];
	Wind::UIText*                 mGoalCounters[3];
	Wind::UIText*                 mScoreText;
	Wind::UIText*                 mTimeText;
	Wind::UIButton*               mBoosterButtons[3];
	Match3                        mMatch3;
	std::vector<CellVisual>       mCellGraphics;
	float                         mMatchTime;
	float                         mTime;
	Wind::FontPtr                 mFonts[3];
	Wind::MusicPtr                mMusic;
	Wind::SoundPtr                mSounds[2];
	int                           mBoosterCount[MaxBoosterTypesPerLevel] {};
	int                           mSelectedBooster = -1;
	bool                          mGameComplete = false;
	bool                          mLevelComplete = false;
	int                           mBlockingActionCounter = 0;
};
