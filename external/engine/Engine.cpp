#include "Engine.h"

#include "Audio.h"
#include "BitmapRender.h"
#include "Blitter.h"
#include "Color.h"
#include "Config.h"
#include "Gl.h"
#include "GlContext.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "Input.h"
#include "Texture.h"
#include "SdlWindow.h"
#include "TextRender.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace Wind {

namespace {

constexpr uint64_t MaxFrameTicks = 300;

} // namespace

struct Engine::Implementation {
	SdlWindow&              mWindow;
	GlContext               mGlContext;
	Input                   mInput;
	Graphics                mGraphics;
	Audio                   mAudio;
	BitmapRenderer          mBitmapRenderer;
	Blitter                 mBlitter;
	TextRenderer            mTextRenderer;
	std::vector<TexturePtr> mTextures;
	uint64_t                mElapsedTicks;
	float                   mAccumTime;
	bool                    mQuit;
	bool                    mAppInBackground;
	DisplayOrientation      mDisplayOrientation;

	explicit Implementation(SdlWindow& window)
	    : mWindow { window }
	    , mGlContext(mWindow)
	    , mGraphics(mWindow)
	    , mBitmapRenderer { mGraphics }
	    , mBlitter { mGraphics }
	    , mTextRenderer { mGraphics }
	    , mElapsedTicks(0)
	    , mAccumTime(0.f)
	    , mQuit(false)
	    , mAppInBackground(false)
	    , mDisplayOrientation { DisplayOrientation::portrait } {
		RegisterTexture("images/null.png"); // placeholder
	}

	void       Start(const RenderCallback& renderCkb, const UpdateCallback& updateCbk);
	TexturePtr RegisterTexture(std::string_view fileName);
	void       ParseEvent();
};

void Engine::Implementation::Start(const RenderCallback& renderCbk, const UpdateCallback& updateCbk) {
	constexpr float fixedTimeStep = 1.f / 60.f;

	mWindow.Show();
	mAccumTime = 0.f;
	updateCbk(fixedTimeStep); // first update
	mElapsedTicks = SDL_GetTicks();
	while (! mQuit) {
		const uint64_t currentTicks = SDL_GetTicks();
		uint64_t       lastFrameTicks = currentTicks - mElapsedTicks;
		mElapsedTicks = currentTicks;
		lastFrameTicks = std::min(lastFrameTicks, MaxFrameTicks);
		const float lastFrameSeconds = static_cast<float>(lastFrameTicks) * 0.001f;

		// Update app logic with a fixed time step
		mAccumTime += lastFrameSeconds;
		const int steps = static_cast<int>(std::floor(mAccumTime / fixedTimeStep));
		mAccumTime -= steps * fixedTimeStep;
		for (int i = 0; i < steps; ++i) {
			ParseEvent();
			updateCbk(fixedTimeStep);
		}

		if (! mAppInBackground) {
			mGraphics.BeginFrame();
			if (! mQuit) {
				renderCbk(lastFrameSeconds);
			}
			mGraphics.EndFrame();
			SDL_GL_SwapWindow(mWindow);
			SDL_HideCursor();
		}
	}
}

TexturePtr Engine::Implementation::RegisterTexture(std::string_view fileName) {
	try {
		for (auto& b : mTextures) {
			if (b->GetFileName() == fileName) {
				return b;
			}
		}
		char path[260];
		snprintf(path, sizeof(path), "%s%s", ASSETS_FOLDER, fileName.data());
		auto texture = std::make_unique<Texture>(fileName, path);
		mTextures.push_back(std::move(texture));
		return mTextures.back();
	}
	catch (const std::exception& e) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
		return mTextures.empty() ? nullptr : mTextures[0]; // placeholder
	}
}

void Engine::Implementation::ParseEvent() {
	mInput.BeginFrame();
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_EVENT_WINDOW_RESIZED:
			mWindow.OnResize(event.window.data1, event.window.data2);
			break;
		case SDL_EVENT_WILL_ENTER_BACKGROUND:
			mAppInBackground = true;
			break;
		case SDL_EVENT_WILL_ENTER_FOREGROUND:
			mAppInBackground = false;
			break;
		case SDL_EVENT_QUIT:
			mQuit = true;
			break;
		case SDL_EVENT_DISPLAY_ORIENTATION: {
			SDL_DisplayOrientation orientation = SDL_GetCurrentDisplayOrientation(event.display.displayID);
			switch (orientation) {
			case SDL_ORIENTATION_PORTRAIT:
			case SDL_ORIENTATION_PORTRAIT_FLIPPED:
				SDL_Log("Portrait mode");
				mDisplayOrientation = DisplayOrientation::portrait;
				break;

			case SDL_ORIENTATION_LANDSCAPE:
			case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
				SDL_Log("Landscape mode");
				mDisplayOrientation = DisplayOrientation::landscape;
				break;

			default:
				SDL_Log("Unknown orientation");
				break;
			}
		} break;
		default:
			mInput.ParseEvent(event, mWindow);
			break;
		}
	}
}

Engine::Engine(SdlWindow& window)
    : mPimpl(std::make_unique<Implementation>(window)) {
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(1);
	mPimpl->mGraphics.InitGL();
}

Engine::~Engine() = default;

const SdlWindow& Engine::GetWindow() const {
	return mPimpl->mWindow;
}

DisplayOrientation Engine::GetDisplayOrientation() const {
	return mPimpl->mDisplayOrientation;
}

void Engine::Quit() {
	mPimpl->mQuit = true;
}

Input& Engine::GetInput() const {
	return mPimpl->mInput;
}

TextRenderer& Engine::GetTextRenderer() const {
	return mPimpl->mTextRenderer;
}

Audio& Engine::GetAudio() const {
	return mPimpl->mAudio;
}
Graphics& Engine::GetGraphics() const {
	return mPimpl->mGraphics;
}

const Blitter& Engine::GetBlitter() const {
	return mPimpl->mBlitter;
}

BitmapRenderer& Engine::GetBitmapRenderer() const {
	return mPimpl->mBitmapRenderer;
}

void Engine::Start(const RenderCallback& renderCbk, const UpdateCallback& updateCbk) {
	mPimpl->Start(renderCbk, updateCbk);
}

TexturePtr Engine::LoadTexture(std::string_view fileName) {
	return mPimpl->RegisterTexture(fileName);
}

} // namespace Wind
