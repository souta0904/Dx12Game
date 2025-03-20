#pragma once
#include "Quaternion.h"
#include "Vector3.h"
#include <vector>

// 制御点
struct ControlPoint {
	Vector3 mPosition;
	float mRadius;
	//Vector3 mUp;// この制御点の上方向
};

// コース
class Course {
public:
	void AddControlPoint(ControlPoint cp) { mControlPoints.emplace_back(cp); }
	// 追加された制御点からコースを作成
	void Create();
	// コースの描画
	void DrawPrimitive();

	// 中心
	struct CenterInfo {
		Vector3 mPosition;
		float mRadius;
		Quaternion mRotate;
	};
	// 周囲(円周)
	struct CircumferenceInfo {
		Vector3 mPosition;
		Quaternion mRotate;
	};
	CenterInfo GetCenterInfo(uint32_t section, float t);
	CircumferenceInfo GetCircumferenceInfo(uint32_t section, float t, float angle);

	const uint32_t GetSectionNum() const { return mSectionNum; }

private:
	// スプライン曲線
	// パラメータtにおける値を計算
	template <typename T>
	T CatmullRom(const T& p0, const T& p1, const T& p2, const T& p3, float t) {
		return 0.5f * ((-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t + (-p0 + p2) * t + 2.0f * p1);
	}

	// 始点と終点から向きを計算
	Quaternion CalcDirection(const Vector3& start, const Vector3& end);

private:
	std::vector<ControlPoint> mControlPoints;// 制御点
	uint32_t mSectionNum;// 区間の数

	// 中心線用
	struct LinePosition {
		Vector3 mStart;
		Vector3 mEnd;
	};
	std::vector<LinePosition> mCenterLine;// 点線
	// 上下左右の線用
	std::vector<Vector3> mLinePoints[4];
};
