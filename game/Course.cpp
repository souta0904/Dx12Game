#include "Course.h"
#include "graphics/LineRenderer.h"
#include "MathUtil.h"
#include "Player.h"
#include <algorithm>

// 始点と終点から向きを計算
Quaternion Course::CalcDirection(const Vector3& start, const Vector3& end) {
	Quaternion result = {};
	Vector3 direction = end - start;// 向き
	if (Length(direction) > 0.001f) {
		direction.Normalize();
	}
	Vector3 up = Cross(Vector3::kUnitZ, direction);
	if (Length(up) > 0.001f) {
		result = Quaternion(Normalize(up), acosf(Dot(Vector3::kUnitZ, direction)));
	}
	return result;
}

Course::CenterInfo Course::GetCenterInfo(uint32_t section, float t) {
	CenterInfo result = {};
	ControlPoint p0, p1, p2, p3;
	section = MathUtil::Clamp<uint32_t>(section, 0, mSectionNum - 1);
	if (section <= 0) {// 最初の区間の場合
		p0 = p1 = mControlPoints[section];
		p2 = mControlPoints[section + 1];
		p3 = mControlPoints[section + 2];
	} else if (section >= mSectionNum - 1) {// 最後の区間の場合
		p0 = mControlPoints[section - 1];
		p1 = mControlPoints[section];
		p2 = p3 = mControlPoints[section + 1];
	} else {
		p0 = mControlPoints[section - 1];
		p1 = mControlPoints[section];
		p2 = mControlPoints[section + 1];
		p3 = mControlPoints[section + 2];
	}
	// 座標、半径の計算
	result.mPosition = CatmullRom(p0.mPosition, p1.mPosition, p2.mPosition, p3.mPosition, t);
	result.mRadius = CatmullRom(p0.mRadius, p1.mRadius, p2.mRadius, p3.mRadius, t);
	// 今の点と少し先の点から姿勢を計算
	float deltaT = 0.001f;// 少し先
	float nextT = t + deltaT;
	if (nextT >= 1.0f) {
		if (section < mSectionNum - 2) {
			nextT -= 1.0f;
			// 次の区間
			++section;
			p0 = mControlPoints[section - 1];
			p1 = mControlPoints[section];
			p2 = mControlPoints[section + 1];
			p3 = mControlPoints[section + 2];
		} else {
			nextT = 1.0f;
		}
	}
	Vector3 nextPosition = CatmullRom(p0.mPosition, p1.mPosition, p2.mPosition, p3.mPosition, nextT);// 少し先の座標
	result.mRotate = CalcDirection(result.mPosition, nextPosition);
	return result;
}

Course::CircumferenceInfo Course::GetCircumferenceInfo(uint32_t section, float t, float angle) {
	CircumferenceInfo result = {};
	CenterInfo centerInfo = GetCenterInfo(section, t);
	result.mPosition = centerInfo.mPosition + Vector3(cosf(angle) * centerInfo.mRadius, sinf(angle) * centerInfo.mRadius, 0.0f) * centerInfo.mRotate;
	// 今の点と少し先の点から姿勢を計算
	float deltaT = 0.001f;// 少し先
	float nextT = t + deltaT;
	if (nextT >= 1.0f) {
		if (section < mSectionNum - 1) {
			nextT -= 1.0f;
			// 次の区間
			++section;
		} else {
			nextT = 1.0f;
		}
	}
	CenterInfo nextCenterInfo = GetCenterInfo(section, nextT);
	Vector3 nextPosition = nextCenterInfo.mPosition + Vector3(cosf(angle) * nextCenterInfo.mRadius, sinf(angle) * nextCenterInfo.mRadius, 0.0f) * nextCenterInfo.mRotate;
	result.mRotate = CalcDirection(result.mPosition, nextPosition);
	return result;
}

void Course::Create() {
	mSectionNum = static_cast<uint32_t>(mControlPoints.size()) - 1;
	for (uint32_t section = 0; section < GetSectionNum(); ++section) {
		// 中心線
		for (uint32_t i = 0; i < kDivNum; ++i) {
			CenterInfo currInfo = GetCenterInfo(section, static_cast<float>(i) / kDivNum);
			CenterInfo nextInfo = GetCenterInfo(section, (i + 0.25f) / kDivNum);// 長さ1/4の点線
			LinePosition line = { currInfo.mPosition, nextInfo.mPosition };
			mCenterLine.emplace_back(line);
		}
		// 上下左右の線
		float angles[] = { MathUtil::kPiOver2,-MathUtil::kPiOver2,MathUtil::kPi,0.0f };
		for (uint32_t i = 0; i < 4; ++i) {
			for (uint32_t j = 0; j < kDivNum; ++j) {
				CircumferenceInfo currInfo = GetCircumferenceInfo(section, static_cast<float>(j) / kDivNum, angles[i]);
				mLinePoints[i].emplace_back(currInfo.mPosition);
			}
		}
	}
}

void Course::DrawPrimitive() {
	LineRenderer& pr = LineRenderer::GetInstance();

	int32_t currSection = mPlayer->GetCurrSection();
	float t = mPlayer->GetT();
	float pos = currSection * kDivNum + t * kDivNum;// プレイヤーがいる場所
	// フェード
	int32_t start = kDivNum * 0;// 0区間先
	int32_t end = start + kDivNum * 2;// 2区間先
	// 中心線
	for (int32_t i = 0; i < mCenterLine.size(); ++i) {
		if (i > pos + end ||// 前
			i < pos - end) {// 後
			continue;
		}
		float a = 1.0f;
		if (i >= pos + start) {// 前
			a = 1.0f - (i - (pos + start)) / (end - start);
		} else if (i <= pos - start) {// 後
			a = 1.0f - (i - (pos - start)) / -(end - start);
		}
		pr.DrawLine3(mCenterLine[i].mStart, mCenterLine[i].mEnd, Vector4(1.0f, 1.0f, 1.0f, a));
	}
	// 上下左右の線
	for (uint32_t i = 0; i < 4; ++i) {
		for (int32_t j = 0; j < mLinePoints[i].size() - 1; ++j) {
			if (j > pos + end ||// 前
				j < pos - end) {// 後
				continue;
			}
			float a = 1.0f;
			if (j >= pos + start) {// 前
				a = 1.0f - (j - (pos + start)) / (end - start);
			} else if (j <= pos - start) {// 後
				a = 1.0f - (j - (pos - start)) / -(end - start);
			}
			pr.DrawLine3(mLinePoints[i][j], mLinePoints[i][j + 1], Vector4(1.0f, 1.0f, 1.0f, a));
		}
	}
}
