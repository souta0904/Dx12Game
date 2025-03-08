#include "Course.h"
#include "MathUtil.h"
#include "graphics/LineRenderer.h"
#include <algorithm>

void Course::Create() {
	mSectionNum = static_cast<uint32_t>(mControlPoints.size()) - 1;
	for (uint32_t section = 0; section < GetSectionNum(); ++section) {
		// 1区間で20分割
		uint32_t kDivNum = 30;
		for (uint32_t i = 0; i < kDivNum - 1; ++i) {
			Info curr = GetInfoAtT(section, static_cast<float>(i) / kDivNum);
			Info next = GetInfoAtT(section, static_cast<float>(i + 0.25f) / kDivNum);
			Line line = { curr.mPosition, next.mPosition };
			mLines.emplace_back(line);
		}
		if (section < mSectionNum - 1) {
			Info curr = GetInfoAtT(section, static_cast<float>(kDivNum - 1) / kDivNum);
			Info next = GetInfoAtT(section, static_cast<float>(kDivNum - 0.75f) / kDivNum);
			Line line = { curr.mPosition, next.mPosition };
			mLines.emplace_back(line);
		}

		for (uint32_t i = 0; i < kDivNum - 1; ++i) {
			Info curr = GetInfoAtT(section, static_cast<float>(i) / kDivNum);
			Info next = GetInfoAtT(section, static_cast<float>(i + 1) / kDivNum);
			Vector3 offset = Vector3(cosf(MathUtil::kPiOver2) * curr.mRadius, sinf(MathUtil::kPiOver2) * curr.mRadius, 0.0f);
			Vector3 nextOffset = Vector3(cosf(MathUtil::kPiOver2) * next.mRadius, sinf(MathUtil::kPiOver2) * next.mRadius, 0.0f);
			Line line = { curr.mPosition + offset * curr.mRotate, next.mPosition + nextOffset * next.mRotate };
			mLinesUp.emplace_back(line);
		}
		if (section < mSectionNum - 1) {
			Info curr = GetInfoAtT(section, static_cast<float>(kDivNum - 1) / kDivNum);
			Info next = GetInfoAtT(section + 1, 0.0f);
			Vector3 offset = Vector3(cosf(MathUtil::kPiOver2) * curr.mRadius, sinf(MathUtil::kPiOver2) * curr.mRadius, 0.0f);
			Vector3 nextOffset = Vector3(cosf(MathUtil::kPiOver2) * next.mRadius, sinf(MathUtil::kPiOver2) * next.mRadius, 0.0f);
			Line line = { curr.mPosition + offset * curr.mRotate, next.mPosition + nextOffset * next.mRotate };
			mLinesUp.emplace_back(line);
		}

		for (uint32_t i = 0; i < kDivNum - 1; ++i) {
			Info curr = GetInfoAtT(section, static_cast<float>(i) / kDivNum);
			Info next = GetInfoAtT(section, static_cast<float>(i + 1) / kDivNum);
			Vector3 offset = Vector3(cosf(-MathUtil::kPiOver2) * curr.mRadius, sinf(-MathUtil::kPiOver2) * curr.mRadius, 0.0f);
			Vector3 nextOffset = Vector3(cosf(-MathUtil::kPiOver2) * next.mRadius, sinf(-MathUtil::kPiOver2) * next.mRadius, 0.0f);
			Line line = { curr.mPosition + offset * curr.mRotate, next.mPosition + nextOffset * next.mRotate };
			mLinesDown.emplace_back(line);
		}
		if (section < mSectionNum - 1) {
			Info curr = GetInfoAtT(section, static_cast<float>(kDivNum - 1) / kDivNum);
			Info next = GetInfoAtT(section + 1, 0.0f);
			Vector3 offset = Vector3(cosf(-MathUtil::kPiOver2) * curr.mRadius, sinf(-MathUtil::kPiOver2) * curr.mRadius, 0.0f);
			Vector3 nextOffset = Vector3(cosf(-MathUtil::kPiOver2) * next.mRadius, sinf(-MathUtil::kPiOver2) * next.mRadius, 0.0f);
			Line line = { curr.mPosition + offset * curr.mRotate, next.mPosition + nextOffset * next.mRotate };
			mLinesDown.emplace_back(line);
		}

		for (uint32_t i = 0; i < kDivNum - 1; ++i) {
			Info curr = GetInfoAtT(section, static_cast<float>(i) / kDivNum);
			Info next = GetInfoAtT(section, static_cast<float>(i + 1) / kDivNum);
			Vector3 offset = Vector3(cosf(-MathUtil::kPi) * curr.mRadius, sinf(-MathUtil::kPi) * curr.mRadius, 0.0f);
			Vector3 nextOffset = Vector3(cosf(-MathUtil::kPi) * next.mRadius, sinf(-MathUtil::kPi) * next.mRadius, 0.0f);
			Line line = { curr.mPosition + offset * curr.mRotate, next.mPosition + nextOffset * next.mRotate };
			mLinesLeft.emplace_back(line);
		}
		if (section < mSectionNum - 1) {
			Info curr = GetInfoAtT(section, static_cast<float>(kDivNum - 1) / kDivNum);
			Info next = GetInfoAtT(section + 1, 0.0f);
			Vector3 offset = Vector3(cosf(-MathUtil::kPi) * curr.mRadius, sinf(-MathUtil::kPi) * curr.mRadius, 0.0f);
			Vector3 nextOffset = Vector3(cosf(-MathUtil::kPi) * next.mRadius, sinf(-MathUtil::kPi) * next.mRadius, 0.0f);
			Line line = { curr.mPosition + offset * curr.mRotate, next.mPosition + nextOffset * next.mRotate };
			mLinesLeft.emplace_back(line);
		}

		for (uint32_t i = 0; i < kDivNum - 1; ++i) {
			Info curr = GetInfoAtT(section, static_cast<float>(i) / kDivNum);
			Info next = GetInfoAtT(section, static_cast<float>(i + 1) / kDivNum);
			Vector3 offset = Vector3(cosf(0.0f) * curr.mRadius, sinf(0.0f) * curr.mRadius, 0.0f);
			Vector3 nextOffset = Vector3(cosf(0.0f) * next.mRadius, sinf(0.0f) * next.mRadius, 0.0f);
			Line line = { curr.mPosition + offset * curr.mRotate, next.mPosition + nextOffset * next.mRotate };
			mLinesRight.emplace_back(line);
		}
		if (section < mSectionNum - 1) {
			Info curr = GetInfoAtT(section, static_cast<float>(kDivNum - 1) / kDivNum);
			Info next = GetInfoAtT(section + 1, 0.0f);
			Vector3 offset = Vector3(cosf(0.0f) * curr.mRadius, sinf(0.0f) * curr.mRadius, 0.0f);
			Vector3 nextOffset = Vector3(cosf(0.0f) * next.mRadius, sinf(0.0f) * next.mRadius, 0.0f);
			Line line = { curr.mPosition + offset * curr.mRotate, next.mPosition + nextOffset * next.mRotate };
			mLinesRight.emplace_back(line);
		}
	}
}

// ある区間のtにおける情報を計算
Course::Info Course::GetInfoAtT(uint32_t section, float t) {
	ControlPoint p0, p1, p2, p3;
	section = MathUtil::Clamp<uint32_t>(section, 0, mSectionNum - 1);
	if (section <= 0) {// 最初の区間
		p0 = p1 = mControlPoints[section];
		p2 = mControlPoints[section + 1];
		p3 = mControlPoints[section + 2];
	} else if (section >= mSectionNum - 1) {// 最後の区間
		p0 = mControlPoints[section - 1];
		p1 = mControlPoints[section];
		p2 = p3 = mControlPoints[section + 1];
	} else {
		p0 = mControlPoints[section - 1];
		p1 = mControlPoints[section];
		p2 = mControlPoints[section + 1];
		p3 = mControlPoints[section + 2];
	}
	Info result;
	result.mPosition = CatmullRom(p0.mPosition, p1.mPosition, p2.mPosition, p3.mPosition, t);
	result.mRadius = CatmullRom(p0.mRadius, p1.mRadius, p2.mRadius, p3.mRadius, t);

	// 回転の計算
	float nextT = MathUtil::Min(t + 0.001f, 1.0f);
	Vector3 nextPos = CatmullRom(p0.mPosition, p1.mPosition, p2.mPosition, p3.mPosition, nextT);
	Vector3 dir = nextPos - result.mPosition;
	if (Length(dir) > 0.001f) {
		dir.Normalize();
	}
	Vector3 cross = Cross(Vector3::kUnitZ, dir);
	if (Length(cross) > 0.001f) {
		result.mRotate = Quaternion(Normalize(cross), acosf(Dot(Vector3::kUnitZ, dir)));
	}

	return result;
}

void Course::DrawPrimitive() {
	LineRenderer& pr = LineRenderer::GetInstance();
	for (uint32_t i = 0; i < mLines.size(); ++i) {
		pr.DrawLine3(mLines[i].mStart, mLines[i].mEnd);
	}
	for (uint32_t i = 0; i < mLinesUp.size(); ++i) {
		pr.DrawLine3(mLinesUp[i].mStart, mLinesUp[i].mEnd);
	}
	for (uint32_t i = 0; i < mLinesDown.size(); ++i) {
		pr.DrawLine3(mLinesDown[i].mStart, mLinesDown[i].mEnd);
	}
	for (uint32_t i = 0; i < mLinesLeft.size(); ++i) {
		pr.DrawLine3(mLinesLeft[i].mStart, mLinesLeft[i].mEnd);
	}
	for (uint32_t i = 0; i < mLines.size(); ++i) {
		pr.DrawLine3(mLinesRight[i].mStart, mLinesRight[i].mEnd);
	}
}
