#pragma once

namespace Wind {
class Sdl {
public:
	Sdl(int flags);
	~Sdl();
	Sdl(const Sdl&) = delete;
	// TODO uncopyable
};
} // namespace Wind
