#include "Input.h"
#include "SdlWindow.h"
#include <cassert>

namespace Wind {

Input::Input()
    : mScaleX(1.f)
    , mScaleY(1.f)
    , mMouseX(0)
    , mMouseY(0)
    , mMouseButtonDown(false)
    , mMouseButtonPressed(false)
    , mFingerDown(false)
    , mFingerPressed(false)
    , mKeyDown { false }
    , mKeyPressed {}
    , mNumKeyPressed { 0 } {
}

void Input::SetCoordinatesScale(float x, float y) {
	mScaleX = x;
	mScaleY = y;
}

bool Input::GetKeyPressed(KeyCode key) const {
	for (int i = 0; i < mNumKeyPressed; ++i) {
		if (mKeyPressed[i] == key) {
			return true;
		}
	}
	return false;
}

bool Input::GetKeyDown(KeyCode key) const {
	assert(key < 256);
	return mKeyDown[key];
}

float Input::GetMouseX() const {
	return mMouseX * mScaleX;
}

float Input::GetMouseY() const {
	return mMouseY * mScaleY;
}

Vec2 Input::GetMouseCoord() const {
	return { GetMouseX(), GetMouseY() };
}

bool Input::GetMouseButtonDown() const {
	return mMouseButtonDown;
}

bool Input::GetMouseButtonPressed() const {
	return mMouseButtonPressed;
}

bool Input::GetFingerDown() const {
	return mFingerDown;
}

bool Input::GetFingerPressed() const {
	return mFingerPressed;
}

void Input::BeginFrame() {
	mMouseButtonPressed = false;
	mFingerPressed = false;
	mNumKeyPressed = 0;
}

void Input::ParseEvent(const SDL_Event& event, const SdlWindow& window) {
	SDL_Keycode sym = 0;
	switch (event.type) {
	case SDL_KEYDOWN:
		sym = event.key.keysym.sym;
		assert(sym >= 0);
		if (sym >= 0 && sym < 256) {
			mKeyDown[sym] = true;
		}
		if (mNumKeyPressed < (int)std::size(mKeyPressed)) {
			mKeyPressed[mNumKeyPressed++] = sym;
		}
		break;
	case SDL_KEYUP:
		sym = event.key.keysym.sym;
		assert(sym >= 0);
		if (sym >= 0 && sym < 256) {
			mKeyDown[sym] = false;
		}
		break;
	case SDL_FINGERDOWN:
		mFingerDown = true;
		break;
	case SDL_FINGERUP:
		mFingerDown = false;
		break;
	case SDL_FINGERMOTION:
		mMouseX = event.tfinger.x * static_cast<float>(window.GetWidth());
		mMouseY = event.tfinger.y * static_cast<float>(window.GetHeight());
		break;
	case SDL_MOUSEBUTTONDOWN:
		mMouseButtonPressed = true;
		mMouseButtonDown = true;
		break;
	case SDL_MOUSEBUTTONUP:
		mMouseButtonDown = false;
		mMouseButtonPressed = false;
		break;
	case SDL_MOUSEMOTION:
		mMouseX = static_cast<float>(event.motion.x);
		mMouseY = static_cast<float>(event.motion.y);
		break;
	case SDL_WINDOWEVENT:
		if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
			mMouseX = -1000.f;
			mMouseY = -1000.f;
		}
		break;
	default:
		break;
	}
}

} // namespace Wind
