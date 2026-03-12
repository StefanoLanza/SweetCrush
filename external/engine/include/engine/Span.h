#pragma once

#include <cstddef>

namespace Wind {

// TODO Replace with std::span in C++ 20
template <class T>
class Span {
public:
	Span()
	    : mData(nullptr)
	    , mSize(0) {
	}
	Span(T* data, size_t size)
	    : mData(data)
	    , mSize(size) {
	}
	template <size_t N>
	Span(T (&data)[N])
	    : mData { data }
	    , mSize { N } {
	}
	T* Data() const {
		return mData;
	}
	size_t Size() const {
		return mSize;
	}
	T& operator[](size_t i) noexcept {
		return mData[i];
	}
	T const& operator[](size_t i) const noexcept {
		return mData[i];
	}
	T* begin() const noexcept {
		return mData;
	}
	T* end() const noexcept {
		return mData + mSize;
	}

	bool Empty() const {
		return mSize == 0;
	}

private:
	T*     mData;
	size_t mSize;
};
} // namespace Wind
