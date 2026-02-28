#pragma once

#include <cstdint>
#include <string_view>

namespace Wind {

inline uint32_t HashFloat4(const float* f) {
	uint32_t h[4];
	// Use memcpy to treat float bits as uint32_t without violating aliasing rules
	memcpy(h, f, sizeof h);
	uint32_t seed = 0x9e3779b9; // Golden ratio constant
	for (int i = 0; i < 4; i++) {
		seed ^= h[i] + 0x9e3779b1 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

inline uint32_t HashUint4(const uint32_t* u) {
	uint32_t seed = 0x9e3779b9; // Golden ratio constant
	for (int i = 0; i < 4; i++) {
		seed ^= u[i] + 0x9e3779b1 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

inline uint64_t Hash(const char* str) {
	return static_cast<uint64_t>(std::hash<std::string_view>{}(str));
}

} // namespace Huawei