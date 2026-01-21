#pragma once

#include "ActionMgr.h"
#include "Board.h"
#include "BoardGenerator.h"
#include "EffectInfoPanel.h"
#include "Level.h"
#include "Match3.h"
#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>

class TileSelector;
struct TileSelectionEvent;
struct GameSettings;
struct MatchStats;
class GameDataModule;
class GameRenderer;

class PlayScreen final : public Wind::GameScreen {
public:
	PlayScreen(Wind::Engine& engine, const GameRenderer& gameRenderer, const AppConfig& gameConfig, const GameSettings& gameSettings,
	           ActionMgr& renderActionMgr, MatchStats& matchStats, const GameDataModule& gameDataModule);
	~PlayScreen();

	const char*        GetName() const override;
	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen, const void* payload) override;
	void               Exit() override;
	void               ParseConfig(const char* varName, const char* varValue) override;

private:
	void SelectBooster(const Wind::Input& input);
	void NewGame();
	void NextLevel();
	void ReplayLevel();
	void StartLevel();
	void SetupNewBoardAnimation();
	void OnTileSelectionEvent(const TileSelectionEvent& event);
	void OnPieceRemoved(const Cell& cell);
	void OnMatch3Event(const Match3Event& event);
	void CheckLevelCompletion();
	void DrawUI() const;
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
	ActionMgr                     mActionMgr;
	ActionMgr&                    mRenderActionMgr;
	MatchStats&                   mMatchStats;
	const GameDataModule&         mGameDataModule;
	Board                         mBoard;
	BoardGenerator                mBoardGenerator;
	std::unique_ptr<TileSelector> mCellSelector;
	EffectInfoPanel               mEffectInfoPanel;
	Wind::UIPanel                 mPanel;
	Wind::UIButton                mPauseButton;
	Wind::UIPanel                 mBoostersPanel;
	Wind::UIButton                mBoosterButtons[3];
	Match3                        mMatch3;
	std::vector<CellVisual>       mCellGraphics;
	float                         mMatchTime;
	float                         mTime;
	Wind::FontPtr                 mFonts[3];
	Wind::MusicPtr                mMusic;
	Wind::SoundPtr                mSounds[2];
	int                           mBoosterCount[MaxBoosterTypesPerLevel] {};
	int                           mSelectedBooster = -1;
	Wind::Vec2                    mSelectedBoosterCoord {};
	bool                          mGameComplete = false;
	bool                          mLevelComplete = false;
};
