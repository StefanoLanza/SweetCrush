#include "Input.h"
#include "SdlWindow.h"
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
		mMouseCoord.x = event.tfinger.x * static_cast<float>(window.GetWidth());
		mMouseCoord.y = event.tfinger.y * static_cast<float>(window.GetHeight());
		mMappedMouseCoord = mMouseCoord;
		break;
	case SDL_FINGERUP:
		mFingerDown = false;
		break;
	case SDL_FINGERMOTION:
		mMouseCoord.x = event.tfinger.x * static_cast<float>(window.GetWidth());
		mMouseCoord.y = event.tfinger.y * static_cast<float>(window.GetHeight());
		mMappedMouseCoord = mMouseCoord;
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
		mMouseCoord.x = static_cast<float>(event.motion.x);
		mMouseCoord.y = static_cast<float>(event.motion.y);
		mMappedMouseCoord = mMouseCoord;
		break;
	case SDL_WINDOWEVENT:
		if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
			mMouseCoord = { - 1000.f, -1000.f };
		}
		break;
	default:
		break;
	}
}

} // namespace Wind
