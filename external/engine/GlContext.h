#pragma once

#include <memory>

#include <SDL2/SDL.h>

namespace Wind {
class SdlWindow;

class GlContext {
public:
	GlContext(SdlWindow& sdlWindow);
	operator SDL_GLContext() const;

private:
	std::unique_ptr<void, void (*)(SDL_GLContext)> mContext;
};
} // namespace Wind