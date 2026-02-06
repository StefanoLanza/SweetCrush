#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

struct MatchStats;
class GameDataModule;

class LevelCompleteScreen final : public Wind::Screen {
public:
	explicit LevelCompleteScreen(MatchStats& matchStats, const GameDataModule& gameDataModule);

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	MatchStats&           mMatchStats;
	const GameDataModule& mGameDataModule;
	Wind::UIText          mTitle;
	Wind::UIText          mSubTitle;
	Wind::UIButton        mNextLevelButton;
	Wind::UICanvas        mCanvas;
	float                 mAccumTime = 0.f;
};
