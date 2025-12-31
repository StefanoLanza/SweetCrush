#pragma once

#include "Config.h"
#include "FwdDecl.h"
#include <functional>

#include <memory>
#include <string_view>

namespace Wind {

using RenderCallback = std::function<void(float dt)>;
using UpdateCallback = std::function<void(float dt)>;

enum class DisplayOrientation {
	portrait,
	landscape
};

class Engine final {
public:
	explicit Engine(SdlWindow& window);
	~Engine();

	const SdlWindow&   GetWindow() const;
	DisplayOrientation GetDisplayOrientation() const;

	void Start(const RenderCallback& renderCbk, const UpdateCallback& updateCbk);
	void Quit();

	Input&          GetInput() const;
	Graphics&       GetGraphics() const;
	const Blitter&  GetBlitter() const;
	BitmapRenderer& GetBitmapRenderer() const;
	TextRenderer&   GetTextRenderer() const;
	Audio&          GetAudio() const;
	TexturePtr      LoadTexture(std::string_view fileName, bool generateMips);

private:
	struct Implementation;
	std::unique_ptr<Implementation> mPimpl;
};

} // namespace Wind
