#include "Quaternion.h"
#include "MathUtil.h"
#include "Vector3.h"
#include <cassert>

Quaternion::Quaternion()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	, w(1.0f) {

}

Quaternion::Quaternion(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w) {

}

Quaternion::Quaternion(const Vector3& axis, float angle) {
	//assert(std::fabs(Length(axis) - 1.0f) < MathUtil::kEpsilon);
	assert(std::fabs(Length(axis) - 1.0f) < 0.001f);
	float half = angle * 0.5f;
	float sin = std::sinf(half);
	x = axis.x * sin;
	y = axis.y * sin;
	z = axis.z * sin;
	w = std::cosf(half);
}

void Quaternion::Normalize() {
	float lenSq = x * x + y * y + z * z + w * w;
	assert(lenSq > MathUtil::kEpsilon);
	float invLen = 1.0f / std::sqrtf(lenSq);
	x *= invLen;
	y *= invLen;
	z *= invLen;
	w *= invLen;
}

Vector3 Quaternion::GetAxis() const {
	float sinSq = 1.0f - w * w;
	if (sinSq <= 0.0f) {
		return Vector3::kUnitX;
	}
	float invSin = 1.0f / std::sqrtf(sinSq);
	return Vector3(x * invSin, y * invSin, z * invSin);
}

float Quaternion::GetAngle() const {
	return std::acosf(w) * 2.0f;
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
	Quaternion result;
	result.x = q1.w * q2.x + q1.x * q2.w + q1.z * q2.y - q1.y * q2.z;
	result.y = q1.w * q2.y + q1.y * q2.w + q1.x * q2.z - q1.z * q2.x;
	result.z = q1.w * q2.z + q1.z * q2.w + q1.y * q2.x - q1.x * q2.y;
	result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	return result;
}

Vector3 operator*(const Vector3& v, const Quaternion& q) {
	Quaternion result;
	result = Conjugate(q) * Quaternion(v.x, v.y, v.z, 0.0f) * q;
	return Vector3(result.x, result.y, result.z);
}

Quaternion& operator*=(Quaternion& q1, const Quaternion& q2) {
	q1 = q1 * q2;
	return q1;
}

Vector3& operator*=(Vector3& v, const Quaternion& q) {
	v = v * q;
	return v;
}

Quaternion Conjugate(const Quaternion& q) {
	Quaternion result;
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	result.w = q.w;
	return result;
}

float Dot(const Quaternion& q1, const Quaternion& q2) {
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t) {
	float invT = 1.0f - t;
	return Quaternion(
		q1.x * invT + q2.x * t,
		q1.y * invT + q2.y * t,
		q1.z * invT + q2.z * t,
		q1.w * invT + q2.w * t
	);
}

Quaternion Normalize(const Quaternion& q) {
	Quaternion result = q;
	result.Normalize();
	return result;
}

Quaternion Pow(const Quaternion& q, float e) {
	if (std::fabs(q.w) > 1.0f - MathUtil::kEpsilon) {
		return q;
	}
	float half = std::acosf(q.w);
	float newHalf = half * e;
	Quaternion result;
	result.w = std::cosf(newHalf);
	newHalf = std::sinf(newHalf) / std::sinf(half);
	result.x = q.x * newHalf;
	result.y = q.y * newHalf;
	result.z = q.z * newHalf;
	return result;
}

Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t) {
	if (t <= 0.0f) {
		return q1;
	}
	if (t >= 1.0f) {
		return q2;
	}
	float cosOmega = Dot(q1, q2);
	float x = q2.x;
	float y = q2.y;
	float z = q2.z;
	float w = q2.w;
	if (cosOmega < 0.0f) {
		x = -q2.x;
		y = -q2.y;
		z = -q2.z;
		w = -q2.w;
		cosOmega = -cosOmega;
	}
	float k0, k1;
	if (cosOmega > 1.0f - MathUtil::kEpsilon) {
		k0 = 1.0f - t;
		k1 = t;
	} else {
		float sinOmega = std::sqrt(1.0f - cosOmega * cosOmega);
		float omega = std::atan2f(sinOmega, cosOmega);
		float invSinOmega = 1.0f / sinOmega;
		k0 = std::sinf((1.0f - t) * omega) * invSinOmega;
		k1 = std::sinf(t * omega) * invSinOmega;
	}
	return Quaternion(
		q1.x * k0 + x * k1,
		q1.y * k0 + y * k1,
		q1.z * k0 + z * k1,
		q1.w * k0 + w * k1
	);
}
