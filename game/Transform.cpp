#include "Transform.h"

void Transform::Update() {
	mLocal = CreateAffine4(mScale, mRotate, mTranslate);
	if (mParent) {
		mWorld = mParent->mWorld * mLocal;
	} else {
		mWorld = mLocal;
	}
}

void Transform::SetParent(Transform* parent) {
	if (mParent) {
		mScale = mWorld.GetScale();
		mRotate = mWorld.GetRotate();
		mTranslate = mWorld.GetTranslate();
	}
	if (parent) {
		Matrix4 local = mWorld * Inverse(parent->mWorld);
		mScale = local.GetScale();
		mRotate = local.GetRotate();
		mTranslate = local.GetTranslate();
	}
}
