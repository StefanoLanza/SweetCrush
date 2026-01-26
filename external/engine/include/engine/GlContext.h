#pragma once

#include <memory>

#include <SDL3/SDL.h>

namespace Wind {
class SdlWindow;

class GlContext final {
public:
	explicit GlContext(SdlWindow& sdlWindow);
	operator SDL_GLContext() const;

private:
	std::unique_ptr<SDL_GLContextState, bool (*)(SDL_GLContext)> mContext;
};
} // namespace Wind