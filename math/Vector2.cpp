#include "Vector2.h"
#include "MathUtil.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cassert>

const Vector2 Vector2::kZero(0.0f, 0.0f);
const Vector2 Vector2::kOne(1.0f, 1.0f);
const Vector2 Vector2::kUnitX(1.0f, 0.0f);
const Vector2 Vector2::kUnitY(0.0f, 1.0f);

Vector2::Vector2()
	: x(0.0f)
	, y(0.0f) {

}

Vector2::Vector2(float x, float y)
	: x(x)
	, y(y) {

}

Vector2::Vector2(const Vector3& v)
	: x(v.x)
	, y(v.y) {

}

Vector2::Vector2(const Vector4& v)
	: x(v.x)
	, y(v.y) {

}

void Vector2::Normalize() {
	float lenSq = x * x + y * y;
	assert(lenSq > MathUtil::kEpsilon);
	float invLen = 1.0f / std::sqrt(lenSq);
	x *= invLen;
	y *= invLen;
}

bool operator==(const Vector2& v1, const Vector2& v2) {
	return v1.x == v2.x && v1.y == v2.y;
}

bool operator!=(const Vector2& v1, const Vector2& v2) {
	return v1.x != v2.x || v1.y != v2.y;
}

Vector2 operator-(const Vector2& v) {
	return Vector2(-v.x, -v.y);
}

Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	return Vector2(v1.x + v2.x, v1.y + v2.y);
}

Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	return Vector2(v1.x - v2.x, v1.y - v2.y);
}

Vector2 operator*(const Vector2& v1, const Vector2& v2) {
	return Vector2(v1.x * v2.x, v1.y * v2.y);
}

Vector2 operator*(const Vector2& v, float scalar) {
	return Vector2(v.x * scalar, v.y * scalar);
}

Vector2 operator*(float scalar, const Vector2& v) {
	return Vector2(scalar * v.x, scalar * v.y);
}

Vector2 operator/(const Vector2& v, float scalar) {
	assert(scalar > MathUtil::kEpsilon);
	float invScalar = 1.0f / scalar;
	return Vector2(v.x * invScalar, v.y * invScalar);
}

Vector2& operator+=(Vector2& v1, const Vector2& v2) {
	v1 = v1 + v2;
	return v1;
}

Vector2& operator-=(Vector2& v1, const Vector2& v2) {
	v1 = v1 - v2;
	return v1;
}

Vector2& operator*=(Vector2& v1, const Vector2& v2) {
	v1 = v1 * v2;
	return v1;
}

Vector2& operator*=(Vector2& v, float scalar) {
	v = v * scalar;
	return v;
}

Vector2& operator/=(Vector2& v, float scalar) {
	assert(scalar > MathUtil::kEpsilon);
	float invScalar = 1.0f / scalar;
	v *= invScalar;
	return v;
}

float Cross(const Vector2& v1, const Vector2& v2) {
	return v1.x * v2.y - v1.y * v2.x;
}

float Dot(const Vector2& v1, const Vector2& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

float Length(const Vector2& v) {
	return std::sqrt(v.x * v.x + v.y * v.y);
}

float LengthSq(const Vector2& v) {
	return v.x * v.x + v.y * v.y;
}

Vector2 Normalize(const Vector2& v) {
	Vector2 result = v;
	result.Normalize();
	return result;
}
