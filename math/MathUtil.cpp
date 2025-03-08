#include "MathUtil.h"
#include "EulerAngle.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"

namespace MathUtil {

	EulerAngle ToEulerAngle(const Matrix4& m) {
		EulerAngle result;
		float sy = -m.m[0][2];
		if (sy <= -1.0f) {
			result.y = -90.0f;
		} else if (sy >= 1.0f) {
			result.y = 90.0f;
		} else {
			result.y = ToDegrees(std::asinf(sy));
		}
		if (std::fabs(sy) > 1.0f - kEpsilon) {
			result.x = ToDegrees(std::atan2f(-m.m[2][1], m.m[1][1]));
			result.z = 0.0f;
		} else {
			result.x = ToDegrees(std::atan2f(m.m[1][2], m.m[2][2]));
			result.z = ToDegrees(std::atan2f(m.m[0][1], m.m[0][0]));
		}
		return result;
	}

	EulerAngle ToEulerAngle(const Quaternion& q) {
		EulerAngle result;
		float sy = -2.0f * (q.x * q.z - q.w * q.y);
		if (std::fabs(sy) > 1.0f - kEpsilon) {
			result.x = ToDegrees(std::atan2f(-q.y * q.z + q.w * q.x, 0.5f - q.x * q.x - q.z * q.z));
			result.y = ToDegrees(kPiOver2 * sy);
			result.z = 0.0f;
		} else {
			result.x = ToDegrees(std::atan2f(q.y * q.z + q.w * q.x, 0.5f - q.x * q.x - q.y * q.y));
			result.y = ToDegrees(std::asinf(sy));
			result.z = ToDegrees(std::atan2f(q.x * q.y + q.w * q.z, 0.5f - q.y * q.y - q.z * q.z));
		}
		return result;
	}

	Matrix4 ToMatrix4(const EulerAngle& e) {
		Vector3 euler;
		euler.x = ToRadians(e.x);
		euler.y = ToRadians(e.y);
		euler.z = ToRadians(e.z);
		float sx = std::sinf(euler.x);
		float cx = std::cosf(euler.x);
		float sy = std::sinf(euler.y);
		float cy = std::cosf(euler.y);
		float sz = std::sinf(euler.z);
		float cz = std::cosf(euler.z);
		Matrix4 result;
		result.m[0][0] = cy * cz;
		result.m[0][1] = cy * sz;
		result.m[0][2] = -sy;
		result.m[1][0] = sx * sy * cz - cx * sz;
		result.m[1][1] = sx * sy * sz + cx * cz;
		result.m[1][2] = sx * cy;
		result.m[2][0] = cx * sy * cz + sx * sz;
		result.m[2][1] = cx * sy * sz - sx * cz;
		result.m[2][2] = cx * cy;
		return result;
	}

	Matrix4 ToMatrix4(const Quaternion& q) {
		float ww = q.w * 2.0f;
		float xx = q.x * 2.0f;
		float yy = q.y * 2.0f;
		float zz = q.z * 2.0f;
		Matrix4 result;
		result.m[0][0] = 1.0f - yy * q.y - zz * q.z;
		result.m[0][1] = xx * q.y + ww * q.z;
		result.m[0][2] = xx * q.z - ww * q.y;
		result.m[1][0] = xx * q.y - ww * q.z;
		result.m[1][1] = 1.0f - xx * q.x - zz * q.z;
		result.m[1][2] = yy * q.z + ww * q.x;
		result.m[2][0] = xx * q.z + ww * q.y;
		result.m[2][1] = yy * q.z - ww * q.x;
		result.m[2][2] = 1.0f - xx * q.x - yy * q.y;
		return result;
	}

	Quaternion ToQuaternion(const EulerAngle& e) {
		Vector3 half;
		half.x = ToRadians(e.x) * 0.5f;
		half.y = ToRadians(e.y) * 0.5f;
		half.z = ToRadians(e.z) * 0.5f;
		float sx = std::sinf(half.x);
		float cx = std::cosf(half.x);
		float sy = std::sinf(half.y);
		float cy = std::cosf(half.y);
		float sz = std::sinf(half.z);
		float cz = std::cosf(half.z);
		return Quaternion(
			cx * cy * cz + sx * sy * sz,
			sx * cy * cz - cx * sy * sz,
			cx * sy * cz + sx * cy * sz,
			cx * cy * sz - sx * sy * cz
		);
	}

	Quaternion ToQuaternion(const Matrix4& m) {
		float tx = m.m[0][0] - m.m[1][1] - m.m[2][2];
		float ty = m.m[1][1] - m.m[0][0] - m.m[2][2];
		float tz = m.m[2][2] - m.m[0][0] - m.m[1][1];
		float tw = m.m[0][0] + m.m[1][1] + m.m[2][2];
		float tm = tx;
		uint32_t i = 0;
		if (ty > tm) {
			tm = ty;
			i = 1;
		}
		if (tz > tm) {
			tm = tz;
			i = 2;
		}
		if (tw > tm) {
			tm = tw;
			i = 3;
		}
		float max = std::sqrtf(tm + 1.0f) * 0.5f;
		float inv4max = 0.25f / max;
		Quaternion result;
		switch (i) {
		case 0:
			return Quaternion(
				max,
				(m.m[0][1] + m.m[1][0]) * inv4max,
				(m.m[2][0] + m.m[0][2]) * inv4max,
				(m.m[1][2] - m.m[2][1]) * inv4max
			);
		case 1:
			return Quaternion(
				(m.m[0][1] + m.m[1][0]) * inv4max,
				max,
				(m.m[1][2] + m.m[2][1]) * inv4max,
				(m.m[2][0] - m.m[0][2]) * inv4max
			);
		case 2:
			return Quaternion(
				(m.m[2][0] + m.m[0][2]) * inv4max,
				(m.m[1][2] + m.m[2][1]) * inv4max,
				max,
				(m.m[0][1] - m.m[1][0]) * inv4max
			);
		default:
			return Quaternion(
				(m.m[1][2] - m.m[2][1]) * inv4max,
				(m.m[2][0] - m.m[0][2]) * inv4max,
				(m.m[0][1] - m.m[1][0]) * inv4max,
				max
			);
		}
	}
}
