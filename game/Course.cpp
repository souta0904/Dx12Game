#include "Course.h"
#include "GameScene.h"
#include "graphics/LineRenderer.h"
#include "MathUtil.h"
#include "Player.h"
#include <algorithm>
#include <cassert>

Course::Course(GameScene* gameScene)
	: mGameScene(gameScene) {

}

void Course::AddPoint(CoursePoint point) {
	mCoursePoints.emplace_back(point);
}

void Course::Create() {
	// クリア
	mCenterLine.clear();
	for (uint32_t i = 0; i < 4; ++i) {
		mAroundLines[i].clear();
	}
	mCircles.clear();

	// ライン
	mSectionNum = static_cast<uint32_t>(mCoursePoints.size()) - 1;// 区間数
	float rad[4] = { MathUtil::kPiOver2,-MathUtil::kPiOver2,MathUtil::kPi,0.0f };// 上下左右の角度
	for (uint32_t section = 0; section < mSectionNum; ++section) {
		for (uint32_t i = 0; i < mDivNum; ++i) {
			// 中心線
			static const float deltaT = 0.5f;// 点線なので長さ1/4
			float st = section + static_cast<float>(i) / mDivNum;
			float et = st + deltaT / mDivNum;
			CenterInfo sInfo = GetCenterInfo(st);// start
			CenterInfo eInfo = GetCenterInfo(et);// end
			CenterLine cLine;
			cLine.mStart = sInfo.mPosition;
			cLine.mEnd = eInfo.mPosition;
			cLine.mT = st;
			mCenterLine.emplace_back(cLine);

			// 上下左右の線
			for (uint32_t j = 0; j < 4; ++j) {
				AroundInfo aLine = GetAroundInfo(st, rad[j]);
				AroundLine line;
				line.mPosition = aLine.mPosition;
				line.mT = st;
				mAroundLines[j].emplace_back(line);
			}
		}
	}
	// 上下左右の線の最後
	for (uint32_t i = 0; i < 4; ++i) {
		AroundLine line;
		line.mT = static_cast<float>(mSectionNum);
		AroundInfo aLine = GetAroundInfo(line.mT, rad[i]);
		line.mPosition = aLine.mPosition;
		mAroundLines[i].emplace_back(line);
	}

	// 円
	static const uint32_t kPointNum = 32;// 円を作る点の数
	for (uint32_t section = 0; section < mSectionNum; ++section) {
		for (uint32_t i = 0; i < mCircleNum; ++i) {
			Circle circle;
			circle.mT = section + 1.0f / mCircleNum * i;
			for (uint32_t j = 0; j < kPointNum; ++j) {
				float r = MathUtil::k2Pi / kPointNum * j;
				AroundInfo info = GetAroundInfo(circle.mT, r);
				circle.mPositions.push_back(info.mPosition);
			}
			mCircles.emplace_back(circle);
		}
	}
	Circle circle;// 最後
	circle.mT = static_cast<float>(mSectionNum);
	for (uint32_t i = 0; i < kPointNum; ++i) {
		float r = MathUtil::k2Pi / kPointNum * i;
		AroundInfo info = GetAroundInfo(circle.mT, r);
		circle.mPositions.push_back(info.mPosition);
	}
	mCircles.emplace_back(circle);
}

void Course::DrawPrimitive() {
	LineRenderer& pr = LineRenderer::GetInstance();
	Player* p = mGameScene->GetPlayer();
	float pt = p->GetCurrT();
	float t2Cam = p->GetT2Cam();
	// 透過フラグ
	bool useTransparent = true;

	// 中心線
	for (CenterLine& line : mCenterLine) {
		float a = 1.0f;
		if (useTransparent) {
			a = CalcTransparent(pt, line.mT);
			if (a <= 0.0f) {
				continue;
			}
			// カメラに近いとき描画しない
			if (std::fabs(pt + t2Cam - line.mT) <= 0.2f) {
				continue;
			}
		}
		pr.DrawLine3(line.mStart, line.mEnd, Vector4(1.0f, 1.0f, 1.0f, a));
	}

	// 上下左右の線
	for (uint32_t i = 0; i < 4; ++i) {
		for (int32_t j = 0; j < mAroundLines[i].size() - 1; ++j) {
			AroundLine& line = mAroundLines[i][j];
			float a = 1.0f;
			if (useTransparent) {
				a = CalcTransparent(pt, line.mT);
				if (a <= 0.0f) {
					continue;
				}
			}
			pr.DrawLine3(line.mPosition, mAroundLines[i][j + 1].mPosition, Vector4(1.0f, 1.0f, 1.0f, a));
		}
	}

	// 円
	for (Circle& circle : mCircles) {
		float a = 1.0f;
		if (useTransparent) {
			a = CalcTransparent(pt, circle.mT);
			if (a <= 0.0f) {
				continue;
			}
		}
		for (uint32_t i = 0; i < circle.mPositions.size(); ++i) {
			if (i < circle.mPositions.size() - 1) {
				pr.DrawLine3(circle.mPositions[i], circle.mPositions[i + 1], Vector4(1.0f, 1.0f, 1.0f, a));
			} else {// 最後
				pr.DrawLine3(circle.mPositions[i], circle.mPositions[0], Vector4(1.0f, 1.0f, 1.0f, a));
			}
		}
	}
}

Course::CenterInfo Course::GetCenterInfo(float t) {
	CenterInfo result = {};
	CoursePoint p0, p1, p2, p3;
	t = MathUtil::Clamp(t, 0.0f, static_cast<float>(mSectionNum));// t
	uint32_t idx = MathUtil::Clamp(static_cast<uint32_t>(t), 0u, mSectionNum - 1);// 後ろの点のインデックス
	if (t >= mSectionNum) {
		t = 1.0f;
	} else {
		t = t - static_cast<uint32_t>(t);// 小数部分
	}
	if (idx <= 0) {// 最初の区間
		p0 = p1 = mCoursePoints[idx];
		p2 = mCoursePoints[idx + 1];
		p3 = mCoursePoints[idx + 2];
	} else if (idx >= mSectionNum - 1) {// 最後の区間
		p0 = mCoursePoints[idx - 1];
		p1 = mCoursePoints[idx];
		p2 = p3 = mCoursePoints[idx + 1];
	} else {
		p0 = mCoursePoints[idx - 1];
		p1 = mCoursePoints[idx];
		p2 = mCoursePoints[idx + 1];
		p3 = mCoursePoints[idx + 2];
	}
	// 座標と半径
	result.mPosition = CatmullRom::GetPoint(p0.mPosition, p1.mPosition, p2.mPosition, p3.mPosition, t);
	result.mRadius = CatmullRom::GetPoint(p0.mRadius, p1.mRadius, p2.mRadius, p3.mRadius, t);
	// 回転
	Vector3 tan = CatmullRom::GetTangent(p0.mPosition, p1.mPosition, p2.mPosition, p3.mPosition, t);
	result.mRotate = MathUtil::QuaternionFromVector3(Vector3::kUnitZ, tan);
	return result;
}

Course::AroundInfo Course::GetAroundInfo(float t, float rad) {
	AroundInfo result = {};
	CenterInfo cInfo = GetCenterInfo(t);
	result.mPosition = cInfo.mPosition + Vector3(std::cosf(rad) * cInfo.mRadius, std::sinf(rad) * cInfo.mRadius, 0.0f) * cInfo.mRotate;
	result.mRotate = cInfo.mRotate;
	return result;
}

// 透過を計算
float Course::CalcTransparent(float pt, float t) {
	if (t > pt + mFadeEnd ||
		t < pt - mFadeEnd) {
		return 0.0f;
	}
	if (t > pt + mFadeStart ||
		t < pt + mFadeStart) {
		return 1.0f - (std::fabs(t - pt) - mFadeStart) / (mFadeEnd - mFadeStart);
	}
	return 1.0f;
}
