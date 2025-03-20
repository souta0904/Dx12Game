#pragma once
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"

// トランスフォーム
class Transform {
public:
	void Update();
	void SetParent(Transform* parent);
	const Matrix4& GetLocalMat() const { return mLocalMat; }
	const Matrix4& GetWorldMat() const { return mWorldMat; }
public:
	Vector3 mScale = Vector3::kOne;
	Quaternion mRotate;
	Vector3 mTranslate;
private:
	Matrix4 mLocalMat;
	Matrix4 mWorldMat;
	Transform* mParent = nullptr;
};
