#include "Vector3.h"
#include "MathUtil.h"
#include "Vector2.h"
#include "Vector4.h"
#include <cassert>

const Vector3 Vector3::kZero(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::kOne(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::kUnitX(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::kUnitY(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::kUnitZ(0.0f, 0.0f, 1.0f);

Vector3::Vector3()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f) {

}

Vector3::Vector3(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z) {

}

Vector3::Vector3(const Vector2& v)
	: x(v.x)
	, y(v.y)
	, z(0.0f) {

}

Vector3::Vector3(const Vector4& v)
	: x(v.x)
	, y(v.y)
	, z(v.z) {

}

void Vector3::Normalize() {
	float lenSq = x * x + y * y + z * z;
	//assert(lenSq > MathUtil::kEpsilon);
	float invLen = 1.0f / std::sqrt(lenSq);
	x *= invLen;
	y *= invLen;
	z *= invLen;
}

bool operator==(const Vector3& v1, const Vector3& v2) {
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

bool operator!=(const Vector3& v1, const Vector3& v2) {
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}

Vector3 operator-(const Vector3& v) {
	return Vector3(-v.x, -v.y, -v.z);
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vector3 operator*(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

Vector3 operator*(const Vector3& v, float scalar) {
	return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
}

Vector3 operator*(float scalar, const Vector3& v) {
	return Vector3(scalar * v.x, scalar * v.y, scalar * v.z);
}

Vector3 operator/(const Vector3& v, float scalar) {
	assert(scalar > MathUtil::kEpsilon);
	float invScalar = 1.0f / scalar;
	return Vector3(v.x * invScalar, v.y * invScalar, v.z * invScalar);
}

Vector3& operator+=(Vector3& v1, const Vector3& v2) {
	v1 = v1 + v2;
	return v1;
}

Vector3& operator-=(Vector3& v1, const Vector3& v2) {
	v1 = v1 - v2;
	return v1;
}

Vector3& operator*=(Vector3& v1, const Vector3& v2) {
	v1 = v1 * v2;
	return v1;
}

Vector3& operator*=(Vector3& v, float scalar) {
	v = v * scalar;
	return v;
}

Vector3& operator/=(Vector3& v, float scalar) {
	assert(scalar > MathUtil::kEpsilon);
	float invScalar = 1.0f / scalar;
	v *= invScalar;
	return v;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return Vector3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Length(const Vector3& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float LengthSq(const Vector3& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

Vector3 Normalize(const Vector3& v) {
	Vector3 result = v;
	result.Normalize();
	return result;
}
