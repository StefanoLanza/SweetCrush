#pragma once

#include "Board.h"
#include "BoostInfoPanel.h"
#include "Match3.h"
#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>

class TileSelector;
struct TileSelectionEvent;
struct GameSettings;
struct MatchStats;

class PlayScreen final : public Wind::GameScreen {
public:
	PlayScreen(Wind::Engine& engine, const GameConfig& gameConfig, const GameSettings& gameSettings, Wind::ActionMgr& renderActionMgr,
	           MatchStats& matchStats);
	~PlayScreen();

	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen) override;
	void               Exit() override;

private:
	void NewGame();
	void NextLevel();
	void ReplayLevel();
	void StartLevel();
	void SetupNewBoardAnimation();
	void OnCellSelectionEvent(const TileSelectionEvent& event);
	void OnTileRemoved(const Cell& cell);
	void OnMatch3Event(const Match3Event& event);
	void CheckLevelCompletion();
	void DrawBoard(const Wind::BitmapRenderer& bitmapRender) const;
	void DrawUI() const;
	int  IncreaseScore(const Match& match);
	void PlayMusic() const;
	void StopMusic() const;
	void PauseMusic() const;
	void ResumeMusic() const;
	void PlaySound(int idx) const;

private:
	Wind::Engine&                 mEngine;
	const GameConfig&             mGameConfig;
	const GameSettings&           mGameSettings;
	Wind::ActionMgr&              mRenderActionMgr;
	MatchStats&                   mMatchStats;
	Board                         mBoard;
	std::unique_ptr<TileSelector> mTileSelector;
	BoostInfoPanel                mBoostInfoPanel;
	Wind::UIPanel                 mPanel;
	Wind::UIButton                mPauseButton;
	Match3                        mMatch3;
	int                           mBoardFillCounter;
	int                           mAnimCounter;
	float                         mTime;
	Wind::BitmapPtr               mSelection;
	Wind::FontPtr                 mFonts[3];
	Wind::MusicPtr                mMusic;
	Wind::SoundPtr                mSounds[2];
};
