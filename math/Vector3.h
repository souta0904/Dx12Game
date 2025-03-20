#pragma once

class Vector2;
class Vector4;

// 3Dベクトル
class Vector3 {
public:
	float x;
	float y;
	float z;

	static const Vector3 kZero;
	static const Vector3 kOne;
	static const Vector3 kUnitX;
	static const Vector3 kUnitY;
	static const Vector3 kUnitZ;

	Vector3();
	Vector3(float x, float y, float z);
	Vector3(const Vector2& v);
	Vector3(const Vector4& v);

	void Normalize();
};

bool operator==(const Vector3& v1, const Vector3& v2);
bool operator!=(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(const Vector3& v1, const Vector3& v2);
Vector3 operator*(const Vector3& v, float scalar);
Vector3 operator*(float scalar, const Vector3& v);
Vector3 operator/(const Vector3& v, float scalar);
Vector3& operator+=(Vector3& v1, const Vector3& v2);
Vector3& operator-=(Vector3& v1, const Vector3& v2);
Vector3& operator*=(Vector3& v1, const Vector3& v2);
Vector3& operator*=(Vector3& v, float scalar);
Vector3& operator/=(Vector3& v, float scalar);

Vector3 Cross(const Vector3& v1, const Vector3& v2);
float Dot(const Vector3& v1, const Vector3& v2);
float Length(const Vector3& v);
float LengthSq(const Vector3& v);
Vector3 Normalize(const Vector3& v);
