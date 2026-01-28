#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace Wind {

template <typename Tag, typename T = uint32_t>
class Id {
public:
	using Type = T;

	Id()
	    : mValue(0) {
	}

	// Implicitly convert a client-specific Id to this Id
	// E.g. GameScreenId to ScreenId
	template <typename E>
	requires std::is_enum_v<E> &&
	         std::is_convertible_v<std::underlying_type_t<E>, Type>
	Id(E oth)
	    : mValue(static_cast<T>(oth)) {
		
	}

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
