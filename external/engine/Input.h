#pragma once

#include <SDL/include/SDL.h>
#include <engine/Math.h>

namespace Wind {

class SdlWindow;
using KeyCode = unsigned int;

class Input {
public:
	Input();

	bool  GetKeyPressed(KeyCode key) const;
	bool  GetKeyDown(KeyCode key) const;
	Vec2  GetMouseCoord() const;
	void  SetMappedMouseCoord(Vec2 mouseCoord);
	Vec2  GetMappedMouseCoord() const;
	bool  GetMouseButtonDown() const;
	bool  GetMouseButtonPressed() const;
	bool  GetFingerDown() const;
	bool  GetFingerPressed() const;
	void  BeginFrame();
	void  ParseEvent(const SDL_Event& event, const SdlWindow& window);

private:
	Vec2    mMouseCoord;
	Vec2    mMappedMouseCoord;
	bool    mMouseButtonDown;
	bool    mMouseButtonPressed;
	bool    mFingerDown;
	bool    mFingerPressed;
	bool    mKeyDown[256];
	KeyCode mKeyPressed[256];
	int     mNumKeyPressed;
};

} // namespace Wind
