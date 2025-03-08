#include "EulerAngle.h"
#include "MathUtil.h"

EulerAngle::EulerAngle()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f) {

}

EulerAngle::EulerAngle(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z) {

}

void EulerAngle::Normalize() {
	y = MathUtil::Wrap180(y);
	if (y < -90.0f) {
		y = -180.0f - y;
		x += 180.0f;
		z += 180.0f;
	} else if (y > 90.0f) {
		y = 180.0f - y;
		x += 180.0f;
		z += 180.0f;
	}
	if (std::fabs(y) > 90.0f - MathUtil::kEpsilon) {
		x += z;
		z = 0.0f;
	} else {
		z = MathUtil::Wrap180(z);
	}
	x = MathUtil::Wrap180(x);
}
