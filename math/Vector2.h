#pragma once

class Vector3;
class Vector4;

class Vector2 {
public:
	float x;
	float y;

	static const Vector2 kZero;
	static const Vector2 kOne;
	static const Vector2 kUnitX;
	static const Vector2 kUnitY;

	Vector2();
	Vector2(float x, float y);
	Vector2(const Vector3& v);
	Vector2(const Vector4& v);

	void Normalize();
};

bool operator==(const Vector2& v1, const Vector2& v2);
bool operator!=(const Vector2& v1, const Vector2& v2);
Vector2 operator-(const Vector2& v);
Vector2 operator+(const Vector2& v1, const Vector2& v2);
Vector2 operator-(const Vector2& v1, const Vector2& v2);
Vector2 operator*(const Vector2& v1, const Vector2& v2);
Vector2 operator*(const Vector2& v, float scalar);
Vector2 operator*(float scalar, const Vector2& v);
Vector2 operator/(const Vector2& v, float scalar);
Vector2& operator+=(Vector2& v1, const Vector2& v2);
Vector2& operator-=(Vector2& v1, const Vector2& v2);
Vector2& operator*=(Vector2& v1, const Vector2& v2);
Vector2& operator*=(Vector2& v, float scalar);
Vector2& operator/=(Vector2& v, float scalar);

float Cross(const Vector2& v1, const Vector2& v2);
float Dot(const Vector2& v1, const Vector2& v2);
float Length(const Vector2& v);
float LengthSq(const Vector2& v);
Vector2 Normalize(const Vector2& v);
