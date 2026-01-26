#pragma once

#include <memory>

struct SDL_Window;

namespace Wind {

class SdlWindow final {
public:
	SdlWindow(const char* title, int width, int height, const char* iconFile, bool fullscreen);
	
	operator SDL_Window*() const;
	void Show();
	int  GetWidth() const;
	int  GetHeight() const;
	void OnResize(int newWidth, int newHeight);

private:
	std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> mSDLWindow;
	int                                                mWidth;
	int                                                mHeight;
};

} // namespace Wind