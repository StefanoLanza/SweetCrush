#pragma once

#include "Gl.h"

#include <optional>

namespace Wind {

class GPUTimer final {
public:
	GPUTimer();
	void                  Start();
	std::optional<double> End();
	void                  Reset();
	bool                  IsAvailable() const;

private:
	static constexpr int bufferSize = 4;
	GLuint               mStartQueries[bufferSize];
	GLuint               mEndQueries[bufferSize];
	unsigned int         mSubmitted;
	bool                 mIsAvailable;
};

} // namespace Wind