#pragma once

#include "Id.h"

#include <cstring>

namespace Wind {

class Screen;
using ScreenId = Id<Screen>;

enum class ScreenOp {
	keep,
	goTo,
	push,
	pop,
	back,
};

struct ScreenEvent {
	ScreenOp mOp;
	ScreenId mNext { 0 }; // replace, push
	char     mParams[16] {};
};

struct ScreenNavArgs {
	ScreenId mPrev { 0 };
	char     mParams[16] {};
};

inline ScreenEvent Continue() {
	ScreenEvent ev;
	ev.mOp = ScreenOp::keep;
	return ev;
}

inline ScreenEvent GoTo(ScreenId next) {
	ScreenEvent ev;
	ev.mOp = ScreenOp::goTo;
	ev.mNext = next;
	return ev;
}

template <class T>
inline ScreenEvent GoTo(ScreenId next, T param) {
	ScreenEvent ev;
	ev.mOp = ScreenOp::goTo;
	std::memcpy(ev.mParams, &param, sizeof param);
	return ev;
}

template <class T>
inline ScreenEvent GoBack(T param) {
	ScreenEvent ev;
	ev.mOp = ScreenOp::back;
	std::memcpy(ev.mParams, &param, sizeof param);
	return ev;
}

template <class T>
inline ScreenEvent Pop(T param) {
	ScreenEvent ev;
	ev.mOp = ScreenOp::pop;
	std::memcpy(ev.mParams, &param, sizeof param);
	return ev;
}

class UICanvas;
class UIRenderer;
class Input;
class Engine;

class Screen {
public:
	virtual ~Screen() = default;
	virtual const char*      GetName() const = 0;
	virtual ScreenEvent Tick(float dt, const Input& input) = 0;
	virtual void             LoadAssets(Wind::Engine& engine) = 0;
	virtual void             Draw(UIRenderer& uiRenderer) = 0;
	virtual void             Enter(ScreenId prevScreen, const void* payload) = 0; // TODO ScreenNavArgs
	virtual void             Exit() = 0;
	virtual void             ParseConfig(const char* varName, const char* varValue) = 0;
};

} // namespace Wind
