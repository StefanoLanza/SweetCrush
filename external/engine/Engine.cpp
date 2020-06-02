#include "Engine.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "ActionMgr.h"
#include "Audio.h"
#include "BitmapRender.h"
#include "Blitter.h"
#include "Color.h"
#include "Gl.h"
#include "GlContext.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "Input.h"
#include "Sdl.h"
#include "SdlSurface.h"
#include "SdlWindow.h"
#include "TextRender.h"

namespace Wind {

namespace {

constexpr uint32_t MaxFrameTicks = 300;

} // namespace

struct Engine::Implementation {
	Sdl                    mSdl;
	SdlWindow              mWindow;
	GlContext              mGlContext;
	Input                  mInput;
	Graphics               mGraphics;
	Audio                  mAudio;
	BitmapRenderer         mBitmapRenderer;
	Blitter                mBlitter;
	TextRenderer           mTextRenderer;
	ActionMgr              mActionMgr;
	std::vector<BitmapPtr> mBitmaps;
	uint32_t               mElapsedTicks;
	float                  mAccumTime;
	bool                   mQuit;
	bool                   mBackground;

	Implementation(const char* title, int windowWidth, int windowHeight)
	    : mSdl { SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE }
	    , mWindow { title, windowWidth, windowHeight }
	    , mGlContext(mWindow)
	    , mGraphics(mWindow)
	    , mBitmapRenderer { mGraphics }
	    , mBlitter { mGraphics }
	    , mTextRenderer { mGraphics }
	    , mElapsedTicks(0)
	    , mAccumTime(0.f)
	    , mQuit(false)
	    , mBackground(false) {
		RegisterBitmap("null.png"); // placeholder
	}

	void      Start(const RenderCallback& renderCkb, const UpdateCallback& updateCbk);
	BitmapPtr RegisterBitmap(const char* fileName);
	void      ParseEvent();
};

void Engine::Implementation::Start(const RenderCallback& renderCbk, const UpdateCallback& updateCbk) {
	constexpr float fixedTimeStep = 1.f / 60.f;

	mWindow.Show();
	mAccumTime = 0.f;
	updateCbk(fixedTimeStep); // first update
	mElapsedTicks = SDL_GetTicks();
	while (! mQuit) {
		const uint32_t currentTicks = SDL_GetTicks();
		uint32_t       lastFrameTicks = currentTicks - mElapsedTicks;
		mElapsedTicks = currentTicks;
		lastFrameTicks = std::min(lastFrameTicks, MaxFrameTicks);
		const float lastFrameSeconds = static_cast<float>(lastFrameTicks) * 0.001f;

		// Update game logic with a fixed time step
		mAccumTime += lastFrameSeconds;
		const int steps = static_cast<int>(std::floor(mAccumTime / fixedTimeStep));
		mAccumTime -= steps * fixedTimeStep;
		for (int i = 0; i < steps; ++i) {
			ParseEvent();
			mActionMgr.RunActions(fixedTimeStep);
			updateCbk(fixedTimeStep);
		}

		if (! mBackground) {
			SDL_GL_SwapWindow(mWindow);
			SDL_ShowCursor(SDL_DISABLE);
			renderCbk(lastFrameSeconds);
			mGraphics.Flush();
		}
	}
}

BitmapPtr Engine::Implementation::RegisterBitmap(const char* fileName) {
	assert(fileName);
	try {
		for (auto& b : mBitmaps) {
			if (b->GetFileName() == fileName) {
				return b;
			}
		}
		char path[260];
		snprintf(path, sizeof(path), "%s%s", ASSETS_FOLDER, fileName);
		auto bitmap = std::make_unique<SdlSurface>(fileName, path);
		mBitmaps.push_back(std::move(bitmap));
		return mBitmaps.back();
	}
	catch (const std::exception& e) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
		return mBitmaps[0]; // placeholder
	}
}

void Engine::Implementation::ParseEvent() {
	mInput.BeginFrame();
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		mInput.ParseEvent(event, mWindow);
		switch (event.type) {
			case SDL_APP_WILLENTERBACKGROUND:
				mBackground = true;
				break;
			case SDL_APP_WILLENTERFOREGROUND:
				mBackground = false;
				break;
			case SDL_QUIT:
				mQuit = true;
				break;
			default:
				break;
		}
	}
}

Engine::Engine(const char* windowTitle, int windowWidth, int windowHeight)
    : mPimpl(std::make_unique<Implementation>(windowTitle, windowWidth, windowHeight)) {
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(1);
	mPimpl->mGraphics.InitGL();
}

Engine::~Engine() = default;

const SdlWindow& Engine::GetWindow() const {
	return mPimpl->mWindow;
}

int Engine::GetWindowWidth() const {
	return mPimpl->mWindow.GetWidth();
}

int Engine::GetWindowHeight() const {
	return mPimpl->mWindow.GetHeight();
}

void Engine::Quit() {
	mPimpl->mQuit = true;
}

Input& Engine::GetInput() {
	return mPimpl->mInput;
}

TextRenderer& Engine::GetTextRenderer() const {
	return mPimpl->mTextRenderer;
}

ActionMgr& Engine::GetTickActionMgr() const {
	return mPimpl->mActionMgr;
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

BitmapPtr Engine::LoadBitmap(const char* fileName) {
	return mPimpl->RegisterBitmap(fileName);
}

} // namespace Wind
