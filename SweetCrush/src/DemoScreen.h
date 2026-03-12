#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

class GameRenderer;

class DemoScreen final : public Wind::Screen {
public:
	explicit DemoScreen(Wind::Engine& engine, const GameRenderer& gameRenderer);

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	void AnimateUI();

private:
	Wind::Engine&       mEngine;
	const GameRenderer& mGameRenderer;
	Wind::UICanvas      mCanvas;
	float               mAccumTime;
};
