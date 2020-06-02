#pragma once

#include <memory>

struct SDL_Window;

namespace Wind {
	class SdlWindow {
	public:
		SdlWindow(const char* title, int width, int height);
		operator SDL_Window*() const;
		void Show();
		int GetWidth() const;
		int GetHeight() const;

	private:
		std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> mSDLWindow;
		int mWidth;
		int mHeight;
	};
}