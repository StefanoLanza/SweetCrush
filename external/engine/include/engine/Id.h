#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace Wind {

template <typename Tag, typename T = uint32_t, T invalid = 0>
class Id {
public:
	using Type = T;

	Id()
	    : mValue(static_cast<T>(-1)) {
	}

	// Constructor for raw values
	constexpr explicit Id(Type val)
	    : mValue(val) {
	}

	// Implicitly convert a client-specific Id to this Id
	// E.g. GameScreenId to ScreenId
	template <typename E>
	requires std::is_enum_v<E>&& std::is_convertible_v<std::underlying_type_t<E>, Type> constexpr Id(E oth)
	    : mValue(static_cast<T>(oth)) {
	}

	void Reset() {
		mValue = invalid;
	}

	constexpr explicit operator bool() const {
		return mValue != invalid;
	}

	constexpr T Get() const {
		return mValue;
	}

	bool IsValid() const {
		return mValue != invalid;
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
