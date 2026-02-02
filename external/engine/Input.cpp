#include "Input.h"
#include "SdlWindow.h"

#include <cassert>
#include <iterator> // std::size

namespace Wind {

Input::Input()
    : mMouseCoord { -100.f, -100.f }
    , mMappedMouseCoord { -100.f, -100.f }
    , mMouseButtonDown(false)
    , mMouseButtonPressed(false)
    , mMouseButtonReleased(false)
    , mFingerDown(false)
    , mFingerPressed(false)
    , mFingerReleased(false)
    , mKeyDown { false }
    , mKeyPressed {}
    , mNumKeyPressed { 0 } {
}

bool Input::GetKeyJustPressed(KeyCode key) const {
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

Vec2 Input::GetMouseCoord() const {
	return mMouseCoord;
}

void Input::SetMappedMouseCoord(Vec2 mouseCoord) {
	mMappedMouseCoord = mouseCoord;
}

Vec2 Input::GetMappedMouseCoord() const {
	return mMappedMouseCoord;
}

bool Input::GetMouseButtonDown(MouseButton button) const {
	return mMouseButtonDown[(int)button];
}

bool Input::GetMouseButtonPressed(MouseButton button) const {
	return mMouseButtonPressed[(int)button];
}

bool Input::GetMouseButtonReleased(MouseButton button) const {
	return mMouseButtonReleased[(int)button];
}

bool Input::GetFingerDown() const {
	return mFingerDown;
}

bool Input::GetFingerPressed() const {
	return mFingerPressed;
}

bool Input::GetFingerReleased() const {
	return mFingerReleased;
}

void Input::BeginFrame() {
	for (int i = 0; i < 3; ++i) {
		mMouseButtonPressed[i] = false;
		mMouseButtonReleased[i] = false;
	}
	mFingerPressed = false;
	mFingerReleased = false;
	mNumKeyPressed = 0;
}

void Input::ParseEvent(const SDL_Event& event, const SdlWindow& window) {
	SDL_Keycode sym = 0;
#if SDL_MAJOR_VERSION == 3
	switch (event.type) {
	case SDL_EVENT_KEY_DOWN:
		sym = event.key.key;
		if (sym < 256) {
			mKeyDown[sym] = true;
		}
		if (mNumKeyPressed < (int)std::size(mKeyPressed)) {
			mKeyPressed[mNumKeyPressed++] = sym;
		}
		break;
	case SDL_EVENT_KEY_UP:
		sym = event.key.key;
		if (sym < 256) {
			mKeyDown[sym] = false;
		}
		break;
	case SDL_EVENT_FINGER_DOWN:
		mFingerDown = true;
		mMouseCoord.x = event.tfinger.x * static_cast<float>(window.GetWidth());
		mMouseCoord.y = event.tfinger.y * static_cast<float>(window.GetHeight());
		mMappedMouseCoord = mMouseCoord;
		mFingerPressed = true;
		break;
	case SDL_EVENT_FINGER_UP:
		mFingerDown = false;
		mFingerReleased = true;
		break;
	case SDL_EVENT_FINGER_MOTION:
		mMouseCoord.x = event.tfinger.x * static_cast<float>(window.GetWidth());
		mMouseCoord.y = event.tfinger.y * static_cast<float>(window.GetHeight());
		mMappedMouseCoord = mMouseCoord;
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		if (event.button.button >= 1 && event.button.button < 4) {
			mMouseButtonPressed[event.button.button - 1] = true;
			mMouseButtonDown[event.button.button - 1] = true;
			mMouseButtonReleased[event.button.button - 1] = false;
		}
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		if (event.button.button >= 1 && event.button.button < 4) {
			mMouseButtonDown[event.button.button - 1] = false;
			mMouseButtonPressed[event.button.button - 1] = false;
			mMouseButtonReleased[event.button.button - 1] = true;
		}
		break;
	case SDL_EVENT_MOUSE_MOTION:
		mMouseCoord.x = static_cast<float>(event.motion.x);
		mMouseCoord.y = static_cast<float>(event.motion.y);
		mMappedMouseCoord = mMouseCoord;
		break;
	case SDL_EVENT_WINDOW_MOUSE_LEAVE:
		mMouseCoord = { -1000.f, -1000.f };
		break;
	default:
		break;
	}

#elif SDL_MAJOR_VERSION == 2
	// TODO
#endif
}

} // namespace Wind
