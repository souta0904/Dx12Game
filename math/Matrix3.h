#pragma once

class Vector2;

class Matrix3 {
public:
	float m[3][3];

	Matrix3();

	Vector2 GetScale() const;
	float GetRotate() const;
	Vector2 GetTranslate() const;
};

Matrix3 operator*(const Matrix3& m1, const Matrix3& m2);
Vector2 operator*(const Vector2& v, const Matrix3& m);
Matrix3& operator*=(Matrix3& m1, const Matrix3& m2);
Vector2& operator*=(Vector2& v, const Matrix3& m);

Matrix3 CreateIdentity3();
Matrix3 CreateScale3(const Vector2& scale);
Matrix3 CreateScale3(float scale);
Matrix3 CreateRotate3(float rotate);
Matrix3 CreateTranslate3(const Vector2& translate);
Matrix3 CreateAffine3(const Vector2& scale, float rotate, const Vector2& translate);
Matrix3 CreateAffine3(float scale, float rotate, const Vector2& translate);

float Determinant(const Matrix3& m);
Matrix3 Inverse(const Matrix3& m);
Matrix3 Transpose(const Matrix3& m);
