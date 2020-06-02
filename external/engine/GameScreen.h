#pragma once

namespace Wind {

using GameScreenId = unsigned;

class UICanvas;

class GameScreen {
public:
	virtual ~GameScreen() = default;
	virtual GameScreenId Tick(float dt) = 0;
	virtual void         LoadAssets() = 0;
	virtual void         BuildUI(UICanvas& canvas) = 0;
	virtual void         Draw(GameScreenId topScreen) const = 0;
	virtual void         Enter(GameScreenId prevScreen) = 0;
	virtual void         Exit() = 0;
};

} // namespace Wind
