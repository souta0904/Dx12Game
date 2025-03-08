#pragma once
#include "Quaternion.h"
#include "Vector3.h"
#include <vector>

// 制御点
struct ControlPoint {
	Vector3 mPosition;
	float mRadius;
};

// コース
class Course {
public:
	void AddControlPoint(ControlPoint cp) { mControlPoints.emplace_back(cp); }
	// コースを構築
	void Create();
	// コースをDrawLineで描画
	void DrawPrimitive();

	struct Info {
		Vector3 mPosition;
		Quaternion mRotate;
		float mRadius;
	};
	// ある区間のtにおける情報を計算
	Info GetInfoAtT(uint32_t section, float t);

	const uint32_t GetSectionNum() const { return mSectionNum; }

private:
	template <typename T>
	T CatmullRom(const T& p0, const T& p1, const T& p2, const T& p3, float t) {
		return 0.5f * ((-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t + (-p0 + p2) * t + 2.0f * p1);
	}

private:
	std::vector<ControlPoint> mControlPoints;
	uint32_t mSectionNum;// 区間数

	/*enum DIRECTION { UP, DOWN, LEFT, RIGHT, DIRECTION_MAX };
	std::vector<Vector3> mPositions[DIRECTION_MAX];*/
	struct Line {
		Vector3 mStart;
		Vector3 mEnd;
	};
	std::vector<Line> mLines;
	std::vector<Line> mLinesUp;
	std::vector<Line> mLinesDown;
	std::vector<Line> mLinesLeft;
	std::vector<Line> mLinesRight;
};
