#pragma once

#include "Id.h"

namespace Wind {

class GameScreen;
using ScreenId = Id<GameScreen>;

enum class ScreenOp {
	keep,
	replace,
	push,
	pop,
};

struct ScreenTransition {
	ScreenOp mOp;
	ScreenId mNext { 0 }; // replace, push
	char     mPayload[16] {};
};

class UICanvas;
class UIRenderer;

class GameScreen {
public:
	virtual ~GameScreen() = default;
	virtual const char*      GetName() const = 0;
	virtual ScreenTransition Tick(float dt, const Input& input) = 0;
	virtual void             LoadAssets(Wind::Engine& engine) = 0;
	virtual void             BuildUI(UICanvas& canvas) = 0;
	virtual void             Draw(UIRenderer& uiRenderer) = 0;
	virtual void             Enter(ScreenId prevScreen, const void* payload) = 0;
	virtual void             Exit() = 0;
	virtual void             ParseConfig(const char* varName, const char* varValue) = 0;
};

} // namespace Wind
