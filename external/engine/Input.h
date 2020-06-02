#pragma once

#include <SDL2/include/SDL.h>
#include <engine/Math.h>

namespace Wind {

class SdlWindow;
using KeyCode = unsigned int;

class Input {
public:
	Input();

	void  SetCoordinatesScale(float x, float y);
	bool  GetKeyPressed(KeyCode key) const;
	bool  GetKeyDown(KeyCode key) const;
	float GetMouseX() const;
	float GetMouseY() const;
	Vec2  GetMouseCoord() const;
	bool  GetMouseButtonDown() const;
	bool  GetMouseButtonPressed() const;
	bool  GetFingerDown() const;
	bool  GetFingerPressed() const;
	void  BeginFrame();
	void  ParseEvent(const SDL_Event& event, const SdlWindow& window);

private:
	float   mScaleX;
	float   mScaleY;
	float   mMouseX;
	float   mMouseY;
	bool    mMouseButtonDown;
	bool    mMouseButtonPressed;
	bool    mFingerDown;
	bool    mFingerPressed;
	bool    mKeyDown[256];
	KeyCode mKeyPressed[256];
	int     mNumKeyPressed;
};

} // namespace Wind
