#pragma once
#include "Vector3.h"

// 円
class CircleCollider {
public:
	virtual void OnCollision() = 0;

	const Vector3& GetCenter() const { return mCenter; }
	float GetRadius() const { return mRadius; }
	void SetCenter(const Vector3& center) { mCenter = center; }
	void SetRadius(float radius) { mRadius = radius; }
private:
	Vector3 mCenter;
	float mRadius;
};

// 円と円の判定
inline bool Collision(CircleCollider* a, CircleCollider* b) {
	Vector3 d = a->GetCenter() - b->GetCenter();
	float r = a->GetRadius() + b->GetRadius();
	if (LengthSq(d) <= r * r) {
		return true;
	}
	return false;
}
