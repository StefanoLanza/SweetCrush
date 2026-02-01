#pragma once

#include "Id.h"

#include <cstring>
#include <type_traits>

namespace Wind {

class Screen;
using ScreenId = Id<Screen>;

enum class ScreenOp {
	keep,
	goTo,
	back,
};

enum class ScreenTransition {
	none,
	fadeIn,
	fadeOut,
	slideIn,
	slideOut,
	pixelate,
	dissolve,
};

struct ScreenEvent {
	ScreenOp         mOp;
	ScreenId         mNext { 0 }; // replace, push
	char             mParams[16] {};
	ScreenTransition mTransition = ScreenTransition::none;
};

struct ScreenNavArgs {
	ScreenId mPrev { 0 };
	char     mParams[16] {};
};

class UICanvas;
class UIRenderer;
class Input;
class Engine;

class Screen {
public:
	virtual ~Screen() = default;
	virtual const char* GetName() const = 0;
	virtual ScreenEvent Tick(float dt, const Input& input) = 0;
	virtual void        LoadAssets(Wind::Engine& engine) = 0;
	virtual void        Draw(UIRenderer& uiRenderer, float dt) = 0;
	virtual void        Enter(const ScreenNavArgs& args) = 0;
	virtual void        Exit() = 0;
	virtual void        ParseConfig(const char* varName, const char* varValue) = 0;

	// Events
	static ScreenEvent Continue();
	static ScreenEvent GoTo(ScreenId next, ScreenTransition transition = ScreenTransition::none);
	template <class T>
	static ScreenEvent GoTo(ScreenId next, T param, ScreenTransition transition = ScreenTransition::none);
	static ScreenEvent GoBack(ScreenTransition transition = ScreenTransition::none);
	template <class T>
	inline ScreenEvent GoBack(T param, ScreenTransition transition = ScreenTransition::none);
};

inline ScreenEvent Screen::Continue() {
	ScreenEvent ev;
	ev.mOp = ScreenOp::keep;
	return ev;
}

inline ScreenEvent Screen::GoTo(ScreenId next, ScreenTransition transition) {
	ScreenEvent ev;
	ev.mOp = ScreenOp::goTo;
	ev.mNext = next;
	ev.mTransition = transition;
	return ev;
}

template <class T>
inline ScreenEvent Screen::GoTo(ScreenId next, T param, ScreenTransition transition) {
	static_assert(std::is_trivially_copyable_v<T>);
	ScreenEvent ev;
	ev.mOp = ScreenOp::goTo;
	static_assert(sizeof param >= sizeof ev.mParams);
	std::memcpy(ev.mParams, &param, sizeof param);
	ev.mTransition = transition;
	return ev;
}

inline ScreenEvent Screen::GoBack(ScreenTransition transition) {
	ScreenEvent ev;
	ev.mOp = ScreenOp::back;
	ev.mTransition = transition;
	return ev;
}

template <class T>
inline ScreenEvent Screen::GoBack(T param, ScreenTransition transition) {
	static_assert(std::is_trivially_copyable_v<T>);
	ScreenEvent ev;
	ev.mOp = ScreenOp::back;
	std::memcpy(ev.mParams, &param, sizeof param);
	ev.mTransition = transition;
	return ev;
}

} // namespace Wind
