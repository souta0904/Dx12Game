#pragma once
#include "Vector3.h"
#include <cstdint>

// ライト共通
struct LightCommon {
	uint32_t mDirectionalLightNum;
	uint32_t mPointLightNum;
	uint32_t mSpotLightNum;
};

// 平行光源
struct DirectionalLight {
	Vector3 mColor;
	float mIntensity;
	Vector3 mDirection;
};

// 点光源
struct PointLight {
	Vector3 mColor;
	float mIntensity;
	Vector3 mPosition;
	float mRadius;
	float mDecay;
};

// スポットライト
struct SpotLight {
	Vector3 mColor;
	float mIntensity;
	Vector3 mDirection;
	Vector3 mPosition;
	float mRadius;
	float mDecay;
	float mInner;
	float mOuter;
};
