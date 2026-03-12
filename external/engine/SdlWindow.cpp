#include "SdlWindow.h"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <string>
#include <cassert>

namespace Wind {

// Width and height are ignored on Android
SdlWindow::SdlWindow(const char* title, int width, int height, const char* iconFile, bool fullscreen)
    : mSDLWindow(
          SDL_CreateWindow(title, width, height,
                           SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_RESIZABLE),
          SDL_DestroyWindow) {
	if (! mSDLWindow) {
		SDL_LogError(0, "Failed to create SDL window. Error: %s", SDL_GetError());
		throw std::runtime_error(std::string("Error creating window: ") + SDL_GetError());
	}
	SDL_GetWindowSize(mSDLWindow.get(), &mWidth, &mHeight);

	if (iconFile) {
		SetIcon(iconFile);
	}
}

void SdlWindow::SetIcon(const char* iconFile) {
	assert(iconFile);
	SDL_Surface* icon = SDL_LoadPNG(iconFile);
	if (icon) {
		SDL_SetWindowIcon(mSDLWindow.get(), icon);
		SDL_DestroySurface(icon);
	}
	else {
		SDL_Log("Failed to load icon: %s", SDL_GetError());
	}
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

void SdlWindow::OnResize(int newWidth, int newHeight) {
	mWidth = newWidth;
	mHeight = newHeight;
}

SdlWindow::operator SDL_Window*() const {
	return mSDLWindow.get();
}

} // namespace Wind