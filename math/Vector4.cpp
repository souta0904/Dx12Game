#include "Vector4.h"
#include "MathUtil.h"
#include "Vector2.h"
#include "Vector3.h"
#include <cassert>

const Vector4 Vector4::kZero(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::kOne(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 Vector4::kUnitX(1.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::kUnitY(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4 Vector4::kUnitZ(0.0f, 0.0f, 1.0f, 0.0f);
const Vector4 Vector4::kUnitW(0.0f, 0.0f, 0.0f, 1.0f);

Vector4::Vector4()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	, w(0.0f) {

}

Vector4::Vector4(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w) {

}

Vector4::Vector4(const Vector2& v)
	: x(v.x)
	, y(v.y)
	, z(0.0f)
	, w(0.0f) {

}

Vector4::Vector4(const Vector3& v)
	: x(v.x)
	, y(v.y)
	, z(v.z)
	, w(0.0f) {

}

void Vector4::Normalize() {
	float lenSq = x * x + y * y + z * z + w * w;
	assert(lenSq > MathUtil::kEpsilon);
	float invLen = 1.0f / std::sqrt(lenSq);
	x *= invLen;
	y *= invLen;
	z *= invLen;
	w *= invLen;
}

bool operator==(const Vector4& v1, const Vector4& v2) {
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

bool operator!=(const Vector4& v1, const Vector4& v2) {
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
}

Vector4 operator-(const Vector4& v) {
	return Vector4(-v.x, -v.y, -v.z, -v.w);
}

Vector4 operator+(const Vector4& v1, const Vector4& v2) {
	return Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

Vector4 operator-(const Vector4& v1, const Vector4& v2) {
	return Vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

Vector4 operator*(const Vector4& v1, const Vector4& v2) {
	return Vector4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

Vector4 operator*(const Vector4& v, float scalar) {
	return Vector4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
}

Vector4 operator*(float scalar, const Vector4& v) {
	return Vector4(scalar * v.x, scalar * v.y, scalar * v.z, scalar * v.w);
}

Vector4 operator/(const Vector4& v, float scalar) {
	assert(scalar > MathUtil::kEpsilon);
	float invScalar = 1.0f / scalar;
	return Vector4(v.x * invScalar, v.y * invScalar, v.z * invScalar, v.w * invScalar);
}

Vector4& operator+=(Vector4& v1, const Vector4& v2) {
	v1 = v1 + v2;
	return v1;
}

Vector4& operator-=(Vector4& v1, const Vector4& v2) {
	v1 = v1 - v2;
	return v1;
}

Vector4& operator*=(Vector4& v1, const Vector4& v2) {
	v1 = v1 * v2;
	return v1;
}

Vector4& operator*=(Vector4& v, float scalar) {
	v = v * scalar;
	return v;
}

Vector4& operator/=(Vector4& v, float scalar) {
	assert(scalar > MathUtil::kEpsilon);
	float invScalar = 1.0f / scalar;
	v *= invScalar;
	return v;
}

float Dot(const Vector4& v1, const Vector4& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

float Length(const Vector4& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

float LengthSq(const Vector4& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

Vector4 Normalize(const Vector4& v) {
	Vector4 result = v;
	result.Normalize();
	return result;
}
