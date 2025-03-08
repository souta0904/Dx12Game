#include "Matrix4.h"
#include "MathUtil.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <cassert>

Matrix4::Matrix4() {
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

Vector3 Matrix4::GetScale() const {
	return Vector3(
		std::sqrtf(m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2]),
		std::sqrtf(m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2]),
		std::sqrtf(m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2])
	);
}

Quaternion Matrix4::GetRotate() const {
	Vector3 xAxis = Normalize(Vector3(m[0][0], m[1][0], m[2][0]));
	Vector3 yAxis = Normalize(Vector3(m[0][1], m[1][1], m[2][1]));
	Vector3 zAxis = Normalize(Vector3(m[0][2], m[1][2], m[2][2]));
	Matrix4 result;
	result.m[0][0] = xAxis.x;
	result.m[0][1] = yAxis.x;
	result.m[0][2] = zAxis.x;
	result.m[1][0] = xAxis.y;
	result.m[1][1] = yAxis.y;
	result.m[1][2] = zAxis.y;
	result.m[2][0] = xAxis.z;
	result.m[2][1] = yAxis.z;
	result.m[2][2] = zAxis.z;
	return MathUtil::ToQuaternion(result);
}

Vector3 Matrix4::GetTranslate() const {
	return Vector3(m[3][0], m[3][1], m[3][2]);
}

Matrix4 operator*(const Matrix4& m1, const Matrix4& m2) {
	Matrix4 result;
	result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
	result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
	result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
	result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return result;
}

Vector3 operator*(const Vector3& v, const Matrix4& m) {
	return Vector3(
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]
	);
}

Matrix4& operator*=(Matrix4& m1, const Matrix4& m2) {
	m1 = m1 * m2;
	return m1;
}

Vector3& operator*=(Vector3& v, const Matrix4& m) {
	v = v * m;
	return v;
}

Matrix4 CreateIdentity4() {
	return Matrix4();
}

Matrix4 CreateScale4(const Vector3& scale) {
	Matrix4 result;
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	return result;
}

Matrix4 CreateScale4(float scale) {
	Matrix4 result;
	result.m[0][0] = scale;
	result.m[1][1] = scale;
	result.m[2][2] = scale;
	return result;
}

Matrix4 CreateRotateX(float rotate) {
	float cos = std::cosf(rotate);
	float sin = std::sinf(rotate);
	Matrix4 result;
	result.m[1][1] = cos;
	result.m[1][2] = sin;
	result.m[2][1] = -sin;
	result.m[2][2] = cos;
	return result;
}

Matrix4 CreateRotateY(float rotate) {
	float cos = std::cosf(rotate);
	float sin = std::sinf(rotate);
	Matrix4 result;
	result.m[0][0] = cos;
	result.m[0][2] = -sin;
	result.m[2][0] = sin;
	result.m[2][2] = cos;
	return result;
}

Matrix4 CreateRotateZ(float rotate) {
	float cos = std::cosf(rotate);
	float sin = std::sinf(rotate);
	Matrix4 result;
	result.m[0][0] = cos;
	result.m[0][1] = sin;
	result.m[1][0] = -sin;
	result.m[1][1] = cos;
	return result;
}

Matrix4 CreateRotate4(const Quaternion& rotate) {
	float ww = 2.0f * rotate.w;
	float xx = 2.0f * rotate.x;
	float yy = 2.0f * rotate.y;
	float zz = 2.0f * rotate.z;
	Matrix4 result;
	result.m[0][0] = 1.0f - yy * rotate.y - zz * rotate.z;
	result.m[0][1] = xx * rotate.y + ww * rotate.z;
	result.m[0][2] = xx * rotate.z - ww * rotate.y;
	result.m[1][0] = xx * rotate.y - ww * rotate.z;
	result.m[1][1] = 1.0f - xx * rotate.x - zz * rotate.z;
	result.m[1][2] = yy * rotate.z + ww * rotate.x;
	result.m[2][0] = xx * rotate.z + ww * rotate.y;
	result.m[2][1] = yy * rotate.z - ww * rotate.x;
	result.m[2][2] = 1.0f - xx * rotate.x - yy * rotate.y;
	return result;
}

Matrix4 CreateTranslate4(const Vector3& translate) {
	Matrix4 result;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}

Matrix4 CreateAffine4(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
	float ww = 2.0f * rotate.w;
	float xx = 2.0f * rotate.x;
	float yy = 2.0f * rotate.y;
	float zz = 2.0f * rotate.z;
	Matrix4 result;
	result.m[0][0] = scale.x * (1.0f - yy * rotate.y - zz * rotate.z);
	result.m[0][1] = scale.x * (xx * rotate.y + ww * rotate.z);
	result.m[0][2] = scale.x * (xx * rotate.z - ww * rotate.y);
	result.m[1][0] = scale.y * (xx * rotate.y - ww * rotate.z);
	result.m[1][1] = scale.y * (1.0f - xx * rotate.x - zz * rotate.z);
	result.m[1][2] = scale.y * (yy * rotate.z + ww * rotate.x);
	result.m[2][0] = scale.z * (xx * rotate.z + ww * rotate.y);
	result.m[2][1] = scale.z * (yy * rotate.z - ww * rotate.x);
	result.m[2][2] = scale.z * (1.0f - xx * rotate.x - yy * rotate.y);
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}

Matrix4 CreateAffine4(float scale, const Quaternion& rotate, const Vector3& translate) {
	float ww = 2.0f * rotate.w;
	float xx = 2.0f * rotate.x;
	float yy = 2.0f * rotate.y;
	float zz = 2.0f * rotate.z;
	Matrix4 result;
	result.m[0][0] = scale * (1.0f - yy * rotate.y - zz * rotate.z);
	result.m[0][1] = scale * (xx * rotate.y + ww * rotate.z);
	result.m[0][2] = scale * (xx * rotate.z - ww * rotate.y);
	result.m[1][0] = scale * (xx * rotate.y - ww * rotate.z);
	result.m[1][1] = scale * (1.0f - xx * rotate.x - zz * rotate.z);
	result.m[1][2] = scale * (yy * rotate.z + ww * rotate.x);
	result.m[2][0] = scale * (xx * rotate.z + ww * rotate.y);
	result.m[2][1] = scale * (yy * rotate.z - ww * rotate.x);
	result.m[2][2] = scale * (1.0f - xx * rotate.x - yy * rotate.y);
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}

Matrix4 CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
	Vector3 zAxis = Normalize(target - eye);
	Vector3 xAxis = Normalize(Cross(up, zAxis));
	Vector3 yAxis = Normalize(Cross(zAxis, xAxis));
	Matrix4 result;
	result.m[0][0] = xAxis.x;
	result.m[0][1] = yAxis.x;
	result.m[0][2] = zAxis.x;
	result.m[1][0] = xAxis.y;
	result.m[1][1] = yAxis.y;
	result.m[1][2] = zAxis.y;
	result.m[2][0] = xAxis.z;
	result.m[2][1] = yAxis.z;
	result.m[2][2] = zAxis.z;
	result.m[3][0] = -Dot(xAxis, eye);
	result.m[3][1] = -Dot(yAxis, eye);
	result.m[3][2] = -Dot(zAxis, eye);
	return result;
}

Matrix4 CreateOrthographic(float left, float top, float right, float bottom, float nearZ, float farZ) {
	Matrix4 result;
	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farZ - nearZ);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearZ / (nearZ - farZ);
	return result;
}

Matrix4 CreatePerspectiveFov(float fovY, float aspectRatio, float nearZ, float farZ) {
	float yScale = 1.0f / std::tanf(fovY / 2.0f);
	float xScale = yScale / aspectRatio;
	Matrix4 result;
	result.m[0][0] = xScale;
	result.m[1][1] = yScale;
	result.m[2][2] = farZ / (farZ - nearZ);
	result.m[2][3] = 1.0f;
	result.m[3][2] = -(nearZ * farZ) / (farZ - nearZ);
	result.m[3][3] = 0.0f;
	return result;
}

float Determinant(const Matrix4& m) {
	return
		m.m[0][0] * ((m.m[1][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])) + (m.m[1][2] * (m.m[2][3] * m.m[3][1] - m.m[2][1] * m.m[3][3])) + (m.m[1][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]))) -
		m.m[0][1] * ((m.m[1][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])) + (m.m[1][2] * (m.m[2][3] * m.m[3][0] - m.m[2][0] * m.m[3][3])) + (m.m[1][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]))) +
		m.m[0][2] * ((m.m[1][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1])) + (m.m[1][1] * (m.m[2][3] * m.m[3][0] - m.m[2][0] * m.m[3][3])) + (m.m[1][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]))) -
		m.m[0][3] * ((m.m[1][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])) + (m.m[1][1] * (m.m[2][2] * m.m[3][0] - m.m[2][0] * m.m[3][2])) + (m.m[1][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])));
}

Matrix4 Inverse(const Matrix4& m) {
	float determinant =
		m.m[0][0] * ((m.m[1][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])) + (m.m[1][2] * (m.m[2][3] * m.m[3][1] - m.m[2][1] * m.m[3][3])) + (m.m[1][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]))) -
		m.m[0][1] * ((m.m[1][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])) + (m.m[1][2] * (m.m[2][3] * m.m[3][0] - m.m[2][0] * m.m[3][3])) + (m.m[1][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]))) +
		m.m[0][2] * ((m.m[1][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1])) + (m.m[1][1] * (m.m[2][3] * m.m[3][0] - m.m[2][0] * m.m[3][3])) + (m.m[1][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]))) -
		m.m[0][3] * ((m.m[1][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])) + (m.m[1][1] * (m.m[2][2] * m.m[3][0] - m.m[2][0] * m.m[3][2])) + (m.m[1][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])));
	assert(std::fabs(determinant) > MathUtil::kEpsilon);
	float invDeterminant = 1.0f / determinant;
	Matrix4 result;
	result.m[0][0] = (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) * invDeterminant;
	result.m[0][1] = (m.m[0][2] * m.m[2][1] - m.m[0][1] * m.m[2][2]) * invDeterminant;
	result.m[0][2] = (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]) * invDeterminant;
	result.m[1][0] = (m.m[1][2] * m.m[2][0] - m.m[1][0] * m.m[2][2]) * invDeterminant;
	result.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[0][2] * m.m[2][0]) * invDeterminant;
	result.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * invDeterminant;
	result.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]) * invDeterminant;
	result.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * invDeterminant;
	result.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * invDeterminant;
	result.m[3][0] = -(m.m[3][0] * result.m[0][0] + m.m[3][1] * result.m[1][0] + m.m[3][2] * result.m[2][0]);
	result.m[3][1] = -(m.m[3][0] * result.m[0][1] + m.m[3][1] * result.m[1][1] + m.m[3][2] * result.m[2][1]);
	result.m[3][2] = -(m.m[3][0] * result.m[0][2] + m.m[3][1] * result.m[1][2] + m.m[3][2] * result.m[2][2]);
	return result;
}

Matrix4 Transpose(const Matrix4& m) {
	Matrix4 result;
	result.m[0][0] = m.m[0][0];
	result.m[0][1] = m.m[1][0];
	result.m[0][2] = m.m[2][0];
	result.m[0][3] = m.m[3][0];
	result.m[1][0] = m.m[0][1];
	result.m[1][1] = m.m[1][1];
	result.m[1][2] = m.m[2][1];
	result.m[1][3] = m.m[3][1];
	result.m[2][0] = m.m[0][2];
	result.m[2][1] = m.m[1][2];
	result.m[2][2] = m.m[2][2];
	result.m[2][3] = m.m[3][2];
	result.m[3][0] = m.m[0][3];
	result.m[3][1] = m.m[1][3];
	result.m[3][2] = m.m[2][3];
	result.m[3][3] = m.m[3][3];
	return result;
}
