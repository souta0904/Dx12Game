#pragma once

class EulerAngle {
public:
	float x;
	float y;
	float z;

	EulerAngle();
	EulerAngle(float x, float y, float z);

	void Normalize();
};
