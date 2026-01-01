#include "Input.h"
#include "SdlWindow.h"

#include <iterator> // std::size
#include <cassert>

namespace Wind {

Input::Input()
    : mMouseCoord { 0.f, 0.f }
    , mMappedMouseCoord { 0.f, 0.f }
    , mMouseButtonDown(false)
    , mMouseButtonPressed(false)
    , mFingerDown(false)
    , mFingerPressed(false)
    , mKeyDown { false }
    , mKeyPressed {}
    , mNumKeyPressed { 0 } {
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

Vec2 Input::GetMouseCoord() const {
	return mMouseCoord;
}

void Input::SetMappedMouseCoord(Vec2 mouseCoord) {
	mMappedMouseCoord = mouseCoord;
}

Vec2 Input::GetMappedMouseCoord() const {
	return mMappedMouseCoord;
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
		break;
	case SDL_EVENT_FINGER_UP:
		mFingerDown = false;
		break;
	case SDL_EVENT_FINGER_MOTION:
		mMouseCoord.x = event.tfinger.x * static_cast<float>(window.GetWidth());
		mMouseCoord.y = event.tfinger.y * static_cast<float>(window.GetHeight());
		mMappedMouseCoord = mMouseCoord;
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		mMouseButtonPressed = true;
		mMouseButtonDown = true;
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		mMouseButtonDown = false;
		mMouseButtonPressed = false;
		break;
	case SDL_EVENT_MOUSE_MOTION:
		mMouseCoord.x = static_cast<float>(event.motion.x);
		mMouseCoord.y = static_cast<float>(event.motion.y);
		mMappedMouseCoord = mMouseCoord;
		break;
	case SDL_EVENT_WINDOW_MOUSE_LEAVE:
		mMouseCoord = { - 1000.f, -1000.f };
		break;
	default:
		break;
	}

#elif  SDL_MAJOR_VERSION == 2
	// TODO
#endif
}

} // namespace Wind
