#pragma once

class Quaternion;
class Vector3;

class Matrix4 {
public:
	float m[4][4];

	Matrix4();

	Vector3 GetScale() const;
	Quaternion GetRotate() const;
	Vector3 GetTranslate() const;
};

Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);
Vector3 operator*(const Vector3& v, const Matrix4& m);
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);
Vector3& operator*=(Vector3& v, const Matrix4& m);

Matrix4 CreateIdentity4();
Matrix4 CreateScale4(const Vector3& scale);
Matrix4 CreateScale4(float scale);
Matrix4 CreateRotateX(float rotate);
Matrix4 CreateRotateY(float rotate);
Matrix4 CreateRotateZ(float rotate);
Matrix4 CreateRotate4(const Quaternion& rotate);
Matrix4 CreateTranslate4(const Vector3& translate);
Matrix4 CreateAffine4(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
Matrix4 CreateAffine4(float scale, const Quaternion& rotate, const Vector3& translate);
Matrix4 CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
Matrix4 CreateOrthographic(float left, float top, float right, float bottom, float nearZ, float farZ);
Matrix4 CreatePerspectiveFov(float fovY, float aspectRatio, float nearZ, float farZ);

float Determinant(const Matrix4& m);
Matrix4 Inverse(const Matrix4& m);
Matrix4 Transpose(const Matrix4& m);
