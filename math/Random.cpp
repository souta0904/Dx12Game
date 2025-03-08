#include "Random.h"

std::mt19937 Random::sEngine;

void Random::Initialize() {
	std::random_device rd;
	sEngine.seed(rd());
}

int32_t Random::Rand(int32_t min, int32_t max) {
	std::uniform_int_distribution<int32_t> dist(min, max);
	return dist(sEngine);
}

float Random::Rand(float min, float max) {
	std::uniform_real_distribution<float> dist(min, max);
	return dist(sEngine);
}

Vector2 Random::Rand(const Vector2& min, const Vector2& max) {
	return Vector2(
		min.x + (max.x - min.x) * Rand(0.0f, 1.0f),
		min.y + (max.y - min.y) * Rand(0.0f, 1.0f)
	);
}

Vector3 Random::Rand(const Vector3& min, const Vector3& max) {
	return Vector3(
		min.x + (max.x - min.x) * Rand(0.0f, 1.0f),
		min.y + (max.y - min.y) * Rand(0.0f, 1.0f),
		min.z + (max.z - min.z) * Rand(0.0f, 1.0f)
	);
}

Vector4 Random::Rand(const Vector4& min, const Vector4& max) {
	return Vector4(
		min.x + (max.x - min.x) * Rand(0.0f, 1.0f),
		min.y + (max.y - min.y) * Rand(0.0f, 1.0f),
		min.z + (max.z - min.z) * Rand(0.0f, 1.0f),
		min.w + (max.w - min.w) * Rand(0.0f, 1.0f)
	);
}
