#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

struct MatchStats;
class GameDataModule;
class GameRenderer;
struct Level;

class LevelStartScreen final : public Wind::Screen {
public:
	explicit LevelStartScreen(const MatchStats& matchStats, const GameDataModule& gameDataModule, const GameRenderer& gameRenderer);

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	void DrawPieces(const Level& level) const;
	void DrawIceBlocks(const Level& level) const;
	void DrawStars(const Level& level) const;
	void DrawIcons(const uint32_t icons[], int count) const;
	void AnimateUI();

private:
	const MatchStats&     mMatchStats;
	const GameDataModule& mGameDataModule;
	const GameRenderer&   mGameRenderer;
	Wind::UICanvas        mCanvas;
	Wind::UIText*         mTitle = nullptr;
	Wind::UIPanel*        mPanel = nullptr;
	Wind::UIButton*       mPlayButton = nullptr;
	Wind::UIText*         mGoalDesc = nullptr;
	float                 mAccumTime = 0;
};
