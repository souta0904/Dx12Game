#pragma once

class Vector2;
class Vector3;

// 4Dベクトル
class Vector4 {
public:
	float x;
	float y;
	float z;
	float w;

	static const Vector4 kZero;
	static const Vector4 kOne;
	static const Vector4 kUnitX;
	static const Vector4 kUnitY;
	static const Vector4 kUnitZ;
	static const Vector4 kUnitW;

	Vector4();
	Vector4(float x, float y, float z, float w);
	Vector4(const Vector2& v);
	Vector4(const Vector3& v);

	void Normalize();
};

bool operator==(const Vector4& v1, const Vector4& v2);
bool operator!=(const Vector4& v1, const Vector4& v2);
Vector4 operator-(const Vector4& v);
Vector4 operator+(const Vector4& v1, const Vector4& v2);
Vector4 operator-(const Vector4& v1, const Vector4& v2);
Vector4 operator*(const Vector4& v1, const Vector4& v2);
Vector4 operator*(const Vector4& v, float scalar);
Vector4 operator*(float scalar, const Vector4& v);
Vector4 operator/(const Vector4& v, float scalar);
Vector4& operator+=(Vector4& v1, const Vector4& v2);
Vector4& operator-=(Vector4& v1, const Vector4& v2);
Vector4& operator*=(Vector4& v1, const Vector4& v2);
Vector4& operator*=(Vector4& v, float scalar);
Vector4& operator/=(Vector4& v, float scalar);

float Dot(const Vector4& v1, const Vector4& v2);
float Length(const Vector4& v);
float LengthSq(const Vector4& v);
Vector4 Normalize(const Vector4& v);
