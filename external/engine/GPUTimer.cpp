#include "GPUTimer.h"
#include "Gl.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>

#include <iterator> // std::size

namespace Wind {

#if defined(__ANDROID__)
namespace {

PFNGLQUERYCOUNTEREXTPROC        pglQueryCounterEXT = nullptr;
PFNGLGETQUERYOBJECTUI64VEXTPROC pglGetQueryObjectui64vEXT = nullptr;

} // namespace
#endif

GPUTimer::GPUTimer()
    : mStartQueries {}
    , mEndQueries {}
    , mSubmitted { 0 }
    , mIsAvailable { true } {
	glGenQueries(std::size(mStartQueries), mStartQueries);
	glGenQueries(std::size(mEndQueries), mEndQueries);

#if defined(__ANDROID__)
	if (! pglQueryCounterEXT) {
		pglQueryCounterEXT = reinterpret_cast<PFNGLQUERYCOUNTEREXTPROC>(SDL_GL_GetProcAddress("glQueryCounterEXT"));
		pglGetQueryObjectui64vEXT = reinterpret_cast<PFNGLGETQUERYOBJECTUI64VEXTPROC>(SDL_GL_GetProcAddress("glGetQueryObjectui64vEXT"));
	}
	mIsAvailable = (pglGetQueryObjectui64vEXT != nullptr) && (pglQueryCounterEXT != nullptr);

	GLint disjointOccurred = 0;
	glGetIntegerv(GL_GPU_DISJOINT_EXT, &disjointOccurred);
	if (glGetError() != GL_NO_ERROR) {
		SDL_LogError(0, "Unsupported GL_GPU_DISJOINT_EXT. Disabling GPU timer");
		mIsAvailable = false;
	}
#endif
}

void GPUTimer::Start() {
#if defined(__ANDROID__)
	if (! mIsAvailable) {
		return;
	}

	pglQueryCounterEXT(mStartQueries[mSubmitted % bufferSize], GL_TIMESTAMP_EXT);
	if (glGetError() != GL_NO_ERROR) {
		SDL_LogError(0, "glQueryCounterEXT Error: %d. Disabling GPU timers", glGetError());
		mIsAvailable = false; // disable
	}
#else
	glQueryCounter(mStartQueries[mSubmitted % bufferSize], GL_TIMESTAMP);
	if (glGetError() != GL_NO_ERROR) {
		SDL_LogError(0, "glQueryCounter Error: %d. Disabling GPU timers", glGetError());
		mIsAvailable = false; // disable
	}
#endif
}

std::optional<double> GPUTimer::End() {
#if defined(__ANDROID__)
	if (! mIsAvailable) {
		return std::nullopt;
	}
	pglQueryCounterEXT(mEndQueries[mSubmitted % bufferSize], GL_TIMESTAMP_EXT);
	if (glGetError() != GL_NO_ERROR) {
		SDL_LogError(0, "glQueryCounterEXT Error: %d", glGetError());
	}
#else
	glQueryCounter(mEndQueries[mSubmitted % bufferSize], GL_TIMESTAMP);
	if (glGetError() != GL_NO_ERROR) {
		SDL_LogError(0, "glQueryCounter Error: %d", glGetError());
	}
#endif
	else {
		++mSubmitted;
	}
	if (mSubmitted < bufferSize) {
		return std::nullopt;
	}

	const unsigned pollFrame = mSubmitted % bufferSize; // oldest

	GLuint available = 0;
	int    iter = 0;
	while (! available && iter < 4) {
		glGetQueryObjectuiv(mEndQueries[pollFrame], GL_QUERY_RESULT_AVAILABLE, &available);
		++iter;
	}

	GLint disjointOccurred = 0;
#if defined(__ANDROID__)
	// On disjoint exception, clear query buffer
	glGetIntegerv(GL_GPU_DISJOINT_EXT, &disjointOccurred);
	if (disjointOccurred) {
		SDL_LogDebug(0, "GPU disjoint detected");
		mSubmitted = 0;
	}
	if (glGetError() != GL_NO_ERROR) {
		mIsAvailable = false;
		return std::nullopt;
	}
#endif

	if (available && (0 == disjointOccurred)) {
		GLuint64 start_ns = 0, end_ns = 0;
#if defined(__ANDROID__)
		pglGetQueryObjectui64vEXT(mStartQueries[pollFrame], GL_QUERY_RESULT, &start_ns);
		pglGetQueryObjectui64vEXT(mEndQueries[pollFrame], GL_QUERY_RESULT, &end_ns);
		if (glGetError() != GL_NO_ERROR) {
			SDL_LogError(0, "glGetQueryObjectui64vEXT Error: %d", glGetError());
			return std::nullopt;
		}
#else
		glGetQueryObjectui64v(mStartQueries[pollFrame], GL_QUERY_RESULT, &start_ns);
		glGetQueryObjectui64v(mEndQueries[pollFrame], GL_QUERY_RESULT, &end_ns);
		if (glGetError() != GL_NO_ERROR) {
			SDL_LogError(0, "glGetQueryObjectui64v Error: %d", glGetError());
			return std::nullopt;
		}
#endif
		return (double)(end_ns - start_ns) / 1e6;
	}
	return std::nullopt;
}

void GPUTimer::Reset() {
	mSubmitted = 0;
}

bool GPUTimer::IsAvailable() const {
	return mIsAvailable;
}

} // namespace Wind
