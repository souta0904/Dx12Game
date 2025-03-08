#include "Matrix3.h"
#include "MathUtil.h"
#include "Vector2.h"
#include <cassert>

Matrix3::Matrix3() {
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
}

Vector2 Matrix3::GetScale() const {
	return Vector2(
		std::sqrtf(m[0][0] * m[0][0] + m[0][1] * m[0][1]),
		std::sqrtf(m[1][0] * m[1][0] + m[1][1] * m[1][1])
	);
}

float Matrix3::GetRotate() const {
	return std::atan2f(m[0][1], m[0][0]);
}

Vector2 Matrix3::GetTranslate() const {
	return Vector2(m[2][0], m[2][1]);
}

Matrix3 operator*(const Matrix3& m1, const Matrix3& m2) {
	Matrix3 result;
	result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2];
	result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2];
	result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2];
	return result;
}

Vector2 operator*(const Vector2& v, const Matrix3& m) {
	return Vector2(
		v.x * m.m[0][0] + v.y * m.m[1][0] + m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + m.m[2][1]
	);
}

Matrix3& operator*=(Matrix3& m1, const Matrix3& m2) {
	m1 = m1 * m2;
	return m1;
}

Vector2& operator*=(Vector2& v, const Matrix3& m) {
	v = v * m;
	return v;
}

Matrix3 CreateIdentity3() {
	return Matrix3();
}

Matrix3 CreateScale3(const Vector2& scale) {
	Matrix3 result;
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	return result;
}

Matrix3 CreateScale3(float scale) {
	Matrix3 result;
	result.m[0][0] = scale;
	result.m[1][1] = scale;
	return result;
}

Matrix3 CreateRotate3(float rotate) {
	float cos = std::cosf(rotate);
	float sin = std::sinf(rotate);
	Matrix3 result;
	result.m[0][0] = cos;
	result.m[0][1] = sin;
	result.m[1][0] = -sin;
	result.m[1][1] = cos;
	return result;
}

Matrix3 CreateTranslate3(const Vector2& translate) {
	Matrix3 result;
	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;
	return result;
}

Matrix3 CreateAffine3(const Vector2& scale, float rotate, const Vector2& translate) {
	float cos = std::cosf(rotate);
	float sin = std::sinf(rotate);
	Matrix3 result;
	result.m[0][0] = scale.x * cos;
	result.m[0][1] = sin;
	result.m[1][0] = -sin;
	result.m[1][1] = scale.y * cos;
	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;
	return result;
}

Matrix3 CreateAffine3(float scale, float rotate, const Vector2& translate) {
	float cos = std::cosf(rotate);
	float sin = std::sinf(rotate);
	Matrix3 result;
	result.m[0][0] = scale * cos;
	result.m[0][1] = sin;
	result.m[1][0] = -sin;
	result.m[1][1] = scale * cos;
	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;
	return result;
}

float Determinant(const Matrix3& m) {
	return
		m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] - m.m[2][1]) +
		m.m[0][1] * (m.m[1][2] * m.m[2][0] - m.m[1][0] - m.m[2][2]) +
		m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] - m.m[2][0]);
}

Matrix3 Inverse(const Matrix3& m) {
	float determinant =
		m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] - m.m[2][1]) +
		m.m[0][1] * (m.m[1][2] * m.m[2][0] - m.m[1][0] - m.m[2][2]) +
		m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] - m.m[2][0]);
	assert(std::fabs(determinant) > MathUtil::kEpsilon);
	float invDeterminant = 1.0f / determinant;
	Matrix3 result;
	result.m[0][0] = (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) * invDeterminant;
	result.m[0][1] = (m.m[0][2] * m.m[2][1] - m.m[0][1] * m.m[2][2]) * invDeterminant;
	result.m[0][2] = (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]) * invDeterminant;
	result.m[1][0] = (m.m[1][2] * m.m[2][0] - m.m[1][0] * m.m[2][2]) * invDeterminant;
	result.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[0][2] * m.m[2][0]) * invDeterminant;
	result.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * invDeterminant;
	result.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]) * invDeterminant;
	result.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * invDeterminant;
	result.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * invDeterminant;
	return result;
}

Matrix3 Transpose(const Matrix3& m) {
	Matrix3 result;
	result.m[0][0] = m.m[0][0];
	result.m[0][1] = m.m[1][0];
	result.m[0][2] = m.m[2][0];
	result.m[1][0] = m.m[0][1];
	result.m[1][1] = m.m[1][1];
	result.m[1][2] = m.m[2][1];
	result.m[2][0] = m.m[0][2];
	result.m[2][1] = m.m[1][2];
	result.m[2][2] = m.m[2][2];
	return result;
}
