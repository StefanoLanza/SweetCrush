#pragma once

#include "Color.h"
#include "Config.h"
#include "FwdDecl.h"
#include <functional>
#include <memory>

namespace Wind {

using RenderCallback = std::function<void(float dt)>;
using UpdateCallback = std::function<void(float dt)>;

class Engine {
public:
	Engine(const char* windowTitle, int windowWidth, int windowHeight);
	~Engine();

	const SdlWindow& GetWindow() const;
	int              GetWindowWidth() const;
	int              GetWindowHeight() const;

	void Start(const RenderCallback& renderCbk, const UpdateCallback& updateCbk);
	void Quit();

	Input&          GetInput();
	Graphics&       GetGraphics() const;
	const Blitter&  GetBlitter() const;
	BitmapRenderer& GetBitmapRenderer() const;
	TextRenderer&   GetTextRenderer() const;
	ActionMgr&      GetTickActionMgr() const;
	Audio&          GetAudio() const;

	BitmapPtr LoadBitmap(const char* fileName);

private:
	struct Implementation;
	std::unique_ptr<Implementation> mPimpl;
};

} // namespace Wind
