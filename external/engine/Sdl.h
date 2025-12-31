#pragma once

namespace Wind {

class Sdl final {
public:
	explicit Sdl(unsigned int flags, bool createDefaultDepthBuffer);
	~Sdl();
	Sdl(const Sdl&) = delete;
	// TODO uncopyable
};

} // namespace Wind
