#pragma once
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"

class Transform {
public:
	void Update();
	void SetParent(Transform* parent);
	const Matrix4& GetLocal() const { return mLocal; }
	const Matrix4& GetWorld() const { return mWorld; }
public:
	Vector3 mScale = Vector3::kOne;
	Quaternion mRotate;
	Vector3 mTranslate;
private:
	Matrix4 mLocal;
	Matrix4 mWorld;
	Transform* mParent = nullptr;
};
