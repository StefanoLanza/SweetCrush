#pragma once

#include <cstdint>

namespace Wind {

template <typename Tag, typename T = uint32_t>
class Id {
public:
	using value_type = T;

	constexpr explicit Id(T v)
	    : mValue(v) {
	}

	constexpr T Get() const {
		return mValue;
	}

	friend constexpr bool operator==(Id a, Id b) {
		return a.mValue == b.mValue;
	}

	friend constexpr bool operator!=(Id a, Id b) {
		return a.mValue != b.mValue;
	}

private:
	T mValue;
};

} // namespace Wind
