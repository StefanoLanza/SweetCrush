#pragma once

#include "Maths.h"
#include <SDL3/SDL.h>

namespace Wind {

class SdlWindow;
using KeyCode = unsigned int;

enum class MouseButton {
	left,
	middle,
	right,
};

class Input final {
public:
	Input();

	bool GetKeyJustPressed(KeyCode key) const;
	bool GetKeyDown(KeyCode key) const;
	Vec2 GetMouseCoord() const;
	void SetMappedMouseCoord(Vec2 mouseCoord);
	Vec2 GetMappedMouseCoord() const;
	bool GetMouseButtonDown(MouseButton butto) const;
	bool GetMouseButtonPressed(MouseButton button) const;
	bool GetMouseButtonReleased(MouseButton button) const;
	bool GetFingerDown() const;
	bool GetFingerPressed() const;
	bool GetFingerReleased() const;
	void BeginFrame();
	void ParseEvent(const SDL_Event& event, const SdlWindow& window);

private:
	Vec2    mMouseCoord;
	Vec2    mMappedMouseCoord;
	bool    mMouseButtonDown[3];
	bool    mMouseButtonPressed[3];
	bool    mMouseButtonReleased[3];
	bool    mFingerDown;
	bool    mFingerPressed;
	bool    mFingerReleased;
	bool    mKeyDown[256];
	KeyCode mKeyPressed[256];
	int     mNumKeyPressed;
};

} // namespace Wind
