#include "Sdl.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <stdexcept>

namespace Wind {

Sdl::Sdl(unsigned int flags) {
	SDL_LogInfo(0, "Initializing SDL");
	if (!SDL_Init(flags)) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init SDL");
	}
#if defined(__ANDROID__)
	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);*/
#else
	if (int res = SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); res < 0) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init SDL");
	}	
	if (int res = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); res < 0) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init SDL");
	}	
	if (int res = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1); res < 0) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init SDL");
	}	
	// OpenGL core profile - deprecated functions are disabled
	if (int res = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); res < 0) {
		SDL_LogError(0, "%s", SDL_GetError());
		throw std::runtime_error("Failed to init SDL");
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
