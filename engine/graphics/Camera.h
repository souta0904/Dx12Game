#pragma once
#include "MathUtil.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"

enum class ProjectionMode {
	kOrthographic,
	kPerspective
};

class Camera {
public:
	void UpdateMatrix();
	const Matrix4& GetViewMat() const { return mViewMat; }
	const Matrix4& GetProjectionMat() const { return mProjectionMat; }
	const Matrix4& GetViewProjectionMat() const { return mViewProjectionMat; }
public:
	Quaternion mRotate;
	Vector3 mTranslate;
	ProjectionMode mProjectionMode = ProjectionMode::kPerspective;
	float mNearZ = 0.1f;
	float mFarZ = 1000.0f;
	float mFovY = MathUtil::kPiOver2;
private:
	Matrix4 mViewMat;
	Matrix4 mProjectionMat;
	Matrix4 mViewProjectionMat;
};
