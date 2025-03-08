#pragma once
#include <cmath>
#include <limits>
#include <numbers>

class EulerAngle;
class Matrix4;
class Quaternion;

namespace MathUtil {
	const float kPi = std::numbers::pi_v<float>;
	const float k2Pi = kPi * 2.0f;
	const float kPiOver2 = kPi / 2.0f;
	const float k1OverPi = 1.0f / kPi;
	const float k1Over2Pi = 1.0f / k2Pi;
	const float k180OverPi = 180.0f / kPi;
	const float kPiOver180 = kPi / 180.0f;
	const float kEpsilon = std::numeric_limits<float>::epsilon();

	template <typename T>
	inline T Max(const T& a, const T& b) {
		return a > b ? a : b;
	}

	template <typename T>
	inline T Min(const T& a, const T& b) {
		return a < b ? a : b;
	}

	template <typename T>
	inline T Clamp(const T& v, const T& low, const T& high) {
		return Min(high, Max(low, v));
	}

	template <typename T>
	inline T Lerp(const T& a, const T& b, float t) {
		return (1.0f - t) * a + t * b;
	}

	inline int Sign(float x) {
		return (x > 0.0f) - (x < 0.0f);
	}

	inline float ToDegrees(float angle) {
		return angle * k180OverPi;
	}

	inline float ToRadians(float angle) {
		return angle * kPiOver180;
	}

	inline float WrapPi(float angle) {
		angle += kPi;
		angle -= std::floor(angle * k1Over2Pi) * k2Pi;
		angle -= kPi;
		return angle;
	}

	inline float Wrap180(float angle) {
		angle += 180.0f;
		angle -= std::floor(angle / 360.0f) * 360.0f;
		angle -= 180.0f;
		return angle;
	}

	EulerAngle ToEulerAngle(const Matrix4& m);
	EulerAngle ToEulerAngle(const Quaternion& q);
	Matrix4 ToMatrix4(const EulerAngle& e);
	Matrix4 ToMatrix4(const Quaternion& q);
	Quaternion ToQuaternion(const EulerAngle& e);
	Quaternion ToQuaternion(const Matrix4& m);
}
