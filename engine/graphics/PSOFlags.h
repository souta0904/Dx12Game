#pragma once
#include <cstdint>

// パイプライン用フラグ
enum class PSOFlags : uint32_t {
	kNone = 0,
	kTexture = 1 << 0,		// テクスチャあり
	kSkin = 1 << 1,			// スキンあり
	kNormalBlend = 1 << 2,	// αブレンド
	kCurrNone = 1 << 3,		// カリングなし
	kWireframe = 1 << 4		// ワイヤーフレーム表示

};

// ビット演算
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
