#pragma once
#include <cstdint>

enum class PSOFlags : uint32_t {
	kNone = 0,
	kHasSkin = 1 << 0,
	kHasTexture = 1 << 1,
	kAlphaBlend = 1 << 2,
	kCurrNone = 1 << 3,
	kWireframe = 1 << 4
};

inline PSOFlags operator&(PSOFlags a, PSOFlags b) {
	return static_cast<PSOFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline PSOFlags operator|(PSOFlags a, PSOFlags b) {
	return static_cast<PSOFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline PSOFlags operator^(PSOFlags a, PSOFlags b) {
	return static_cast<PSOFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
}

inline PSOFlags operator~(PSOFlags a) {
	return static_cast<PSOFlags>(~static_cast<uint32_t>(a));
}

inline PSOFlags& operator&=(PSOFlags& a, PSOFlags b) {
	a = a & b;
	return a;
}

inline PSOFlags& operator|=(PSOFlags& a, PSOFlags b) {
	a = a | b;
	return a;
}

inline PSOFlags& operator^=(PSOFlags& a, PSOFlags b) {
	a = a ^ b;
	return a;
}
