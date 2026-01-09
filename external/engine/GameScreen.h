#pragma once

namespace Wind {

using GameScreenId = unsigned;

class UICanvas;

class GameScreen {
public:
	virtual ~GameScreen() = default;
	virtual const char*  GetName() const = 0;
	virtual GameScreenId Tick(float dt, const Input& input) = 0;
	virtual void         LoadAssets() = 0;
	virtual void         BuildUI(UICanvas& canvas) = 0;
	virtual void         Draw(GameScreenId topScreen) const = 0;
	virtual void         Enter(GameScreenId prevScreen) = 0;
	virtual void         Exit() = 0;
	virtual void         ParseConfig(const char* varName, const char* varValue) = 0;
};

} // namespace Wind
