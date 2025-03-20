#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <random>

class Random {
public:
	static void Initialize();
	static int32_t Rand(int32_t min, int32_t max);
	static float Rand(float min, float max);
	static Vector2 Rand(const Vector2& min, const Vector2& max);
	static Vector3 Rand(const Vector3& min, const Vector3& max);
	static Vector4 Rand(const Vector4& min, const Vector4& max);
private:
	static std::mt19937 sEngine;
};
