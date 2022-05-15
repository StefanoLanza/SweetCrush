#include "SdlWindow.h"

#include <SDL2/SDL.h>
#include <stdexcept>
#include <string>

namespace Wind {
SdlWindow::SdlWindow(const char* title, int width, int height)
    : mSDLWindow(SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                                  SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL),
                 SDL_DestroyWindow) {
	if (! mSDLWindow) {
		SDL_LogError(0, "Failed to create SDL window");
		throw std::runtime_error(std::string("Error creating window: ") + SDL_GetError());
	}
	SDL_GetWindowSize(mSDLWindow.get(), &mWidth, &mHeight);
}

void SdlWindow::Show() {
	SDL_ShowWindow(mSDLWindow.get());
}

int SdlWindow::GetWidth() const {
	return mWidth;
}

int SdlWindow::GetHeight() const {
	return mHeight;
}

SdlWindow::operator SDL_Window*() const {
	return mSDLWindow.get();
}
} // namespace Wind