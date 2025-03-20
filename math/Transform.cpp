#include "Transform.h"

// 行列を更新
void Transform::Update() {
	mLocalMat = CreateAffine4(mScale, mRotate, mTranslate);
	if (mParent) {
		mWorldMat = mParent->mWorldMat * mLocalMat;
	} else {
		mWorldMat = mLocalMat;
	}
}

void Transform::SetParent(Transform* parent) {
	if (mParent) {
		// 親を解除
		mScale = mWorldMat.GetScale();
		mRotate = mWorldMat.GetRotate();
		mTranslate = mWorldMat.GetTranslate();
	}
	if (parent) {
		// 親を設定
		Matrix4 local = mWorldMat * Inverse(parent->mWorldMat);
		mScale = local.GetScale();
		mRotate = local.GetRotate();
		mTranslate = local.GetTranslate();
	}
	mParent = parent;
}
