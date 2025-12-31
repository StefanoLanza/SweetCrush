#include "Sdl.h"
#include <SDL3/SDL.h>
#include <stdexcept>

namespace Wind {

Sdl::Sdl(unsigned int flags, bool createDefaultDepthBuffer) {
	SDL_LogInfo(0, "Initializing SDL");
	if (! SDL_Init(flags)) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init SDL");
	}
#if defined(__ANDROID__)
	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);*/
#else
	if (int res = SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); res < 0) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init GL");
	}
	// OpenGL context 3.2 is required for debugging with RenderDoc
	// OpenGL 3.3 is required for glVertexAttribDivisor, used for instancing
	if (int res = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); res < 0) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init GL");
	}
	if (int res = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3); res < 0) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init GL");
	}
	// OpenGL core profile - deprecated functions are disabled
	if (int res = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); res < 0) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init SDL");
	}
	if (! createDefaultDepthBuffer) {
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	}
#endif
#ifdef _DEBUG
	// Debug OpenGL context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
}

Sdl::~Sdl() {
	SDL_LogInfo(0, "Shutting down SDL");
	SDL_Quit();
}

} // namespace Wind
