#pragma once
#include "Quaternion.h"
#include "Vector3.h"
#include <vector>

class GameScene;

// スプライン曲線
namespace CatmullRom {

	template <typename T>
	T GetPoint(const T& p0, const T& p1, const T& p2, const T& p3, float t) {
		T a = -p0 + 3.0f * p1 - 3.0f * p2 + p3;
		T b = 2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3;
		T c = -p0 + p2;
		T d = 2.0f * p1;
		return 0.5f * (a * t * t * t + b * t * t + c * t + d);
	}

	template <typename T>
	T GetTangent(const T& p0, const T& p1, const T& p2, const T& p3, float t) {
		T a = -p0 + 3.0f * p1 - 3.0f * p2 + p3;
		T b = 2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3;
		T c = -p0 + p2;
		return 0.5f * (3.0f * a * t * t + 2.0f * b * t + c);
	}

};

struct CoursePoint {
	Vector3 mPosition;	// 座標
	float mRadius;		// 半径
	//float mRotate;	// 回転
};

class Course {
public:
	struct CenterInfo {// 中心
		Vector3 mPosition;
		float mRadius;
		Quaternion mRotate;
	};
	struct AroundInfo {// 円周上
		Vector3 mPosition;
		Quaternion mRotate;
	};
private:
	struct CenterLine {
		Vector3 mStart;
		Vector3 mEnd;
		float mT;
	};
	struct AroundLine {
		Vector3 mPosition;
		float mT;
	};
	struct Circle {
		std::vector<Vector3> mPositions;
		float mT;
	};

public:
	static float CalcTransparent(float pt, float t);

	Course(GameScene* gameScene);
	void AddPoint(CoursePoint point);
	void Create();
	void Update() {}
	void DrawPrimitive();

	CenterInfo GetCenterInfo(float t);
	AroundInfo GetAroundInfo(float t, float rad, float margin = 0.0f);

	const uint32_t GetSectionNum() const { return mSectionNum; }

public:
	static const float kDistFromAround;
	static const float kFadeStart;
	static const float kFadeEnd;

private:
	GameScene* mGameScene;
	std::vector<CoursePoint> mCoursePoints;
	uint32_t mSectionNum;// 区間数

	uint32_t mDivNum = 30;// 1区間の分割数
	std::vector<CenterLine> mCenterLine;
	std::vector<AroundLine> mAroundLines[4];// 上下左右

	uint32_t mCircleNum = 3;// 1区間の円の数
	std::vector<Circle> mCircles;
};
