#pragma once

class Vector3;

class Quaternion {
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion();
	Quaternion(float x, float y, float z, float w);
	// 軸と角度(ラジアン)から
	Quaternion(const Vector3& axis, float angle);

	void Normalize();

	Vector3 GetAxis() const;
	float GetAngle() const;
};

Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Vector3 operator*(const Vector3& v, const Quaternion& q);
Quaternion& operator*=(Quaternion& q1, const Quaternion& q2);
Vector3& operator*=(Vector3& v, const Quaternion& q);

Quaternion Conjugate(const Quaternion& q);
float Dot(const Quaternion& q1, const Quaternion& q2);
Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);
Quaternion Normalize(const Quaternion& q);
Quaternion Pow(const Quaternion& q, float e);
Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);
